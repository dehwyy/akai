#pragma once

#include <string>
#include <vector>
#include "function.hpp"
#include "lib.hpp"

namespace {
    void shiftToNextNonSpace(std::string input, int& pos) {
        while (pos < input.length() - 1 && std::isspace(input.at(pos))) {
            ++pos;
        }
    }
}  // namespace

namespace parser {
    class IntoFunction {
        public:
            virtual Box<function::Function> AsFunction() = 0;
    };

    class ConstantToken : public IntoFunction {
        private:
            double value;

        public:
            ConstantToken(double value) : value(value) {}
            ConstantToken(std::string s, int& pos) {
                int initialPos = pos++;
                while (pos < s.length() && ConstantToken::Matches(s.at(pos))) {
                    ++pos;
                }

                value = std::stod(s.substr(initialPos, pos - initialPos));
            }

            Box<function::Function> AsFunction() override { return NewBox<function::Constant>(value); }

            static bool Matches(char ch) { return std::isdigit(ch) || ch == '.'; }
    };

    class VariableToken : public IntoFunction {
        private:
            char var;

        public:
            VariableToken(char var) : var(var) {}

            Box<function::Function> AsFunction() override {
                return NewBox<function::LinearVariable>(var);
            }
            static bool Matches(char s) { return std::isalpha(s); }
    };

    class MathFunctionToken : public IntoFunction {
        private:
            std::string name;
            Box<IntoFunction> inner;

        public:
            MathFunctionToken(std::string name, Box<IntoFunction> inner)
                : name(name), inner(std::move(inner)) {}

            Box<function::Function> AsFunction() override {
                return NewBox<function::MathFunction>(name, inner->AsFunction());
            }
    };

    class OperationToken : public IntoFunction {
        private:
            char op;
            Box<IntoFunction> lhs;
            Box<IntoFunction> rhs;
            friend class TokensSequence;

            OperationToken* with(Box<IntoFunction> rhs) {
                this->rhs = std::move(rhs);
                return this;
            }

            static bool Matches(char s) {
                return s == '+' || s == '-' || s == '*' || s == '/' || s == '^';
            }

        public:
            // OperationToken(char op, Box<IntoFunction> lhs) : op(op), lhs(std::move(lhs)) {}
            OperationToken(char op, Box<IntoFunction> lhs, Box<IntoFunction> rhs)
                : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

            Box<function::Function> AsFunction() override {
                return NewBox<function::Operation>(op, lhs->AsFunction(), rhs->AsFunction());
            }
    };

    class TokensSequence : public IntoFunction {
        private:
            Option<Box<IntoFunction>> function;
            static bool Matches(char ch) { return ch == '('; }
            static TokensSequence Extract(const std::string& seq, int& pos) {
                // ! NOTE: seq.at(0) should be "("
                int startPos = ++pos;
                int parenthises = 1;
                while (pos++ < seq.length() && parenthises > 0) {
                    if (seq.at(pos) == '(') {
                        ++parenthises;
                    } else if (seq.at(pos) == ')') {
                        --parenthises;
                    }
                }

                std::string s = seq.substr(startPos, pos - startPos - 1);
                if (s.length() == 0 || parenthises != 0) {
                    throw std::runtime_error("Invalid sequence");
                }

                return TokensSequence(s);
            }

        public:
            TokensSequence(const std::string& s) {
                int pos = 0;

                // TODO:
                Option<Box<IntoFunction>> lhs = Option<Box<IntoFunction>>::None();  // either lhs or rhs
                Option<Box<IntoFunction>> rhs = Option<Box<IntoFunction>>::None();  // either lhs or rhs
                Option<char> op = Option<char>::None();

                while (pos < s.length()) {
                    shiftToNextNonSpace(s, pos);

                    char ch = s.at(pos);

                    // Parse as number
                    if (ConstantToken::Matches(ch)) {
                        Log::Print("New const token");
                        if (op.isNone()) {
                            lhs.set(NewBox<ConstantToken>(s, pos));
                        } else {
                            rhs.set(NewBox<ConstantToken>(s, pos));
                        }
                    }
                    // Parse as variable
                    else if (VariableToken::Matches(ch)) {
                        Log::Print("New variable token");
                        if (op.isNone()) {
                            lhs.set(NewBox<VariableToken>(ch));
                        } else {
                            rhs.set(NewBox<VariableToken>(ch));
                        }
                        pos++;
                    }
                    // Parse as operation
                    else if (OperationToken::Matches(ch)) {
                        Log::Print("New operation token");
                        // TODO:
                        if (op.isNone()) {
                        }
                        op.set(std::move(ch));
                        pos++;
                        continue;
                    }
                    // Parse as nested token
                    else if (TokensSequence::Matches(ch)) {
                        Log::Print("New nested TokensSequence");

                        if (op.isNone()) {
                            lhs.set(NewBox<TokensSequence>(TokensSequence::Extract(s, pos)));
                        } else {
                            rhs.set(NewBox<TokensSequence>(TokensSequence::Extract(s, pos)));
                        }
                    }

                    if (op.isSome() && rhs.isSome()) {
                        if (lhs.isNone()) {
                            lhs.set(NewBox<ConstantToken>(0.));
                        }

                        Log::Print("New OperationToken");
                        lhs.set(NewBox<OperationToken>(op.unwrap(), lhs.unwrap(), rhs.unwrap()));
                        // fn.set(Box<OperationToken>(op.unwrap()->with(std::move(fn.unwrap()))));
                    }
                }

                if (lhs.isSome()) {
                    function = std::move(lhs);
                }
            }

            Box<function::Function> AsFunction() override {
                if (function.isNone()) {
                    throw std::runtime_error("TokensSequence has no function");
                }

                return function.unwrap()->AsFunction();
            }
    };
}  // namespace parser
