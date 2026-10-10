#pragma once

#include <cstdint>
#include <string_view>

// Numerical discretization schemes for parabolic PDEs (Hoffman CFD)
enum class SolverScheme : std::uint8_t
{
    FTCS,
    DUFORT_FRANKEL,
    Laasonen,
    CrankNicolson
};

// [[nodiscard]]    : Warns the compiler if the returned string_view is ignored by the caller.
// constexpr        : Guarantees compile-time evaluation for constant expressions (implicitly inline).
// std::string_view : Lightweight, non-allocating reference to a static string literal.
// noexcept         : Guarantees that this function will never throw an exception.
[[nodiscard]] constexpr std::string_view To_String(SolverScheme scheme) noexcept
{
    switch (scheme)
    {
        case SolverScheme::FTCS:                    return "FTCS";
        case SolverScheme::DUFORT_FRANKEL:          return "DUFORT_FRANKEL";
        case SolverScheme::Laasonen:                return "Laasonen";
        case SolverScheme::CrankNicolson:           return "CrankNicolson";
        default:                                    return "UNKNOWN";
    }
}
