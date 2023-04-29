#pragma once

#include <kernel/stream.hpp>
#include <kernel/types.hpp>

class UART : public IOStream<char>
{
public:
    UART() = default;

    char get() override;
    void put(const char& value) override;
};
