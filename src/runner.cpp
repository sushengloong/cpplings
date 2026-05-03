#include "cpplings/runner.hpp"

#include "cpplings/util.hpp"
#include "cpplings/worktree.hpp"

#include <cstdlib>
#include <iostream>

namespace cpplings {
namespace {

std::string shell_quote(const std::string& value) {
    std::string quoted = "'";
    for (const char ch : value) {
        if (ch == '\'') {
            quoted += "'\\''";
        } else {
            quoted += ch;
        }
    }
    quoted += "'";
    return quoted;
}

std::string pick_compiler() {
    if (const char* from_env = std::getenv("CXX")) {
        if (std::string(from_env).empty()) {
            return "c++";
        }
        return from_env;
    }

    return "c++";
}

int run_command(const std::string& command) {
    return std::system(command.c_str());
}

} // namespace

RunResult run_exercise(const std::filesystem::path& root, const Exercise& exercise) {
    const std::filesystem::path source = work_path(root, exercise);

    if (!std::filesystem::exists(source)) {
        return {false, "missing source file: " + source.string()};
    }

    if (has_not_done_marker(source)) {
        return {
            false,
            "still marked as unfinished. Remove `// I AM NOT DONE` when you are ready to test it.",
        };
    }

    const std::filesystem::path build_dir = root / ".cpplings" / "bin";
    std::filesystem::create_directories(build_dir);

#ifdef _WIN32
    const std::filesystem::path binary = build_dir / (exercise.name + ".exe");
#else
    const std::filesystem::path binary = build_dir / exercise.name;
#endif

    const std::string compiler = pick_compiler();
    const std::string compile_command =
        shell_quote(compiler) + " -std=" + shell_quote(exercise.standard) +
        " -Wall -Wextra -pedantic -g " + shell_quote(source.string()) + " -o " +
        shell_quote(binary.string());

    std::cout << "Compiling " << exercise.name << "...\n" << std::flush;
    const int compile_status = run_command(compile_command);
    if (compile_status != 0) {
        return {false, "compile failed"};
    }

    std::cout << "Running " << exercise.name << "...\n" << std::flush;
    const int run_status = run_command(shell_quote(binary.string()));
    if (run_status != 0) {
        return {false, "program exited with a failure"};
    }

    return {true, "passed"};
}

RunResult check_exercise(const std::filesystem::path& root, const Exercise& exercise, bool quiet) {
    if (!quiet) {
        std::cout << "\n==> " << exercise.name << "\n" << std::flush;
    }

    RunResult result = run_exercise(root, exercise);

    if (!quiet) {
        std::cout << (result.ok ? "PASS: " : "FIX ME: ") << result.message << "\n";
    }

    return result;
}

} // namespace cpplings
