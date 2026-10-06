#pragma once

#include "Core/Geometry/Geometry.hpp"
#include "Config/SolverInputs.hpp"

struct Geometries
{
    Geometry G1;


    explicit Geometries()
    :G1(SolverInputs::Geometry::Width, SolverInputs::Geometry::Height)
    {}
};