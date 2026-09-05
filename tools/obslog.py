#!/usr/bin/env python3
"""obslog - Observatory event log recorder and validator (ISS-008).

The log is the single source of truth (RFC-0001 / ADR-0001): ``add`` appends
exactly one event, ``check`` validates existing files, and neither command
ever rewrites or deletes a historical line. Envelope rules follow
observatory/schema/event.schema.json (schema_v 1, with the v1.1 optional
run/teaching fields from ADR-0004); the type registry below mirrors
observatory/schema/data-model.md section 5 and is kept in sync by
tools/test_obslog.py. Standard library only.
"""

from __future__ import annotations

import argparse
import json
import re
import sys
from datetime import datetime
from pathlib import Path

SCHEMA_V = 1
LOG_DIR_PARTS = ("observatory", "data", "log")

# Mirrors the "type" enum in observatory/schema/event.schema.json.
# tools/test_obslog.py asserts the two stay identical.
EVENT_TYPES = frozenset({
    "project.created",
    "charter.adopted",
    "charter.amended",
    "member.onboarded",
    "member.location_changed",
    "member.task_changed",
    "milestone.created",
    "milestone.completed",
    "issue.created",
    "issue.status_changed",
    "rfc.opened",
    "rfc.decided",
    "discussion.opened",
    "discussion.turn",
    "discussion.closed",
    "adr.accepted",
    "adr.superseded",
    "pr.opened",
    "pr.reviewed",
    "pr.merged",
    "pr.rejected",
    "commit.recorded",
    "release.tagged",
    "module.registered",
    "module.deprecated",
    "module.dependency_changed",
    "interaction.logged",
    "metrics.snapshot",
    # v1.1 (ADR-0004)
    "project.state_discrepancy",
    "run.opened",
    "run.checkpoint",
    "run.blocked",
    "run.resumed",
    "mentor.question",
    "mentor.response",
    "learn.lookup",
    "learn.misconception",
    "learn.resolved",
    "ambient.note",
})

REQUIRED_FIELDS = ("seq", "ts", "type", "actor", "schema_v", "payload")
KNOWN_FIELDS = frozenset(REQUIRED_FIELDS) | {
    "refs",
    "run_id",
    "phase_id",
    "logical_seq",
    "process_mode",
}

ACTOR_RE = re.compile(r"^(owner|system|member:[a-z][a-z-]*)$")
REF_RE = re.compile(
    r"^(issue|rfc|adr|discussion|pr|commit|release|milestone|module|member|run):[^\s]+$"
)
RUN_ID_RE = re.compile(r"^RUN-[0-9]{4}$")
PHASE_ID_RE = re.compile(r"^P[0-9]$")

REF_KINDS = (
    "issue/rfc/adr/discussion/pr/commit/release/milestone/module/member/run"
)


class ObslogError(Exception):
    """Fatal, user-facing error; message is printed and the tool exits 1."""


def default_root() -> Path:
    # tools/obslog.py lives one level under the repo root; anchoring there
    # keeps the documented commands working from any working directory.
    return Path(__file__).resolve().parent.parent


def resolve_root(value: str | None) -> Path:
    return Path(value).resolve() if value else default_root()


def log_dir_under(root: Path) -> Path:
    return root.joinpath(*LOG_DIR_PARTS)


def data_lines(text: str) -> list[str]:
    """Split raw file text into data lines, dropping the empty tail that
    follows a final newline. Per DEC-RUN0001-P1-002 each line is stripped of
    trailing \\r so Windows working-tree CRLF never sticks to parsed content."""
    lines = text.split("\n")
    if lines and lines[-1] == "":
        lines.pop()
    return [line.rstrip("\r\n") for line in lines]


def is_iso8601_with_offset(value: str) -> bool:
    try:
        parsed = datetime.fromisoformat(value)
    except ValueError:
        return False
    return parsed.tzinfo is not None and parsed.utcoffset() is not None


def envelope_problems(event: object) -> list[tuple[str, str]]:
    """Validate one event against the envelope rules. Returns (severity,
    message) pairs; severity is "error" or "warning". Unknown types and
    unknown envelope fields are warnings (forward compatibility, ADR-0004);
    everything else that violates the schema is an error."""
    problems: list[tuple[str, str]] = []
    if not isinstance(event, dict):
        return [("error", "event is not a JSON object")]

    for field in REQUIRED_FIELDS:
        if field not in event:
            problems.append(("error", f"missing required field '{field}'"))

    if "seq" in event:
        seq = event["seq"]
        if isinstance(seq, bool) or not isinstance(seq, int) or seq < 1:
            problems.append(("error", f"'seq' must be an integer >= 1, got {seq!r}"))

    if "ts" in event:
        ts = event["ts"]
        if not isinstance(ts, str) or not is_iso8601_with_offset(ts):
            problems.append((
                "error",
                f"'ts' must be ISO8601 with UTC offset "
                f"(e.g. 2026-09-05T09:00:00+08:00), got {ts!r}",
            ))

    if "type" in event:
        etype = event["type"]
        if not isinstance(etype, str):
            problems.append(("error", f"'type' must be a string, got {etype!r}"))
        elif etype not in EVENT_TYPES:
            problems.append((
                "warning",
                f"unknown event type {etype!r} (not in the registry; warning "
                f"only so logs written before the type was added stay valid)",
            ))

    if "actor" in event:
        actor = event["actor"]
        if not isinstance(actor, str) or not ACTOR_RE.match(actor):
            problems.append((
                "error",
                f"'actor' must be 'owner', 'system' or 'member:<id>', got {actor!r}",
            ))

    if "schema_v" in event:
        version = event["schema_v"]
        if isinstance(version, bool) or version != SCHEMA_V:
            problems.append(("error", f"'schema_v' must be {SCHEMA_V}, got {version!r}"))

    if "refs" in event:
        refs = event["refs"]
        if not isinstance(refs, list) or not all(isinstance(r, str) for r in refs):
            problems.append(("error", f"'refs' must be an array of strings, got {refs!r}"))
        else:
            for ref in refs:
                if not REF_RE.match(ref):
                    problems.append((
                        "error",
                        f"bad ref {ref!r} (expected <kind>:<id> with kind one of {REF_KINDS})",
                    ))

    if "payload" in event:
        payload = event["payload"]
        if not isinstance(payload, dict):
            problems.append((
                "error",
                f"'payload' must be a JSON object, got {type(payload).__name__}",
            ))

    if "run_id" in event:
        run_id = event["run_id"]
        if not isinstance(run_id, str) or not RUN_ID_RE.match(run_id):
            problems.append(("error", f"'run_id' must match RUN-NNNN, got {run_id!r}"))

    if "phase_id" in event:
        phase_id = event["phase_id"]
        if not isinstance(phase_id, str) or not PHASE_ID_RE.match(phase_id):
            problems.append(("error", f"'phase_id' must match P<N> (P0-P9), got {phase_id!r}"))

    if "logical_seq" in event:
        logical = event["logical_seq"]
        if isinstance(logical, bool) or not isinstance(logical, int) or logical < 1:
            problems.append(("error", f"'logical_seq' must be an integer >= 1, got {logical!r}"))

    if "process_mode" in event:
        mode = event["process_mode"]
        if not isinstance(mode, str) or not mode.strip():
            problems.append(("error", f"'process_mode' must be a non-empty string, got {mode!r}"))

    unknown = sorted(set(event) - KNOWN_FIELDS)
    if unknown:
        names = ", ".join(repr(name) for name in unknown)
        problems.append((
            "warning",
            f"unknown envelope field(s) {names} (ignored for forward compatibility)",
        ))
    return problems


def read_text_or_none(path: Path) -> tuple[str | None, str]:
    try:
        return path.read_text(encoding="utf-8"), ""
    except OSError as exc:
        return None, f"cannot read file: {exc}"
    except UnicodeDecodeError as exc:
        return None, f"file is not valid UTF-8: {exc}"


def scan_max_seq(log_dir: Path) -> int:
    """Largest seq across every *.jsonl under log_dir. Unparseable lines are
    skipped here; surfacing them is check's job."""
    max_seq = 0
    if not log_dir.is_dir():
        return 0
    for path in sorted(log_dir.glob("*.jsonl")):
        text, _ = read_text_or_none(path)
        if text is None:
            continue
        for line in data_lines(text):
            try:
                event = json.loads(line)
            except json.JSONDecodeError:
                continue
            if not isinstance(event, dict):
                continue
            seq = event.get("seq")
            if isinstance(seq, int) and not isinstance(seq, bool) and seq > max_seq:
                max_seq = seq
    return max_seq


def display_path(path: Path, root: Path) -> str:
    try:
        return str(path.relative_to(root))
    except ValueError:
        return str(path)


def cmd_add(args: argparse.Namespace) -> int:
    root = resolve_root(args.root)
    log_dir = log_dir_under(root)

    # Collect every problem before writing anything, so the caller fixes
    # them all in one pass instead of rediscovering them one by one.
    problems: list[str] = []

    try:
        payload = json.loads(args.payload)
    except json.JSONDecodeError as exc:
        payload = None
        problems.append(
            f"--payload is not valid JSON: {exc.msg} "
            f"(example: --payload '{{\"issue_id\": \"ISS-008\"}}')"
        )
    else:
        if not isinstance(payload, dict):
            problems.append(
                f"--payload must be a JSON object, got {type(payload).__name__} "
                f"(example: --payload '{{\"issue_id\": \"ISS-008\"}}')"
            )

    if args.type not in EVENT_TYPES:
        problems.append(
            f"unknown --type {args.type!r}: add only accepts registered types; "
            f"the list lives in observatory/schema/data-model.md section 5"
        )

    if not ACTOR_RE.match(args.actor or ""):
        problems.append(
            f"--actor must be 'owner', 'system' or 'member:<id>' "
            f"(e.g. member:mora), got {args.actor!r}"
        )

    refs = args.ref or []
    for ref in refs:
        if not REF_RE.match(ref):
            problems.append(
                f"bad --ref {ref!r}: expected <kind>:<id> with kind one of "
                f"{REF_KINDS} (e.g. issue:ISS-008)"
            )

    if args.ts is not None and not is_iso8601_with_offset(args.ts):
        problems.append(
            f"--ts must be ISO8601 with UTC offset "
            f"(e.g. 2026-09-05T09:00:00+08:00), got {args.ts!r}"
        )

    if args.run_id is not None and not RUN_ID_RE.match(args.run_id):
        problems.append(f"--run-id must match RUN-NNNN (e.g. RUN-0001), got {args.run_id!r}")

    if args.phase_id is not None and not PHASE_ID_RE.match(args.phase_id):
        problems.append(f"--phase-id must match P<N> (e.g. P2), got {args.phase_id!r}")

    if args.logical_seq is not None and args.logical_seq < 1:
        problems.append(f"--logical-seq must be >= 1, got {args.logical_seq}")

    if args.process_mode is not None and not args.process_mode.strip():
        problems.append(f"--process-mode must be a non-empty string, got {args.process_mode!r}")

    if problems:
        print(f"obslog: event rejected, {len(problems)} problem(s):", file=sys.stderr)
        for problem in problems:
            print(f"  - {problem}", file=sys.stderr)
        print("nothing was written; fix the arguments and run again", file=sys.stderr)
        return 1

    if args.file is not None:
        target = Path(args.file).resolve()
    else:
        today = datetime.now().astimezone().date().isoformat()
        target = log_dir / f"{today}-tools.jsonl"

    seq = scan_max_seq(log_dir) + 1
    ts = args.ts or datetime.now().astimezone().isoformat(timespec="seconds")

    event: dict[str, object] = {
        "seq": seq,
        "ts": ts,
        "schema_v": SCHEMA_V,
        "type": args.type,
        "actor": args.actor,
        "refs": refs,
        "payload": payload,
    }
    # Optional v1.1 fields go right after schema_v, matching the field order
    # already used in observatory/data/log/2026-09-06-run0001.jsonl.
    if args.run_id is not None:
        event["run_id"] = args.run_id
    if args.phase_id is not None:
        event["phase_id"] = args.phase_id
    if args.logical_seq is not None:
        event["logical_seq"] = args.logical_seq
    if args.process_mode is not None:
        event["process_mode"] = args.process_mode

    leftover = envelope_problems(event)
    if leftover:  # unreachable if the checks above are complete; keep as a guard
        for severity, message in leftover:
            print(f"obslog: internal validation {severity}: {message}", file=sys.stderr)
        print("nothing was written", file=sys.stderr)
        return 1

    target.parent.mkdir(parents=True, exist_ok=True)
    # Compact separators match the style of the existing handwritten logs.
    line = json.dumps(event, ensure_ascii=False, separators=(",", ":")) + "\n"
    with open(target, "a", encoding="utf-8", newline="\n") as handle:
        handle.write(line)

    print(f"appended seq={seq} type={args.type} -> {display_path(target, root)}")
    return 0


def cmd_check(args: argparse.Namespace) -> int:
    root = resolve_root(args.root)
    log_dir = log_dir_under(root)

    if not log_dir.is_dir():
        raise ObslogError(
            f"log directory not found: {log_dir}\n"
            f"  (run this from the repository, or pass --root <repo-root>)"
        )

    files = sorted(log_dir.glob("*.jsonl"))
    if not files:
        print(f"no .jsonl files found in {log_dir} - nothing to check")
        return 0

    total_errors = 0
    total_warnings = 0
    total_lines = 0
    max_seq: int | None = None
    last_logical: dict[str, int] = {}
    per_file: list[tuple[str, int, int, int]] = []

    for path in files:
        rel = display_path(path, root)
        file_errors = 0
        file_warnings = 0
        text, read_problem = read_text_or_none(path)
        if text is None:
            print(f"ERROR {rel}: {read_problem}")
            per_file.append((rel, 0, 1, 0))
            total_errors += 1
            continue

        lines = data_lines(text)
        for lineno, line in enumerate(lines, 1):
            is_final = lineno == len(lines)
            try:
                event = json.loads(line)
            except json.JSONDecodeError as exc:
                if is_final:
                    # Crash safety: a half-written trailing line is skipped,
                    # not fatal (data-model section 1).
                    file_warnings += 1
                    print(
                        f"WARN  {rel}:{lineno}  truncated final line "
                        f"(JSON parse failed: {exc.msg}); skipped"
                    )
                else:
                    file_errors += 1
                    print(f"ERROR {rel}:{lineno}  invalid JSON: {exc.msg}")
                continue

            if not isinstance(event, dict):
                file_errors += 1
                print(f"ERROR {rel}:{lineno}  event is not a JSON object")
                continue

            for severity, message in envelope_problems(event):
                if severity == "warning":
                    file_warnings += 1
                    print(f"WARN  {rel}:{lineno}  {message}")
                else:
                    file_errors += 1
                    print(f"ERROR {rel}:{lineno}  {message}")

            seq = event.get("seq")
            if isinstance(seq, int) and not isinstance(seq, bool):
                if max_seq is None:
                    max_seq = seq
                elif seq == max_seq:
                    file_errors += 1
                    print(f"ERROR {rel}:{lineno}  duplicate seq {seq} (already seen)")
                elif seq < max_seq:
                    file_errors += 1
                    print(
                        f"ERROR {rel}:{lineno}  seq went backwards: {seq} "
                        f"after {max_seq} (the log is append-only and "
                        f"globally monotonic)"
                    )
                elif seq > max_seq + 1:
                    file_warnings += 1
                    print(
                        f"WARN  {rel}:{lineno}  seq gap: expected {max_seq + 1}, "
                        f"got {seq}"
                    )
                    max_seq = seq
                else:
                    max_seq = seq

            run_id = event.get("run_id")
            logical = event.get("logical_seq")
            if isinstance(logical, int) and not isinstance(logical, bool):
                if not isinstance(run_id, str):
                    file_warnings += 1
                    print(
                        f"WARN  {rel}:{lineno}  'logical_seq' without a valid "
                        f"'run_id'; cannot check run ordering"
                    )
                else:
                    previous = last_logical.get(run_id)
                    if previous is not None and logical < previous:
                        file_errors += 1
                        print(
                            f"ERROR {rel}:{lineno}  logical_seq went backwards "
                            f"in {run_id}: {logical} after {previous}"
                        )
                    last_logical[run_id] = max(previous or 0, logical)

        per_file.append((rel, len(lines), file_errors, file_warnings))
        total_errors += file_errors
        total_warnings += file_warnings
        total_lines += len(lines)

    print()
    for rel, count, file_errors, file_warnings in per_file:
        print(f"{rel}: {count} lines, {file_errors} error(s), {file_warnings} warning(s)")
    print(
        f"total: {len(files)} file(s), {total_lines} lines, "
        f"{total_errors} error(s), {total_warnings} warning(s)"
    )
    if total_errors:
        print(f"result: FAILED ({total_errors} error(s))")
        return 1
    print("result: OK")
    return 0


def cmd_view(args: argparse.Namespace) -> int:
    print("not implemented yet (ISS-011)")
    return 2


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="obslog",
        description=(
            "Observatory event log recorder and validator (ISS-008). "
            "The log is append-only: add appends one event, check validates "
            "existing files, history lines are never rewritten."
        ),
    )
    sub = parser.add_subparsers(dest="command", required=True, metavar="{add,check,view}")

    p_add = sub.add_parser(
        "add",
        help="append one event to the log",
        description="Validate the envelope and append exactly one event.",
    )
    p_add.add_argument(
        "--type",
        required=True,
        help="event type from the registry (observatory/schema/data-model.md section 5)",
    )
    p_add.add_argument(
        "--actor",
        required=True,
        help="'owner', 'system' or 'member:<id>' (e.g. member:mora)",
    )
    p_add.add_argument(
        "--ref",
        action="append",
        default=None,
        metavar="KIND:ID",
        help="reference such as issue:ISS-008; repeat for multiple refs",
    )
    p_add.add_argument(
        "--payload",
        required=True,
        help='event payload as a JSON object string, e.g. \'{"issue_id": "ISS-008"}\'',
    )
    p_add.add_argument(
        "--ts",
        default=None,
        help="ISO8601 timestamp with UTC offset; default: now (local offset)",
    )
    p_add.add_argument("--run-id", default=None, help="optional, RUN-NNNN (v1.1)")
    p_add.add_argument("--phase-id", default=None, help="optional, P0-P9 (v1.1)")
    p_add.add_argument(
        "--logical-seq",
        type=int,
        default=None,
        help="optional, monotonically increasing within --run-id (v1.1)",
    )
    p_add.add_argument(
        "--process-mode",
        default=None,
        help="optional free-form execution-mode note, e.g. pair/solo/gate (v1.1)",
    )
    p_add.add_argument(
        "--file",
        default=None,
        help="target .jsonl file; default: observatory/data/log/<today>-tools.jsonl",
    )
    p_add.add_argument(
        "--root",
        default=None,
        help="repository root; default: inferred from this script's location",
    )
    p_add.set_defaults(func=cmd_add)

    p_check = sub.add_parser(
        "check",
        help="validate all existing log files",
        description="Envelope-validate every line and check global seq ordering.",
    )
    p_check.add_argument(
        "--root",
        default=None,
        help="repository root; default: inferred from this script's location",
    )
    p_check.set_defaults(func=cmd_check)

    p_view = sub.add_parser(
        "view",
        help="read-only projections of the log (not implemented yet)",
    )
    p_view.set_defaults(func=cmd_view)
    return parser


def main(argv: list[str] | None = None) -> int:
    for stream in (sys.stdout, sys.stderr):
        if hasattr(stream, "reconfigure"):
            stream.reconfigure(errors="replace")
    args = build_parser().parse_args(argv)
    try:
        return args.func(args)
    except ObslogError as exc:
        print(f"obslog: error: {exc}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
