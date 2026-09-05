// Data-structure tests for WorldGraph v0 (ISS-005).
// No raylib on purpose: headless, fast, runnable from any CI or runbook.
// Custom CHECK instead of <cassert>: the runbook builds --config Release and
// CMake's Release flags define NDEBUG, which would turn every assert into
// ((void)0) - a suite that cannot fail is not a suite.
#include "world_graph.h"

#include <cstdio>

namespace {

int g_failures = 0;

#define CHECK(cond)                                                                   \
    do {                                                                              \
        if (!(cond)) {                                                                \
            ++g_failures;                                                             \
            std::printf("  CHECK failed: %s  (%s:%d)\n", #cond, __FILE__, __LINE__);  \
        }                                                                             \
    } while (0)

#define RUN_TEST(fn)                                                                  \
    do {                                                                              \
        const int before = g_failures;                                                \
        std::printf("[ RUN  ] %s\n", #fn);                                            \
        fn();                                                                         \
        std::printf("[ %s ] %s\n", (g_failures == before) ? " OK  " : "FAIL ", #fn);  \
    } while (0)

void test_add_room_ids_increase() {
    WorldGraph g;
    const RoomId a = g.add_room("lobby", 0.0f, 0.0f, 4.0f, 6.0f);
    const RoomId b = g.add_room("hall", 4.0f, 0.0f, 4.0f, 6.0f);
    const RoomId c = g.add_room("pool", 0.0f, 6.0f, 8.0f, 6.0f);
    CHECK(a == 0);
    CHECK(b == a + 1);
    CHECK(c == b + 1);
    CHECK(g.rooms().size() == 3);
    CHECK(g.rooms()[0].name == "lobby");
    CHECK(g.rooms()[2].w == 8.0f);
}

void test_connect_is_idempotent() {
    WorldGraph g;
    const RoomId a = g.add_room("a", 0.0f, 0.0f, 1.0f, 1.0f);
    const RoomId b = g.add_room("b", 2.0f, 0.0f, 1.0f, 1.0f);

    const int v0 = g.version();
    g.connect(a, b);
    CHECK(g.version() == v0 + 1);
    CHECK(g.connections().size() == 1);

    g.connect(a, b);  // same order
    g.connect(b, a);  // reversed: same doorway
    g.connect(a, a);  // self-doorway: no-op
    CHECK(g.connections().size() == 1);
    CHECK(g.version() == v0 + 1);
}

void test_disconnect_is_idempotent() {
    WorldGraph g;
    const RoomId a = g.add_room("a", 0.0f, 0.0f, 1.0f, 1.0f);
    const RoomId b = g.add_room("b", 2.0f, 0.0f, 1.0f, 1.0f);
    g.connect(a, b);
    const int v1 = g.version();

    g.disconnect(b, a);  // reversed hits the same edge
    CHECK(g.connections().empty());
    CHECK(g.version() == v1 + 1);

    g.disconnect(a, b);  // already gone: no-op
    CHECK(g.version() == v1 + 1);
}

void test_connected_truth_table() {
    WorldGraph g;
    const RoomId a = g.add_room("a", 0.0f, 0.0f, 1.0f, 1.0f);
    const RoomId b = g.add_room("b", 2.0f, 0.0f, 1.0f, 1.0f);
    const RoomId c = g.add_room("c", 4.0f, 0.0f, 1.0f, 1.0f);

    CHECK(!g.connected(a, b));
    g.connect(a, b);
    CHECK(g.connected(a, b));
    CHECK(g.connected(b, a));  // undirected
    CHECK(!g.connected(a, c));  // never linked
    g.disconnect(a, b);
    CHECK(!g.connected(a, b));
    CHECK(!g.connected(a, a));  // no self-doorway was ever stored
}

void test_find_room() {
    WorldGraph g;
    const RoomId a = g.add_room("lobby", 1.0f, 2.0f, 3.0f, 4.0f);
    const Room* r = g.find_room(a);
    CHECK(r != nullptr);
    if (r != nullptr) {
        CHECK(r->id == a);
        CHECK(r->name == "lobby");
        CHECK(r->x == 1.0f && r->z == 2.0f && r->w == 3.0f && r->d == 4.0f);
    }
    CHECK(g.find_room(a + 100) == nullptr);
    CHECK(g.find_room(-1) == nullptr);
}

void test_version_tracks_real_changes_only() {
    WorldGraph g;
    CHECK(g.version() == 0);
    const RoomId a = g.add_room("a", 0.0f, 0.0f, 1.0f, 1.0f);
    const RoomId b = g.add_room("b", 2.0f, 0.0f, 1.0f, 1.0f);
    const RoomId c = g.add_room("c", 4.0f, 0.0f, 1.0f, 1.0f);
    CHECK(g.version() == 3);

    g.connect(a, b);
    CHECK(g.version() == 4);
    g.connect(a, b);  // no-op
    CHECK(g.version() == 4);
    g.disconnect(a, c);  // never connected: no-op
    CHECK(g.version() == 4);
    g.disconnect(a, b);
    CHECK(g.version() == 5);
    g.disconnect(a, b);  // no-op
    CHECK(g.version() == 5);
}

}  // namespace

int main() {
    RUN_TEST(test_add_room_ids_increase);
    RUN_TEST(test_connect_is_idempotent);
    RUN_TEST(test_disconnect_is_idempotent);
    RUN_TEST(test_connected_truth_table);
    RUN_TEST(test_find_room);
    RUN_TEST(test_version_tracks_real_changes_only);

    if (g_failures == 0) {
        std::printf("test_worldgraph: all checks passed\n");
        return 0;
    }
    std::printf("test_worldgraph: %d check(s) FAILED\n", g_failures);
    return 1;
}
