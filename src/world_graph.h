// WorldGraph v0 - data-structure part of ISS-005.
// API fixed by the cross-branch contract (scene/mutation compile against it).
//
// M0 invariant: rooms are append-only and never removed at runtime, so a
// RoomId stays valid for the whole process lifetime and is never reused.
// add_room() assigns ids 0, 1, 2, ... monotonically; next id == rooms_.size()
// holds only because of this invariant. When deletion semantics ever appear,
// revisit: id->index mapping, find_room() result lifetime, and every RoomId
// copy held by scene/mutation code.
//
// Borrow rules (not expressible in the signatures):
// - find_room() and rooms() are views into rooms_; any add_room() may
//   reallocate the vector and invalidate them. Copy the Room out if it must
//   survive a mutation (vector push_back invalidates all references on
//   reallocation).
// - connections() is invalidated by connect()/disconnect().
// - version() exists so build_scene can compare before/after and re-project
//   wholesale instead of holding references across mutations.
//
// Doorways are undirected and stored once, normalized as (min, max).
// connect(a, a) is a no-op: a self-doorway is meaningless at M0 and there is
// no error channel in this API to report one.
#pragma once

#include <string>
#include <vector>

using RoomId = int;

struct Room {
    RoomId id;
    std::string name;
    float x, z;   // min corner on floor plane
    float w, d;   // extents along x and z
};

struct Connection { RoomId a, b; };  // doorway exists iff connection present

class WorldGraph {
public:
    RoomId add_room(std::string name, float x, float z, float w, float d);
    void connect(RoomId a, RoomId b);       // no-op if already connected
    void disconnect(RoomId a, RoomId b);    // no-op if not connected
    bool connected(RoomId a, RoomId b) const;
    const Room* find_room(RoomId id) const; // nullptr if absent
    const std::vector<Room>& rooms() const;
    const std::vector<Connection>& connections() const;
    int version() const;                    // +1 on every topology mutation (add/connect/disconnect)
private:
    std::vector<Room> rooms_;
    std::vector<Connection> connections_;
    int version_ = 0;
};
