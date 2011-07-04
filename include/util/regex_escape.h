#pragma once

#include <string>

std::string regex_escape (std::string const &wc);
void regex_escape (std::string &regex, std::string const &wc);
