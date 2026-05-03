#include <cassert>
#include <string>

std::string describe_temperature(int celsius) {
    if (celsius < 0) {
        return "freezing";
    }

    if (celsius > 0) {
        return "warm";
    }

    return "unknown";
}

int main() {
    // I AM NOT DONE

    assert(describe_temperature(-3) == "freezing");
    assert(describe_temperature(0) == "cold");
    assert(describe_temperature(25) == "warm");
}
