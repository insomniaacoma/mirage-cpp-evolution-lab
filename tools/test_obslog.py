"""Tests for tools/obslog.py (ISS-008).

Every test runs the CLI in a subprocess against a temporary --root, so the
real observatory/data/log/ directory is never read or written by these tests.
Standard library only.
"""

from __future__ import annotations

import json
import subprocess
import sys
import tempfile
import unittest
from datetime import datetime
from pathlib import Path

TOOLS_DIR = Path(__file__).resolve().parent
OBSLOG = TOOLS_DIR / "obslog.py"
REPO_ROOT = TOOLS_DIR.parent

if str(TOOLS_DIR) not in sys.path:
    sys.path.insert(0, str(TOOLS_DIR))
import obslog  # noqa: E402


def run_obslog(*args: str) -> subprocess.CompletedProcess:
    return subprocess.run(
        [sys.executable, str(OBSLOG), *args],
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )


def envelope(seq: int = 1, **overrides: object) -> dict:
    event = {
        "seq": seq,
        "ts": "2026-09-05T09:00:00+08:00",
        "schema_v": 1,
        "type": "issue.status_changed",
        "actor": "member:vera",
        "refs": ["issue:ISS-003"],
        "payload": {"issue_id": "ISS-003", "from": "open", "to": "in-progress"},
    }
    event.update(overrides)
    return event


class ObslogTestCase(unittest.TestCase):
    """Base fixture: an isolated temporary repository root."""

    def setUp(self) -> None:
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        self.root = Path(tmp.name)
        self.log_dir = self.root / "observatory" / "data" / "log"
        self.log_dir.mkdir(parents=True)

    def write_log(
        self,
        name: str,
        events: list[dict],
        *,
        newline: str = "\n",
        final_newline: bool = True,
        truncate_last: bool = False,
    ) -> Path:
        lines = [json.dumps(event, ensure_ascii=False) for event in events]
        if truncate_last and lines:
            lines[-1] = lines[-1][: len(lines[-1]) // 2]
        text = newline.join(lines)
        if final_newline:
            text += newline
        path = self.log_dir / name
        path.write_text(text, encoding="utf-8", newline="")
        return path


class TestAdd(ObslogTestCase):
    def test_add_continues_seq_and_uses_default_file(self) -> None:
        self.write_log("2026-09-05-seed.jsonl", [envelope(1), envelope(2), envelope(5)])
        result = run_obslog(
            "add",
            "--type", "issue.status_changed",
            "--actor", "member:vera",
            "--ref", "issue:ISS-003",
            "--payload", '{"issue_id":"ISS-003","from":"open","to":"in-progress"}',
            "--root", str(self.root),
        )
        self.assertEqual(result.returncode, 0, result.stderr)

        today = datetime.now().astimezone().date().isoformat()
        target = self.log_dir / f"{today}-tools.jsonl"
        self.assertTrue(target.exists(), "default --file should be <today>-tools.jsonl")
        self.assertIn(f"appended seq=6", result.stdout)

        lines = target.read_text(encoding="utf-8").splitlines()
        self.assertEqual(len(lines), 1)
        event = json.loads(lines[0])
        self.assertEqual(event["seq"], 6)
        self.assertEqual(event["type"], "issue.status_changed")
        self.assertEqual(event["actor"], "member:vera")
        self.assertEqual(event["schema_v"], 1)
        self.assertEqual(event["refs"], ["issue:ISS-003"])
        ts = datetime.fromisoformat(event["ts"])
        self.assertIsNotNone(ts.tzinfo, "default ts must carry a UTC offset")

    def test_add_writes_to_custom_file_with_parent_dirs(self) -> None:
        custom = self.root / "somewhere" / "else" / "out.jsonl"
        result = run_obslog(
            "add",
            "--type", "run.checkpoint",
            "--actor", "owner",
            "--payload", '{"run_id":"RUN-0001","phase_id":"P2","snapshot_path":"x.json"}',
            "--run-id", "RUN-0001",
            "--phase-id", "P2",
            "--logical-seq", "1",
            "--process-mode", "pair",
            "--file", str(custom),
            "--root", str(self.root),
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        event = json.loads(custom.read_text(encoding="utf-8").splitlines()[0])
        self.assertEqual(event["seq"], 1)  # no fixture logs -> starts at 1
        self.assertEqual(event["run_id"], "RUN-0001")
        self.assertEqual(event["phase_id"], "P2")
        self.assertEqual(event["logical_seq"], 1)
        self.assertEqual(event["process_mode"], "pair")

    def test_add_rejects_invalid_payload(self) -> None:
        for bad_payload in ["not json", "[1, 2]", '"a string"']:
            with self.subTest(payload=bad_payload):
                result = run_obslog(
                    "add",
                    "--type", "issue.created",
                    "--actor", "owner",
                    "--payload", bad_payload,
                    "--root", str(self.root),
                )
                self.assertEqual(result.returncode, 1)
                self.assertIn("--payload", result.stderr)
                self.assertIn("nothing was written", result.stderr)
                self.assertEqual(list(self.log_dir.glob("*.jsonl")), [])

    def test_add_rejects_unknown_type(self) -> None:
        result = run_obslog(
            "add",
            "--type", "event.from.the.future",
            "--actor", "owner",
            "--payload", "{}",
            "--root", str(self.root),
        )
        self.assertEqual(result.returncode, 1)
        self.assertIn("--type", result.stderr)
        self.assertIn("event.from.the.future", result.stderr)
        self.assertEqual(list(self.log_dir.glob("*.jsonl")), [])

    def test_add_reports_all_problems_at_once(self) -> None:
        result = run_obslog(
            "add",
            "--type", "issue.created",
            "--actor", "Wizard",
            "--ref", "bogus-ref",
            "--payload", "{}",
            "--root", str(self.root),
        )
        self.assertEqual(result.returncode, 1)
        self.assertIn("--actor", result.stderr)
        self.assertIn("--ref", result.stderr)
        self.assertIn("2 problem(s)", result.stderr)


class TestCheck(ObslogTestCase):
    def test_check_accepts_clean_log(self) -> None:
        self.write_log("2026-09-05-x.jsonl", [envelope(1), envelope(2)])
        result = run_obslog("check", "--root", str(self.root))
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertNotIn("ERROR", result.stdout)
        self.assertIn("2 lines, 0 error(s), 0 warning(s)", result.stdout)
        self.assertIn("result: OK", result.stdout)

    def test_check_strips_crlf(self) -> None:
        # DEC-RUN0001-P1-002: Windows working-tree CRLF must not break parsing.
        self.write_log("2026-09-05-x.jsonl", [envelope(1), envelope(2)], newline="\r\n")
        result = run_obslog("check", "--root", str(self.root))
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertNotIn("ERROR", result.stdout)
        self.assertIn("2 lines, 0 error(s), 0 warning(s)", result.stdout)

    def test_check_tolerates_truncated_final_line(self) -> None:
        self.write_log(
            "2026-09-05-x.jsonl",
            [envelope(1), envelope(2)],
            truncate_last=True,
            final_newline=False,
        )
        result = run_obslog("check", "--root", str(self.root))
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn("truncated final line", result.stdout)
        self.assertIn("2 lines, 0 error(s), 1 warning(s)", result.stdout)

    def test_check_reports_broken_line_before_eof_as_error(self) -> None:
        path = self.log_dir / "2026-09-05-x.jsonl"
        # The broken line sits between two good lines, so it is not the
        # final line and must be an error, not a tolerated truncation.
        lines = [json.dumps(envelope(1)), "{broken json", json.dumps(envelope(2))]
        path.write_text("\n".join(lines) + "\n", encoding="utf-8", newline="")
        result = run_obslog("check", "--root", str(self.root))
        self.assertEqual(result.returncode, 1)
        self.assertIn("ERROR", result.stdout)
        self.assertIn("invalid JSON", result.stdout)

    def test_check_unknown_type_is_warning_not_error(self) -> None:
        # Dee's condition (RFC-0004 / ADR-0004): unknown types must never fail
        # the check; add-time writes are the ones that get rejected.
        self.write_log("2026-09-05-x.jsonl", [envelope(1, type="event.from.the.future")])
        result = run_obslog("check", "--root", str(self.root))
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertNotIn("ERROR", result.stdout)
        self.assertIn("WARN", result.stdout)
        self.assertIn("event.from.the.future", result.stdout)

    def test_check_detects_seq_regression(self) -> None:
        self.write_log("2026-09-05-x.jsonl", [envelope(5), envelope(4)])
        result = run_obslog("check", "--root", str(self.root))
        self.assertEqual(result.returncode, 1)
        self.assertIn("seq went backwards", result.stdout)

    def test_check_detects_duplicate_seq(self) -> None:
        self.write_log("2026-09-05-x.jsonl", [envelope(5), envelope(5)])
        result = run_obslog("check", "--root", str(self.root))
        self.assertEqual(result.returncode, 1)
        self.assertIn("duplicate seq 5", result.stdout)

    def test_check_warns_on_seq_gap(self) -> None:
        self.write_log("2026-09-05-x.jsonl", [envelope(1), envelope(3)])
        result = run_obslog("check", "--root", str(self.root))
        self.assertEqual(result.returncode, 0, result.stdout)
        self.assertIn("seq gap: expected 2, got 3", result.stdout)
        self.assertIn("1 warning(s)", result.stdout)

    def test_check_detects_logical_seq_regression_within_run(self) -> None:
        self.write_log(
            "2026-09-05-x.jsonl",
            [
                envelope(1, run_id="RUN-0001", logical_seq=2),
                envelope(2, run_id="RUN-0001", logical_seq=1),
            ],
        )
        result = run_obslog("check", "--root", str(self.root))
        self.assertEqual(result.returncode, 1)
        self.assertIn("logical_seq went backwards in RUN-0001", result.stdout)

    def test_check_rejects_wrong_schema_v(self) -> None:
        self.write_log("2026-09-05-x.jsonl", [envelope(1, schema_v=2)])
        result = run_obslog("check", "--root", str(self.root))
        self.assertEqual(result.returncode, 1)
        self.assertIn("schema_v", result.stdout)

    def test_check_missing_log_dir_is_friendly_error(self) -> None:
        tmp = tempfile.TemporaryDirectory()
        self.addCleanup(tmp.cleanup)
        result = run_obslog("check", "--root", tmp.name)
        self.assertEqual(result.returncode, 1)
        self.assertIn("log directory not found", result.stderr)
        self.assertIn("--root", result.stderr)


class TestRegistry(ObslogTestCase):
    def test_event_types_match_schema_file(self) -> None:
        schema_path = REPO_ROOT / "observatory" / "schema" / "event.schema.json"
        schema = json.loads(schema_path.read_text(encoding="utf-8"))
        self.assertEqual(
            set(obslog.EVENT_TYPES),
            set(schema["properties"]["type"]["enum"]),
            "obslog EVENT_TYPES and event.schema.json have drifted apart",
        )


class TestView(ObslogTestCase):
    def test_view_is_a_stub_with_exit_2(self) -> None:
        result = run_obslog("view")
        self.assertEqual(result.returncode, 2)
        self.assertIn("not implemented yet (ISS-011)", result.stdout)


if __name__ == "__main__":
    unittest.main()
