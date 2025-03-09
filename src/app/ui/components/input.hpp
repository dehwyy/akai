#pragma once

#include <iostream>
#include <string>
#include "deps/imgui-sfml/imgui.h"

namespace {
    const int BUFFER_SIZE = 256;
}

namespace input {
    class Input {
        private:
        public:
            Input(const std::string& label, std::string& value) {
                static char buffer[BUFFER_SIZE];
                if (ImGui::InputText(label.c_str(), buffer, BUFFER_SIZE)) {
                    std::cout << buffer << std::endl;
                    value = buffer;
                }
            }
    };
}  // namespace input
