#pragma once

#include "input.h"
#include "clonceleste_lib.h"
#include "render_interface.h"

// ##
// Game Globals
// ##
constexpr int tset = 5;

// ##
// Game Structs
// ##
struct GameState
{
    IVec2 playerPos;
};

// ##
// Game Functions (Expuestas)
// ##
extern "C"
{
    EXPORT_FN void update_game(RenderData* renderDataIn, Input* inputIn);
}
