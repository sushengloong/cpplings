#include <cassert>
#include <string>
#include <vector>

class ScopeLog {
  public:
    explicit ScopeLog(std::vector<std::string> &events) : events(events) {
        events.push_back("enter");
    }

    ~ScopeLog() {}

  private:
    std::vector<std::string> &events;
};

int main() {
    // I AM NOT DONE

    std::vector<std::string> events;

    {
        ScopeLog guard(events);
        assert((events == std::vector<std::string>{"enter"}));
    }

    assert((events == std::vector<std::string>{"enter", "exit"}));
}
