#pragma once

class NonCopyable
{
public:
    NonCopyable() {};
    ~NonCopyable() {};

    // deleted methods should be public for better error messages
    NonCopyable(const NonCopyable &other) = delete;
    void operator=(const NonCopyable &other) = delete;

    NonCopyable(const NonCopyable &&other) = delete;
    void operator=(const NonCopyable &&other) = delete;
};