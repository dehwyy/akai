#pragma once

#include <map>
#include <string>
#include "lib.hpp"

result::Result<float, std::string> parse_and_eval(const std::string& expr,
                                                  const std::map<std::string, float>& variables);
