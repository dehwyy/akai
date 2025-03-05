#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "lib/prelude.hpp"

namespace variables {
    // class Variable {
    //     private:
    //         std::string name;
    //         double value;

    //     public:
    //         Variable(std::string name, double value) : name(std::move(name)), value(value) {}

    //         void SetValue(double value) { this->value = value; }
    //         double GetValue() { return value; }
    // };

    // class Variables {
    //     private:
    //         std::unordered_map<std::string, std::unique_ptr<Variable>> variables;

    //     public:
    //         void SetVariable(std::string name, double value) {
    //             variables[name] = std::make_unique<Variable>(name, value);
    //         }

    //         double GetVariable(std::string name) { return variables[name]->GetValue(); }
    // };

    class VariablesContainer {
        private:
            std::unordered_map<std::string, double> variables;

        public:
            VariablesContainer() { std::cout << "VariablesContainer created" << std::endl; }
            ~VariablesContainer() { std::cout << "VariablesContainer destroyed" << std::endl; }

            void put(std::string name, double value) { variables.emplace(name, value); }
            Option<double> get(std::string name) {
                return Option<double>::TryFrom([this, &name]() { return variables.at(name); });
            }
    };
}  // namespace variables
