#include "cpplings/util.hpp"

#include "cpplings/types.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

namespace cpplings {

std::string trim(const std::string& value) {
    const auto begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }

    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}

std::vector<std::string> split(const std::string& line, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(line);
    std::string part;

    while (std::getline(stream, part, delimiter)) {
        parts.push_back(trim(part));
    }

    return parts;
}

std::string status_label(bool ready_to_check) {
    return ready_to_check ? "ready" : "pending";
}

std::string read_file(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("could not open " + path.string());
    }

    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

bool has_not_done_marker(const std::filesystem::path& path) {
    return read_file(path).find(kNotDoneMarker) != std::string::npos;
}

} // namespace cpplings
