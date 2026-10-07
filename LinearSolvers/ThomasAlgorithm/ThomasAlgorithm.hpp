#pragma once
#include "LinearSolvers/Matrix/ADI_Matrix/TridiagonalMatrix/TridiagonalMatrix.hpp"
#include "LinearSolvers/Matrix/ADI_Matrix/RHS/RHS.hpp"
#include "ADI/Field1D/Field1D.hpp"

class ThomasAlgorithm
{
public:
    // (Stateless Class)
    ThomasAlgorithm() = delete;


    static void Solve(
        TridiagonalMatrix& A3_Obj,
        RHS& RHS_Obj,
        Field1D& Solution);

private:
    // (Forward Sweep)
    static void ForwardElimination(
        TridiagonalMatrix& TridiagonalMatrix,
        RHS& RHS_Obj);

    // (Back Substitution)
    static void BackSubstitution(
        const TridiagonalMatrix& TridiagonalMatrix,
        const RHS& RHS_Obj,
        Field1D& Solution);
};
