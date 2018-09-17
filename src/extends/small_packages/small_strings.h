#pragma once

#include <string>
#include <list>
#include <memory>

namespace small_strings {
std::shared_ptr<std::list<std::string>> 
    split(const std::string &str, const char c);
}//namespace small_strings
