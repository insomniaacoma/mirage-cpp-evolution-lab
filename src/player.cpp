#include "player.h"

#include <cmath>

namespace {

constexpr float kPitchLimitRad = kPitchLimitDeg * 3.14159265358979f / 180.0f;

// Rest-position skin. push_out() parks the player at exactly face - r, but
// float rounding can land that a hair inside the face, so the strict
// closest-point test would report ~1e-7 of residual penetration forever and
// re-fire the resolver every frame. Resting a skin's width off the face makes
// the resolver converge and every predicate agree.
constexpr float kCollisionSkin = 1e-4f;

float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// Vertical gate: a solid only blocks if its y-span overlaps the body [0,
// kBodyHeight]. Without this the lintels above every doorway (y >= 2.6) would
// seal the whole map to a floor-walking player.
bool spans_body(const SceneBox& b) {
    return b.bounds.min.y < kBodyHeight && b.bounds.max.y > 0.0f;
}

// 2D circle (px, pz, r) vs the box footprint, closest-point test.
bool circle_hits(const SceneBox& b, float px, float pz, float r) {
    const float cx = clampf(px, b.bounds.min.x, b.bounds.max.x);
    const float cz = clampf(pz, b.bounds.min.z, b.bounds.max.z);
    const float dx = px - cx;
    const float dz = pz - cz;
    return dx * dx + dz * dz < r * r;
}

// Push `p` out of [lo, hi] along the axis being resolved, to the NEARER face.
// Works with zero velocity (a mutation can solidify a doorway the player
// stands in) and never moves the center across the box, so a squeezed player
// pops out on the side they are already closest to instead of clipping
// through. Ties resolve low, deterministically.
float push_out(float p, float lo, float hi, float r) {
    const float out_lo = lo - r - kCollisionSkin;
    const float out_hi = hi + r + kCollisionSkin;
    return (std::fabs(p - out_lo) <= std::fabs(out_hi - p)) ? out_lo : out_hi;
}

} // namespace

void Player::update(const InputFrame& input, float dt, const std::vector<SceneBox>& solids) {
    // Look: raw deltas scaled once here. Mouse down (dy > 0) must look down,
    // hence the minus. Mouse up gives dy < 0, pitch grows, view goes up.
    yaw += input.look_dx * kMouseSensitivity;
    pitch -= input.look_dy * kMouseSensitivity;
    pitch = clampf(pitch, -kPitchLimitRad, kPitchLimitRad);

    // Move: yaw-relative. forward = (sin yaw, -cos yaw), right = (cos yaw,
    // sin yaw). Normalize when both axes are driven so keyboard diagonals are
    // not 41% faster than straight lines.
    const float mf = clampf(input.move_forward, -1.0f, 1.0f);
    const float ms = clampf(input.move_strafe, -1.0f, 1.0f);
    float vx = std::sinf(yaw) * mf + std::cosf(yaw) * ms;
    float vz = -std::cosf(yaw) * mf + std::sinf(yaw) * ms;
    const float len = std::sqrtf(vx * vx + vz * vz);
    if (len > 1.0f) {
        vx /= len;
        vz /= len;
    }
    vx *= kWalkSpeed * dt;
    vz *= kWalkSpeed * dt;

    // Slide: resolve x fully, then z. Per-axis resolution is what keeps
    // wall-sliding from sticking on corners. The resolve runs even with zero
    // velocity: the frame a mutation seals a doorway, the player standing in
    // it must be pushed out on the next update, not walked through.
    x += vx;
    for (const SceneBox& b : solids) {
        if (!spans_body(b)) continue;
        if (circle_hits(b, x, z, kPlayerRadius)) {
            x = push_out(x, b.bounds.min.x, b.bounds.max.x, kPlayerRadius);
        }
    }
    z += vz;
    for (const SceneBox& b : solids) {
        if (!spans_body(b)) continue;
        if (circle_hits(b, x, z, kPlayerRadius)) {
            z = push_out(z, b.bounds.min.z, b.bounds.max.z, kPlayerRadius);
        }
    }
    // Floor y = 0 hardcoded (M0): no vertical motion exists yet.
}

bool player_penetrates(const std::vector<SceneBox>& solids, float x, float z, float radius) {
    for (const SceneBox& b : solids) {
        if (!spans_body(b)) continue;
        if (circle_hits(b, x, z, radius)) return true;
    }
    return false;
}
