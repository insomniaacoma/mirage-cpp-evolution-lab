#include "scene.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace {

constexpr float kWallHeight = 3.2f;
constexpr float kWallThick = 0.3f;
constexpr float kDoorWidth = 2.2f;
constexpr float kDoorHeight = 2.6f;
constexpr float kEps = 0.01f;

struct Doorway {
    float min; // interval along the shared edge
    float max;
};

// One edge of a room rect. along_z=true: the wall runs along z on an x=coord line
// (west/east edges); along_z=false: it runs along x on a z=coord line (north/south).
struct Edge {
    bool along_z;
    float coord;
    float lo, hi;
};

struct SharedEdge {
    bool valid = false;
    bool along_z = false; // true: edge is a z=const line (rooms side by side on x)
    float coord = 0;      // the constant coordinate of the shared edge
    float lo = 0, hi = 0; // overlap range along the edge
};

SharedEdge shared_edge(const Room& a, const Room& b) {
    SharedEdge e;
    const auto overlap = [](float a0, float a1, float b0, float b1, float& lo, float& hi) {
        lo = (a0 > b0) ? a0 : b0;
        hi = (a1 < b1) ? a1 : b1;
        return hi - lo > kDoorWidth; // too small to fit a door is as good as no edge
    };
    if (std::fabs((a.x + a.w) - b.x) < kEps && overlap(a.z, a.z + a.d, b.z, b.z + b.d, e.lo, e.hi)) {
        e.valid = true; e.along_z = true; e.coord = b.x;
    } else if (std::fabs((b.x + b.w) - a.x) < kEps && overlap(a.z, a.z + a.d, b.z, b.z + b.d, e.lo, e.hi)) {
        e.valid = true; e.along_z = true; e.coord = a.x;
    } else if (std::fabs((a.z + a.d) - b.z) < kEps && overlap(a.x, a.x + a.w, b.x, b.x + b.w, e.lo, e.hi)) {
        e.valid = true; e.along_z = false; e.coord = b.z;
    } else if (std::fabs((b.z + b.d) - a.z) < kEps && overlap(a.x, a.x + a.w, b.x, b.x + b.w, e.lo, e.hi)) {
        e.valid = true; e.along_z = false; e.coord = a.z;
    }
    return e;
}

// One wall of one room: axis-aligned, inset inside the room so rooms sharing a
// boundary produce back-to-back slabs instead of z-fighting geometry.
void emit_wall(std::vector<SceneBox>& solids, const Room& r,
               bool along_z, float coord, float lo, float hi,
               const std::vector<Doorway>& doors, Color color) {
    // Segments = [lo, hi] minus door intervals (sorted, disjoint by construction).
    std::vector<Doorway> sorted(doors);
    std::sort(sorted.begin(), sorted.end(),
              [](const Doorway& a, const Doorway& b) { return a.min < b.min; });
    auto emit_segment = [&](float s0, float s1, float y0, float y1) {
        if (s1 - s0 <= kEps || y1 - y0 <= kEps) return;
        SceneBox box{};
        if (along_z) {
            box.bounds = BoundingBox{
                Vector3{coord, y0, s0},
                Vector3{coord + kWallThick, y1, s1}};
        } else {
            box.bounds = BoundingBox{
                Vector3{s0, y0, coord},
                Vector3{s1, y1, coord + kWallThick}};
        }
        box.color = color;
        solids.push_back(box);
    };

    float cursor = lo;
    for (const Doorway& d : sorted) {
        emit_segment(cursor, d.min, 0.0f, kWallHeight); // solid wall up to the door
        emit_segment(d.min, d.max, kDoorHeight, kWallHeight); // lintel above the door
        cursor = d.max;
    }
    emit_segment(cursor, hi, 0.0f, kWallHeight);
}

} // namespace

Scene build_scene(const WorldGraph& graph) {
    Scene scene;

    const auto& rooms = graph.rooms();
    const auto& conns = graph.connections();

    // Floors (decor, non-solid).
    for (const Room& r : rooms) {
        SceneBox floor{};
        floor.bounds = BoundingBox{
            Vector3{r.x, -0.15f, r.z},
            Vector3{r.x + r.w, 0.0f, r.z + r.d}};
        floor.color = Color{26, 27, 34, 255};
        scene.decor.push_back(floor);
    }

    // Walls: per room per edge, minus doorway gaps from graph connections.
    for (const Room& r : rooms) {
        const Edge edges[4] = {
            {true, r.x, r.z, r.z + r.d},           // west
            {true, r.x + r.w, r.z, r.z + r.d},     // east
            {false, r.z, r.x, r.x + r.w},          // north
            {false, r.z + r.d, r.x, r.x + r.w},    // south
        };
        for (const Edge& edge : edges) {
            std::vector<Doorway> doors;
            for (const Connection& c : conns) {
                const Room* other = graph.find_room(c.a == r.id ? c.b : (c.b == r.id ? c.a : -1));
                if (!other) continue;
                SharedEdge se = shared_edge(r, *other);
                if (!se.valid || se.along_z != edge.along_z || std::fabs(se.coord - edge.coord) > kEps) continue;
                float center = 0.5f * (se.lo + se.hi);
                Doorway d{center - 0.5f * kDoorWidth, center + 0.5f * kDoorWidth};
                if (d.min < edge.lo) d.min = edge.lo;
                if (d.max > edge.hi) d.max = edge.hi;
                doors.push_back(d);
            }
            emit_wall(scene.solids, r, edge.along_z, edge.coord, edge.lo, edge.hi, doors,
                      Color{58, 59, 72, 255});
        }
    }

    return scene;
}
