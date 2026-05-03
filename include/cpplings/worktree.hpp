#pragma once

#include "cpplings/types.hpp"

#include <filesystem>
#include <vector>

namespace cpplings {

std::filesystem::path template_path(const std::filesystem::path& root, const Exercise& exercise);
std::filesystem::path work_path(const std::filesystem::path& root, const Exercise& exercise);
void reset_work_copy(const std::filesystem::path& root, const Exercise& exercise);
int initialize_work_tree(const std::filesystem::path& root, const std::vector<Exercise>& exercises,
                         bool overwrite_existing);
void ensure_work_tree(const std::filesystem::path& root, const std::vector<Exercise>& exercises);

} // namespace cpplings
