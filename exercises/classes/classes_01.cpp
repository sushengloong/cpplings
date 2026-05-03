#include <cassert>

class Counter {
  public:
    void increment() {
        value + 1;
    }

    int current() const {
        return value;
    }

  private:
    int value = 0;
};

int main() {
    // I AM NOT DONE

    Counter counter;
    counter.increment();
    counter.increment();

    assert(counter.current() == 2);
}
