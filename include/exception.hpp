#pragma once

#include <stdexcept>
#include <string>

namespace bcf {

class ConnectException : public std::runtime_error
{
public:
    explicit ConnectException(const std::string& message)
        : std::runtime_error(message)
    {
    }

    explicit ConnectException(const char* message)
        : std::runtime_error(message)
    {
    }
};

class BcfCommonException : public std::runtime_error
{
public:
    explicit BcfCommonException(const std::string& message)
        : std::runtime_error(message)
    {
    }

    explicit BcfCommonException(const char* message)
        : std::runtime_error(message)
    {
    }
};
}// namespace bcf
