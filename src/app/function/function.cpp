#include "function.hpp"

using namespace function;

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
