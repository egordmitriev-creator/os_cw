#include "task_manager.h"
#include <string>
#include <sstream>
#include <iostream>
#include <array>
#include <memory>

std::string get_task_list() {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen("ps aux", "r"), fclose);
    
    if (!pipe) {
        return "Failed to get task list";
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    return result;
}
