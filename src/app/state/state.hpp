#pragma once
#include <lib.hpp>
#include <set>
#include <string>
#include "function/function.hpp"
#include "function/parser.hpp"
#include "function/variables.hpp"

namespace state {
    class State {
        private:
            std::string functionInput = "x";
            std::string previousRenderFunctionInput = "";
            variables::VariablesContainer variables;
            Option<Rc<function::Function>> function;

            inline void updatePreviousRenderFunctionInput() {
                previousRenderFunctionInput = functionInput;
            }
            void updateFunctionVariablesDefinition(const std::set<std::string>& variables) {
                // Log::Info("New function variables: ", variables);
                for (const auto& var : variables) {
                    if (!this->variables.exists(var)) {
                        this->variables.put(var, 1.);
                    }
                }
            }

            inline bool shouldRebuildFunction() const {
                return functionInput != previousRenderFunctionInput;
            }

        public:
            State() = default;
            ~State() = default;

            void updateFunctionIfNecessary() {
                if (shouldRebuildFunction()) {
                    updatePreviousRenderFunctionInput();
                    try {
                        // function::F f = parser::TokensSequence(functionInput).AsFunction();
                        // updateFunctionVariablesDefinition(f->GetDependantVariables());
                        // function.set(std::move(f));

                    } catch (const std::exception& e) {
                        // Log::Error(e.what());
                        function.reset();
                    }
                }
            }

            inline Option<Rc<function::Function>>& getFunction() { return function; }

            inline std::string& getFunctionInput() { return functionInput; }
            inline variables::VariablesContainer& getFunctionVariables() { return variables; }
    };
}  // namespace state
