#pragma once
#include <cmath>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>
#include "lib.hpp"
#include "variables.hpp"

using namespace variables;

namespace function {

    class Function {
        protected:
            std::set<std::string> dependantVariables = {};

        public:
            std::set<std::string> GetDependantVariables() const { return dependantVariables; };

            virtual ~Function() = default;
            virtual Result<double> GetValue(const VariablesContainer& vars) const = 0;
    };

    using F = Box<Function>;

    class Operation : public Function {
        private:
            std::set<std::string> dependantVariables = {};
            Box<Function> leftFn;
            Box<Function> rightFn;
            char op;  // "+", "-", "*", "/", "^" (TODO)

        public:
            Operation(char op, Box<Function>&& l, Box<Function>&& r) : op(op) {
                for (auto& v : l->GetDependantVariables()) {
                    dependantVariables.insert(v);
                }
                for (auto& v : r->GetDependantVariables()) {
                    dependantVariables.insert(v);
                }

                leftFn = std::move(l);
                rightFn = std::move(r);
            }

            Result<double> GetValue(const VariablesContainer& vars) const override {
                Result<double> lValue = leftFn->GetValue(vars);
                Result<double> rValue = rightFn->GetValue(vars);

                auto [lv, err1] = lValue.get();
                if (err1) {
                    return Result<double>::Err(err1);
                }

                auto [rv, err] = rValue.get();
                if (err) {
                    return Result<double>::Err(err);
                }

                double r;

                switch (this->op) {
                    case '+':
                        r = lv + rv;
                        break;
                    case '-':
                        r = lv - rv;
                        break;
                    case '*':
                        r = lv * rv;
                        break;
                    case '/':
                        // Log::Print("rv: ", rv, "lv: ", lv);
                        if (rv == 0) {
                            return Result<double>::Err("Division by zero");
                        }
                        r = lv / rv;
                        break;
                    case '^':
                        r = std::pow(lv, rv);
                        break;
                    default:
                        return Result<double>::Err("Unknown operation");
                }

                return Result<double>::Ok(r);
            }
    };

    /// @brief Only "f(x) = x"
    class LinearVariable : public Function {
        private:
            std::string var;

        public:
            // LinearVariable(std::string var) { this->dependantVariables.insert(var); }
            LinearVariable(char var) : var(std::string(1, var)) {
                // Log::Print("var constructor");
                this->var = std::string(1, var);
                this->dependantVariables.insert(this->var);
            }

            Result<double> GetValue(const VariablesContainer& vars) const override {
                auto value = vars.get(var);

                if (value.isNone()) {
                    Log::Warn("Value: None");
                    return Result<double>::Err("Variable not found");
                }

                return Result<double>::Ok(value.unwrap());
            };
    };

    class Constant : public Function {
        private:
            double value;

        public:
            Constant(double value) { this->value = value; }

            Result<double> GetValue(const VariablesContainer&) const override {
                return Result<double>::Ok(value);
            }
    };

    class MathFunction : public Function {
        private:
            const static std::unordered_map<std::string, double (*)(double)> mathFunctions;
            std::string name;
            Box<Function> inner;

        public:
            MathFunction(std::string name, Box<Function>&& inner)
                : name(name), inner(std::move(inner)) {}

            /// @brief Clarifies whether function with such name exists
            /// @param name function name
            bool Exists(std::string name) { return mathFunctions.find(name) != mathFunctions.end(); }

            Result<double> GetValue(const VariablesContainer& vars) const override {
                auto [x, err] = inner->GetValue(vars).get();
                if (err) {
                    return Result<double>::Err(err);
                }
                double r = 0;

                // TODO: make a hashmap instead: [name: string, function_callback: double(*)(double) ]
                if (name == "sin") {
                    r = std::sin(x);
                } else if (name == "cos") {
                    r = std::cos(x);
                } else if (name == "tan") {
                    r = std::tan(x);
                } else if (name == "tg") {
                    r = std::tan(x);
                } else if (name == "sqrt") {
                    r = std::sqrt(x);
                } else if (name == "exp") {
                    r = std::exp(x);
                } else if (name == "ln") {
                    r = std::log(x);
                } else if (name == "lg") {
                    r = std::log10(x);
                } else if (name == "abs") {
                    r = std::fabs(x);
                } else {
                    return Result<double>::Err("Unknown function");
                }

                return Result<double>::Ok(r);
            }
    };

    const std::unordered_map<std::string, double (*)(double)> MathFunction::mathFunctions = {
        {"sin", std::sin},
        {"cos", std::cos},
        {"tan", std::tan},
        {"tg", std::tan},
        {"sqrt", std::sqrt},
        {"exp", std::exp},
        {"ln", std::log},
        {"lg", std::log10},
        {"abs", std::fabs},
        {"arctg", std::atan},
        {"arcsin", std::asin},
        {"arccos", std::acos},
        {"arctg", std::atan},
    };
}  // namespace function
