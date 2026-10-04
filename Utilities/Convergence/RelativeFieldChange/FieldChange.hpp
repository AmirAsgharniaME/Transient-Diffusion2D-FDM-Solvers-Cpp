#pragma once

#include "Core/Field/Field.hpp"

namespace RelativeFieldChange
{
    [[nodiscard]]double Return(
        const Field& Field_n_Obj,
        const Field& Field_nPlus1_Obj,
        const double dt,
        const double t_scale
    );
}