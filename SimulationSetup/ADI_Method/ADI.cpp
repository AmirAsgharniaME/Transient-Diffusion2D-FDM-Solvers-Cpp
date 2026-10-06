#include "SimulationSetup/ADI_Method/ADI.hpp"


ADI::ADI(SimulationContext& ctx)
    :T_nPlus1_Star_Row(SweepDirection::X,ctx.grid),
     T_nPlus1_Col(SweepDirection::Y,ctx.grid),
     T_nPlus1_Star(ctx.grid),
     A3(SweepDirection::X,ctx.grid),
     rhs(SweepDirection::X,ctx.grid)
     {}
