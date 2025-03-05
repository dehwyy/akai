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
            Option<Box<Function>> nested = Option<Box<Function>>::None();
            std::set<std::string> dependantVariables = {};

        public:
            Function() = default;
            std::set<std::string> GetDependantVariables() const { return dependantVariables; }

            virtual ~Function() = default;
            virtual Result<double> GetValue(VariablesContainer& vars) const = 0;
    };

    using F = Box<Function>;

    class Operation : public Function {
        private:
            Option<Box<Function>> leftFn;
            Option<Box<Function>> rightFn;
            char op;  // "+", "-", "*", "/", "^" (TODO)

        public:
            Operation(char op, Box<Function> l, Box<Function> r) : op(op) {
                std::set_union(l.get()->GetDependantVariables().begin(),
                               l.get()->GetDependantVariables().end(),
                               r.get()->GetDependantVariables().begin(),
                               r.get()->GetDependantVariables().end(),
                               this->dependantVariables.begin());

                leftFn = Option<F>::Some(std::move(l));
                rightFn = Option<F>::Some(std::move(r));
            }

            Result<double> GetValue(VariablesContainer& vars) const override {
                Result<double> lValue = leftFn.unwrapRef().get()->GetValue(vars);
                Result<double> rValue = rightFn.unwrapRef().get()->GetValue(vars);

                auto [lv, err] = lValue.unwrapRef();
                if (err) {
                    return Result<double>::Err(err);
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
        public:
            LinearVariable(std::string var) { this->dependantVariables.insert(var); }
            LinearVariable(char var) { this->dependantVariables.insert(std::string(1, var)); }

            Result<double> GetValue(VariablesContainer& vars) const override {
                auto val = vars.get(*this->dependantVariables.begin());
                if (val.is_none()) {
                    return Result<double>::Err("Variable not found");
                }

                return Result<double>::Ok(val.unwrap());
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
