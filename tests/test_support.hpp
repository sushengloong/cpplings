#pragma once

#include "cpplings/types.hpp"

#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

namespace test_support {

inline std::filesystem::path make_temp_root(const std::string& name) {
    const auto stamp = std::chrono::steady_clock::now().time_since_epoch().count();
    std::filesystem::path root =
        std::filesystem::temp_directory_path() / ("cpplings_" + name + "_" + std::to_string(stamp));
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root / "exercises");
    return root;
}

inline void write_file(const std::filesystem::path& path, const std::string& contents) {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream file(path);
    assert(file && "test fixture file should be writable");
    file << contents;
}

inline std::string read_file(const std::filesystem::path& path) {
    std::ifstream file(path);
    assert(file && "test fixture file should be readable");
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

inline std::string exercise_source(bool done) {
    return std::string("#include <cassert>\n\nint main() {\n") +
           (done ? "" : "    // I AM NOT DONE\n") + "    assert(true);\n}\n";
}

} // namespace test_support
