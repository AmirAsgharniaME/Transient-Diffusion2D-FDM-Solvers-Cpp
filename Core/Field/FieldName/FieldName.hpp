#pragma once

#include <cstdint>
#include <string_view>

// Enumeration for physical field variable identifiers
enum class FieldName : std::uint8_t
{
    T,
    TAnalytical
};

// [[nodiscard]]    : Warns the caller if the returned value is discarded.
// constexpr        : Enables compile-time evaluation whenever arguments are constant expressions.
// inline           : Prevents ODR (One Definition Rule) violations across translation units.
// std::string_view : Zero-overhead, non-allocating view into static string literals.
// noexcept         : Guarantees no exceptions will be thrown since no dynamic allocation occurs.
[[nodiscard]] constexpr std::string_view To_String_View(FieldName fieldName) noexcept
{
    switch (fieldName)
    {
        case FieldName::T:            return "T";
        case FieldName::TAnalytical:  return "TAnalytical";
        default:                      return "UNKNOWN";
    }

}
