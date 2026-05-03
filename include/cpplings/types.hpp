#pragma once

#include <filesystem>
#include <string>

namespace cpplings {

inline constexpr const char* kManifestPath = "exercises/manifest.txt";
inline constexpr const char* kNotDoneMarker = "I AM NOT DONE";
inline constexpr const char* kWorkRoot = ".cpplings/work";

struct Exercise {
    std::string name;
    std::filesystem::path path;
    std::string standard;
    std::string hint;
};

struct RunResult {
    bool ok = false;
    std::string message;
};

struct Progress {
    int total = 0;
    int marked_done = 0;
    int pending = 0;
    int missing = 0;
};

} // namespace cpplings
