#pragma once

#include <string>

namespace small_json{
class JsonObject {
public:
    JsonObject() = default;
    virtual ~JsonObject() = default;
    virtual int GetInt() = 0;
    virtual std::string GetString() = 0;
    virtual JsonObject GetObject() = 0;
};
class JsonArray {
public:
    JsonArray() = default;
    virtual ~JsonArray() = default;
};
}
