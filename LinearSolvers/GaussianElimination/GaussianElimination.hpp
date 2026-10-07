#pragma once
#include <cstddef>
#include "LinearSolvers/Matrix/CoefficientMatrix.hpp"
#include "LinearSolvers/Matrix/RHS.hpp"
#include "Core/Field/Field.hpp"


//Gaussian Elimination with Partial Row Pivoting followed by Back Substitution
class GaussianElimination
{

public:

 GaussianElimination() = delete;   //Creating an object of this class is prohibited.
    //A_Obj Solution = RHS
    static void Solve_nPlus1(
        CoefficientMatrix& A_Obj,
        RHS& RHS_Obj,
        Field& Solution);



private:
    static void ForwardElimination(
        CoefficientMatrix& A_Obj,
        RHS& RHS_Obj);
/*
 * Back Substitution Formula:
 * 
 *   x[i] = ( b[i] - Sum_{j = i+1}^{n-1} ( a[i][j] * x[j] ) ) / a[i][i]
 * 
 * Execution Steps:
 *   1. Initialize 'Sum' as b[i] (RHS value).
 *   2. Subtract the product of known variables: Sum -= a[i][j] * x[j] (for j from i+1 to n-1).
 *   3. Calculate the final value: x[i] = Sum / a[i][i].
 */

    static void BackSubstitution(
        const CoefficientMatrix& A_Obj,
        const RHS& RHS_Obj,
        Field& Solution);

    [[nodiscard]] static std::size_t FindPivotRowIndex(
        const CoefficientMatrix& A_Obj,
        const std::size_t PivotIndex);

    static void SwapRows(
        CoefficientMatrix& A_Obj,
        RHS& RHS_Obj,
        std::size_t FirstRowIndex,
        std::size_t SecondRowIndex);
};
