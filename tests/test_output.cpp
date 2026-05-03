#include "cpplings/manifest.hpp"
#include "cpplings/output.hpp"
#include "cpplings/worktree.hpp"

#include "test_support.hpp"

#include <cassert>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

void test_output_helpers_share_actionable_format() {
    const fs::path root = test_support::make_temp_root("output");
    test_support::write_file(root / "exercises" / "manifest.txt",
                             "hello|exercises/hello.cpp|c++17|Hint\n");
    test_support::write_file(root / "exercises" / "hello.cpp",
                             test_support::exercise_source(false));

    const std::vector<cpplings::Exercise> exercises = cpplings::load_manifest(root);
    cpplings::ensure_work_tree(root, exercises);

    std::ostringstream out;
    cpplings::print_progress_and_next(out, root, exercises);
    const std::string rendered = out.str();

    assert(rendered.find("Progress: 0/1 marked done (0%), 1 pending") != std::string::npos);
    assert(rendered.find("Next exercise: hello") != std::string::npos);
    assert(rendered.find("Open: ") != std::string::npos);
    assert(rendered.find("Run:  cpplings run hello") != std::string::npos);

    fs::remove_all(root);
}

void test_help_mentions_progress() {
    std::ostringstream out;
    cpplings::print_help(out);
    assert(out.str().find("cpplings progress") != std::string::npos);
}

} // namespace

int main() {
    test_output_helpers_share_actionable_format();
    test_help_mentions_progress();
}
