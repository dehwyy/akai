#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include "lib.hpp"

using namespace logger;

namespace variables {
    class VariablesContainer : public serialize::Serializable {
        private:
            std::unordered_map<std::string, double> variables;

        public:
            VariablesContainer() { Log::Trace("VariablesContainer()"); }
            ~VariablesContainer() { Log::Trace("~VariablesContainer()"); }

            void put(std::string name, double value) {
                Log::Trace("VariablesContainer::put(name, value), name='", name, "', value=", value);
                variables.insert_or_assign(name, value);
            }
            Option<double> get(std::string name) const {
                try {
                    // TODO: make inline?
                    double varValue = variables.at(name);
                    return Option<double>::Some(std::move(varValue));
                } catch (const std::out_of_range& e) {
                    return Option<double>::None();
                }
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
