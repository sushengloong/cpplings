#include "cpplings/cli.hpp"

#include "cpplings/manifest.hpp"
#include "cpplings/output.hpp"
#include "cpplings/progress.hpp"
#include "cpplings/runner.hpp"
#include "cpplings/util.hpp"
#include "cpplings/worktree.hpp"

#include <chrono>
#include <exception>
#include <filesystem>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace cpplings {
namespace {

std::map<std::filesystem::path, std::filesystem::file_time_type>
snapshot_exercise_times(const std::filesystem::path& root, const std::vector<Exercise>& exercises) {
    std::map<std::filesystem::path, std::filesystem::file_time_type> snapshot;

    for (const Exercise& exercise : exercises) {
        const std::filesystem::path path = work_path(root, exercise);
        if (std::filesystem::exists(path)) {
            snapshot[path] = std::filesystem::last_write_time(path);
        }
    }

    return snapshot;
}

int command_list(const std::filesystem::path& root, const std::vector<Exercise>& exercises) {
    print_progress_summary(std::cout, calculate_progress(root, exercises));
    std::cout << "Exercises\n";
    for (const Exercise& exercise : exercises) {
        const bool looks_done = exercise_looks_done(root, exercise);
        std::cout << "  [" << status_label(looks_done) << "] " << exercise.name << "  "
                  << work_path(root, exercise).string() << "\n";
    }
    return 0;
}

int command_progress(const std::filesystem::path& root, const std::vector<Exercise>& exercises) {
    print_progress_and_next(std::cout, root, exercises);
    return 0;
}

int command_init(const std::filesystem::path& root, const std::vector<Exercise>& exercises) {
    const int copied = initialize_work_tree(root, exercises, false);
    if (copied == 0) {
        std::cout << "Exercise work tree already exists at " << (root / kWorkRoot) << "\n";
    } else {
        std::cout << "Created " << copied << " editable exercise "
                  << (copied == 1 ? "copy" : "copies") << " under " << (root / kWorkRoot) << "\n";
    }
    print_progress_and_next(std::cout, root, exercises);
    return 0;
}

int command_reset(const std::filesystem::path& root, const std::vector<Exercise>& exercises,
                  const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cerr << "usage: cpplings reset <name>\n";
        return 2;
    }

    const std::optional<Exercise> exercise = find_exercise(exercises, args[2]);
    if (!exercise) {
        std::cerr << "unknown exercise: " << args[2] << "\n";
        return 2;
    }

    reset_work_copy(root, *exercise);

    std::cout << "Reset " << exercise->name << "\n";
    print_progress_summary(std::cout, calculate_progress(root, exercises));
    print_exercise_block(std::cout, "Exercise", root, *exercise, true, true);
    return 0;
}

int command_run(const std::filesystem::path& root, const std::vector<Exercise>& exercises,
                const std::vector<std::string>& args) {
    if (args.size() < 3) {
        std::cerr << "usage: cpplings run <name>\n";
        return 2;
    }

    const std::optional<Exercise> exercise = find_exercise(exercises, args[2]);
    if (!exercise) {
        std::cerr << "unknown exercise: " << args[2] << "\n";
        return 2;
    }

    const RunResult result = check_exercise(root, *exercise, false);
    std::cout << "\n";
    if (result.ok) {
        print_progress_and_next(std::cout, root, exercises);
    } else {
        print_progress_summary(std::cout, calculate_progress(root, exercises));
        print_exercise_block(std::cout, "Exercise", root, *exercise, true, true);
    }
    return result.ok ? 0 : 1;
}

int command_verify(const std::filesystem::path& root, const std::vector<Exercise>& exercises) {
    print_progress_summary(std::cout, calculate_progress(root, exercises));
    for (const Exercise& exercise : exercises) {
        const RunResult result = check_exercise(root, exercise, false);
        if (!result.ok) {
            std::cout << "\n";
            print_exercise_block(std::cout, "Exercise", root, exercise, true, true);
            return 1;
        }
    }

    std::cout << "\nAll exercises passed. Nice work.\n";
    return 0;
}

int command_next(const std::filesystem::path& root, const std::vector<Exercise>& exercises) {
    const std::optional<Exercise> next = next_exercise(root, exercises);
    if (!next) {
        return command_verify(root, exercises);
    }

    print_progress_summary(std::cout, calculate_progress(root, exercises));
    print_exercise_block(std::cout, "Next exercise", root, *next, true, false);
    const RunResult result = check_exercise(root, *next, false);
    if (!result.ok) {
        std::cout << "\n";
        print_hint_command_line(std::cout, *next);
    }
    return result.ok ? 0 : 1;
}

int command_hint(const std::filesystem::path& root, const std::vector<Exercise>& exercises,
                 const std::vector<std::string>& args) {
    std::optional<Exercise> exercise;

    if (args.size() >= 3) {
        exercise = find_exercise(exercises, args[2]);
        if (!exercise) {
            std::cerr << "unknown exercise: " << args[2] << "\n";
            return 2;
        }
    } else {
        exercise = next_exercise(root, exercises);
        if (!exercise) {
            print_progress_and_next(std::cout, root, exercises);
            return 0;
        }
    }

    print_progress_summary(std::cout, calculate_progress(root, exercises));
    print_exercise_block(std::cout, "Exercise", root, *exercise, true, false);
    std::cout << "Hint: " << exercise->hint << "\n";
    return 0;
}

int command_watch(const std::filesystem::path& root, const std::vector<Exercise>& exercises) {
    std::cout << "Watching exercises. Press Ctrl-C to stop.\n";
    std::map<std::filesystem::path, std::filesystem::file_time_type> last_snapshot =
        snapshot_exercise_times(root, exercises);

    (void)command_verify(root, exercises);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(700));
        const std::map<std::filesystem::path, std::filesystem::file_time_type> current_snapshot =
            snapshot_exercise_times(root, exercises);

        if (current_snapshot != last_snapshot) {
            last_snapshot = current_snapshot;
            std::cout << "\nChange detected.\n";
            (void)command_verify(root, exercises);
        }
    }
}

} // namespace

int run_cli(int argc, char** argv) {
    try {
        const std::filesystem::path root = find_project_root();
        const std::vector<Exercise> exercises = load_manifest(root);

        std::vector<std::string> args;
        args.reserve(static_cast<std::size_t>(argc));
        for (int i = 0; i < argc; ++i) {
            args.emplace_back(argv[i]);
        }

        if (args.size() > 1 && args[1] == "init") {
            return command_init(root, exercises);
        }

        ensure_work_tree(root, exercises);

        if (args.size() == 1) {
            print_help(std::cout);
            std::cout << "\n";
            print_progress_and_next(std::cout, root, exercises);
            return 0;
        }

        const std::string command = args[1];

        if (command == "help" || command == "--help" || command == "-h") {
            print_help(std::cout);
            return 0;
        }
        if (command == "list") {
            return command_list(root, exercises);
        }
        if (command == "progress") {
            return command_progress(root, exercises);
        }
        if (command == "run") {
            return command_run(root, exercises, args);
        }
        if (command == "verify") {
            return command_verify(root, exercises);
        }
        if (command == "next") {
            return command_next(root, exercises);
        }
        if (command == "hint") {
            return command_hint(root, exercises, args);
        }
        if (command == "reset") {
            return command_reset(root, exercises, args);
        }
        if (command == "watch") {
            return command_watch(root, exercises);
        }

        std::cerr << "unknown command: " << command << "\n\n";
        print_help(std::cout);
        return 2;
    } catch (const std::exception& error) {
        std::cerr << "error: " << error.what() << "\n";
        return 1;
    }
}

} // namespace cpplings
