#include "LinearSolvers/GaussianElimination/GaussianElimination.hpp"
#include <stdexcept>
#include <cmath>

void GaussianElimination::Solve_nPlus1(
    CoefficientMatrix& A_Obj,
    RHS& RHS_Obj,
    Field& Solution1D_Obj)
{
    const std::size_t NumARows = A_Obj.GetRows();
    const std::size_t NumACols = A_Obj.GetCols();

    if (NumARows != NumACols)
    {
        throw std::invalid_argument("Solve: matrix A_Obj must be square.");
    }

    if (RHS_Obj.GetSize() != NumARows || Solution1D_Obj.Get_Nx()*Solution1D_Obj.Get_Ny() != NumARows)
    {
        throw std::invalid_argument("Solve: size of RHS and Solution must match matrix size.");
    }

    ForwardElimination(A_Obj, RHS_Obj);
    BackSubstitution(A_Obj, RHS_Obj, Solution1D_Obj);
}

void GaussianElimination::ForwardElimination(
    CoefficientMatrix& A_Obj,
    RHS& RHS_Obj)
{
    const std::size_t NumARows = A_Obj.GetRows();
    const std::size_t NumACols = A_Obj.GetCols();

    if (NumARows != NumACols)
    {
        throw std::invalid_argument("ForwardElimination: matrix A must be square.");
    }

    if (RHS_Obj.GetSize() != NumARows)
    {
        throw std::invalid_argument("ForwardElimination: size of RHS must match matrix size.");
    }

    if (NumARows == 0)
    {
        return;
    }

    for (std::size_t PivotColIndex = 0; PivotColIndex < NumACols - 1; ++PivotColIndex)
    {
        std::size_t PivotRowIndex = PivotColIndex;

        const std::size_t Candidate_PivotRowIndex = FindPivotRowIndex(A_Obj, PivotColIndex);

        if (Candidate_PivotRowIndex != PivotRowIndex)
        {
            SwapRows(A_Obj, RHS_Obj, PivotRowIndex, Candidate_PivotRowIndex);
        }

        const double PivotValue = A_Obj[PivotRowIndex][PivotColIndex];

        if (std::fabs(PivotValue) < 1.0e-14)
        {
            throw std::runtime_error("ForwardElimination: singular or nearly singular matrix detected.");
        }


        for (std::size_t RowIndex = PivotRowIndex + 1; RowIndex < NumARows; ++RowIndex)
        {
            const double Factor = A_Obj[RowIndex][PivotColIndex] / PivotValue;

            A_Obj[RowIndex][PivotColIndex] = 0.0;

            for (std::size_t ColIndex = PivotColIndex + 1; ColIndex < NumACols; ++ColIndex)
            {
                const double NewValue =
                    A_Obj[RowIndex][ColIndex] - Factor * A_Obj[PivotRowIndex][ColIndex];
                A_Obj[RowIndex][ColIndex] = NewValue;
            }

            const double NewRHS =
                RHS_Obj[RowIndex] - Factor * RHS_Obj[PivotRowIndex];
            RHS_Obj[RowIndex] = NewRHS;
        }
    }

    if (std::fabs(A_Obj[NumARows - 1][NumACols - 1]) < 1.0e-14)
    {
        throw std::runtime_error("ForwardElimination: zero pivot found on last row.");
    }
}

// Mathematical formulation of back-substitution:
// x_j = ( b_j - \sum_{i=j+1}^{N-1} A_{j,i} * x_i ) / A_{j,j}
//
// Where:
//   j         : Current row index, processed from N-1 down to 0
//   i         : Column index, ranging over the known terms to the right of
//               the diagonal, where i > j
//   A_{j,i}   : Matrix coefficient at row j and column i
//   A_{j,j}   : Diagonal coefficient at row j and column j
//   b_j       : RHS value corresponding to row j
//   x_i       : Previously computed solution value for column i
//   sum_upper : Sum of known upper-triangular terms:
//               \sum_{i=j+1}^{N-1} A_{j,i} * x_i
//   x_j       : Unknown solution value computed for row j



void GaussianElimination::BackSubstitution(
    const CoefficientMatrix& A_Obj,
    const RHS& RHS_Obj,
    Field& Solution)
{
    const std::size_t NumARows = A_Obj.GetRows();
    const std::size_t NumACols = A_Obj.GetCols();

    const std::size_t Nx = Solution.Get_Nx();
    const std::size_t Ny = Solution.Get_Ny();

    if (NumARows != NumACols)
    {
        throw std::invalid_argument("BackSubstitution: matrix A must be square.");
    }

    if (RHS_Obj.GetSize() != NumARows || Nx * Ny != NumARows)
    {
        throw std::invalid_argument("BackSubstitution: size of RHS and Solution must match matrix size.");
    }

// Mathematical formula:
// x_j = ( b_j - \sum_{i=j+1}^{N-1} A_{j,i} * x_i ) / A_{j,j}

    // Iterate backward from the last row (N-1) down to the first row (0)
    for (std::ptrdiff_t row = static_cast<std::ptrdiff_t>(NumARows) - 1; row >= 0; --row)
    {
        const std::size_t GlobalRowIndex = static_cast<std::size_t>(row);

        const std::size_t CurrentJ = GlobalRowIndex / Nx;
        const std::size_t CurrentI = GlobalRowIndex % Nx;

        /// Calculate the sum of known upper-triangular terms:
        // \sum_{i=j+1}^{N-1} A_{j,i} * x_i

        double sum_upper = 0.0;

        for (std::size_t GlobalColumnIndex = GlobalRowIndex + 1;
             GlobalColumnIndex < NumACols;
             ++GlobalColumnIndex)
        {
            const std::size_t KnownJ = GlobalColumnIndex / Nx;
            const std::size_t KnownI = GlobalColumnIndex % Nx;

            sum_upper += A_Obj[GlobalRowIndex][GlobalColumnIndex]
                       * Solution[KnownJ][KnownI];
        }

        const double A_jj = A_Obj[GlobalRowIndex][GlobalRowIndex];

        if (std::fabs(A_jj) < 1.0e-14)
        {
            throw std::runtime_error("BackSubstitution: zero diagonal entry detected.");
        }

        const double b_j = RHS_Obj[GlobalRowIndex];

        // Compute the current unknown:
        // x_j = (b_j - sum_upper) / A_{j,j}

        Solution[CurrentJ][CurrentI] = (b_j - sum_upper) / A_jj;
    }
}




std::size_t GaussianElimination::FindPivotRowIndex(
    const CoefficientMatrix& A_Obj,
    const std::size_t PivotIndex)
{
    const std::size_t NumARows = A_Obj.GetRows();

    // Ensure the pivot index is within matrix bounds
    if (PivotIndex >= NumARows)
    {
        throw std::out_of_range("FindPivotRowIndex: PivotIndex is out of range.");
    }

    // Initialize the best candidate with the current diagonal element
    std::size_t CandidateRowIndex = PivotIndex;
    double MaxCandidateAbsValue = std::fabs(A_Obj[PivotIndex][PivotIndex]);

    // Search subsequent rows for the largest absolute pivot value
    for (std::size_t RowIndex = PivotIndex + 1; RowIndex < NumARows; ++RowIndex)
    {
        const double CurrentAbsValue = std::fabs(A_Obj.GetValue(RowIndex, PivotIndex));

        if (CurrentAbsValue > MaxCandidateAbsValue)
        {
            MaxCandidateAbsValue = CurrentAbsValue;
            CandidateRowIndex = RowIndex;
        }
    }

    return CandidateRowIndex;
}



void GaussianElimination::SwapRows(
    CoefficientMatrix& A_Obj,
    RHS& RHS_Obj,
    std::size_t FirstRowIndex,
    std::size_t SecondRowIndex)
{
    const std::size_t NumACols = A_Obj.GetCols();

    if (FirstRowIndex >= A_Obj.GetRows() || SecondRowIndex >= A_Obj.GetRows())
    {
        throw std::out_of_range("SwapRows: row index is out of range.");
    }

    if (RHS_Obj.GetSize() != A_Obj.GetRows())
    {
        throw std::invalid_argument("SwapRows: size of RHS must match matrix size.");
    }

    if (FirstRowIndex == SecondRowIndex)
    {
        return;
    }

    for (std::size_t ColIndex = 0; ColIndex < NumACols; ++ColIndex)
    {
        const double Temp = A_Obj[FirstRowIndex][ColIndex];
        A_Obj[FirstRowIndex][ColIndex] = A_Obj[SecondRowIndex][ColIndex];
        A_Obj[SecondRowIndex][ColIndex] = Temp;
    }

    const double TempRHS = RHS_Obj[FirstRowIndex];
    RHS_Obj[FirstRowIndex] = RHS_Obj[SecondRowIndex];
    RHS_Obj[SecondRowIndex] = TempRHS;
}
