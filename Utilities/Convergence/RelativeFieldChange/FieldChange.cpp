#include "Utilities/Convergence/RelativeFieldChange/FieldChange.hpp"
#include <cstddef>
#include <stdexcept>
#include <cmath>

namespace RelativeFieldChange
{
/*
Normalized L2 norm of the discrete time derivative between two consecutive time steps for a  scalar field.

ReturnFieldChange computes:

    (t_scale / dt) * [ sqrt( sum_{j, i} (Field_nPlus1(j, i) - Field_n(j, i))^2 )
                       ---------------------------------------------------------------
                           sqrt( sum_{j, i} Field_nPlus1(j, i)^2 ) + epsilon        ]

where:
    Field_n       is the  field at the current time step,
    Field_nPlus1  is the  field at the next time step,
    dt              is the physical time step size (must be > 0),
    t_scale         is the characteristic physical time scale (e.g. L^2 / alpha),
                    used to make the stopping criterion non-dimensional and
                    independent of dt,
    epsilon         prevents division by zero,
    j               denotes row indexing (changing row),
    i               denotes column indexing (changing column).

In explicit and pseudo-transient schemes, dividing by dt recovers the discrete
time derivative dU/dt, which directly reflects the steady-state spatial residual.
*/
double Return(
    const Field& Field_n_Obj,
    const Field& Field_nPlus1_Obj,
    const double dt,
    const double t_scale)
{
    if (dt <= 0.0)
    {
        throw std::invalid_argument("Time step dt must be strictly positive.");
    }

    const std::size_t Ny = Field_n_Obj.Get_Ny();
    const std::size_t Nx = Field_n_Obj.Get_Nx();

    if (Field_nPlus1_Obj.Get_Ny() != Ny || Field_nPlus1_Obj.Get_Nx() != Nx)
    {
        throw std::invalid_argument(" Field dimensions (Ny, Nx) must match.");
    }

    double Sum1 = 0.0; // Σ (Field_nPlus1_Obj(j, i) - Field_n_Obj(j, i))^2
    double Sum2 = 0.0; // Σ (Field_nPlus1_Obj(j, i))^2

    for (std::size_t j = 0; j < Ny; ++j)
    {
        for (std::size_t i = 0; i < Nx; ++i)
        {
            const double Difference = Field_nPlus1_Obj[j][i] - Field_n_Obj[j][i];
            const double Field_nplus1 = Field_nPlus1_Obj[j][i];

            Sum1 += Difference * Difference;
            Sum2 += Field_nplus1 * Field_nplus1;
        }
    }

    const double relative_change = std::sqrt(Sum1) / (std::sqrt(Sum2) + 1.0e-30);

    return (t_scale / dt) * relative_change;
}

}
