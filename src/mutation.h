// Scripted world mutation (ISS-006). One deterministic trigger, one graph
// edit, one perceptible cue. The applied_ latch makes "happens exactly once"
// a property of the object, not of the caller's discipline.
//
// The room ids and the dwell budget are wiring data owned by main.cpp (which
// also owns build_demo_world); this module only knows the RULE:
// "dwell in the trigger room long enough, then seal one door and open another".
#pragma once
#include "player.h"
#include "world_graph.h"

struct MutationSpec {
    RoomId trigger_room;  // dwell here to arm the mutation (B, "classroom")
    RoomId seal_a, seal_b; // connection removed (C-D: the way you came is gone)
    RoomId open_a, open_b; // connection added (D-E: the way out appears)
    float dwell_seconds;   // cumulative, not continuous (see update())
};

// Cue design constants; main renders from the countdowns in MutationCue.
inline constexpr float kFlashTotal   = 0.6f; // fullscreen translucent flash
inline constexpr float kCaptionTotal = 3.0f; // fading caption

struct MutationCue {
    float flash   = 0.0f; // seconds left of the flash (0 = off)
    float caption = 0.0f; // seconds left of the caption
};

struct MutationResult {
    bool fired = false;   // true on the single frame the graph edit happens
    MutationCue cue;      // current countdowns (main renders from these)
};

class Mutation {
public:
    explicit Mutation(const MutationSpec& spec) : spec_(spec) {}

    // Call once per frame, AFTER Player::update. Applies the graph edit itself
    // through the ISS-005 mutation entry points (never bypassing the graph);
    // the caller rebuilds the scene when WorldGraph::version() changes.
    MutationResult update(WorldGraph& graph, const Player& player, float dt);

    bool applied() const { return applied_; }
    MutationCue cue() const { return cue_; }

private:
    MutationSpec spec_;
    bool applied_ = false;
    float dwell_ = 0.0f; // seconds accumulated inside the trigger room
    MutationCue cue_;
};
