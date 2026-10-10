#pragma once
#include <cstdint>
#include <string_view>


enum class Label : std::uint8_t
{
    Steady_State
};



[[nodiscard]] constexpr std::string_view To_String_View(Label Label_) noexcept
{
    switch (Label_)
    {
        case Label::Steady_State:  return "Steady_State";
        default:                   return "UNKNOWN";
    }
}
