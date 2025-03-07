#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "lib/prelude.hpp"

namespace variables {
    class VariablesContainer : public serialize::Serializable {
        private:
            std::unordered_map<std::string, double> variables;

        public:
            VariablesContainer() { std::cout << "VariablesContainer created" << std::endl; }
            ~VariablesContainer() { std::cout << "VariablesContainer destroyed" << std::endl; }

            void put(std::string name, double value) {
                Log::Trace("name: ", name, " value: ", value);
                variables.insert_or_assign(name, value);
            }
            Option<double> get(std::string name) {
                return Option<double>::TryFrom([this, &name]() { return variables.at(name); });
            }

            std::string String() override {
                auto ser = serialize::SerializerBuilder().withTitle("VariablesContainer");
                for (auto& [key, value] : variables) {
                    ser->addField(key, value);
                }

                return ser->build()->String();
            }
    };
}  // namespace variables
