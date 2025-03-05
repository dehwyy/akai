#pragma once
#include <cmath>
#include <set>
#include <string>
#include <vector>
#include "lib/prelude.hpp"
#include "variables.hpp"

using namespace variables;

namespace function {

    class Function {
        protected:
            std::set<std::string> dependantVariables = {};

        public:
            std::set<std::string> GetDependantVariables() const { return dependantVariables; };

            virtual ~Function() = default;
            virtual Result<double> GetValue(VariablesContainer& vars) const = 0;
    };

    using F = Box<Function>;

    class Operation : public Function {
        private:
            std::set<std::string> dependantVariables = {};
            Option<Rc<Function>> leftFn;
            Option<Rc<Function>> rightFn;
            char op;  // "+", "-", "*", "/", "^" (TODO)

        public:
            Operation(char op, Box<Function> l, Box<Function> r) : op(op) {
                for (auto& v : l.get()->GetDependantVariables()) {
                    dependantVariables.insert(v);
                }
                for (auto& v : r.get()->GetDependantVariables()) {
                    dependantVariables.insert(v);
                }

                leftFn = Option<Rc<Function>>::Some(std::move(l));
                rightFn = Option<Rc<Function>>::Some(std::move(r));
            }

            Result<double> GetValue(VariablesContainer& vars) const override {
                std::cout << "call1" << std::endl;
                Result<double> lValue = leftFn.unwrapRef().get()->GetValue(vars);
                Result<double> rValue = rightFn.unwrapRef().get()->GetValue(vars);
                std::cout << "call2" << std::endl;
                auto [lv, err1] = lValue.unwrapRef();
                if (err1) {
                    return Result<double>::Err(err1);
                }

                auto [rv, err] = rValue.unwrapRef();
                if (err) {
                    return Result<double>::Err(err);
                }

                switch (this->op) {
                    case '+':
                        return Result<double>::Ok(lv + rv);
                    case '-':
                        return Result<double>::Ok(lv - rv);
                    case '*':
                        return Result<double>::Ok(lv * rv);
                    case '/':
                        return Result<double>::Ok(lv / rv);
                    case '^':
                        return Result<double>::Ok(std::pow(lv, rv));
                    default:
                        return Result<double>::Err("Unknown operation");
                }
            }
    };

    /// @brief Only "f(x) = x"
    class LinearVariable : public Function {
        private:
            std::string var;

        public:
            // LinearVariable(std::string var) { this->dependantVariables.insert(var); }
            LinearVariable(char var) : var(std::string(1, var)) {
                this->var = std::string(1, var);
                this->dependantVariables.insert(this->var);
            }

            Result<double> GetValue(VariablesContainer& vars) const override {
                auto value = vars.get(var);
                if (value.isNone()) {
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

            Result<double> GetValue(VariablesContainer&) const override {
                return Result<double>::Ok(value);
            }
    };
}  // namespace function
