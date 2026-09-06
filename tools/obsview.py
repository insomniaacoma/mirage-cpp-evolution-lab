#!/usr/bin/env python3
"""OW0 view data generator (ISS-011, ADR-0003).

Reads observatory/data/log/*.jsonl + observatory/snapshots/RUN-*/P*.json,
computes projections generator-side (the frontend only renders), and writes
observatory/view/data.js as `window.OBS_DATA = {...}`.

Every block carries source: "repository" | "fixture" (ISS-013). Only stdlib.
Usage: python tools/obsview.py
"""
import glob
import json
import os
import re
import sys
from datetime import datetime, timezone, timedelta

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LOG_DIR = os.path.join(ROOT, "observatory", "data", "log")
SNAP_DIR = os.path.join(ROOT, "observatory", "snapshots")
VIEW_DIR = os.path.join(ROOT, "observatory", "view")

TZ = timezone(timedelta(hours=8))

# ADR-0005 cognitive-load policy: legacy English discussions get a Chinese
# summary (summary_zh); discussions logged from now on are Chinese-first.
DISCUSSION_SUMMARY_ZH = {
    "D-0001": "Mora 提议用 append-only JSONL 做观测站历史（一行一个事件）；Dee 反对手写会 schema 漂移、要求先有校验；Mora 妥协——envelope 的 JSON Schema 当天冻结，校验器放进 ISS-008；Arden 附加三条纪律（日志唯一事实源、投影只读、改 schema 走 ADR）；Dee 有条件接受。结论 → ADR-0001。",
    "D-0002": "选 M0 的窗口/渲染库。Rin 担心 raylib 挡住 Owner 未来的图形学习，条件是把调用面收敛到两个文件，让将来替换是真的可行；Vera 要求静态链接不要 DLL；Nico 只要快。接受 raylib 5.5 → ADR-0002（Autonomous Project Decision）。",
    "D-0003": "OW0 观测站技术线。Ash 支持『静态 Web UI + Python generator』，但要求所有投影计算只在 generator 侧、前端只渲染，否则两处逻辑会漂移；Dot 要求 fixture 数据隔离且默认关闭，Gate 视图永远不许显示 fixture。接受方案 C → ADR-0003。",
    "D-0004": "事件模型 v1.1：加 run/phase/logical_seq 与 mentor/learn/ambient 事件族。Arden 要求 ambient 的四条硬约束逐字进 ADR（锚定真实活动/不改状态/Gate 默认过滤/不得含技术事实）；Dee 妥协——未知事件类型在校验时只报 warning 不报 error，保证旧日志永远合法。接受 → ADR-0004。",
}


def load_events():
    events = []
    for path in sorted(glob.glob(os.path.join(LOG_DIR, "*.jsonl"))):
        lines = open(path, encoding="utf-8").read().splitlines()
        for i, line in enumerate(lines):
            line = line.strip()
            if not line:
                continue
            try:
                events.append(json.loads(line))
            except json.JSONDecodeError:
                if i == len(lines) - 1:
                    print(f"warn: truncated final line in {os.path.basename(path)} (ignored)")
                else:
                    raise SystemExit(f"FATAL: corrupt line {i+1} in {path}")
    events.sort(key=lambda e: e["seq"])
    return events


def project_issues(events):
    """Issue kanban state + status history purely from events."""
    issues = {}
    for e in events:
        p = e.get("payload", {})
        t = e["type"]
        if t == "issue.created":
            issues[p["issue_id"]] = {
                "id": p["issue_id"],
                "title": p.get("title", ""),
                "owner": p.get("owner", ""),
                "milestone": p.get("milestone", ""),
                "hands_on": p.get("hands_on", False),
                "status": "open",
                "history": [{"from": "-", "to": "open", "ts": e["ts"]}],
            }
        elif t == "issue.status_changed":
            it = issues.setdefault(p["issue_id"], {"id": p["issue_id"], "title": p["issue_id"], "status": "open", "history": []})
            it["status"] = p.get("to", it["status"])
            it["history"].append({"from": p.get("from"), "to": p.get("to"), "ts": e["ts"]})
    return list(issues.values())


def project_discussions(events):
    out = {}
    order = []
    for e in events:
        p = e.get("payload", {})
        t = e["type"]
        if t == "discussion.opened":
            out[p["discussion_id"]] = {
                "id": p["discussion_id"], "topic": p.get("topic", ""),
                "participants": p.get("participants", []), "turns": [],
                "outcome": None, "adr": None,
            }
            order.append(p["discussion_id"])
        elif t == "discussion.turn":
            d = out.setdefault(p["discussion_id"], {"id": p["discussion_id"], "topic": "(unrecorded opening)", "participants": [], "turns": [], "outcome": None, "adr": None})
            d["turns"].append({"member": p.get("member"), "stance": p.get("stance"), "text": p.get("text", ""), "ts": e["ts"]})
        elif t == "discussion.closed":
            d = out.get(p["discussion_id"])
            if d:
                d["outcome"] = p.get("outcome")
                d["adr"] = p.get("adr")
                d["stance_summary"] = p.get("stance_summary")
    for did, d in out.items():
        if did in DISCUSSION_SUMMARY_ZH:
            d["summary_zh"] = DISCUSSION_SUMMARY_ZH[did]
    return [out[k] for k in order if k in out]


def project_modules(events):
    mods = {}
    for e in events:
        p = e.get("payload", {})
        if e["type"] == "module.registered":
            mods[p["module_id"]] = {"id": p["module_id"], "owner": p.get("owner"), "ts": e["ts"], "deps": []}
        elif e["type"] == "module.dependency_changed":
            m = mods.setdefault(p["from_module"], {"id": p["from_module"], "owner": None, "ts": None, "deps": []})
            if p.get("added", True):
                m["deps"].append(p["to_module"])
            elif p["to_module"] in m["deps"]:
                m["deps"].remove(p["to_module"])
    return list(mods.values())


def project_reviews(events):
    prs = {}
    order = []
    for e in events:
        p = e.get("payload", {})
        t = e["type"]
        pid = p.get("pr_id")
        if not pid:
            continue
        if t == "pr.opened":
            prs[pid] = {"id": pid, "title": p.get("title", pid), "branch": p.get("branch", ""), "author": p.get("author"), "timeline": []}
            order.append(pid)
        if pid in prs and t in ("pr.reviewed", "pr.merged", "pr.rejected"):
            prs[pid]["timeline"].append({"type": t, "actor": e["actor"], "verdict": p.get("verdict"), "ts": e["ts"]})
    return [prs[k] for k in order]


def project_studio(events, snapshots):
    """Member location/state at each checkpoint.

    Locations come from member.onboarded events as the default, overlaid by
    member.location_changed events, then by snapshot agent_states (which may
    omit location entirely - P1 did). Snapshot states are repository artifacts.
    """
    default_loc = {}
    default_role = {}
    for e in events:
        p = e.get("payload", {})
        if e["type"] == "member.onboarded":
            default_loc[p["member_id"]] = p.get("location", "design-room")
            default_role[p["member_id"]] = p.get("role", "")
        elif e["type"] == "member.location_changed":
            default_loc[p["member_id"]] = p.get("to", default_loc.get(p["member_id"]))
    out = []
    for snap in snapshots:
        members = {}
        for mid, st in (snap.get("agent_states") or {}).items():
            merged = {"location": default_loc.get(mid, "design-room"), "role": default_role.get(mid, "")}
            merged.update(st)
            members[mid] = merged
        out.append({
            "checkpoint": snap.get("phase"),
            "run_id": snap.get("run_id"),
            "members": members,
        })
    return out


def project_metrics(events):
    out = []
    for e in events:
        if e["type"] == "metrics.snapshot":
            out.append({"version": e["payload"].get("version"), "ts": e["ts"], "metrics": e["payload"].get("metrics", {})})
    return out


def project_checkpoints(snapshots):
    return [{
        "phase": s.get("phase"),
        "phase_name": s.get("phase_name"),
        "run_id": s.get("run_id"),
        "git_sha": s.get("git_sha"),
        "context_budget": s.get("context_budget"),
        "build_status": s.get("build_status"),
        "tests": s.get("tests"),
        "blockers": s.get("blockers", []),
    } for s in snapshots]


def load_snapshots():
    snaps = []
    for path in sorted(glob.glob(os.path.join(SNAP_DIR, "RUN-*", "P*.json"))):
        try:
            snaps.append(json.load(open(path, encoding="utf-8")))
        except json.JSONDecodeError as ex:
            print(f"warn: bad snapshot {path}: {ex}")
    snaps.sort(key=lambda s: (s.get("run_id", ""), s.get("phase", "")))
    return snaps


def fixture_blocks():
    """Things the event model cannot know yet (ISS-013): marked and default-hidden."""
    return {
        "source": "fixture",
        "note": "SAMPLE ONLY - no event source exists for these fields yet; OW1 candidates for auto-collection",
        "studio_live": {
            "member:vic": {"editing": "src/world_graph.cpp", "branch": "feature/m0-worldgraph"},
            "member:milo": {"editing": "tools/obslog.py", "branch": "feature/ow0-obslog"},
        },
        "ci": None,
    }


def main():
    events = load_events()
    snapshots = load_snapshots()
    data = {
        "generated_at": datetime.now(TZ).isoformat(timespec="seconds"),
        "source_note": "repository blocks are pure projections of observatory/data/log + snapshots; fixture blocks are sample-only and UI-hidden by default (ISS-013)",
        "events": events,
        "issues": {"source": "repository", "items": project_issues(events)},
        "discussions": {"source": "repository", "items": project_discussions(events)},
        "modules": {"source": "repository", "items": project_modules(events)},
        "reviews": {"source": "repository", "items": project_reviews(events)},
        "studio": {"source": "repository", "items": project_studio(events, snapshots)},
        "checkpoints": {"source": "repository", "items": project_checkpoints(snapshots)},
        "metrics": {"source": "repository", "items": project_metrics(events)},
        "fixture": fixture_blocks(),
    }
    os.makedirs(VIEW_DIR, exist_ok=True)
    out_path = os.path.join(VIEW_DIR, "data.js")
    with open(out_path, "w", encoding="utf-8", newline="\n") as f:
        f.write("// generated by tools/obsview.py - do not edit; regenerate with `python tools/obsview.py`\n")
        f.write("window.OBS_DATA = ")
        json.dump(data, f, ensure_ascii=False, separators=(",", ":"))
        f.write(";\n")
    print(f"ok: {out_path} ({len(events)} events, {len(snapshots)} checkpoints)")


if __name__ == "__main__":
    sys.exit(main())
