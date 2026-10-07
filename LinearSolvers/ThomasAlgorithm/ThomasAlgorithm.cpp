#include "LinearSolvers/ThomasAlgorithm/ThomasAlgorithm.hpp"
#include <stdexcept>
#include <cmath>
/*
 * Performs the forward elimination stage of the Thomas algorithm.
 *
 * The lower-diagonal entries are eliminated mathematically without being
 * explicitly set to zero. The main diagonal and right-hand-side vector
 * are modified in place, while the upper diagonal remains unchanged.
 *
 *
 * The Thomas algorithm is a specialized form of Gaussian elimination
 * designed exclusively for tridiagonal linear systems. Unlike standard
 * Gaussian elimination, it eliminates only the lower-diagonal entry
 * directly below each pivot because all other entries below the main
 * diagonal are already zero.
 *
 * Therefore, no full row operations are required. For a system with N
 * equations, the Thomas algorithm has O(N) computational complexity and
 * O(N) storage requirements, whereas standard Gaussian elimination for
 * a dense system generally requires O(N^3) operations and O(N^2) storage.
 *
 * During this stage, the lower-diagonal entries are eliminated
 * mathematically without being explicitly set to zero. The main diagonal
 * and the right-hand-side vector are modified in place, while the upper
 * diagonal remains unchanged.
 *
 * Unlike Gaussian elimination with partial pivoting, the standard Thomas
 * algorithm does not perform row exchanges. Therefore, a zero or near-zero
 * pivot causes a runtime error and may indicate that the method is not
 * numerically suitable for the given system.
 * 
 * A zero or near-zero pivot does not necessarily mean that the
 * tridiagonal system has no unique solution. It only means that
 * the standard Thomas algorithm without pivoting cannot proceed
 * reliably with the current ordering of equations.
 * 
 * TODO: Add the Pivoted Thomas Algorithm (Tridiagonal Gaussian Elimination
 * with Partial Pivoting) as a separate solver to improve numerical robustness
 * and handle tridiagonal systems in which the standard Thomas algorithm fails
 * because of a zero or near-zero pivot, even when a unique solution may exist.
 * Unlike the general partial row pivoting implemented in Gaussian elimination,
 * which searches all remaining rows in the current column for the largest
 * available pivot, tridiagonal partial pivoting compares only the current
 * diagonal entry with the adjacent subdiagonal entry and, when necessary,
 * interchanges two adjacent rows. This restricted pivoting preserves the
 * banded structure as much as possible, introduces at most a second
 * superdiagonal, and retains O(N) computational complexity.


 * Solves a tridiagonal linear system using the Thomas algorithm,
 * a specialized form of Gaussian elimination without pivoting.
 *
 * Forward elimination removes each lower-diagonal entry using the
 * modified diagonal entry of the previous row. The main diagonal
 * and RHS are updated in place; the upper diagonal remains unchanged.
 * Lower-diagonal entries are also explicitly set to zero so the
 * stored diagonals reflect the resulting upper-triangular system.
 * Back substitution does not use the lower diagonal.
 *
 * The algorithm requires O(N) operations and O(N) storage. Unlike
 * Gaussian elimination with partial pivoting, it does not swap rows.
 * A zero or near-zero pivot stops this implementation, but does not
 * necessarily mean the original system lacks a unique solution.
 *
 * TODO: Add a separate pivoted tridiagonal solver. Restricted
 * adjacent-row pivoting can introduce a second superdiagonal while
 * retaining O(N) computational complexity.
 */

void ThomasAlgorithm::Solve_nPlus1(
        TridiagonalMatrix& A3_Obj,
        RHS& RHS_Obj,
        Field1D& Solution)
    {
    ForwardElimination(A3_Obj , RHS_Obj);
    BackSubstitution(A3_Obj , RHS_Obj, Solution);
    }

void ThomasAlgorithm::ForwardElimination(
        TridiagonalMatrix& A3_Obj,
        RHS& RHS_Obj)
{
    const std::size_t NumA3Rows = A3_Obj.GetMSize();

    if (NumA3Rows == 0)
    {
        throw std::invalid_argument(
            "ForwardElimination: diagonal vectors must not be empty.");
    }

    if (A3_Obj.GetLSize() != NumA3Rows|| A3_Obj.GetUSize() != NumA3Rows || RHS_Obj.GetSize() != NumA3Rows)
    {
        throw std::invalid_argument(
            "ForwardElimination: all diagonal vectors and RHS must have the same size.");
    }


for (std::size_t RowIndex = 1; RowIndex < NumA3Rows; ++RowIndex)
    {
    const double OriginalDiagonalValue = A3_Obj.M(RowIndex - 1);

    if (std::fabs(OriginalDiagonalValue) < 1.0e-14)
    {
        throw std::runtime_error(
            "ForwardElimination: zero or nearly zero diagonal entry detected.");
    }

    const double Factor =A3_Obj.L(RowIndex)/ OriginalDiagonalValue;


    const double NewDiagonal =A3_Obj.M(RowIndex)- Factor * A3_Obj.U(RowIndex - 1);

    A3_Obj.M(RowIndex) = NewDiagonal;

    const double NewRHS =RHS_Obj[RowIndex]- Factor * RHS_Obj[RowIndex - 1];

    RHS_Obj[RowIndex] = NewRHS;

    A3_Obj.L(RowIndex) = 0.0;

    }

}


void ThomasAlgorithm::BackSubstitution(
    const TridiagonalMatrix& A3_Obj,
    const RHS& RHS_Obj,
    Field1D& Solution)
{
    const std::size_t NumRows = A3_Obj.GetMSize();

    if (NumRows == 0)
    {
        throw std::invalid_argument(
            "ThomasAlgorithm::BackSubstitution: "
            "diagonal vectors must not be empty.");
    }

    if (A3_Obj.GetUSize() != NumRows
        || RHS_Obj.GetSize() != NumRows
        || Solution.GetSize() != NumRows)
    {
        throw std::invalid_argument(
            "ThomasAlgorithm::BackSubstitution: "
            "all vectors must have the same size.");
    }

    const double LastDiagonalValue = A3_Obj.M(NumRows - 1);

    if (std::fabs(LastDiagonalValue) < 1.0e-14)
    {
        throw std::runtime_error(
            "ThomasAlgorithm::BackSubstitution: "
            "zero or near-zero pivot encountered.");
    }

    Solution[NumRows - 1] = RHS_Obj[NumRows - 1] / LastDiagonalValue;


    for (std::ptrdiff_t RowIndex = static_cast<std::ptrdiff_t>(NumRows) - 2; RowIndex >= 0; --RowIndex)
    {
        
        const std::size_t j = static_cast<std::size_t>(RowIndex);

        const double DiagonalValue = A3_Obj.M(j);

        if (std::fabs(DiagonalValue) < 1.0e-14)
        {
            throw std::runtime_error(
                "ThomasAlgorithm::BackSubstitution: "
                "zero or near-zero pivot encountered.");
        }

        // Back substitution: x_j = (b_j - U_j * x_{j+1}) / M_j.
        // Solution[j + 1] has already been computed.


        const double NewSolution =
            (RHS_Obj[j]
             - A3_Obj.U(j)
               * Solution[j + 1])
            / DiagonalValue;

        Solution[j] = NewSolution;
    }
}