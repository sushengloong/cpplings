#include <cassert>
#include <string>

template <typename T>
T max_value(T left, T right) {
    return left;
}

int main() {
    // I AM NOT DONE

    assert(max_value(2, 5) == 5);
    assert(max_value(std::string("zebra"), std::string("apple")) == "zebra");
}
