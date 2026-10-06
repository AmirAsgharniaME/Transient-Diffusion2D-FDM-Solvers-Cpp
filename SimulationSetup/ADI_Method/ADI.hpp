#pragma once
#include "ADI/Field1D/Field1D.hpp"
#include "SimulationSetup/SimulationContext/SimulationContext.hpp"
#include "LinearSolvers/Matrix/ADI_Matrix/TridiagonalMatrix/TridiagonalMatrix.hpp"
#include "LinearSolvers/Matrix/ADI_Matrix/RHS/RHS.hpp"
class ADI
{
    public:
    Field1D T_nPlus1_Star_Row;
    Field1D T_nPlus1_Col;
    Field T_nPlus1_Star;
    TridiagonalMatrix A3;
    RHS rhs;



    explicit ADI(SimulationContext& ctx);


};