#include "mutation.h"

#include <cmath>

namespace {

// Point-in-room on the floor plane, half-open rect [x, x+w) x [z, z+d) so a
// point on a shared edge lands in exactly one room. First match wins; demo
// rooms do not overlap. Kept file-local on purpose: M0 does not pre-build a
// geometry utility library for one consumer (exit.cpp keeps its own copy).
RoomId room_at(const WorldGraph& graph, float x, float z) {
    for (const Room& r : graph.rooms()) {
        if (x >= r.x && x < r.x + r.w && z >= r.z && z < r.z + r.d) {
            return r.id;
        }
    }
    return -1;
}

} // namespace

MutationResult Mutation::update(WorldGraph& graph, const Player& player, float dt) {
    bool fired = false;

    // Cue timers tick every frame once set; main renders from cue().
    if (cue_.flash > 0.0f) cue_.flash -= dt;
    if (cue_.caption > 0.0f) cue_.caption -= dt;
    if (cue_.flash < 0.0f) cue_.flash = 0.0f;
    if (cue_.caption < 0.0f) cue_.caption = 0.0f;

    if (!applied_) {
        // Chosen semantics: CUMULATIVE dwell. Time inside the trigger room
        // adds up across visits; leaving does not reset the counter. The
        // mutation should read as "this place has been wrong for a while",
        // and a player who dips in and out still earns it. The smoke driver
        // covers exactly this: it fires after a full excursion out of the
        // room, which a continuous-stay rule would never allow.
        if (room_at(graph, player.x, player.z) == spec_.trigger_room) {
            dwell_ += dt;
        }
        if (dwell_ >= spec_.dwell_seconds) {
            graph.disconnect(spec_.seal_a, spec_.seal_b);
            graph.connect(spec_.open_a, spec_.open_b);
            applied_ = true;
            cue_.flash = kFlashTotal;
            cue_.caption = kCaptionTotal;
            fired = true;
        }
    }

    return MutationResult{fired, cue_};
}
