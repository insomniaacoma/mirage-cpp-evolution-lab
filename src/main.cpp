// App shell + game wiring (ISS-003/004/006/007).
// Real mode: first-person controller, scripted mutation with cue, exit gate.
// --smoke: hidden window, fixed dt, scripted InputFrame driver
// (DEC-RUN0001-P1-003). Never GetFrameTime/SetTargetFPS there: hidden windows
// get DWM-throttled (REVIEW-0001), so smoke pacing is frame-count based.
#include "raylib.h"
#include "exit.h"
#include "mutation.h"
#include "player.h"
#include "scene.h"
#include "world_graph.h"

#include <cmath>
#include <cstdio>
#include <cstring>

namespace {

constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;

// Alt-tab / debugger stalls must not teleport the player.
constexpr float kMaxFrameDt = 0.1f;

// A single frame's mouse delta above this is a focus-change spike, not a look
// intent; the feel twin of kMaxFrameDt.
constexpr float kMaxLookDelta = 200.0f;

constexpr float kFovyDeg = 70.0f;

// Smoke driver tuning: the script's synthetic "hand".
constexpr float kScriptTurnRate = 4.0f; // rad/s cap on scripted turning
constexpr float kArriveDist = 0.3f;     // waypoint arrival radius
constexpr int kSmokeFrameCap = 3600;    // 60 simulated seconds, then FAIL

float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

bool has_flag(int argc, char** argv, const char* flag) {
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], flag) == 0) return true;
    }
    return false;
}

// Demo world data (ISS-005, temporary coupling on purpose): topology lives
// here until a second consumer forces a data file (pain point P3). Room ids
// ride along so the mutation/exit wiring cannot drift from the add order.
struct DemoWorld {
    WorldGraph graph;
    RoomId corridor = -1;
    RoomId classroom = -1;
    RoomId atrium = -1;
    RoomId storage = -1;
    RoomId exit_hall = -1;
};

DemoWorld build_demo_world() {
    DemoWorld w;
    w.corridor = w.graph.add_room("corridor", 0.0f, 0.0f, 8.0f, 8.0f);
    w.classroom = w.graph.add_room("classroom", 8.0f, 0.0f, 10.0f, 8.0f);
    w.atrium = w.graph.add_room("atrium", 0.0f, 8.0f, 8.0f, 10.0f);
    w.storage = w.graph.add_room("storage", 8.0f, 8.0f, 10.0f, 10.0f);
    w.exit_hall = w.graph.add_room("exit_hall", 18.0f, 8.0f, 6.0f, 10.0f);
    w.graph.connect(w.corridor, w.classroom);
    w.graph.connect(w.corridor, w.atrium);
    w.graph.connect(w.classroom, w.storage);
    w.graph.connect(w.atrium, w.storage); // sealed by the mutation; storage-exit_hall opens (ISS-006)
    return w;
}

// Camera composition lives here, not in player.cpp: Camera3D is a raylib type
// and ADR-0002 keeps the raylib call surface in main.cpp/scene.cpp. The eye
// height is body data and comes from player.h.
Camera3D player_camera(const Player& p) {
    Camera3D cam{};
    cam.position = Vector3{p.x, kEyeHeight, p.z};
    const float cp = std::cosf(p.pitch);
    const Vector3 dir{std::sinf(p.yaw) * cp, std::sinf(p.pitch), -std::cosf(p.yaw) * cp};
    cam.target = Vector3{cam.position.x + dir.x, cam.position.y + dir.y, cam.position.z + dir.z};
    cam.up = Vector3{0.0f, 1.0f, 0.0f};
    cam.fovy = kFovyDeg;
    cam.projection = CAMERA_PERSPECTIVE;
    return cam;
}

void draw_scene(const Scene& scene) {
    for (const SceneBox& b : scene.solids) {
        const Vector3 c{0.5f * (b.bounds.min.x + b.bounds.max.x),
                        0.5f * (b.bounds.min.y + b.bounds.max.y),
                        0.5f * (b.bounds.min.z + b.bounds.max.z)};
        const Vector3 s{b.bounds.max.x - b.bounds.min.x,
                        b.bounds.max.y - b.bounds.min.y,
                        b.bounds.max.z - b.bounds.min.z};
        DrawCube(c, s.x, s.y, s.z, b.color);
    }
    for (const SceneBox& b : scene.decor) {
        const Vector3 c{0.5f * (b.bounds.min.x + b.bounds.max.x),
                        0.5f * (b.bounds.min.y + b.bounds.max.y),
                        0.5f * (b.bounds.min.z + b.bounds.max.z)};
        const Vector3 s{b.bounds.max.x - b.bounds.min.x,
                        b.bounds.max.y - b.bounds.min.y,
                        b.bounds.max.z - b.bounds.min.z};
        DrawCube(c, s.x, s.y, s.z, b.color);
    }
}

InputFrame collect_human_input() {
    InputFrame in;
    in.move_forward = (IsKeyDown(KEY_W) ? 1.0f : 0.0f) - (IsKeyDown(KEY_S) ? 1.0f : 0.0f);
    in.move_strafe = (IsKeyDown(KEY_D) ? 1.0f : 0.0f) - (IsKeyDown(KEY_A) ? 1.0f : 0.0f);
    const Vector2 md = GetMouseDelta();
    in.look_dx = clampf(md.x, -kMaxLookDelta, kMaxLookDelta);
    in.look_dy = clampf(md.y, -kMaxLookDelta, kMaxLookDelta);
    return in;
}

// --- smoke driver (DEC-RUN0001-P1-003) --------------------------------------
// A synthetic player: fills the same InputFrame as the human path, so the
// whole walk goes through Player::update with collision on. Waypoints are
// room centers and sit on door-center sightlines (grid-aligned demo world),
// so the script walks real corridors; nothing writes Player state directly.

InputFrame seek_input(const Player& p, float tx, float tz, float dt) {
    InputFrame in;
    const float desired = std::atan2f(tx - p.x, -(tz - p.z)); // player yaw convention
    const float err = std::remainderf(desired - p.yaw, 6.2831853f); // wrap to [-pi, pi]
    const float max_turn = kScriptTurnRate * dt;
    in.look_dx = clampf(err, -max_turn, max_turn) / kMouseSensitivity; // the script's mouse
    in.move_forward = clampf(std::cosf(err), 0.0f, 1.0f); // slow down when misaligned
    return in;
}

// Script-local copy of the point-in-room predicate, deliberately independent
// of the file-local ones in mutation.cpp/exit.cpp: if any copy drifts, the
// cross-checks below fail the smoke instead of hiding it.
bool script_in_room(const WorldGraph& graph, RoomId id, float x, float z) {
    for (const Room& r : graph.rooms()) {
        if (r.id == id) {
            return x >= r.x && x < r.x + r.w && z >= r.z && z < r.z + r.d;
        }
    }
    return false;
}

bool arrived(const Player& p, float tx, float tz) {
    const float dx = tx - p.x;
    const float dz = tz - p.z;
    return dx * dx + dz * dz < kArriveDist * kArriveDist;
}

enum SmokePhase {
    SP_SEEK_B1,  // spawn (corridor center) -> classroom center
    SP_DWELL1,   // stand in B, 1.5s: first chunk of the cumulative-dwell proof
    SP_SEEK_A,   // full excursion out of B (a continuous-stay rule would reset here)
    SP_SEEK_B2,  // back into B
    SP_DWELL2,   // stand; the mutation must fire on accumulated time
    SP_SETTLE,   // 1s more in B: assert the mutation never fires twice
    SP_SEEK_D,   // classroom -> storage through the B-D door
    SP_SEEK_E,   // storage -> exit hall through the NEW D-E door
    SP_PASS_HOLD // keep walking 0.5s inside E, then PASS
};

int run_smoke(WorldGraph& world, const DemoWorld& ids, Scene& scene, Player& player,
              Mutation& mut, int& built_version) {
    // Borrow rule from world_graph.h: find_room() views die on add_room(); no
    // add_room happens at runtime, but the header asks for copies across
    // mutations, and copying four rooms is free.
    const Room ra = *world.find_room(ids.corridor);
    const Room rb = *world.find_room(ids.classroom);
    const Room rd = *world.find_room(ids.storage);
    const Room re = *world.find_room(ids.exit_hall);
    const float ax = ra.x + 0.5f * ra.w, az = ra.z + 0.5f * ra.d;
    const float bx = rb.x + 0.5f * rb.w, bz = rb.z + 0.5f * rb.d;
    const float dxr = rd.x + 0.5f * rd.w, dzr = rd.z + 0.5f * rd.d;
    const float ex = re.x + 0.5f * re.w, ez = re.z + 0.5f * re.d;

    constexpr float dt = 1.0f / 60.0f; // fixed step; never GetFrameTime in smoke
    SmokePhase phase = SP_SEEK_B1;
    bool failed = false;
    const char* fail_reason = "";
    auto fail = [&](const char* why) {
        if (!failed) {
            failed = true;
            fail_reason = why;
        }
    };

    int dwell1_left = 90;  // 1.5s standing in B
    int settle_left = 60;  // 1.0s standing in B after the fire
    int dwell2_frames = 0;
    int stay_frames = 0;   // consecutive frames with the player inside B
    int fired_count = 0;
    int exit_frame = -1;
    int pass_hold = 30;    // 0.5s of walking inside E before PASS

    for (int frame = 0; frame < kSmokeFrameCap && !failed; ++frame) {
        PollInputEvents(); // keep the hidden window's pump alive without drawing

        // 1) input: the same InputFrame the human path fills.
        InputFrame in;
        switch (phase) {
            case SP_SEEK_B1: in = seek_input(player, bx, bz, dt); break;
            case SP_SEEK_A: in = seek_input(player, ax, az, dt); break;
            case SP_SEEK_B2: in = seek_input(player, bx, bz, dt); break;
            case SP_SEEK_D: in = seek_input(player, dxr, dzr, dt); break;
            case SP_SEEK_E: in = seek_input(player, ex, ez, dt); break;
            default: break; // dwell/settle phases stand still (zeroed InputFrame)
        }

        // 2) simulate: the only place player state changes.
        player.update(in, dt, scene.solids);

        // 3) script-side bookkeeping.
        const bool in_b = script_in_room(world, ids.classroom, player.x, player.z);
        stay_frames = in_b ? stay_frames + 1 : 0;
        if (!mut.applied() && script_in_room(world, ids.exit_hall, player.x, player.z)) {
            fail("player reached the exit room before the mutation (seal broken?)");
        }

        // 4) mutation (applies the graph edit itself).
        const int version_before = world.version();
        const MutationResult mr = mut.update(world, player, dt);
        if (mr.fired) {
            if (fired_count != 0) fail("mutation fired more than once");
            // Cumulative-vs-continuous discriminator: continuous dwell needs
            // 5.0s of UNBROKEN stay (300 frames) to fire; cumulative fires on
            // accumulated time, so the current stay must be well under that.
            if (static_cast<float>(stay_frames) * dt >= 4.0f) {
                fail("mutation fired on a >=4s continuous stay: dwell semantics regressed");
            }
            const int version_after = world.version();
            if (version_after - version_before != 2) {
                fail("graph version moved by other than 2 on the mutation");
            }
            if (exit_reached(world, player, ids.exit_hall, mut.applied())) {
                fail("exit gate true while the player stands in the trigger room");
            }
            if (!failed) {
                ++fired_count;
                std::printf("SMOKE: mutation applied at frame %d (graph version %d -> %d)\n",
                            frame, version_before, version_after);
            }
        }

        // 5) scene rebuild on topology change (same guard as the human loop).
        if (world.version() != built_version) {
            scene = build_scene(world);
            built_version = world.version();
        }

        // 6) collision sanity: the scripted walk must never end a frame inside a wall.
        if (player_penetrates(scene.solids, player.x, player.z, kPlayerRadius)) {
            fail("player ended a frame inside a solid");
        }

        // 7) exit gate.
        const bool gate = exit_reached(world, player, ids.exit_hall, mut.applied());
        if (gate && exit_frame < 0) {
            if (mut.applied() && !script_in_room(world, ids.exit_hall, player.x, player.z)) {
                fail("exit gate true while the script says the player is not in the exit room");
            }
            if (!failed) {
                exit_frame = frame;
                std::printf("SMOKE: exit reached at frame %d\n", frame);
            }
        }

        // 8) phase transitions.
        switch (phase) {
            case SP_SEEK_B1:
                if (arrived(player, bx, bz)) phase = SP_DWELL1;
                break;
            case SP_DWELL1:
                if (--dwell1_left <= 0) {
                    if (mut.applied()) fail("mutation fired before the excursion (budget drift)");
                    phase = SP_SEEK_A;
                }
                break;
            case SP_SEEK_A:
                if (arrived(player, ax, az)) phase = SP_SEEK_B2;
                break;
            case SP_SEEK_B2:
                if (arrived(player, bx, bz)) phase = SP_DWELL2;
                break;
            case SP_DWELL2:
                ++dwell2_frames;
                if (mut.applied()) {
                    phase = SP_SETTLE;
                } else if (dwell2_frames > 600) {
                    fail("mutation never fired during the second stay");
                }
                break;
            case SP_SETTLE:
                if (--settle_left <= 0) phase = SP_SEEK_D;
                break;
            case SP_SEEK_D:
                if (arrived(player, dxr, dzr)) phase = SP_SEEK_E;
                break;
            case SP_SEEK_E:
                if (exit_frame >= 0 && --pass_hold <= 0) {
                    if (!gate) fail("exit gate dropped after being reached");
                    if (!failed) {
                        std::puts("SMOKE: PASS");
                        return 0;
                    }
                }
                break;
            default: break;
        }
    }

    if (failed) {
        std::printf("SMOKE: FAIL %s\n", fail_reason);
    } else {
        std::printf("SMOKE: FAIL frame cap %d reached with no PASS\n", kSmokeFrameCap);
    }
    return 1;
}

} // namespace

int main(int argc, char** argv) {
    const bool smoke = has_flag(argc, argv, "--smoke");

    SetTraceLogLevel(LOG_WARNING);
    if (smoke) SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(kWindowWidth, kWindowHeight, "MIRAGE//AFTERIMAGE - M0");
    if (!smoke) {
        DisableCursor(); // mouse capture; released by CloseWindow (ESC / close). M0 has no pause state.
        SetTargetFPS(60);
    }

    DemoWorld demo = build_demo_world();
    Scene scene = build_scene(demo.graph);
    int built_version = demo.graph.version();

    // Spawn at the corridor center facing +X (the direction the level opens).
    // Borrow ok: no add_room() happens after build_demo_world().
    const Room spawn = *demo.graph.find_room(demo.corridor);
    Player player{spawn.x + 0.5f * spawn.w, spawn.z + 0.5f * spawn.d, 1.5707964f, 0.0f};

    Mutation mut{MutationSpec{.trigger_room = demo.classroom,
                                    .seal_a = demo.atrium,
                                    .seal_b = demo.storage,
                                    .open_a = demo.storage,
                                    .open_b = demo.exit_hall,
                                    .dwell_seconds = 5.0f}};
    const RoomId exit_room = demo.exit_hall;
    bool won = false;

    if (smoke) {
        const int code = run_smoke(demo.graph, demo, scene, player, mut, built_version);
        if (code == 0) {
            // QA probe (Dee, REVIEW-0005 follow-up): one drawn frame at the smoke
            // end state, for eyeball geometry checks that a text log cannot give.
            BeginDrawing();
            ClearBackground(Color{10, 10, 14, 255});
            BeginMode3D(player_camera(player));
            draw_scene(scene);
            EndMode3D();
            DrawText("SMOKE final state", 24, 24, 16, WHITE);
            EndDrawing();
            TakeScreenshot("smoke_final.png");
        }
        CloseWindow();
        return code;
    }

    while (!WindowShouldClose()) { // ESC / window close; releases the mouse via CloseWindow
        const float dt = clampf(GetFrameTime(), 0.0f, kMaxFrameDt);

        const InputFrame input = collect_human_input();
        player.update(input, dt, scene.solids);

        const MutationResult mr = mut.update(demo.graph, player, dt);
        (void)mr; // cue() is read at draw time

        if (demo.graph.version() != built_version) {
            scene = build_scene(demo.graph);
            built_version = demo.graph.version();
        }

        if (!won) won = exit_reached(demo.graph, player, exit_room, mut.applied());

        BeginDrawing();
        ClearBackground(Color{10, 10, 14, 255});
        BeginMode3D(player_camera(player));
        draw_scene(scene);
        DrawGrid(24, 1.0f);
        EndMode3D();

        const MutationCue cue = mut.cue();
        if (cue.flash > 0.0f) {
            const unsigned char a = static_cast<unsigned char>(190.0f * (cue.flash / kFlashTotal));
            DrawRectangle(0, 0, kWindowWidth, kWindowHeight, Color{214, 205, 255, a});
        }
        if (cue.caption > 0.0f) {
            const unsigned char a = static_cast<unsigned char>(230.0f * (cue.caption / kCaptionTotal));
            DrawText("the layout disagrees with your memory", 24, kWindowHeight - 64, 24,
                     Color{214, 205, 255, a});
        }

        if (won) {
            const char* line = "LAYER 01 - CLEARED";
            const int w = MeasureText(line, 48);
            DrawText(line, (kWindowWidth - w) / 2, kWindowHeight / 2 - 48, 48,
                     Color{235, 231, 246, 255});
            const char* sub = "you can keep walking - ESC quits";
            const int sw = MeasureText(sub, 20);
            DrawText(sub, (kWindowWidth - sw) / 2, kWindowHeight / 2 + 16, 20,
                     Color{158, 156, 172, 255});
        } else {
            DrawText("WASD move - mouse look - ESC quit", 24, 24, 16, Color{122, 122, 138, 255});
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
