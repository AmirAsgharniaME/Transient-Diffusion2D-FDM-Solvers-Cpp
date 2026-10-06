#pragma once

#include "Core/Field/Field.hpp"
#include "Core/Setup/Meshes.hpp"
#include "Core/Setup/Geometries.hpp"
#include "Config/SolverInputs.hpp"
#include "Core/Setup/Boundaries.hpp"

#include "Core/FileWriter/FileWriter.hpp"
#include "Config/FileName.hpp"
#include "Config/PathManager.hpp"
#include "Config/Label.hpp"
#include "Verification/AnalyticalSolution/AnalyticalSolution.hpp"



struct Fields
{
    Field T_0;
    Field T_n;
    Field T_nPlus1;
    Field T_SteadyState;
    Field T_Analytical_n;



    explicit Fields(const Meshes& Meshes_Obj, const Boundaries& Boundaries_Obj,const Geometries& Geometries_Obj)
        :T_0(Meshes_Obj.Grid),
         T_n(Meshes_Obj.Grid),
         T_nPlus1(Meshes_Obj.Grid),
         T_SteadyState(Meshes_Obj.Grid),
         T_Analytical_n(Meshes_Obj.Grid)

    {
        //Apply Initial Conditions To Field
        T_0.SetIntitialProfile(SolverInputs::InitialCondition::InitialProfile);
        //-------------------------------------------------------------------------------------------------
        //Apply Boundary Conditions To Field at time 0
        T_0.ApplyBoundaryCondition(Boundaries_Obj.TopWall);
        T_0.ApplyBoundaryCondition(Boundaries_Obj.BottomWall);
        T_0.ApplyBoundaryCondition(Boundaries_Obj.RightWall);
        T_0.ApplyBoundaryCondition(Boundaries_Obj.LeftWall);

        //-------------------------------------------------------------------------------------------------
        //Write the Initial Field to a file
        FileWriter::WriteField
            (
            FileName::Create(FieldName::T,0.0),
            T_0,
            Meshes_Obj.Grid,
            Path::Create(OutputCategory::Initial)
            );
            //-------------------------------------------------------------------------------------------------


        AnalyticalSolution::Pass(
            T_SteadyState,
            Geometries_Obj.G1,
            Meshes_Obj.Grid,
            Boundaries_Obj);

        FileWriter::WriteField
            (
            FileName::Create(FieldName::TAnalytical,Label::Steady_State),
            T_SteadyState,
            Meshes_Obj.Grid,
            Path::Create(OutputCategory::Analytical)
            );

    }
};