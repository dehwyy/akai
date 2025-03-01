#pragma once
#include <memory>
#include <string>
#include <unordered_map>

namespace variable {
    class Variable {
        private:
            std::string name;
            double value;

        public:
            Variable(std::string name, double value) : name(std::move(name)), value(value) {}

            void SetValue(double value) { this->value = value; }
            double GetValue() { return value; }
    };

    class Variables {
        private:
            std::unordered_map<std::string, std::unique_ptr<Variable>> variables;

        public:
            void SetVariable(std::string name, double value) {
                variables[name] = std::make_unique<Variable>(name, value);
            }

            double GetVariable(std::string name) { return variables[name]->GetValue(); }
    };
}  // namespace variable
