#include "cpplings/manifest.hpp"

#include "cpplings/util.hpp"

#include <algorithm>
#include <fstream>
#include <set>
#include <stdexcept>

namespace cpplings {
namespace {

std::string default_hint() {
    return "No hint is registered for this exercise yet. Read the comments and assertions in the "
           "file.";
}

std::string auto_name_from_path(const std::filesystem::path& relative_path,
                                const std::set<std::string>& used_names) {
    std::string candidate = relative_path.stem().string();
    if (used_names.count(candidate) == 0) {
        return candidate;
    }

    std::vector<std::string> parts;
    for (const std::filesystem::path& part : relative_path.parent_path()) {
        if (part != "." && part != "exercises") {
            parts.push_back(part.string());
        }
    }
    parts.push_back(relative_path.stem().string());

    candidate.clear();
    for (const std::string& part : parts) {
        if (!candidate.empty()) {
            candidate += "_";
        }
        candidate += part;
    }

    if (used_names.count(candidate) == 0) {
        return candidate;
    }

    int suffix = 2;
    while (used_names.count(candidate + "_" + std::to_string(suffix)) != 0) {
        ++suffix;
    }
    return candidate + "_" + std::to_string(suffix);
}

} // namespace

std::filesystem::path find_project_root() {
    std::filesystem::path current = std::filesystem::current_path();

    while (true) {
        if (std::filesystem::exists(current / kManifestPath)) {
            return current;
        }

        if (!current.has_parent_path() || current == current.parent_path()) {
            break;
        }

        current = current.parent_path();
    }

    throw std::runtime_error("could not find exercises/manifest.txt from the current directory");
}

std::vector<std::filesystem::path> discover_exercise_templates(const std::filesystem::path& root) {
    const std::filesystem::path exercise_root = root / "exercises";
    std::vector<std::filesystem::path> paths;

    if (!std::filesystem::exists(exercise_root)) {
        return paths;
    }

    for (const std::filesystem::directory_entry& entry :
         std::filesystem::recursive_directory_iterator(exercise_root)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".cpp") {
            continue;
        }

        paths.push_back(std::filesystem::relative(entry.path(), root));
    }

    std::sort(paths.begin(), paths.end(), [](const auto& left, const auto& right) {
        return left.generic_string() < right.generic_string();
    });

    return paths;
}

std::vector<Exercise> load_manifest(const std::filesystem::path& root) {
    const std::filesystem::path manifest_path = root / kManifestPath;
    std::ifstream manifest(manifest_path);

    if (!manifest) {
        throw std::runtime_error("could not open " + manifest_path.string());
    }

    std::vector<Exercise> exercises;
    std::set<std::string> known_paths;
    std::set<std::string> used_names;
    std::string line;
    int line_number = 0;

    while (std::getline(manifest, line)) {
        ++line_number;
        const std::string cleaned = trim(line);

        if (cleaned.empty() || cleaned[0] == '#') {
            continue;
        }

        std::vector<std::string> parts = split(cleaned, '|');
        if (parts.size() != 4) {
            throw std::runtime_error(manifest_path.string() + ":" + std::to_string(line_number) +
                                     " expected name|path|standard|hint");
        }

        const std::filesystem::path path = std::filesystem::path(parts[1]);
        const std::string path_key = path.generic_string();

        if (used_names.count(parts[0]) != 0) {
            throw std::runtime_error(manifest_path.string() + ":" + std::to_string(line_number) +
                                     " duplicate exercise name: " + parts[0]);
        }
        if (known_paths.count(path_key) != 0) {
            throw std::runtime_error(manifest_path.string() + ":" + std::to_string(line_number) +
                                     " duplicate exercise path: " + path_key);
        }

        exercises.push_back(Exercise{
            parts[0],
            path,
            parts[2].empty() ? "c++17" : parts[2],
            parts[3],
        });
        used_names.insert(parts[0]);
        known_paths.insert(path_key);
    }

    for (const std::filesystem::path& path : discover_exercise_templates(root)) {
        const std::string path_key = path.generic_string();
        if (known_paths.count(path_key) != 0) {
            continue;
        }

        const std::string name = auto_name_from_path(path, used_names);
        exercises.push_back(Exercise{name, path, "c++17", default_hint()});
        used_names.insert(name);
        known_paths.insert(path_key);
    }

    if (exercises.empty()) {
        throw std::runtime_error("no exercises found in manifest or exercises/**/*.cpp");
    }

    return exercises;
}

std::optional<Exercise> find_exercise(const std::vector<Exercise>& exercises,
                                      const std::string& name) {
    const auto found =
        std::find_if(exercises.begin(), exercises.end(),
                     [&](const Exercise& exercise) { return exercise.name == name; });

    if (found == exercises.end()) {
        return std::nullopt;
    }

    return *found;
}

} // namespace cpplings
