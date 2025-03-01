#pragma once
#include <string>

namespace state {
    class State {
        private:
            std::string functionInput = "";

        public:
            State() = default;
            ~State() = default;

            void SetFunctionInput(char* input) { functionInput = {input}; }
            std::string GetFunctionInput() { return functionInput; }
    };
}  // namespace state
