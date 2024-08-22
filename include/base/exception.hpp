#pragma once

#include <stdexcept>
#include <string>

namespace bcf
{
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
class ConnectException : public BcfCommonException
{
public:
    explicit ConnectException(const std::string& message)
        : BcfCommonException(message)
    {
    }

    explicit ConnectException(const char* message)
        : BcfCommonException(message)
    {
    }
};
}// namespace bcf
