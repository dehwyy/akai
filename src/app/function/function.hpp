#pragma once
#include <cmath>
#include <optional>
#include <set>
#include <string>
#include <vector>
#include "../../global/types.hpp"
#include "variable.hpp"

using namespace variable;

namespace function {
    class Function {
        protected:
            Box<Function> nested = nullptr;
            std::set<std::string> dependantVariables = {};

        public:
            Function() = default;
            std::set<std::string> GetDependantVariables() const { return dependantVariables; }

            virtual ~Function() = default;
            virtual double GetValue(variable::Variables& variables) const { return 1; };
    };

    // class Cos : public Function {
    //     public:
    //         double GetValue(variable::Variables& variables) const override {
    //             return std::cos(this->GetValue(variables));
    //         }
    // };

    // class Power : public Function {
    //     public:
    //         double GetValue(variable::Variables& variables) const override {
    //             return std::pow(this->GetValue(variables), this->GetValue(variables));  // ?
    //         }
    // };

    class Operation : public Function {
        private:
            Box<Function> lhs;
            Box<Function> rhs;
            char op;

        public:
            Operation(char op, Box<Function> lhs, Box<Function> rhs)
                : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {
                // TODO: merge dependant variables
            }

            double GetValue(variable::Variables& variables) const override {
                auto l = this->lhs.get()->GetValue(variables);
                auto r = this->rhs.get()->GetValue(variables);
                switch (this->op) {
                    case '+':
                        return l + r;
                    case '-':
                        return l - r;
                    case '*':
                        return l * r;
                    case '/':
                        return l / r;
                    default:
                        return 1;
                }
            }
    };

    /// @brief Only "f(x) = x"
    class LinearVariable : public Function {
        public:
            LinearVariable(std::string var) { this->dependantVariables.insert(var); }
            LinearVariable(char var) {
                std::cout << "this call" << std::endl;
                this->dependantVariables.insert(std::string(1, var));
            }

            double GetValue(variable::Variables& variables) const override {
                return variables.GetVariable(*this->dependantVariables.begin());
            };
    };

    class Constant : public Function {
        private:
            double value;

        public:
            Constant(double value) { this->value = value; }

            double GetValue(variable::Variables&) const override { return value; }
    };
}  // namespace function
