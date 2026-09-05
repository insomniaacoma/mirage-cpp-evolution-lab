// App shell (ISS-003): window + frame loop + clean exit path.
// --smoke: hidden window for the deterministic smoke driver that arrives with the world (ISS-006/007).
#include "raylib.h"

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

} // namespace

int main(int argc, char** argv) {
    const bool smoke = has_flag(argc, argv, "--smoke");

    SetTraceLogLevel(LOG_WARNING);
    if (smoke) SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(kWindowWidth, kWindowHeight, "MIRAGE//AFTERIMAGE - M0");
    // Investigation (REVIEW-0001 follow-up): hidden windows get DWM-throttled well
    // below 60fps, so smoke mode must not rely on wall-clock pacing. The real smoke
    // driver (ISS-006/007) will step the simulation at a fixed dt instead.
    if (!smoke) SetTargetFPS(60);

    // Review finding REVIEW-0001: a hidden window with no driver must not be an
    // infinite loop. The real smoke driver (ISS-006/007) replaces this guard.
    int smoke_frames_left = smoke ? 600 : -1;

    while (!WindowShouldClose()) {
        const float dt = clampf(GetFrameTime(), 0.0f, kMaxFrameDt);
        (void)dt; // first consumer is the ISS-004 controller

        BeginDrawing();
        ClearBackground(Color{10, 10, 14, 255});
        DrawText("MIRAGE M0 - app shell", 24, 24, 20, Color{190, 190, 200, 255});
        EndDrawing();

        if (smoke_frames_left > 0 && --smoke_frames_left == 0) {
            std::puts("SMOKE: shell guard timeout (no world driver yet)");
            break;
        }
    }

    CloseWindow();
    return 0;
}
