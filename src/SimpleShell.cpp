#include "../include/SimpleShell.h"

using namespace std;

void SimpleShell::execute(vector<string>& argv)
{
    int status;
    pid_t child;

    // Spawning a child process
    child = fork();

    // Parent process portion
    if (child > 0) {
        waitpid(child, &status, 0); // Wait for child process to finish
    }
    // Child process portion
    else if (child == 0) {
        // Ensure there's at least one argument
        if (argv.empty()) {
            cerr << "Error: No command to execute" << endl;
            _exit(1);
        }

        if (argv[0] == "cd") {
            if(argv[1].rfind("~/", 0) == 0) {
                //argv[1] = getFullPath(argv[1].erase(0, 1));
                const char* home = getenv("HOME");
                if (home == nullptr) {
                    std::cerr << "Error: HOME environment variable not set." << std::endl;
                    _exit(1);
                }
                argv[1] = std::string(home) + argv[1].erase(0, 1);
            }
            if (chdir((argv[1]).c_str()) == -1) {
                perror("chdir failed");  // Print error message if chdir fails
                _exit(1);
            }
        } else if (argv[0] == "mkdir") {
            // Change to the specified directory
            if (mkdir((argv[1]).c_str(), 0755) == -1) {
                perror("chdir failed");  // Print error message if chdir fails
                _exit(1);
            }

        }else {
            string exec_path = "/bin/" + argv[0];

            // Prepare arguments for exec (must end with nullptr)
            vector<const char*> args;
            for (const auto& arg : argv) {
                args.push_back(arg.c_str());
            }
            args.push_back(nullptr);
            if (execl(exec_path.c_str(), args[0], args[1], args[2]) == -1) {
            //if (execv(exec_path.c_str(), const_cast<char* const*>(args.data())) == -1) {
                perror("execv failed"); // Print error message
                _exit(1);
            }
        }
    }
    else {
        perror("fork failed"); // Error handling if fork fails
        exit(1);
    }
}

void SimpleShell::parse(const string& line, vector<string>& tokens, const string& delimiter)
{
    size_t start = 0;
    size_t end = 0;
    
    while ((end = line.find(delimiter, start)) != string::npos) {
        if (end != start) { // Ignore empty tokens
            tokens.push_back(line.substr(start, end - start));
        }
        start = end + delimiter.length();
    }
    
    if (start < line.length()) { // Add the last token
        tokens.push_back(line.substr(start));
    }
}

void SimpleShell::run()
{
    while (true) {
        string line;
        vector<string> tokens;

        // Print the prompt
        cout << "(" << getpid() << ") % ";

        // Get input from the keyboard
        if (!getline(cin, line)) {
            break; // Exit the shell if input fails (e.g., EOF)
        }

        // Parse the input into tokens
        parse(line, tokens, " ");

        for(const auto& arg : tokens) {
            cout << "arg: " << arg << endl;
        }

        if (tokens.empty()) {
            continue; // Skip empty input
        }

        // Execute the user command
        execute(tokens);
    }
}

int main()
{
    SimpleShell shell;
    shell.run(); // Start the shell loop
    return 0;
}
