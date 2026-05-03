#include "cpplings/manifest.hpp"

#include "test_support.hpp"

#include <cassert>
#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

namespace fs = std::filesystem;

namespace {

void test_manifest_keeps_explicit_entries_and_appends_discovered_cpp_files() {
    const fs::path root = test_support::make_temp_root("manifest");
    test_support::write_file(root / "exercises" / "manifest.txt",
                             "first|exercises/ordered/first.cpp|c++20|First hint\n");
    test_support::write_file(root / "exercises" / "ordered" / "first.cpp",
                             test_support::exercise_source(false));
    test_support::write_file(root / "exercises" / "extra" / "auto_01.cpp",
                             test_support::exercise_source(false));

    const std::vector<cpplings::Exercise> exercises = cpplings::load_manifest(root);

    assert(exercises.size() == 2);
    assert(exercises[0].name == "first");
    assert(exercises[0].standard == "c++20");
    assert(exercises[0].hint == "First hint");
    assert(exercises[1].name == "auto_01");
    assert(exercises[1].path.generic_string() == "exercises/extra/auto_01.cpp");
    assert(exercises[1].standard == "c++17");
    assert(exercises[1].hint.find("No hint") != std::string::npos);

    fs::remove_all(root);
}

void test_manifest_rejects_duplicate_names() {
    const fs::path root = test_support::make_temp_root("duplicate_names");
    test_support::write_file(root / "exercises" / "manifest.txt", "same|exercises/a.cpp|c++17|A\n"
                                                                  "same|exercises/b.cpp|c++17|B\n");
    test_support::write_file(root / "exercises" / "a.cpp", test_support::exercise_source(false));
    test_support::write_file(root / "exercises" / "b.cpp", test_support::exercise_source(false));

    bool threw = false;
    try {
        (void)cpplings::load_manifest(root);
    } catch (const std::runtime_error& error) {
        threw = std::string(error.what()).find("duplicate exercise name") != std::string::npos;
    }

    assert(threw);
    fs::remove_all(root);
}

void test_manifest_rejects_duplicate_paths() {
    const fs::path root = test_support::make_temp_root("duplicate_paths");
    test_support::write_file(root / "exercises" / "manifest.txt",
                             "first|exercises/a.cpp|c++17|A\n"
                             "second|exercises/a.cpp|c++17|B\n");
    test_support::write_file(root / "exercises" / "a.cpp", test_support::exercise_source(false));

    bool threw = false;
    try {
        (void)cpplings::load_manifest(root);
    } catch (const std::runtime_error& error) {
        threw = std::string(error.what()).find("duplicate exercise path") != std::string::npos;
    }

    assert(threw);
    fs::remove_all(root);
}

} // namespace

int main() {
    test_manifest_keeps_explicit_entries_and_appends_discovered_cpp_files();
    test_manifest_rejects_duplicate_names();
    test_manifest_rejects_duplicate_paths();
}
