// First-person controller v0 (ISS-004).
// Input acquisition (raylib) stays in main.cpp; this module only turns an
// InputFrame into motion. Scripted input (DEC-RUN0001-P1-003) fills the same
// struct, so smoke and human share one update path by construction.
//
// ADR-0002 note: this file makes no raylib calls. It includes scene.h for the
// SceneBox collision set (data only); the camera built from this state lives
// in main.cpp because Camera3D is a raylib type.
#pragma once
#include "scene.h"

#include <vector>

// The one input unit of DEC-003: human input (keyboard + GetMouseDelta) and
// script input both fill it; nothing else knows where a frame came from.
struct InputFrame {
    float move_forward = 0.0f; // +1 forward / -1 back (W/S); clamped to [-1,1] in update
    float move_strafe  = 0.0f; // +1 right / -1 left (D/A)
    float look_dx      = 0.0f; // per-frame look delta, pixels for a mouse source
    float look_dy      = 0.0f;
};

// Feel constants (ISS-004 wants them named; hardcoded is legal at M0).
inline constexpr float kEyeHeight        = 1.7f;    // floor to camera; body data, camera math is main's
inline constexpr float kWalkSpeed        = 4.2f;    // u/s at full input
inline constexpr float kMouseSensitivity = 0.0025f; // rad per pixel of look delta
inline constexpr float kPlayerRadius     = 0.35f;   // circle footprint on the floor plane
inline constexpr float kBodyHeight       = 1.8f;    // floor to head top; scene lintels start at 2.6
inline constexpr float kPitchLimitDeg    = 89.0f;

struct Player {
    float x = 0.0f, z = 0.0f; // position on the floor plane (y = 0 hardcoded at M0)
    float yaw   = 0.0f;       // rad; 0 faces -Z, positive turns right (toward +X)
    float pitch = 0.0f;       // rad; positive looks up, clamped to +/- kPitchLimitDeg

    // Moves and resolves collision against `solids` (all treated as solid).
    // dt-scaled and frame-rate independent; safe for any dt <= 0.1.
    void update(const InputFrame& input, float dt, const std::vector<SceneBox>& solids);
};

// Verification aid for the smoke driver, not gameplay: true if the circle at
// (x, z) with `radius` penetrates any solid whose vertical span overlaps the
// player body. Mirrors the rules update() resolves with; if they drift apart
// the smoke driver is the thing that complains.
bool player_penetrates(const std::vector<SceneBox>& solids, float x, float z, float radius);
