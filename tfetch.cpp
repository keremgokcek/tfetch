#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <pwd.h>
#include <unistd.h>

std::string exec(const char* cmd) {

    std::array<char, 128> buffer{};  // Create an array.
    std::string result;  // Create a string variable for returning it later.
    auto pipe = popen(cmd, "r");  // Execute command and assign its contents to pipe variable.

    if (!pipe) throw std::runtime_error("popen() failed!");  // if pipe variable is empty throw exception.

    // Read contents.
    while (!feof(pipe)) {
        if (fgets(buffer.data(), 128, pipe) != nullptr)
            result += buffer.data();
    }

    return result;

}

std::string get_computer_name() {

    std::ifstream file("/sys/devices/virtual/dmi/id/product_name"); // Check the file containing the computer name.
    std::string text, line;  // Create text variable for returning it, create line variable for assigning it to each line of file.

    // Merge all lines to one line.
    while (getline(file, line)) {
        text += line;
    }

    return text;
}

std::string get_cpu_name() {

    std::ifstream file("/proc/cpuinfo");  // Check /proc/cpuinfo file.
    std::string line;  // Create variable for assigning it to each line of file.

    getline(file, line);  // Get first line of file and assign it to line.

    // Find line named model name from file.
    while (line.find("model name") == std::string::npos) {
        getline(file, line);
    }

    // Erase unnecessary parts from string.
    line.erase(0, 13);

    return line;

}

std::string get_distro_name() {

    std::ifstream file("/etc/os-release");  // Check /etc/os-release file for distro name.
    std::string line;  // Create variable for assigning it to each line of file.

    getline(file, line);  // Get first line of file and assign it to line.

    // Find line named PRETTY_NAME from file.
    while (line.find("PRETTY_NAME") == std::string::npos) {
        getline(file, line);
    }

    // Erase unnecessary parts from string.
    line.erase(line.find("PRETTY_NAME"), line.find('=') + 1);
    line.erase(0, 1);
    line.erase(line.size() - 1, line.size());

    return line;

}

int get_ram_total() {

    std::ifstream file("/proc/meminfo");  // Check /proc/meminfo file.
    std::string line;  // Create variable for assigning it to each line of file.
    int ram_total;  // Create int for assigning it total amount of RAM.

    getline(file, line);  // Get first line of file and assign it to line.

    // Find line named MemTotal: from file.
    while (line.find("MemTotal:") == std::string::npos) {
        getline(file, line);
    }

    // Erase unnecessary parts from string.
    line.erase(0, line.find(':') + 1);
    line.erase(line.find("kB") - 1, line.find("kB") + 2);

    // Erase spaces in string.
    while (line.front() == ' ') {
        line.erase(0, 1);
    }

    // Convert string to int for returning.
    ram_total = std::stoi(line);

    return ram_total / 1024;

}

int get_ram_used() {

    std::ifstream file("/proc/meminfo");  // Check /proc/meminfo file.
    std::string line;  // Create variable for assigning it to each line of file.
    int ram_used;  // Create int for assigning it RAM usage.

    getline(file, line);  // Get first line of file and assign it to line.

    // Find line named MemAvailable: from file.
    while (line.find("MemAvailable:") == std::string::npos) {
        getline(file, line);
    }

    // Erase unnecessary parts from string.
    line.erase(0, line.find(':') + 1);
    line.erase(line.find("kB") - 1, line.find("kB") + 2);

    // Erase spaces in string.
    while (line.front() == ' ') {
        line.erase(0, 1);
    }

    // Convert string to int for returning.
    ram_used = std::stoi(line);

    return  get_ram_total() - ram_used / 1024;

}

std::string get_kernel() {

    // Get kernel.
    std::string text = exec("uname -r");

    // Erase line breaks.
    while (text.find('\n') != std::string::npos) {
        text.erase(text.find('\n'), text.find('\n') + 1);
    }

    return text;

}

std::string get_arch() {

    // Get architecture.
    std::string text = exec("uname -m");

    // Erase line breaks.
    while (text.find('\n') != std::string::npos) {
        text.erase(text.find('\n'), text.find('\n') + 1);
    }

    return text;

}

std::string get_terminal() {

    // Get terminal name.
    std::string text = exec("ps -o comm= -p \"$(($(ps -o ppid= -p \"$(($(ps -o sid= -p \"$$\")))\")))\"");

    // Erase line breaks.
    while (text.find('\n') != std::string::npos) {
        text.erase(text.find('\n'), text.find('\n') + 1);
    }

    // Check if terminal is gnome-terminal.
    if (text == "gnome-terminal-") {
        text = "gnome-terminal";
    }

    return text;

}

std::string get_uptime() {

    std::string uptime = exec("uptime -p 2>&1");  // Check the uptime -p command.
    std::string text;  // Create string variable for return it later.
    int values[2];  // Create int array for time.

    if (uptime.find("invalid") == std::string::npos) {  // Check if there -p parameter for uptime.

        // Erase unnecessary parts from string.
        uptime.erase(0, uptime.find("up") + 3);

        // Erase line breaks.
        while (uptime.find('\n') != std::string::npos) {
            uptime.erase(uptime.find('\n'), uptime.find('\n') + 1);
        }

        return uptime;

    } else {  // If isn't there -p parameter for uptime command.

        uptime = exec("uptime");  // Check output of uptime without parameter -p

        // Erase line breaks.
        while (uptime.find('\n') != std::string::npos) {
            uptime.erase(uptime.find('\n'), uptime.find('\n') + 1);
        }

        // Erase unnecessary parts from string.
        uptime.erase(0, uptime.find("up") + 2);
        uptime.erase(uptime.find(','), uptime.rfind(uptime.back()));

        // Erase spaces on string.
        while (uptime.find(' ') != std::string::npos) {
            uptime.erase(uptime.find(' '), uptime.find(' ') + 1);
        }

        values[0] = std::stoi(uptime.substr(0, uptime.find(':')));  // Assign hour part to values[0].
        uptime.erase(0, uptime.find(':') + 1);  // Erase the part we assigned.
        values[1] = std::stoi(uptime);  // Assign minute part to values[1].

        if (values[0] == 0) {  // Check if hour is 0.
            text = std::to_string(values[1]) + " minutes";  // Make it only minutes.
        } else {
            text = std::to_string(values[0]) + " hours " + std::to_string(values[1]) + " minutes";  // Make it hours and minutes.
        }

        return text;
    }

}

std::string* get_resolution() {

    auto* res = new std::string[8];  // Create string pointer that points to a new string array.
    std::string output = exec("xrandr -q  2>&1 | grep \' connected\'");  // Get screen resolutions.
    std::istringstream text(output);  // Create string stream for reading line by line.
    std::string line;  // Create string for assign to each line.

    // Check all lines on output string.
    for (int i = 0; getline(text, line); i++) {

        // Erase unnecessary parts.
        line.erase(0, line.rfind(' ', line.find('x')));
        line.erase(line.find('+'), line.rfind(line.back()));

        // Erase spaces on string.
        while (line.find(' ') != std::string::npos) {
            line.erase(line.find(' '), line.find(' ') + 1);
        }

        // Assign line to our res string array.
        res[i] = line;

    }

    if (res[0].empty()) {
        res[0] = '-';
    }

    return res;

}

std::string get_shell() {

    // Get shell.
    std::string shell = getpwuid(geteuid())->pw_shell;

    // Erase directory names from path.
    shell.erase(0, shell.rfind('/') + 1);

    return shell;

}

std::string get_de() {

    // Define variable.
    std::string de;

    // Check if is there desktop environment.
    if (std::getenv("XDG_CURRENT_DESKTOP") == nullptr) {
        de = '-';
    } else {
        de = std::getenv("XDG_CURRENT_DESKTOP");
    }

    // Check if desktop environment is GNOME.
    if (de.find("GNOME") != std::string::npos) {
        de = "GNOME";
    }

    return de;

}

std::string get_wm() {

    // Detect window manager.
    std::string wm = exec("ps -e | grep -m 1 -o -F -e arcan -e asc -e clayland -e dwc -e fireplace -e gnome-shell"
                          " -e greenfield -e grefsen -e hikari -e kwin -e lipstick -e maynard -e mazecompositor"
                          " -e motorcar -e orbital -e orbment -e perceptia -e river -e rustland -e sway -e ulubis"
                          " -e velox -e wavy -e way-cooler -e wayfire -e wayhouse -e westeros -e westford -e weston"
                          " -e i3 -e dwm -e openbox");

    // Erase line breaks.
    while (wm.find('\n') != std::string::npos) {
        wm.erase(wm.find('\n'), wm.find('\n') + 1);
    }

    if (wm.empty()) {  // Check if variable empty.
        wm = '-';
    } else if (wm.find("gnome-shell") != std::string::npos) {  // Check if window manager is gnome-shell.
        wm = "Mutter";
    }

    return wm;

}

int main(int argc, char* argv[]) {

    // Declare variables.
    std::string computer_name = get_computer_name();
    std::string cpu_name = get_cpu_name();
    std::string distro = get_distro_name();
    int ram_used = get_ram_used();
    int ram_total = get_ram_total();
    std::string kernel = get_kernel();
    std::string arch = get_arch();
    std::string terminal = get_terminal();
    std::string uptime = get_uptime();
    std::string* resolution = get_resolution();
    std::string shell = get_shell();
    std::string de = get_de();
    std::string wm = get_wm();
    std::string clear = "--clear";

    for (int i = 0; i < argc; ++i)  // Clear the console if there is --clear parameter.
        if (argv[i] == clear) {
            system("clear");
            break;
        }

    // Print variables.
    std::cout << "\x1b[1m\x1b[34m    _________________     " << "\x1b[32mComputer Name: \x1b[0m\x1b[34m" << computer_name << std::endl;
    std::cout << "\x1b[1m _#####################_  " << "\x1b[32mCPU Name:      \x1b[0m\x1b[34m" << cpu_name << std::endl;
    std::cout << "\x1b[1m######################### " << "\x1b[32mDistro:        \x1b[0m\x1b[34m" << distro << std::endl;
    std::cout << "\x1b[1m########\"################ " << "\x1b[32mRAM:           \x1b[0m\x1b[34m" << ram_used << " MB / " << ram_total << " MB" << std::endl;
    std::cout << "\x1b[1m########   \"\"############ " << "\x1b[32mKernel:        \x1b[0m\x1b[34m" << kernel << std::endl;
    std::cout << "\x1b[1m########       \"\"######## " << "\x1b[32mArchitecture:  \x1b[0m\x1b[34m" << arch << std::endl;
    std::cout << "\x1b[1m########       __######## " << "\x1b[32mTerminal:      \x1b[0m\x1b[34m" << terminal << std::endl;
    std::cout << "\x1b[1m########   __############ " << "\x1b[32mUptime:        \x1b[0m\x1b[34m" << uptime << std::endl;

    for (int i = 0; !resolution[i].empty(); i++) {
        if (i == 0) {
            std::cout << "\x1b[1m########_################ " << "\x1b[32mResolution " << i + 1 << ":  \x1b[0m\x1b[34m" << resolution[i] << std::endl;
        } else {
            std::cout << "\x1b[1m######################### " << "\x1b[32mResolution " << i + 1 << ":  \x1b[0m\x1b[34m" << resolution[i] << std::endl;
        }
    }

    if (resolution[1].empty()) {
        std::cout << "\x1b[1m######################### " << "\x1b[32mShell:         \x1b[0m\x1b[34m" << shell << std::endl;
        std::cout << "\x1b[1m \"#####################\"  " << "\x1b[32mDE:            \x1b[0m\x1b[34m" << de << std::endl;
        std::cout << "\x1b[1m     \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"      " << "\x1b[32mWM:            \x1b[0m\x1b[34m" << wm << std::endl;
    } else {
        std::cout << "\x1b[1m \"#####################\"  " << "\x1b[32mShell:         \x1b[0m\x1b[34m" << shell << std::endl;
        std::cout << "\x1b[1m     \"\"\"\"\"\"\"\"\"\"\"\"\"\"\"      " << "\x1b[32mDE:            \x1b[0m\x1b[34m" << de << std::endl;
        std::cout << "\x1b[1m\x1b[32m                          WM:            \x1b[0m\x1b[34m" << wm << std::endl;
    }

    delete[] resolution;  // Deleting object for not to make memory leak.

    return 0;

}
