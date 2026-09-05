// Scene = one-way projection of the WorldGraph into renderable/collidable boxes.
// ADR-0002: this file is one of the two licensed raylib call-surface files.
#pragma once
#include "raylib.h"
#include "world_graph.h"

#include <vector>

struct SceneBox {
    BoundingBox bounds;
    Color color;
};

struct Scene {
    std::vector<SceneBox> solids; // walls + lintels; player collision set
    std::vector<SceneBox> decor;  // floors, exit pad; never collide
};

// Rebuilds the whole scene from the graph. Called when WorldGraph::version()
// changes; "rebuild everything" is the honest M0 price for keeping
// "data is real, geometry is fake" as a hard invariant (ISS-005).
Scene build_scene(const WorldGraph& graph);
