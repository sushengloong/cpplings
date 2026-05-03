#pragma once

#include "cpplings/types.hpp"

#include <filesystem>

namespace cpplings {

RunResult run_exercise(const std::filesystem::path& root, const Exercise& exercise);
RunResult check_exercise(const std::filesystem::path& root, const Exercise& exercise, bool quiet);

} // namespace cpplings
