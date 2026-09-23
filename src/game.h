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
enum GameInputType
{
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN,
  JUMP,

  MOUSE_LEFT,
  MOUSE_RIGHT,

  PAUSE,

  GAME_INPUT_COUNT
};

struct KeyMapping
{
  Array<KeyCodeID, 3> keys;
};

struct GameState
{
    bool initialized = false;
    IVec2 playerPos;

    KeyMapping keyMappings[GAME_INPUT_COUNT];
};

// ##
// Game Globals
// ##
static GameState* gameState;


// ##
// Game Functions (Expuestas)
// ##
extern "C"
{
    EXPORT_FN void update_game(GameState* gameState, RenderData* renderDataIn, Input* inputIn);
}
