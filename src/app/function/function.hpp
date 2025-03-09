#pragma once
#include <cmath>
#include <functional>
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
                    // Log::Warn("Value: None");
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
            static bool Exists(std::string name) {
                return mathFunctions.find(name) != mathFunctions.end();
            }

            Result<double> GetValue(const VariablesContainer& vars) const override {
                if (!Exists(name)) {
                    return Result<double>::Err("Unknown function");
                }

                auto [x, err] = inner->GetValue(vars).get();
                if (err) {
                    return Result<double>::Err(err);
                }

                double r = 0;
                std::function<double(double)> f = mathFunctions.at(name);

                return Result<double>::Ok(f(x));
            }
    };

    const std::unordered_map<std::string, double (*)(double)> MathFunction::mathFunctions = {
        {"sin", std::sin},
        {"arcsin", std::asin},
        {"sinh", std::sinh},
        {"arcsinh", std::asinh},

        {"cosh", std::cosh},
        {"cos", std::cos},
        {"arccos", std::acos},
        {"arccosh", std::acosh},

        {"tan", std::tan},
        {"tg", std::tan},
        {"tanh", std::tanh},
        {"tgh", std::tanh},
        {"arctg", std::atan},
        {"arctan", std::atan},
        {"arctanh", std::atanh},
        {"arctgh", std::atanh},

        {"cot", [](double v) { return 1 / std::tan(v); }},
        {"ctg", [](double v) { return 1 / std::tan(v); }},
        {"arccot", [](double v) { return std::atan(1 / v); }},
        {"arcctg", [](double v) { return std::atan(1 / v); }},

        {"sec", [](double v) { return 1 / std::cos(v); }},
        {"arcsec", [](double v) { return std::acos(1 / v); }},

        {"csc", [](double v) { return 1 / std::sin(v); }},
        {"arccsc", [](double v) { return std::asin(1 / v); }},

        {"sqrt", std::sqrt},
        {"exp", std::exp},
        {"ln", std::log},
        {"log", std::log},
        {"lg", std::log10},
        {"abs", std::fabs},
        {"floor", std::floor},
        {"ceil", std::ceil},
    };
}  // namespace function
