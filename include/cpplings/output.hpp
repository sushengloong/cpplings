#pragma once

#include "cpplings/types.hpp"

#include <filesystem>
#include <iosfwd>
#include <string>
#include <vector>

namespace cpplings {

void print_progress_summary(std::ostream& out, const Progress& progress);
void print_open_line(std::ostream& out, const std::filesystem::path& root,
                     const Exercise& exercise);
void print_run_line(std::ostream& out, const Exercise& exercise);
void print_hint_command_line(std::ostream& out, const Exercise& exercise);
void print_exercise_block(std::ostream& out, const std::string& label,
                          const std::filesystem::path& root, const Exercise& exercise,
                          bool include_run, bool include_hint_command);
void print_progress_and_next(std::ostream& out, const std::filesystem::path& root,
                             const std::vector<Exercise>& exercises);
void print_help(std::ostream& out);

} // namespace cpplings
