#include <cassert>
#include <string>

std::string greet(const std::string &name) {
    return "Hello";
}

int main() {
    // I AM NOT DONE

    assert(greet("Ada") == "Hello, Ada!");
    assert(greet("Bjarne") == "Hello, Bjarne!");
}
