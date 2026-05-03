#include <cassert>

void increment(int value) {
    value += 1;
}

int main() {
    // I AM NOT DONE

    int score = 1;
    increment(score);

    assert(score == 2);
}
