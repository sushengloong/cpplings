#include "cpplings/manifest.hpp"
#include "cpplings/worktree.hpp"

#include "test_support.hpp"

#include <cassert>
#include <filesystem>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

void test_work_tree_initializes_missing_files_without_overwriting_solutions() {
    const fs::path root = test_support::make_temp_root("worktree");
    test_support::write_file(root / "exercises" / "manifest.txt",
                             "hello|exercises/hello.cpp|c++17|Hint\n");
    test_support::write_file(root / "exercises" / "hello.cpp",
                             test_support::exercise_source(false));

    const std::vector<cpplings::Exercise> exercises = cpplings::load_manifest(root);
    assert(cpplings::initialize_work_tree(root, exercises, false) == 1);

    const fs::path work_file = cpplings::work_path(root, exercises[0]);
    test_support::write_file(work_file, test_support::exercise_source(true));
    assert(cpplings::initialize_work_tree(root, exercises, false) == 0);
    assert(test_support::read_file(work_file).find(cpplings::kNotDoneMarker) == std::string::npos);

    cpplings::reset_work_copy(root, exercises[0]);
    assert(test_support::read_file(work_file).find(cpplings::kNotDoneMarker) != std::string::npos);

    fs::remove_all(root);
}

void test_overwrite_existing_replaces_work_copy() {
    const fs::path root = test_support::make_temp_root("worktree_overwrite");
    test_support::write_file(root / "exercises" / "manifest.txt",
                             "hello|exercises/hello.cpp|c++17|Hint\n");
    test_support::write_file(root / "exercises" / "hello.cpp",
                             test_support::exercise_source(false));

    const std::vector<cpplings::Exercise> exercises = cpplings::load_manifest(root);
    cpplings::ensure_work_tree(root, exercises);
    test_support::write_file(cpplings::work_path(root, exercises[0]),
                             test_support::exercise_source(true));

    assert(cpplings::initialize_work_tree(root, exercises, true) == 1);
    assert(test_support::read_file(cpplings::work_path(root, exercises[0]))
               .find(cpplings::kNotDoneMarker) != std::string::npos);

    fs::remove_all(root);
}

} // namespace

int main() {
    test_work_tree_initializes_missing_files_without_overwriting_solutions();
    test_overwrite_existing_replaces_work_copy();
}
