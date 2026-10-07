#pragma once
#include "LinearSolvers/Matrix/TridiagonalMatrix.hpp"
#include "LinearSolvers/Matrix/RHS.hpp"
#include "Core/Field/Field1D.hpp"

class ThomasAlgorithm
{
public:
    // (Stateless Class)
    ThomasAlgorithm() = delete;


    static void Solve_nPlus1(
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
