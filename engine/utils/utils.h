#pragma once

namespace engine
{
class NonCopyable
{
public:
    NonCopyable() = default;
    ~NonCopyable() = default;

    NonCopyable(NonCopyable &other) = delete;
    NonCopyable(NonCopyable &&other) = delete;

    void operator=(NonCopyable &other) = delete;
    void operator=(NonCopyable &&other) = delete;
};
} // namespace engine
