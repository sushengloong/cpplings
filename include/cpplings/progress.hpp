#pragma once

#include "cpplings/types.hpp"

#include <filesystem>
#include <optional>
#include <vector>

namespace cpplings {

bool exercise_looks_done(const std::filesystem::path& root, const Exercise& exercise);
Progress calculate_progress(const std::filesystem::path& root,
                            const std::vector<Exercise>& exercises);
int progress_percent(const Progress& progress);
std::optional<Exercise> next_exercise(const std::filesystem::path& root,
                                      const std::vector<Exercise>& exercises);

} // namespace cpplings
