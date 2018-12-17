#pragma once

#include <string>
#include <list>
#include <memory>

namespace small_strings {
std::shared_ptr<std::list<std::string>> 
    split(const std::string &str, const char c);
std::string TrimLeft(const std::string &str, const std::string &trimStr);
}//namespace small_strings
