#include <cassert>

struct Point {
    int x;
    int y;
};

int manhattan_distance(Point point) {
    return point.x + point.x;
}

int main() {
    // I AM NOT DONE

    Point first{3, 4};
    Point second{10, 2};

    assert(manhattan_distance(first) == 7);
    assert(manhattan_distance(second) == 12);
}
