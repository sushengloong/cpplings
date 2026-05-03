#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

namespace {

constexpr const char *kManifestPath = "exercises/manifest.txt";
constexpr const char *kNotDoneMarker = "I AM NOT DONE";
constexpr const char *kWorkRoot = ".cpplings/work";

struct Exercise {
    std::string name;
    fs::path path;
    std::string standard;
    std::string hint;
};

struct RunResult {
    bool ok = false;
    std::string message;
};

std::string trim(const std::string &value) {
    const auto begin = value.find_first_not_of(" \t\r\n");
    if (begin == std::string::npos) {
        return "";
    }

    const auto end = value.find_last_not_of(" \t\r\n");
    return value.substr(begin, end - begin + 1);
}

std::vector<std::string> split(const std::string &line, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream stream(line);
    std::string part;

    while (std::getline(stream, part, delimiter)) {
        parts.push_back(trim(part));
    }

    return parts;
}

std::string shell_quote(const std::string &value) {
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

std::string status_label(bool ready_to_check) {
    return ready_to_check ? "ready" : "pending";
}

std::string default_hint() {
    return "No hint is registered for this exercise yet. Read the comments and assertions in the file.";
}

std::string auto_name_from_path(const fs::path &relative_path,
                                const std::set<std::string> &used_names) {
    std::string candidate = relative_path.stem().string();
    if (!used_names.count(candidate)) {
        return candidate;
    }

    std::vector<std::string> parts;
    for (const fs::path &part : relative_path.parent_path()) {
        if (part != "." && part != "exercises") {
            parts.push_back(part.string());
        }
    }
    parts.push_back(relative_path.stem().string());

    candidate.clear();
    for (const std::string &part : parts) {
        if (!candidate.empty()) {
            candidate += "_";
        }
        candidate += part;
    }

    if (!used_names.count(candidate)) {
        return candidate;
    }

    int suffix = 2;
    while (used_names.count(candidate + "_" + std::to_string(suffix))) {
        ++suffix;
    }
    return candidate + "_" + std::to_string(suffix);
}

fs::path find_project_root() {
    fs::path current = fs::current_path();

    while (true) {
        if (fs::exists(current / kManifestPath)) {
            return current;
        }

        if (!current.has_parent_path() || current == current.parent_path()) {
            break;
        }

        current = current.parent_path();
    }

    throw std::runtime_error("could not find exercises/manifest.txt from the current directory");
}

std::vector<fs::path> discover_exercise_templates(const fs::path &root) {
    const fs::path exercise_root = root / "exercises";
    std::vector<fs::path> paths;

    if (!fs::exists(exercise_root)) {
        return paths;
    }

    for (const fs::directory_entry &entry : fs::recursive_directory_iterator(exercise_root)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".cpp") {
            continue;
        }

        paths.push_back(fs::relative(entry.path(), root));
    }

    std::sort(paths.begin(), paths.end(), [](const fs::path &left, const fs::path &right) {
        return left.generic_string() < right.generic_string();
    });

    return paths;
}

std::vector<Exercise> load_manifest(const fs::path &root) {
    const fs::path manifest_path = root / kManifestPath;
    std::ifstream manifest(manifest_path);

    if (!manifest) {
        throw std::runtime_error("could not open " + manifest_path.string());
    }

    std::vector<Exercise> exercises;
    std::set<std::string> known_paths;
    std::set<std::string> used_names;
    std::string line;
    int line_number = 0;

    while (std::getline(manifest, line)) {
        ++line_number;
        const std::string cleaned = trim(line);

        if (cleaned.empty() || cleaned[0] == '#') {
            continue;
        }

        std::vector<std::string> parts = split(cleaned, '|');
        if (parts.size() != 4) {
            throw std::runtime_error(
                manifest_path.string() + ":" + std::to_string(line_number) +
                " expected name|path|standard|hint");
        }

        const fs::path path = fs::path(parts[1]);
        const std::string path_key = path.generic_string();

        if (used_names.count(parts[0])) {
            throw std::runtime_error(
                manifest_path.string() + ":" + std::to_string(line_number) +
                " duplicate exercise name: " + parts[0]);
        }
        if (known_paths.count(path_key)) {
            throw std::runtime_error(
                manifest_path.string() + ":" + std::to_string(line_number) +
                " duplicate exercise path: " + path_key);
        }

        exercises.push_back(Exercise{
            parts[0],
            path,
            parts[2].empty() ? "c++17" : parts[2],
            parts[3],
        });
        used_names.insert(parts[0]);
        known_paths.insert(path_key);
    }

    for (const fs::path &path : discover_exercise_templates(root)) {
        const std::string path_key = path.generic_string();
        if (known_paths.count(path_key)) {
            continue;
        }

        const std::string name = auto_name_from_path(path, used_names);
        exercises.push_back(Exercise{name, path, "c++17", default_hint()});
        used_names.insert(name);
        known_paths.insert(path_key);
    }

    if (exercises.empty()) {
        throw std::runtime_error("no exercises found in manifest or exercises/**/*.cpp");
    }

    return exercises;
}

std::optional<Exercise> find_exercise(const std::vector<Exercise> &exercises,
                                      const std::string &name) {
    const auto found = std::find_if(
        exercises.begin(),
        exercises.end(),
        [&](const Exercise &exercise) { return exercise.name == name; });

    if (found == exercises.end()) {
        return std::nullopt;
    }

    return *found;
}

std::string read_file(const fs::path &path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("could not open " + path.string());
    }

    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}

bool has_not_done_marker(const fs::path &path) {
    return read_file(path).find(kNotDoneMarker) != std::string::npos;
}

std::string pick_compiler() {
    if (const char *from_env = std::getenv("CXX")) {
        if (std::string(from_env).empty()) {
            return "c++";
        }
        return from_env;
    }

    return "c++";
}

int run_command(const std::string &command) {
    return std::system(command.c_str());
}

fs::path template_path(const fs::path &root, const Exercise &exercise) {
    return root / exercise.path;
}

fs::path work_path(const fs::path &root, const Exercise &exercise) {
    return root / kWorkRoot / exercise.path;
}

int initialize_work_tree(const fs::path &root,
                         const std::vector<Exercise> &exercises,
                         bool overwrite_existing) {
    int copied = 0;

    for (const Exercise &exercise : exercises) {
        const fs::path from = template_path(root, exercise);
        const fs::path to = work_path(root, exercise);

        if (!fs::exists(from)) {
            throw std::runtime_error("missing exercise template: " + from.string());
        }

        if (fs::exists(to) && !overwrite_existing) {
            continue;
        }

        fs::create_directories(to.parent_path());
        fs::copy_file(
            from,
            to,
            overwrite_existing ? fs::copy_options::overwrite_existing : fs::copy_options::none);
        ++copied;
    }

    return copied;
}

void ensure_work_tree(const fs::path &root, const std::vector<Exercise> &exercises) {
    (void)initialize_work_tree(root, exercises, false);
}

RunResult run_exercise(const fs::path &root, const Exercise &exercise) {
    const fs::path source = work_path(root, exercise);

    if (!fs::exists(source)) {
        return {false, "missing source file: " + source.string()};
    }

    if (has_not_done_marker(source)) {
        return {
            false,
            "still marked as unfinished. Remove `// I AM NOT DONE` when you are ready to test it.",
        };
    }

    const fs::path build_dir = root / ".cpplings" / "bin";
    fs::create_directories(build_dir);

#ifdef _WIN32
    const fs::path binary = build_dir / (exercise.name + ".exe");
#else
    const fs::path binary = build_dir / exercise.name;
#endif

    const std::string compiler = pick_compiler();
    const std::string compile_command =
        shell_quote(compiler) + " -std=" + shell_quote(exercise.standard) +
        " -Wall -Wextra -pedantic -g " + shell_quote(source.string()) +
        " -o " + shell_quote(binary.string());

    std::cout << "Compiling " << exercise.name << "..." << std::endl;
    const int compile_status = run_command(compile_command);
    if (compile_status != 0) {
        return {false, "compile failed"};
    }

    std::cout << "Running " << exercise.name << "..." << std::endl;
    const int run_status = run_command(shell_quote(binary.string()));
    if (run_status != 0) {
        return {false, "program exited with a failure"};
    }

    return {true, "passed"};
}

RunResult check_exercise(const fs::path &root, const Exercise &exercise, bool quiet) {
    if (!quiet) {
        std::cout << "\n==> " << exercise.name << std::endl;
    }

    RunResult result = run_exercise(root, exercise);

    if (!quiet) {
        std::cout << (result.ok ? "PASS: " : "FIX ME: ") << result.message << "\n";
    }

    return result;
}

bool exercise_looks_done(const fs::path &root, const Exercise &exercise) {
    const fs::path source = work_path(root, exercise);
    return fs::exists(source) && !has_not_done_marker(source);
}

std::optional<Exercise> next_exercise(const fs::path &root,
                                      const std::vector<Exercise> &exercises) {
    for (const Exercise &exercise : exercises) {
        if (!exercise_looks_done(root, exercise)) {
            return exercise;
        }
    }

    return std::nullopt;
}

std::map<fs::path, fs::file_time_type> snapshot_exercise_times(
    const fs::path &root,
    const std::vector<Exercise> &exercises) {
    std::map<fs::path, fs::file_time_type> snapshot;

    for (const Exercise &exercise : exercises) {
        const fs::path path = work_path(root, exercise);
        if (fs::exists(path)) {
            snapshot[path] = fs::last_write_time(path);
        }
    }

    return snapshot;
}

void print_help() {
    std::cout
        << "Cpplings: a Rustlings-style C++ exercise runner\n\n"
        << "Usage:\n"
        << "  cpplings                 Show the next exercise\n"
        << "  cpplings init            Create missing editable exercise copies\n"
        << "  cpplings list            List exercises\n"
        << "  cpplings run <name>      Compile and run an exercise\n"
        << "  cpplings verify          Check all exercises in order\n"
        << "  cpplings next            Check the next unfinished exercise\n"
        << "  cpplings hint [name]     Show a hint\n"
        << "  cpplings reset <name>    Reset one editable exercise copy\n"
        << "  cpplings watch           Re-run verify when exercise files change\n"
        << "  cpplings help            Show this help\n";
}

void print_next(const fs::path &root, const std::vector<Exercise> &exercises) {
    const std::optional<Exercise> next = next_exercise(root, exercises);

    if (!next) {
        std::cout << "Every exercise is missing the unfinished marker. Run `cpplings verify` for the real test.\n";
        return;
    }

    std::cout << "Next exercise: " << next->name << "\n";
    std::cout << "Open: " << work_path(root, *next) << "\n";
    std::cout << "Run:  cpplings run " << next->name << "\n";
}

int command_list(const fs::path &root, const std::vector<Exercise> &exercises) {
    std::cout << "Exercises\n";
    for (const Exercise &exercise : exercises) {
        const bool looks_done = exercise_looks_done(root, exercise);
        std::cout << "  [" << status_label(looks_done) << "] "
                  << exercise.name << "  " << work_path(root, exercise).string() << "\n";
    }
    return 0;
}

int command_init(const fs::path &root, const std::vector<Exercise> &exercises) {
    const int copied = initialize_work_tree(root, exercises, false);
    if (copied == 0) {
        std::cout << "Exercise work tree already exists at " << (root / kWorkRoot) << "\n";
    } else {
        std::cout << "Created " << copied << " editable exercise "
                  << (copied == 1 ? "copy" : "copies")
                  << " under " << (root / kWorkRoot) << "\n";
    }
    return 0;
}

int command_reset(const fs::path &root,
                  const std::vector<Exercise> &exercises,
                  const std::vector<std::string> &args) {
    if (args.size() < 3) {
        std::cerr << "usage: cpplings reset <name>\n";
        return 2;
    }

    const std::optional<Exercise> exercise = find_exercise(exercises, args[2]);
    if (!exercise) {
        std::cerr << "unknown exercise: " << args[2] << "\n";
        return 2;
    }

    const fs::path from = template_path(root, *exercise);
    const fs::path to = work_path(root, *exercise);
    fs::create_directories(to.parent_path());
    fs::copy_file(from, to, fs::copy_options::overwrite_existing);

    std::cout << "Reset " << exercise->name << "\n";
    std::cout << "Open: " << to << "\n";
    return 0;
}

int command_run(const fs::path &root,
                const std::vector<Exercise> &exercises,
                const std::vector<std::string> &args) {
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
    return result.ok ? 0 : 1;
}

int command_verify(const fs::path &root, const std::vector<Exercise> &exercises) {
    for (const Exercise &exercise : exercises) {
        const RunResult result = check_exercise(root, exercise, false);
        if (!result.ok) {
            std::cout << "\nOpen: " << work_path(root, exercise) << "\n";
            std::cout << "Hint: cpplings hint " << exercise.name << "\n";
            return 1;
        }
    }

    std::cout << "\nAll exercises passed. Nice work.\n";
    return 0;
}

int command_next(const fs::path &root, const std::vector<Exercise> &exercises) {
    const std::optional<Exercise> next = next_exercise(root, exercises);
    if (!next) {
        return command_verify(root, exercises);
    }

    std::cout << "Next exercise: " << next->name << "\n";
    std::cout << "Open: " << work_path(root, *next) << "\n";
    const RunResult result = check_exercise(root, *next, false);
    if (!result.ok) {
        std::cout << "\nOpen: " << work_path(root, *next) << "\n";
        std::cout << "Hint: cpplings hint " << next->name << "\n";
    }
    return result.ok ? 0 : 1;
}

int command_hint(const fs::path &root,
                 const std::vector<Exercise> &exercises,
                 const std::vector<std::string> &args) {
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
            std::cout << "No unfinished exercise found. Try `cpplings verify`.\n";
            return 0;
        }
    }

    std::cout << exercise->name << "\n";
    std::cout << exercise->hint << "\n";
    return 0;
}

int command_watch(const fs::path &root, const std::vector<Exercise> &exercises) {
    std::cout << "Watching exercises. Press Ctrl-C to stop.\n";
    std::map<fs::path, fs::file_time_type> last_snapshot =
        snapshot_exercise_times(root, exercises);

    (void)command_verify(root, exercises);

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(700));
        const std::map<fs::path, fs::file_time_type> current_snapshot =
            snapshot_exercise_times(root, exercises);

        if (current_snapshot != last_snapshot) {
            last_snapshot = current_snapshot;
            std::cout << "\nChange detected.\n";
            (void)command_verify(root, exercises);
        }
    }
}

} // namespace

int main(int argc, char **argv) {
    try {
        const fs::path root = find_project_root();
        const std::vector<Exercise> exercises = load_manifest(root);

        std::vector<std::string> args;
        for (int i = 0; i < argc; ++i) {
            args.emplace_back(argv[i]);
        }

        if (args.size() > 1 && args[1] == "init") {
            return command_init(root, exercises);
        }

        ensure_work_tree(root, exercises);

        if (args.size() == 1) {
            print_help();
            std::cout << "\n";
            print_next(root, exercises);
            return 0;
        }

        const std::string command = args[1];

        if (command == "help" || command == "--help" || command == "-h") {
            print_help();
            return 0;
        }
        if (command == "list") {
            return command_list(root, exercises);
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
        print_help();
        return 2;
    } catch (const std::exception &error) {
        std::cerr << "error: " << error.what() << "\n";
        return 1;
    }
}
