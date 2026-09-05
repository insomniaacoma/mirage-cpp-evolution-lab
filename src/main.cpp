// App shell + world wiring (ISS-003 + ISS-005/scene part).
// --smoke: hidden window for the deterministic smoke driver that arrives with the world (ISS-006/007).
#include "raylib.h"
#include "scene.h"
#include "world_graph.h"

#include <cstdio>
#include <cstring>

namespace {

constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;

// Alt-tab / debugger stalls must not teleport the player once dt-scaled movement exists.
constexpr float kMaxFrameDt = 0.1f;

// raylib 5.5 removed Clamp()/Lerp() from its core API (they moved to raymath.h);
// a 3-line local helper beats pulling a header for one call.
float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

bool has_flag(int argc, char** argv, const char* flag) {
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], flag) == 0) return true;
    }
    return false;
}

// Demo world data (ISS-005, temporary coupling on purpose):
// A corridor - B classroom / C atrium - D storage, E exit hall sealed off.
// Topology lives here until a second consumer forces a data file (pain point P3).
WorldGraph build_demo_world() {
    WorldGraph g;
    const RoomId a = g.add_room("corridor", 0.0f, 0.0f, 8.0f, 8.0f);
    const RoomId b = g.add_room("classroom", 8.0f, 0.0f, 10.0f, 8.0f);
    const RoomId c = g.add_room("atrium", 0.0f, 8.0f, 8.0f, 10.0f);
    const RoomId d = g.add_room("storage", 8.0f, 8.0f, 10.0f, 10.0f);
    /* const RoomId e = */ g.add_room("exit_hall", 18.0f, 8.0f, 6.0f, 10.0f);
    g.connect(a, b);
    g.connect(a, c);
    g.connect(b, d);
    g.connect(c, d); // removed by the scripted mutation; D-E opens (ISS-006)
    return g;
}

Camera3D orbit_camera() {
    // Temporary inspection camera so the empty projection is visible before the
    // ISS-004 controller brings its own camera. Replaced, not extended.
    Camera3D cam{};
    cam.position = Vector3{18.0f, 22.0f, -14.0f};
    cam.target = Vector3{11.0f, 0.0f, 9.0f};
    cam.up = Vector3{0.0f, 1.0f, 0.0f};
    cam.fovy = 55.0f;
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

} // namespace

int main(int argc, char** argv) {
    const bool smoke = has_flag(argc, argv, "--smoke");

    SetTraceLogLevel(LOG_WARNING);
    if (smoke) SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(kWindowWidth, kWindowHeight, "MIRAGE//AFTERIMAGE - M0");
    // Review finding REVIEW-0001: hidden windows get DWM-throttled well below
    // 60fps, so smoke mode must not rely on wall-clock pacing. The real smoke
    // driver (ISS-006/007) will step the simulation at a fixed dt instead.
    if (!smoke) SetTargetFPS(60);

    WorldGraph world = build_demo_world();
    Scene scene = build_scene(world);
    Camera3D camera = orbit_camera();
    // Scene cache: rebuild only when the topology actually changes (Rin's review
    // condition on ISS-003/scene; the naive path rebuilt every frame).
    int built_version = world.version();

    // Review finding REVIEW-0001: a hidden window with no driver must not be an
    // infinite loop. The real smoke driver (ISS-006/007) replaces this guard.
    int smoke_frames_left = smoke ? 600 : -1;

    while (!WindowShouldClose()) {
        const float dt = clampf(GetFrameTime(), 0.0f, kMaxFrameDt);
        (void)dt; // first consumer is the ISS-004 controller

        if (world.version() != built_version) {
            scene = build_scene(world);
            built_version = world.version();
        }

        BeginDrawing();
        ClearBackground(Color{10, 10, 14, 255});
        BeginMode3D(camera);
        draw_scene(scene);
        DrawGrid(24, 1.0f);
        EndMode3D();
        DrawText("MIRAGE M0 - world projection (orbit cam, controller pending)", 24, 24, 20,
                 Color{190, 190, 200, 255});
        EndDrawing();

        if (smoke_frames_left > 0 && --smoke_frames_left == 0) {
            std::puts("SMOKE: shell guard timeout (no world driver yet)");
            break;
        }
    }

    CloseWindow();
    return 0;
}
