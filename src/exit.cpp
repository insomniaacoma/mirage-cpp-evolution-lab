#include "exit.h"

namespace {

// File-local copy of the point-in-room predicate (mutation.cpp keeps the
// production one local too; M0 does not pre-build a geometry utility library
// for two 6-line consumers). If the two copies ever drift, the smoke driver's
// cross-check of exit_reached() against its own geometry fails loudly.
bool in_room(const WorldGraph& graph, RoomId id, float x, float z) {
    for (const Room& r : graph.rooms()) {
        if (r.id == id) {
            return x >= r.x && x < r.x + r.w && z >= r.z && z < r.z + r.d;
        }
    }
    return false;
}

} // namespace

bool exit_reached(const WorldGraph& graph, const Player& player, RoomId exit_room,
                  bool mutation_applied) {
    return mutation_applied && in_room(graph, exit_room, player.x, player.z);
}
