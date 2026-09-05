#include "world_graph.h"

#include <algorithm>
#include <utility>

namespace {

// Edges are stored normalized (a < b), so membership checks never need the
// swapped comparison. Meaningful only because connect()/disconnect() are the
// single insertion/removal paths.
const Connection* find_edge(const std::vector<Connection>& edges, Connection key) {
    for (const Connection& e : edges) {
        if (e.a == key.a && e.b == key.b) {
            return &e;
        }
    }
    return nullptr;
}

Connection normalized(RoomId a, RoomId b) {
    return a < b ? Connection{a, b} : Connection{b, a};
}

}  // namespace

RoomId WorldGraph::add_room(std::string name, float x, float z, float w, float d) {
    const RoomId id = static_cast<RoomId>(rooms_.size());
    rooms_.push_back(Room{id, std::move(name), x, z, w, d});
    ++version_;
    return id;
}

void WorldGraph::connect(RoomId a, RoomId b) {
    if (a == b) {
        return;
    }
    const Connection edge = normalized(a, b);
    if (find_edge(connections_, edge) != nullptr) {
        return;
    }
    connections_.push_back(edge);
    ++version_;
}

void WorldGraph::disconnect(RoomId a, RoomId b) {
    if (a == b) {
        return;
    }
    const Connection edge = normalized(a, b);
    const auto it = std::find_if(connections_.begin(), connections_.end(),
                                 [&edge](const Connection& e) {
                                     return e.a == edge.a && e.b == edge.b;
                                 });
    if (it == connections_.end()) {
        return;
    }
    connections_.erase(it);
    ++version_;
}

bool WorldGraph::connected(RoomId a, RoomId b) const {
    if (a == b) {
        return false;
    }
    return find_edge(connections_, normalized(a, b)) != nullptr;
}

const Room* WorldGraph::find_room(RoomId id) const {
    for (const Room& r : rooms_) {
        if (r.id == id) {
            return &r;
        }
    }
    return nullptr;
}

const std::vector<Room>& WorldGraph::rooms() const {
    return rooms_;
}

const std::vector<Connection>& WorldGraph::connections() const {
    return connections_;
}

int WorldGraph::version() const {
    return version_;
}
