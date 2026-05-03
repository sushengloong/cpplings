#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace cpplings {

std::string trim(const std::string& value);
std::vector<std::string> split(const std::string& line, char delimiter);
std::string status_label(bool ready_to_check);
std::string read_file(const std::filesystem::path& path);
bool has_not_done_marker(const std::filesystem::path& path);

} // namespace cpplings
