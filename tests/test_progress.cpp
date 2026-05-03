#include "cpplings/manifest.hpp"
#include "cpplings/progress.hpp"
#include "cpplings/worktree.hpp"

#include "test_support.hpp"

#include <cassert>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

namespace {

void test_progress_and_next_exercise_use_work_copies() {
    const fs::path root = test_support::make_temp_root("progress");
    test_support::write_file(root / "exercises" / "manifest.txt",
                             "first|exercises/first.cpp|c++17|First\n"
                             "second|exercises/second.cpp|c++17|Second\n");
    test_support::write_file(root / "exercises" / "first.cpp",
                             test_support::exercise_source(false));
    test_support::write_file(root / "exercises" / "second.cpp",
                             test_support::exercise_source(false));

    const std::vector<cpplings::Exercise> exercises = cpplings::load_manifest(root);
    cpplings::ensure_work_tree(root, exercises);

    cpplings::Progress progress = cpplings::calculate_progress(root, exercises);
    assert(progress.total == 2);
    assert(progress.marked_done == 0);
    assert(progress.pending == 2);
    assert(cpplings::progress_percent(progress) == 0);
    std::optional<cpplings::Exercise> next = cpplings::next_exercise(root, exercises);
    assert(next);
    assert(next->name == "first");

    test_support::write_file(cpplings::work_path(root, exercises[0]),
                             test_support::exercise_source(true));
    progress = cpplings::calculate_progress(root, exercises);
    assert(progress.marked_done == 1);
    assert(progress.pending == 1);
    assert(cpplings::progress_percent(progress) == 50);
    next = cpplings::next_exercise(root, exercises);
    assert(next);
    assert(next->name == "second");

    fs::remove(cpplings::work_path(root, exercises[1]));
    progress = cpplings::calculate_progress(root, exercises);
    assert(progress.marked_done == 1);
    assert(progress.missing == 1);

    fs::remove_all(root);
}

void test_empty_progress_is_complete() {
    const cpplings::Progress progress;
    assert(cpplings::progress_percent(progress) == 100);
}

} // namespace

int main() {
    test_progress_and_next_exercise_use_work_copies();
    test_empty_progress_is_complete();
}
