#include "cpplings/output.hpp"

#include "cpplings/progress.hpp"
#include "cpplings/worktree.hpp"

#include <iostream>

namespace cpplings {

void print_progress_summary(std::ostream& out, const Progress& progress) {
    out << "Progress: " << progress.marked_done << "/" << progress.total << " marked done ("
        << progress_percent(progress) << "%)";

    if (progress.pending > 0) {
        out << ", " << progress.pending << " pending";
    }
    if (progress.missing > 0) {
        out << ", " << progress.missing << " missing";
    }

    out << "\n";
}

void print_open_line(std::ostream& out, const std::filesystem::path& root,
                     const Exercise& exercise) {
    out << "Open: " << work_path(root, exercise) << "\n";
}

void print_run_line(std::ostream& out, const Exercise& exercise) {
    out << "Run:  cpplings run " << exercise.name << "\n";
}

void print_hint_command_line(std::ostream& out, const Exercise& exercise) {
    out << "Hint: cpplings hint " << exercise.name << "\n";
}

void print_exercise_block(std::ostream& out, const std::string& label,
                          const std::filesystem::path& root, const Exercise& exercise,
                          bool include_run, bool include_hint_command) {
    out << label << ": " << exercise.name << "\n";
    print_open_line(out, root, exercise);
    if (include_run) {
        print_run_line(out, exercise);
    }
    if (include_hint_command) {
        print_hint_command_line(out, exercise);
    }
}

void print_progress_and_next(std::ostream& out, const std::filesystem::path& root,
                             const std::vector<Exercise>& exercises) {
    print_progress_summary(out, calculate_progress(root, exercises));

    const std::optional<Exercise> next = next_exercise(root, exercises);
    if (next) {
        print_exercise_block(out, "Next exercise", root, *next, true, false);
    } else {
        out << "All exercises are marked done. Run `cpplings verify` for the real test.\n";
    }
}

void print_help(std::ostream& out) {
    out << "Cpplings: a Rustlings-style C++ exercise runner\n\n"
        << "Usage:\n"
        << "  cpplings                 Show the next exercise\n"
        << "  cpplings init            Create missing editable exercise copies\n"
        << "  cpplings list            List exercises\n"
        << "  cpplings progress        Show marked-done progress\n"
        << "  cpplings run <name>      Compile and run an exercise\n"
        << "  cpplings verify          Check all exercises in order\n"
        << "  cpplings next            Check the next unfinished exercise\n"
        << "  cpplings hint [name]     Show a hint\n"
        << "  cpplings reset <name>    Reset one editable exercise copy\n"
        << "  cpplings watch           Re-run verify when exercise files change\n"
        << "  cpplings help            Show this help\n";
}

} // namespace cpplings
