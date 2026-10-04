
#pragma once

#include "Core/Field/Field.hpp"
#include "Core/Geometry/Geometry.hpp"
#include "Core/Mesh/Mesh.hpp"
#include "Core/Setup/Boundaries.hpp"

namespace AnalyticalSolution
{
    void Pass(
        Field& Field_Obj,
        const Geometry& Geometry_Obj,
        const Mesh& Mesh_Obj,
        const Boundaries& Boundaries_Obj);

    void  Pass(
        Field& Field_Obj,
        double Time_,
        double nu,
        const Geometry& Geometry_Obj,
        const Mesh& Mesh_Obj,
        const Boundaries& Boundaries_Obj,
        const Field& Field_0);
}