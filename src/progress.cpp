#include "cpplings/progress.hpp"

#include "cpplings/util.hpp"
#include "cpplings/worktree.hpp"

namespace cpplings {

bool exercise_looks_done(const std::filesystem::path& root, const Exercise& exercise) {
    const std::filesystem::path source = work_path(root, exercise);
    return std::filesystem::exists(source) && !has_not_done_marker(source);
}

Progress calculate_progress(const std::filesystem::path& root,
                            const std::vector<Exercise>& exercises) {
    Progress progress;
    progress.total = static_cast<int>(exercises.size());

    for (const Exercise& exercise : exercises) {
        const std::filesystem::path source = work_path(root, exercise);
        if (!std::filesystem::exists(source)) {
            ++progress.missing;
        } else if (has_not_done_marker(source)) {
            ++progress.pending;
        } else {
            ++progress.marked_done;
        }
    }

    return progress;
}

int progress_percent(const Progress& progress) {
    if (progress.total == 0) {
        return 100;
    }

    return (progress.marked_done * 100) / progress.total;
}

std::optional<Exercise> next_exercise(const std::filesystem::path& root,
                                      const std::vector<Exercise>& exercises) {
    for (const Exercise& exercise : exercises) {
        if (!exercise_looks_done(root, exercise)) {
            return exercise;
        }
    }

    return std::nullopt;
}

} // namespace cpplings
