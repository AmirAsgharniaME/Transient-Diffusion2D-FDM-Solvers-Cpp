#pragma once

#include "Core/Field/Field.hpp"

namespace Discretization::Explicit::FTCS
{
    // Updates Field from time level n to n+1 using Forward Time Central Space
    void Step(
        const Field& Field_n,
        Field& Field_nPlus1,
        double DiffNumberX,
        double DiffNumberY
    ) noexcept;
}