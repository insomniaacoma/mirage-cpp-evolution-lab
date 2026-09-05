// Exit gating (ISS-007). The win condition is deliberately two-factor:
// topology (mutation applied) AND position (inside the exit room). While the
// demo world seals E behind the mutation the topology half is redundant, but
// it keeps the gate honest if world data ever changes under this check.
#pragma once
#include "player.h"
#include "world_graph.h"

bool exit_reached(const WorldGraph& graph, const Player& player, RoomId exit_room,
                  bool mutation_applied);
