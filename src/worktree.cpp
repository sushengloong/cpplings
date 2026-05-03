#include "cpplings/worktree.hpp"

#include <stdexcept>

namespace cpplings {

std::filesystem::path template_path(const std::filesystem::path& root, const Exercise& exercise) {
    return root / exercise.path;
}

std::filesystem::path work_path(const std::filesystem::path& root, const Exercise& exercise) {
    return root / kWorkRoot / exercise.path;
}

void reset_work_copy(const std::filesystem::path& root, const Exercise& exercise) {
    const std::filesystem::path from = template_path(root, exercise);
    const std::filesystem::path to = work_path(root, exercise);

    if (!std::filesystem::exists(from)) {
        throw std::runtime_error("missing exercise template: " + from.string());
    }

    std::filesystem::create_directories(to.parent_path());
    std::filesystem::copy_file(from, to, std::filesystem::copy_options::overwrite_existing);
}

int initialize_work_tree(const std::filesystem::path& root, const std::vector<Exercise>& exercises,
                         bool overwrite_existing) {
    int copied = 0;

    for (const Exercise& exercise : exercises) {
        const std::filesystem::path to = work_path(root, exercise);

        if (std::filesystem::exists(to) && !overwrite_existing) {
            continue;
        }

        reset_work_copy(root, exercise);
        ++copied;
    }

    return copied;
}

void ensure_work_tree(const std::filesystem::path& root, const std::vector<Exercise>& exercises) {
    (void)initialize_work_tree(root, exercises, false);
}

} // namespace cpplings
