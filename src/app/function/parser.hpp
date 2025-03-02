#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../../global/types.hpp"
#include "function.hpp"

namespace {
    /// @returns `true` if EndOfString was reached.
    bool toNextNonSpace(std::string input, int& pos) {
        while (pos < input.length() && std::isspace(input.at(pos))) {
            ++pos;
        }
        return pos >= input.length();
    }
}  // namespace

namespace parser {
    union TokenValue {
            double constant;
            char variable;
            char operation;
            TokenValue* nested;
    };

    enum class TokenType {
        Consant,     // 12
        Variable,    // x
        Operation,   // +
        NestedToken  // (x + y)
    };

    class Token {
        private:
            static bool IsDigit(char c) { return std::isdigit(c) || c == '.'; }
            static bool IsAlpha(char c) { return std::isalpha(c); }
            static bool IsOperation(char c) { return c == '+' || c == '-' || c == '*' || c == '/'; }
            static bool IsNestedTokenStart(char c) { return c == '('; }
            static bool IsNestedTokenEnd(char c) { return c == ')'; }

            static double ParseConstant(std::string input, int& pos) {
                int initialPos = pos;
                while (pos < input.length() && IsDigit(input.at(pos))) {
                    ++pos;
                }

                // std::cout << "ParseConstant: " << input.substr(initialPos, pos - initialPos)
                //           << std::endl;
                double k = std::stod(input.substr(initialPos, pos++ - initialPos));
                std::cout << "Parsed K = " << k << std::endl;
                return k;
            }
            static void RemoveNullTokens(Token* currToken) {
                while (currToken->next.has_value()) {
                    Box<Token> notNullToken = std::move(currToken->next.value());
                    // To 1st non-null-token
                    // While next token is null-token
                    while (!notNullToken->type.has_value()) {
                        // If `next` doesn't exist
                        if (!notNullToken->next.has_value()) {
                            return;
                        }

                        notNullToken = std::move(notNullToken->next.value());
                    }
                    currToken->next = std::move(notNullToken);

                    // Continue linking tokens
                    currToken = currToken->next->get();
                }
            }

            union {
                    double constant;
                    char variable;
                    char operation;
                    Token* nested;
            } data;  //

        public:
            std::optional<TokenType> type = std::nullopt;
            std::optional<Box<Token>> next = std::nullopt;
            Token(std::string input) {
                int pos = 0;
                Token t(input, pos);

                this->type = t.type;
                this->data = t.data;
                this->next = std::move(t.next);
            }
            Token(std::string input, int& pos) {
                std::cout << "call with input = " << input << " and pos = " << pos << std::endl;
                if (toNextNonSpace(input, pos)) {
                    return;
                }

                std::cout << "here! pos = " << pos << std::endl;
                // std::string s = input.substr(pos);
                std::cout << "here1" << std::endl;
                char c = input.at(pos);
                std::cout << "herte2" << std::endl;

                std::cout << "infered C = " << c << std::endl;

                if (IsDigit(c)) {
                    this->type = TokenType::Consant;
                    this->data.constant = Token::ParseConstant(input, pos);
                } else if (IsAlpha(c)) {
                    this->type = TokenType::Variable;
                    this->data.variable = c;
                    ++pos;
                    std::cout << "C is alpha" << std::endl;

                } else if (IsOperation(c)) {
                    this->type = TokenType::Operation;
                    this->data.operation = c;
                    ++pos;
                    std::cout << "C is operation" << std::endl;

                } else if (IsNestedTokenStart(c)) {
                    this->type = TokenType::NestedToken;
                    this->data.nested = new Token(input, ++pos);

                } else if (IsNestedTokenEnd(c)) {
                    // Null token
                    ++pos;
                }

                if (toNextNonSpace(input, pos)) {
                    return;
                }

                std::cout << "next" << std::endl;

                this->next = NewBox<Token>(input, pos);
                RemoveNullTokens(this);
            }

            ~Token() {
                if (this->type == TokenType::NestedToken) {
                    delete this->data.nested;
                }
            }

            double AsConstant() const { return this->data.constant; }
            char AsVariable() const { return this->data.variable; }
            char AsOperation() const { return this->data.operation; }
            Token* AsNestedToken() const { return this->data.nested; }
    };

    class Parser {
        public:
            static std::optional<Box<function::Function>> Parse(Token* token) {
                std::optional<Box<function::Function>> lhs = std::nullopt;

                std::cout << "Before while" << std::endl;
                while (token && token->type.has_value()) {
                    switch (token->type.value()) {
                        case TokenType::Consant:
                            lhs = NewBox<function::Constant>(token->AsConstant());
                            break;

                        case TokenType::Variable:
                            lhs = NewBox<function::LinearVariable>(token->AsVariable());
                            break;

                        case TokenType::Operation: {
                            std::cout << "TokenType == Operation" << std::endl;
                            if (!token->next.has_value()) {
                                std::cout << "No value" << std::endl;
                                return lhs;
                            }

                            if (!lhs.has_value()) {
                                std::cout << "lhs doesn't contain value" << std::endl;
                                lhs = NewBox<function::Constant>(1.);
                            }

                            char op = token->AsOperation();

                            token = token->next->get();
                            std::cout << "Got Token ptr" << std::endl;

                            // TODO: handle std::optional = None
                            lhs = NewBox<function::Operation>(
                                op,
                                std::move(lhs.value()),
                                Parse(token).value_or(NewBox<function::Constant>(1.)));

                            std::cout << "After parsing rhs" << std::endl;
                            break;
                        };

                        case TokenType::NestedToken:
                            std::cout << "TODO: NestedTOken" << std::endl;
                            // TODO: handle std::optional == None
                            lhs = *Parse(token->AsNestedToken());
                    }

                    std::cout << "Next? " << token->next.has_value() << std::endl;
                    if (!token->next.has_value()) {
                        return lhs;
                    }

                    token = token->next->get();
                }

                return lhs;
            }

            // TODO:
            static std::optional<Box<function::Function>> ParseString(std::string input) {
                Token token(input);
                return Parse(&token);
            }
    };
}  // namespace parser
