
#pragma once

#include "Core/Field/Field.hpp"
#include "Core/Geometry/Geometry.hpp"
#include "Core/Mesh/Mesh.hpp"
#include "SimulationSetup/SimulationBoundaries/SimulationBoundaries.hpp"

namespace DiffusionEQ::AnalyticalSolution
{
    void Pass(
        Field& Field_Obj,
        const Geometry& Geometry_Obj,
        const Mesh& Mesh_Obj,
        const SimulationBoundaries& Boundaries_Obj);

    void  Pass(
        Field& Field_Obj,
        double Time_,
        double nu,
        const Geometry& Geometry_Obj,
        const Mesh& Mesh_Obj,
        const SimulationBoundaries& Boundaries_Obj,
        const Field& Field_0);
}