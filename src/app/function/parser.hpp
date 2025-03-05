#pragma once

#include <string>
#include <vector>
#include "function.hpp"
#include "lib/prelude.hpp"

namespace {
    void shiftToNextNonSpace(std::string input, int& pos) {
        while (pos < input.length() - 1 && std::isspace(input.at(pos))) {
            ++pos;
        }
    }
}  // namespace

namespace parser {
    // union TokenValue {
    //         double constant;
    //         char variable;
    //         char operation;
    //         TokenValue* nested;
    // };

    // enum class TokenType {
    //     Consant,     // 12
    //     Variable,    // x
    //     Operation,   // +
    //     NestedToken  // (x + y)
    // };

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

                value = std::stod(s.substr(initialPos, pos-- - initialPos));
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

    class OperationToken : public IntoFunction {
        private:
            char op;
            Box<IntoFunction> lhs;
            Option<Box<IntoFunction>> rhs;
            friend class TokensSequence;

            OperationToken* with(Box<IntoFunction> rhs) {
                this->rhs = std::move(rhs);
                return this;
            }

            static bool Matches(char s) {
                return s == '+' || s == '-' || s == '*' || s == '/' || s == '^';
            }

        public:
            OperationToken(char op, Box<IntoFunction> lhs) : op(op), lhs(std::move(lhs)) {}

            Box<function::Function> AsFunction() override {
                if (rhs.isNone()) {
                    throw std::runtime_error("Operation has no rhs");
                }

                return NewBox<function::Operation>(
                    op, std::move(lhs.get()->AsFunction()), std::move(rhs.unwrap().get()->AsFunction()));
            }
    };

    class TokensSequence : public IntoFunction {
        private:
            Option<Box<IntoFunction>> function;
            static bool Matches(char ch) { return ch == '('; }
            static TokensSequence Extract(const std::string& seq, int& pos) {
                // ! NOTE: seq.at(0) should be "("
                int startPos = pos;
                int parenthises = 1;
                while (++pos < seq.length() && parenthises > 0) {
                    if (seq.at(pos) == '(') {
                        ++parenthises;
                    } else if (seq.at(pos) == ')') {
                        --parenthises;
                    }
                }

                return TokensSequence(seq.substr(startPos, pos-- - startPos));
            }

        public:
            TokensSequence(const std::string& s) {
                int pos = 0;

                Option<Box<IntoFunction>> fn = Option<Box<IntoFunction>>::None();  // either lhs or rhs
                Option<Box<OperationToken>> op = Option<Box<OperationToken>>::None();

                while (pos < s.length()) {
                    shiftToNextNonSpace(s, pos);

                    char ch = s.at(pos);

                    // Parse as number
                    if (ConstantToken::Matches(ch)) {
                        fn.set(NewBox<ConstantToken>(s, pos));
                    }
                    // Parse as variable
                    else if (VariableToken::Matches(ch)) {
                        fn.set(NewBox<VariableToken>(ch));
                    }
                    // Parse as operation
                    else if (OperationToken::Matches(ch)) {
                        op.set(NewBox<OperationToken>(ch, fn.unwrapSwap()));
                        continue;
                    }
                    // Parse as nested token
                    else if (TokensSequence::Matches(ch)) {
                        fn.set(NewBox<TokensSequence>(TokensSequence::Extract(s, pos)));
                    }

                    if (op.isSome()) {
                        fn.set(Box<OperationToken>(op.unwrapSwap()->with(std::move(fn.unwrap()))));
                    }
                }

                if (fn.isSome()) {
                    function = std::move(fn);
                }
            }

            Box<function::Function> AsFunction() override {
                if (function.isNone()) {
                    throw std::runtime_error("TokensSequence has no function");
                }

                return function.unwrap().get()->AsFunction();
            }
    };

    // class Parser {
    //     public:
    //         static Option<Box<function::Function>> Parse(Token* token) {
    //             std::optional<Box<function::Function>> lhs = std::nullopt;

    //             std::cout << "Before while" << std::endl;
    //             while (token && token->type.has_value()) {
    //                 switch (token->type.value()) {
    //                     case TokenType::Consant:
    //                         lhs = NewBox<function::Constant>(token->AsConstant());
    //                         break;

    //                     case TokenType::Variable:
    //                         lhs = NewBox<function::LinearVariable>(token->AsVariable());
    //                         break;

    //                     case TokenType::Operation: {
    //                         std::cout << "TokenType == Operation" << std::endl;
    //                         if (!token->next.has_value()) {
    //                             std::cout << "No value" << std::endl;
    //                             return lhs;
    //                         }

    //                         if (!lhs.has_value()) {
    //                             std::cout << "lhs doesn't contain value" << std::endl;
    //                             lhs = NewBox<function::Constant>(1.);
    //                         }

    //                         char op = token->AsOperation();

    //                         token = token->next->get();
    //                         std::cout << "Got Token ptr" << std::endl;

    //                         // TODO: handle std::optional = None
    //                         lhs = NewBox<function::Operation>(
    //                             op,
    //                             std::move(lhs.value()),
    //                             Parse(token).value_or(NewBox<function::Constant>(1.)));

    //                         std::cout << "After parsing rhs" << std::endl;
    //                         break;
    //                     };

    //                     case TokenType::NestedToken:
    //                         std::cout << "TODO: NestedTOken" << std::endl;
    //                         // TODO: handle std::optional == None
    //                         lhs = *Parse(token->AsNestedToken());
    //                 }

    //                 std::cout << "Next? " << token->next.has_value() << std::endl;
    //                 if (!token->next.has_value()) {
    //                     return lhs;
    //                 }

    //                 token = token->next->get();
    //             }

    //             return lhs;
    //         }

    //         // TODO:
    //         static std::optional<Box<function::Function>> ParseString(std::string input) {
    //             Token token(input);
    //             return Parse(&token);
    //         }
    // };
}  // namespace parser
