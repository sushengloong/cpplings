#pragma once

#include "cpplings/types.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace cpplings {

std::filesystem::path find_project_root();
std::vector<std::filesystem::path> discover_exercise_templates(const std::filesystem::path& root);
std::vector<Exercise> load_manifest(const std::filesystem::path& root);
std::optional<Exercise> find_exercise(const std::vector<Exercise>& exercises,
                                      const std::string& name);

} // namespace cpplings
