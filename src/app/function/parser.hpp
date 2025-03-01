#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../../global/types.hpp"
#include "function.hpp"

namespace parser {
    class Parser {
        public:
            // TODO:
            static std::optional<Box<function::Function>> Parse(std::string input) {
                // const or f(x) = x
                if (input.length() == 1) {
                    char c = input.at(0);
                    if (std::isdigit(c)) {
                        return NewBox<function::Constant>(c - '0');
                    } else if (std::isalpha(c)) {
                        return NewBox<function::Linear>(input.substr(0, 1));
                    }

                    return std::nullopt;
                }

                // operations
                if (input.length() == 3) {
                    return NewBox<function::Operation>(input.at(1),
                                                       *Parser::Parse(input.substr(0, 1)),
                                                       *Parser::Parse(input.substr(2, 1)));
                }

                return std::nullopt;
            }
    };
}  // namespace parser
