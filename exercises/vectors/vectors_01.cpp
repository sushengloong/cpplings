#include <cassert>
#include <vector>

int main() {
    // I AM NOT DONE

    std::vector<int> numbers{1, 2, 3, 4};

    int total = 0;
    for (const int number : numbers) {
        total += number;
    }

    assert(total == 15);
}
