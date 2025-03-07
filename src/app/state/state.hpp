#pragma once
#include <lib.hpp>
#include <string>

namespace state {
    class State {
        private:
            std::string functionInput = "";
            std::string previousRenderFunctionInput = "";

        public:
            State() = default;
            ~State() = default;

            void SetFunctionInput(char* input) {
                std::string s = {input};
                if (s == functionInput) {
                    return;
                }

                previousRenderFunctionInput = functionInput;
                functionInput = s;
            }
            std::string GetFunctionInput() { return functionInput; }
            std::string GetPreviousRenderFunctionInput() { return previousRenderFunctionInput; }
            bool ShouldRebuildFunction() { return functionInput != previousRenderFunctionInput; }
            void UpdatePreviousRenderFunctionInput() { previousRenderFunctionInput = functionInput; }
    };
}  // namespace state
