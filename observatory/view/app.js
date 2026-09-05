/* MIRAGE Observatory OW0 frontend (ISS-012). Renders only; all projections are
   precomputed in data.js by tools/obsview.py (ADR-0003 clause 3). */
"use strict";
const D = window.OBS_DATA;
const state = { tab: "studio", fixture: false, run: "RUN-0001", cursor: null, disc: 0, discStep: 0, member: null, phase: null, track: "M0" };
const ROOMS = {
  "design-room": { x: 40, y: 40, w: 150, h: 110 },
  "gameplay-lab": { x: 220, y: 40, w: 150, h: 110 },
  "systems-lab": { x: 400, y: 40, w: 150, h: 110 },
  "rendering-lab": { x: 40, y: 190, w: 150, h: 110 },
  "audio-lab": { x: 220, y: 190, w: 150, h: 110 },
  "tools-lab": { x: 400, y: 190, w: 150, h: 110 },
  "qa-lab": { x: 220, y: 340, w: 150, h: 110 },
};
const ROLE_COLOR = { lead: "#8ab4ff", gameplay: "#e0b35e", core: "#7bc99a", render: "#d97b7b", audio: "#c39bd3", tools: "#6fc7d9", qa: "#a8a8a8", owner: "#ffffff" };
function roleColor(id, role) {
  const r = role || "";
  for (const k of Object.keys(ROLE_COLOR)) if (r.startsWith(k) || (id || "").replace("member:", "").startsWith(k)) return ROLE_COLOR[k];
  return "#9aa";
}
function esc(s) { const d = document.createElement("div"); d.textContent = String(s == null ? "" : s); return d.innerHTML; }
function short(id) { return (id || "").replace("member:", ""); }

/* ---------- header ---------- */
const TABS = [["studio", "Studio"], ["arch", "Architecture"], ["work", "Work"], ["timeline", "Timeline"], ["disc", "Discussions"], ["metrics", "Metrics"], ["lab", "Process Lab"]];
function renderTabs() {
  document.getElementById("tabs").innerHTML = TABS.map(([k, label]) =>
    `<button data-t="${k}" class="${state.tab === k ? "on" : ""}">${label}</button>`).join("");
  document.querySelectorAll("nav button").forEach(b => b.onclick = () => { state.tab = b.dataset.t; render(); });
}
document.getElementById("fixture-toggle").onchange = e => { state.fixture = e.target.checked; render(); };

/* ---------- helpers ---------- */
function issuesAt(seq) {
  const map = {};
  for (const e of D.events) {
    if (seq != null && e.seq > seq) break;
    const p = e.payload || {};
    if (e.type === "issue.created") map[p.issue_id] = { id: p.issue_id, title: p.title, owner: p.owner, milestone: p.milestone || "", status: "open", hands_on: !!p.hands_on };
    if (e.type === "issue.status_changed" && map[p.issue_id]) map[p.issue_id].status = p.to;
  }
  return Object.values(map);
}
function runEvents() { return D.events.filter(e => e.run_id === state.run); }
function runMeta() {
  const evs = runEvents();
  return { maxSeq: evs.length ? Math.max(...evs.map(e => e.logical_seq || 0)) : 0, minSeq: evs.length ? Math.min(...evs.map(e => e.logical_seq || 1)) : 1 };
}

/* ---------- views ---------- */
function vStudio() {
  const cps = D.studio.items.filter(s => s.run_id === state.run);
  const cp = cps.find(s => s.checkpoint === state.phase) || cps[cps.length - 1] || { members: {} };
  const members = Object.keys(cp.members || {}).sort();
  const chips = cps.map(s => `<span class="chip ${s.checkpoint === cp.checkpoint ? "on" : ""}" data-cp="${s.checkpoint}">checkpoint ${s.checkpoint}</span>`).join("");
  let edges = "";
  const pairs = {};
  for (const d of D.discussions.items) {
    const ps = d.participants.map(short);
    for (let i = 0; i < ps.length; i++) for (let j = i + 1; j < ps.length; j++) {
      const k = [ps[i], ps[j]].sort().join("|"); pairs[k] = (pairs[k] || 0) + 1 + d.turns.length * 0.1;
    }
  }
  const center = r => [r.x + r.w / 2, r.y + r.h / 2];
  const locOf = m => { const st = cp.members[m] || {}; return st.location || (m === "member:owner" ? "design-room" : null); };
  for (const [k, w] of Object.entries(pairs)) {
    const [a, b] = k.split("|");
    const ra = ROOMS[locOf("member:" + a)], rb = ROOMS[locOf("member:" + b)];
    if (!ra || !rb) continue;
    const [x1, y1] = center(ra), [x2, y2] = center(rb);
    edges += `<line x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" stroke="#8ab4ff" stroke-opacity="0.35" stroke-width="${Math.min(4, w)}"/>`;
  }
  let av = "";
  for (const m of members) {
    const st = cp.members[m] || {};
    const room = ROOMS[locOf(m)];
    if (!room) continue;
    const dormant = st.state === "dormant";
    av += `<g class="avatar" data-m="${esc(m)}" opacity="${dormant ? 0.35 : 1}">
      <circle cx="${room.x + room.w / 2}" cy="${room.y + 44}" r="17" fill="${roleColor(m, st.role)}" fill-opacity="0.85"/>
      <text x="${room.x + room.w / 2}" y="${room.y + 48}" text-anchor="middle" fill="#0b0c10" style="font-weight:700">${esc(short(m)[0].toUpperCase())}</text>
      <text x="${room.x + room.w / 2}" y="${room.y + 78}" text-anchor="middle">${esc(short(m))}</text>
      <text x="${room.x + room.w / 2}" y="${room.y + 93}" text-anchor="middle" class="dimt">${esc(st.state || "idle")}${state.fixture && D.fixture.studio_live[m] ? '<tspan class="fixtag"> FIXTURE</tspan>' : ""}</text>
    </g>`;
  }
  const rooms = Object.entries(ROOMS).map(([k, r]) =>
    `<rect x="${r.x}" y="${r.y}" width="${r.w}" height="${r.h}" rx="10" fill="#14161d" stroke="#2a2e3a"/>
     <text x="${r.x + 10}" y="${r.y + 20}" class="dimt">${esc(k)}${k === "audio-lab" ? " (dormant)" : ""}</text>`).join("");
  return `<h2>Studio View <span class="dim">— avatar 状态由事件与 checkpoint 派生，不是动画</span></h2>
    <div class="chips">${chips}</div>
    <svg width="620" height="480" style="background:var(--panel);border:1px solid var(--line);border-radius:8px">${rooms}${edges}${av}</svg>
    <div class="dim" style="margin-top:6px">连线 = 讨论共现强度 · 点击 avatar 查看详情 · dormant 成员半透明不产生 trace</div>`;
}

function vArch() {
  const mods = D.modules.items;
  const W = 760, H = 300;
  const pos = {}; mods.forEach((m, i) => { pos[m.id] = [90 + (i % 3) * 240, 60 + Math.floor(i / 3) * 130]; });
  let svg = "";
  for (const m of mods) for (const dep of m.deps) {
    const [x1, y1] = pos[m.id] || [0, 0]; const [x2, y2] = pos[dep] || [0, 0];
    svg += `<line x1="${x1 + 90}" y1="${y1 + 22}" x2="${x2}" y2="${y2 + 22}" stroke="#e0b35e" stroke-opacity="0.6" marker-end="url(#arr)"/>`;
  }
  for (const m of mods) {
    const [x, y] = pos[m.id];
    svg += `<rect x="${x}" y="${y}" width="180" height="44" rx="8" fill="#1a1d26" stroke="#2a2e3a"/>
      <text x="${x + 12}" y="${y + 19}" style="font-weight:600">${esc(m.id)}</text>
      <text x="${x + 12}" y="${y + 34}" class="dimt">owner: ${esc(short(m.owner) || "?")}</text>`;
  }
  return `<h2>Architecture Graph <span class="dim">— module.registered / dependency_changed 的重放投影</span></h2>
  <svg width="${W}" height="${H}" style="background:var(--panel);border:1px solid var(--line);border-radius:8px">
    <defs><marker id="arr" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="6" markerHeight="6" orient="auto"><path d="M0,0 L10,5 L0,10 z" fill="#e0b35e"/></marker></defs>
    ${svg || `<text x="20" y="30" class="dimt">no module.registered events yet — 模块出生后此处生长</text>`}
  </svg>
  <div class="dim">第三方依赖（raylib）受 ADR-0002 约束：调用面 = src/main.cpp + src/scene.cpp。环检测 / fan-in-out 在模块数量产生真实压力后上线（charter P1）。</div>`;
}

function board(items) {
  const cols = ["open", "in-progress", "in-review", "blocked", "done"];
  return `<div class="board">${cols.map(c => {
    const cards = items.filter(i => i.status === c);
    return `<div class="col"><h4>${c} (${cards.length})</h4>${cards.map(i =>
      `<div class="card"><span class="id">${esc(i.id)}</span><span class="own">${esc(short(i.owner))}</span><span class="badge ${esc(i.milestone.toLowerCase().startsWith("ow0") ? "ow0" : "m0")}">${esc(i.milestone)}</span>${i.hands_on ? '<span class="badge">🎓</span>' : ""}<span class="badge ${esc(i.status)}">${esc(i.status)}</span><span class="ttl">${esc(i.title)}</span></div>`).join("")}</div>`;
  }).join("")}</div>`;
}

function vWork() {
  const all = D.issues.items;
  const m0 = all.filter(i => (i.milestone || "").startsWith("M0"));
  const ow = all.filter(i => !(i.milestone || "").startsWith("M0"));
  return `<h2>Work <span class="dim">— issue.created / status_changed 重放</span></h2>
  <div class="two"><div><h3>Game M0</h3>${board(m0)}</div><div><h3>Observatory OW0</h3>${board(ow)}</div></div>
  <h3>Reviews</h3>${D.reviews.items.map(r => `<div class="ev"><span class="t">${esc(r.id)}</span> <span class="a">${esc(short(r.author))}</span> ${esc(r.branch)} ${r.timeline.map(t => `<span class="pill ${t.verdict && t.verdict.includes("Changes") ? "concern" : "acceptance"}">${esc(t.type)}${t.verdict ? ": " + esc(t.verdict) : ""}</span>`).join(" ")}</div>`).join("") || '<span class="dim">无记录</span>'}`;
}

function vTimeline() {
  const evs = runEvents();
  const { minSeq, maxSeq } = runMeta();
  if (state.cursor == null) state.cursor = maxSeq;
  const list = evs.filter(e => (e.logical_seq || 0) <= state.cursor);
  const shown = list.slice(-40).reverse();
  const issues = issuesAt(list.length ? Math.max(...list.map(e => e.seq)) : 0);
  return `<h2>Timeline <span class="dim">— 按 logical_seq 重放（D4），ts 仅审计</span></h2>
  <div class="panel">
    <input type="range" min="${minSeq}" max="${maxSeq}" value="${state.cursor}" id="cursor">
    <div class="dim">logical_seq ${state.cursor} / ${maxSeq} · 显示最近 40 条 · 此刻 issue 状态：<span class="badge open">open ${issues.filter(i => i.status === "open").length}</span> <span class="badge in-progress">wip ${issues.filter(i => i.status === "in-progress").length}</span> <span class="badge done">done ${issues.filter(i => i.status === "done").length}</span></div>
  </div>
  ${shown.map(e => `<div class="ev"><span class="s">R${esc(e.run_id || "-")}·${esc(e.phase_id || "-")}·${esc(e.logical_seq ?? "-")}</span> <span class="t">${esc(e.type)}</span> <span class="a">${esc(short(e.actor))}</span> <span class="s">${esc(e.ts)}</span> ${esc(JSON.stringify(e.payload).slice(0, 160))}</div>`).join("")}`;
}

function vDisc() {
  const ds = D.discussions.items;
  if (!ds.length) return "<h2>Discussions</h2><span class='dim'>no discussion recorded</span>";
  const d = ds[Math.min(state.disc, ds.length - 1)];
  const step = Math.min(state.discStep, d.turns.length);
  return `<h2>Discussion Replay</h2>
  <div class="chips">${ds.map((x, i) => `<span class="chip ${i === state.disc ? "on" : ""}" data-d="${i}">${esc(x.id)}</span>`).join("")}</div>
  <div class="panel"><b>${esc(d.topic)}</b> <span class="dim">${esc(d.participants.map(short).join(" · "))}</span></div>
  ${d.turns.slice(0, step).map((t, i) => `<div class="ev ${i < step - 1 ? "past" : ""}"><span class="a">${esc(short(t.member))}</span> <span class="pill ${esc(t.stance)}">${esc(t.stance)}</span><br>${esc(t.text)}</div>`).join("")}
  <div style="margin:10px 0">
    <button class="act" id="dprev">◀ prev</button> <button class="act" id="dnext">next ▶</button>
    <span class="dim">step ${step} / ${d.turns.length}</span></div>
  ${step >= d.turns.length ? `<div class="panel gate-ok">outcome: <b>${esc(d.outcome || "pending")}</b>${d.adr ? " · " + esc(d.adr) : ""}${d.stance_summary ? " · " + esc(JSON.stringify(d.stance_summary)) : ""}<br><span class="dim">分歧与条件已按 ADR-0001 P4 原样保留</span></div>` : ""}`;
}

function vMetrics() {
  const ms = D.metrics.items;
  if (!ms.length) return "<h2>Metrics</h2><span class='dim'>no snapshot yet</span>";
  const keys = [...new Set(ms.flatMap(m => Object.keys(m.metrics)))];
  return `<h2>Metrics <span class="dim">— metrics.snapshot（趋势观察，非 KPI，charter §17）</span></h2>
  <table><tr><th>metric</th>${ms.map(m => `<th>${esc(m.version)}</th>`).join("")}</tr>
  ${keys.map(k => {
    const vals = ms.map(m => m.metrics[k]);
    const num = vals.map(v => typeof v === "number" ? v : null);
    const max = Math.max(...num.map(v => v == null ? 0 : v), 1);
    return `<tr><td><code>${esc(k)}</code></td>${vals.map((v, i) => `<td>${num[i] == null ? esc(v) : `${v}<div class="bar"><i style="width:${100 * v / max}%"></i></div>`}</td>`).join("")}</tr>`;
  }).join("")}</table>`;
}

function vLab() {
  const all = D.issues.items;
  const cps = D.checkpoints.items.filter(c => c.run_id === state.run);
  return `<h2>Process Lab <span class="dim">— 同屏对比两条轨道的方法与门禁</span></h2>
  <div class="two"><div><h3>Game — Milestone (M0)</h3>${board(all.filter(i => (i.milestone || "").startsWith("M0")))}</div>
  <div><h3>Observatory — Wave (OW0)</h3>${board(all.filter(i => !(i.milestone || "").startsWith("M0")))}</div></div>
  <h3>Integration Gates / Checkpoints</h3>
  <table><tr><th>checkpoint</th><th>git</th><th>build</th><th>tests</th><th>context</th><th>blockers</th></tr>
  ${cps.map(c => `<tr><td>${esc(c.phase)} ${esc(c.phase_name || "")}</td><td><code>${esc((c.git_sha || "").slice(0, 8))}</code></td><td>${esc(c.build_status || "")}</td><td>${c.tests ? esc(JSON.stringify(c.tests)) : "-"}</td><td>${esc(c.context_budget || "-")}</td><td>${c.blockers && c.blockers.length ? esc(JSON.stringify(c.blockers)) : '<span class="gate-ok">none</span>'}</td></tr>`).join("")}</table>
  <h3>Fixture 盘点（ISS-013）</h3><div class="panel">${state.fixture ? `<span class="fixtag">FIXTURE</span> ${esc(JSON.stringify(D.fixture.studio_live || {}))}` : '<span class="dim">fixture 隐藏中（默认关）。打开右上角 Show Fixture 查看 OW1 自动化采集候选。</span>'}</div>`;
}

/* ---------- member side panel ---------- */
function memberPanel(m) {
  const st = (D.studio.items.filter(s => s.run_id === state.run).slice(-1)[0].members || {})[m] || {};
  const its = D.issues.items.filter(i => short(i.owner) === short(m));
  const evs = runEvents().filter(e => short(e.actor) === short(m)).slice(-6).reverse();
  return `<div class="side"><span class="x" id="sideclose">✕</span><h3>${esc(short(m))}</h3>
  <div class="kv"><b>state</b>${esc(st.state || "idle")}</div>
  <div class="kv"><b>location</b>${esc(st.location || "-")}</div>
  <div class="kv"><b>owned issues</b>${its.map(i => esc(i.id + " " + i.status)).join(", ") || "-"}</div>
  <div class="kv"><b>recent events</b><br>${evs.map(e => `${esc(e.logical_seq ?? e.seq)} · ${esc(e.type)}`).join("<br>") || "-"}</div>
  <div class="kv"><b>worklog</b><a href="../worklogs/${esc(state.run)}/P2/${esc(short(m))}.md">P2 worklog</a></div></div>`;
}

/* ---------- render ---------- */
function render() {
  renderTabs(); // nav highlight follows state.tab (was stuck on Studio forever)
  document.getElementById("meta").innerHTML =
    `generated ${esc(D.generated_at)} · events ${D.events.length} · discussions ${D.discussions.items.length} · ${esc(D.source_note)}`;
  const v = document.getElementById("view");
  v.innerHTML = ({ studio: vStudio, arch: vArch, work: vWork, timeline: vTimeline, disc: vDisc, metrics: vMetrics, lab: vLab })[state.tab]();
  v.querySelectorAll("[data-cp]").forEach(el => el.onclick = () => { state.phase = el.dataset.cp; render(); });
  v.querySelectorAll("[data-d]").forEach(el => el.onclick = () => { state.disc = +el.dataset.d; state.discStep = 0; render(); });
  v.querySelectorAll("[data-m]").forEach(el => el.onclick = () => { state.member = el.dataset.m; v.insertAdjacentHTML("beforeend", memberPanel(state.member)); document.getElementById("sideclose").onclick = () => { state.member = null; render(); }; });
  const cur = document.getElementById("cursor");
  if (cur) cur.oninput = e => { state.cursor = +e.target.value; render(); document.getElementById("cursor").focus(); };
  const dp = document.getElementById("dprev"), dn = document.getElementById("dnext");
  if (dp) dp.onclick = () => { state.discStep = Math.max(0, state.discStep - 1); render(); };
  if (dn) dn.onclick = () => { state.discStep = Math.min(D.discussions.items[state.disc].turns.length, state.discStep + 1); render(); };
}
renderTabs();
render();
