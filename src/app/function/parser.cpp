#include "parser.hpp"
#include <cctype>
#include <cmath>
#include <functional>
#include <map>
#include <sstream>
#include <stack>
#include <string>
#include <vector>
#include "lib.hpp"

namespace {

    enum class TokenType { Number, Variable, Operator, LeftParen, RightParen, Function, Comma, Invalid };

    struct Token {
            TokenType type;
            std::string value;
    };

    // Приоритет операторов
    const std::map<std::string, int> operator_precedence = {{"^", 4},
                                                            {"*", 3},
                                                            {"/", 3},
                                                            {"+", 2},
                                                            {"-", 2}};

    bool is_operator(const std::string& op) {
        return operator_precedence.find(op) != operator_precedence.end();
    }

    int precedence(const std::string& op) {
        return operator_precedence.at(op);
    }

    bool is_right_associative(const std::string& op) {
        return op == "^";
    }

    // Множество поддерживаемых функций (1 аргумент)
    const std::map<std::string, std::function<float(float)>> functions_1arg = {
        {"sin", [](float x) { return std::sin(x); }},
        {"cos", [](float x) { return std::cos(x); }},
        {"tan", [](float x) { return std::tan(x); }},
        {"log", [](float x) { return std::log(x); }},
        {"exp", [](float x) { return std::exp(x); }},
        {"abs", [](float x) { return std::fabs(x); }}};

    // Проверка: строка - функция?
    bool is_function(const std::string& name) {
        return functions_1arg.find(name) != functions_1arg.end();
    }

    // Токенизация с поддержкой многосимвольных переменных и функций
    std::vector<Token> tokenize(const std::string& expr) {
        std::vector<Token> tokens;
        size_t i = 0;

        while (i < expr.size()) {
            char ch = expr[i];

            if (std::isspace(ch)) {
                ++i;
                continue;
            }

            if (std::isdigit(ch) || ch == '.') {
                // Число
                std::string num;
                bool dot_found = (ch == '.');
                num += ch;
                ++i;
                while (i < expr.size() && (std::isdigit(expr[i]) || (!dot_found && expr[i] == '.'))) {
                    if (expr[i] == '.')
                        dot_found = true;
                    num += expr[i++];
                }
                tokens.push_back({TokenType::Number, num});
                continue;
            }

            if (std::isalpha(ch) || ch == '_') {
                // Многосимвольная переменная или функция
                std::string name;
                name += ch;
                ++i;
                while (i < expr.size() && (std::isalnum(expr[i]) || expr[i] == '_')) {
                    name += expr[i++];
                }
                // Если следующий символ '(' — это функция, иначе переменная
                if (i < expr.size() && expr[i] == '(') {
                    tokens.push_back({TokenType::Function, name});
                } else {
                    tokens.push_back({TokenType::Variable, name});
                }
                continue;
            }

            if (ch == ',') {
                tokens.push_back({TokenType::Comma, ","});
                ++i;
                continue;
            }

            if (ch == '(') {
                tokens.push_back({TokenType::LeftParen, "("});
                ++i;
                continue;
            }

            if (ch == ')') {
                tokens.push_back({TokenType::RightParen, ")"});
                ++i;
                continue;
            }

            if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^') {
                tokens.push_back({TokenType::Operator, std::string(1, ch)});
                ++i;
                continue;
            }

            tokens.push_back({TokenType::Invalid, std::string(1, ch)});
            ++i;
        }

        return tokens;
    }

    // Добавляем поддержку унарного минуса в алгоритме сортировочной станции (shunting yard)
    // Для этого заменим унарный минус на специальный оператор "u-", приоритет выше чем ^
    const std::string UNARY_MINUS = "u-";

    result::Result<std::vector<Token>, std::string> shunting_yard(const std::vector<Token>& tokens) {
        std::vector<Token> output;
        std::stack<Token> ops;

        // Функция для проверки, что токен является оператором (включая унарный минус)
        auto is_op = [](const Token& t) {
            return t.type == TokenType::Operator ||
                   (t.type == TokenType::Operator && t.value == UNARY_MINUS);
        };

        for (size_t i = 0; i < tokens.size(); ++i) {
            const Token& token = tokens[i];

            switch (token.type) {
                case TokenType::Number:
                case TokenType::Variable:
                    output.push_back(token);
                    break;

                case TokenType::Function:
                    ops.push(token);
                    break;

                case TokenType::Comma:
                    // Сбрасываем до ближайшей '('
                    while (!ops.empty() && ops.top().type != TokenType::LeftParen) {
                        output.push_back(ops.top());
                        ops.pop();
                    }
                    if (ops.empty() || ops.top().type != TokenType::LeftParen) {
                        return result::Result<std::vector<Token>, std::string>::Err(
                            "Ошибка синтаксиса: отсутствует '(' перед запятой");
                    }
                    break;

                case TokenType::Operator: {
                    std::string op = token.value;

                    // Определяем унарный минус:
                    // Если это '-' и он в начале или после оператора/левой скобки — это унарный минус
                    if (op == "-") {
                        if (i == 0) {
                            op = UNARY_MINUS;
                        } else {
                            const Token& prev = tokens[i - 1];
                            if (prev.type == TokenType::Operator || prev.type == TokenType::LeftParen ||
                                prev.type == TokenType::Comma) {
                                op = UNARY_MINUS;
                            }
                        }
                    }

                    // Теперь оперируем с op (который может быть u-)
                    while (!ops.empty()) {
                        const Token& top = ops.top();
                        if (top.type == TokenType::Operator) {
                            std::string top_op = top.value;

                            // Для унарного минуса приоритет самый высокий (5)
                            int curr_prec = (op == UNARY_MINUS) ? 5 : precedence(op);
                            int top_prec = (top_op == UNARY_MINUS) ? 5 : precedence(top_op);

                            if ((is_right_associative(op) && curr_prec < top_prec) ||
                                (!is_right_associative(op) && curr_prec <= top_prec)) {
                                output.push_back(top);
                                ops.pop();
                                continue;
                            }
                        }
                        break;
                    }

                    ops.push({TokenType::Operator, op});
                    break;
                }

                case TokenType::LeftParen:
                    ops.push(token);
                    break;

                case TokenType::RightParen:
                    while (!ops.empty() && ops.top().type != TokenType::LeftParen) {
                        output.push_back(ops.top());
                        ops.pop();
                    }
                    if (ops.empty()) {
                        return result::Result<std::vector<Token>, std::string>::Err(
                            "Несбалансированные скобки");
                    }
                    ops.pop();  // удаляем '('

                    // Если сверху функция, сбрасываем её в output
                    if (!ops.empty() && ops.top().type == TokenType::Function) {
                        output.push_back(ops.top());
                        ops.pop();
                    }
                    break;

                case TokenType::Invalid:
                    return result::Result<std::vector<Token>, std::string>::Err("Недопустимый символ: " +
                                                                                token.value);

                default:
                    return result::Result<std::vector<Token>, std::string>::Err("Неизвестный токен");
            }
        }

        while (!ops.empty()) {
            if (ops.top().type == TokenType::LeftParen || ops.top().type == TokenType::RightParen) {
                return result::Result<std::vector<Token>, std::string>::Err("Несбалансированные скобки");
            }
            output.push_back(ops.top());
            ops.pop();
        }

        return result::Result<std::vector<Token>, std::string>::Ok(std::move(output));
    }

    using VarMap = std::map<std::string, float>;

    result::Result<float, std::string> eval_rpn(const std::vector<Token>& rpn, const VarMap& variables) {
        std::stack<float> stack;

        for (const Token& token : rpn) {
            if (token.type == TokenType::Number) {
                try {
                    stack.push(std::stof(token.value));
                } catch (...) {
                    return result::Result<float, std::string>::Err("Ошибка при преобразовании числа: " +
                                                                   token.value);
                }
            } else if (token.type == TokenType::Variable) {
                auto it = variables.find(token.value);
                if (it != variables.end()) {
                    stack.push(it->second);
                } else {
                    return result::Result<float, std::string>::Err("Неизвестная переменная: " +
                                                                   token.value);
                }
            } else if (token.type == TokenType::Operator) {
                if (token.value == UNARY_MINUS) {
                    // Унарный минус: проверяем что есть хотя бы один элемент
                    if (stack.empty()) {
                        return result::Result<float, std::string>::Err(
                            "Ошибка: недостаточно операндов для унарного минуса");
                    }
                    float a = stack.top();
                    stack.pop();
                    stack.push(-a);
                } else {
                    if (stack.size() < 2) {
                        return result::Result<float, std::string>::Err(
                            "Недостаточно операндов для операции: " + token.value);
                    }
                    float b = stack.top();
                    stack.pop();
                    float a = stack.top();
                    stack.pop();
                    float result;

                    if (token.value == "+")
                        result = a + b;
                    else if (token.value == "-")
                        result = a - b;
                    else if (token.value == "*")
                        result = a * b;
                    else if (token.value == "/") {
                        if (b == 0.0f)
                            return result::Result<float, std::string>::Err("Деление на ноль");
                        result = a / b;
                    } else if (token.value == "^") {
                        result = std::pow(a, b);
                    } else {
                        return result::Result<float, std::string>::Err("Неизвестный оператор: " +
                                                                       token.value);
                    }
                    stack.push(result);
                }
            } else if (token.type == TokenType::Function) {
                // Обрабатываем функции с одним аргументом
                if (stack.empty())
                    return result::Result<float, std::string>::Err("Ошибка: функция '" + token.value +
                                                                   "' требует аргумент");

                float arg = stack.top();
                stack.pop();

                auto it = functions_1arg.find(token.value);
                if (it == functions_1arg.end()) {
                    return result::Result<float, std::string>::Err("Неизвестная функция: " +
                                                                   token.value);
                }
                float val;
                try {
                    val = it->second(arg);
                } catch (...) {
                    return result::Result<float, std::string>::Err("Ошибка при вычислении функции: " +
                                                                   token.value);
                }
                stack.push(val);
            } else {
                return result::Result<float, std::string>::Err("Неподдерживаемый токен в RPN: " +
                                                               token.value);
            }
        }

        if (stack.size() != 1) {
            return result::Result<float, std::string>::Err(
                "Ошибка вычисления выражения: неверное количество элементов в стеке");
        }

        return result::Result<float, std::string>::Ok(stack.top());
    }

}  // namespace

result::Result<float, std::string> parse_and_eval(const std::string& expr,
                                                  const std::map<std::string, float>& variables) {
    auto tokens = tokenize(expr);

    // Проверка токенов
    for (const auto& t : tokens) {
        if (t.type == TokenType::Invalid) {
            return result::Result<float, std::string>::Err("Недопустимый символ: " + t.value);
        }
    }

    auto rpn_res = shunting_yard(tokens);
    if (!rpn_res.ok()) {
        return result::Result<float, std::string>::Err(rpn_res.unwrap_err());
    }

    return eval_rpn(rpn_res.unwrap(), variables);
}
