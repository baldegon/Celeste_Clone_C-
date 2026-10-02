#include "game.h"

#include "assets.h"
#include "clonceleste_lib.h"


// #######################################################
//                      Game Constants
// #######################################################


// #######################################################
//                      Game Structs
// #######################################################

// #######################################################
//                      Game Functions 
// #######################################################
bool just_pressed(GameInputType type)
{
    KeyMapping mapping = gameState->keyMappings[type];
    for(int idx = 0; idx < mapping.keys.count; idx++){
        if(input->keys[mapping.keys[idx]].justPressed)
        {
            return true;
        }
    }

    return false;
}

bool is_down(GameInputType type)
{
    KeyMapping mapping = gameState->keyMappings[type];
    for(int idx = 0; idx < mapping.keys.count; idx++)
    {
        if(input->keys[mapping.keys[idx]].isDown)
        {
            return true;
        }
    }

    return false;
}

Tile* get_tile(int x, int y)
{
    Tile* tile = nullptr;

    if(x >= 0 && x < WORLD_GRID.x && y >= 0 && y < WORLD_GRID.y)
    {
        tile = &gameState->worldGrid[x][y];
    }

    return tile;
}

Tile* get_tile(IVec2 worldPos){
    int x = worldPos.x / TILESIZE;
    int y = worldPos.y / TILESIZE;

    return get_tile(x, y);
}

// #######################################################
//                      Game Functions (Expuestas)
// #######################################################
EXPORT_FN void update_game(GameState* gameStateIn,RenderData* renderDataIn, Input* inputIn)
{
if(renderData != renderDataIn)
{
    gameState = gameStateIn;
    renderData = renderDataIn;
    input = inputIn;
}

if(!gameState -> initialized)
{
    renderData->gameCamera.dimensions = {WORLD_WIDTH, WORLD_HEIGHT};
    gameState->initialized = true;

    // Key Mappings
    {
      gameState->keyMappings[MOVE_UP].keys.add(KEY_W);
      gameState->keyMappings[MOVE_UP].keys.add(KEY_UP);
      gameState->keyMappings[MOVE_LEFT].keys.add(KEY_A);
      gameState->keyMappings[MOVE_LEFT].keys.add(KEY_LEFT);
      gameState->keyMappings[MOVE_DOWN].keys.add(KEY_S);
      gameState->keyMappings[MOVE_DOWN].keys.add(KEY_DOWN);
      gameState->keyMappings[MOVE_RIGHT].keys.add(KEY_D);
      gameState->keyMappings[MOVE_RIGHT].keys.add(KEY_RIGHT);
      gameState->keyMappings[MOUSE_LEFT].keys.add(KEY_MOUSE_LEFT);
      gameState->keyMappings[MOUSE_RIGHT].keys.add(KEY_MOUSE_RIGHT);
      gameState->keyMappings[JUMP].keys.add(KEY_SPACE);
      gameState->keyMappings[PAUSE].keys.add(KEY_ESCAPE);

    }
    renderData->gameCamera.position.x = 160;
    renderData->gameCamera.position.y = -90;
    
    //Tileset
    {
        IVec2 tilesPosition = {48,0};
    
        for(int y = 0; y < 5; y++)
        {
            for(int x = 0; x< 4; x++)
            {
                gameState->tileCoords.add({tilesPosition.x + x * 8, tilesPosition.y + y * 8});
            }
        }
    
        // Black Inside
        gameState->tileCoords.add({tilesPosition.x, tilesPosition.y + 5 * 8});
    }
}

if(is_down(MOUSE_LEFT)){
    IVec2 mousePosWorld = input->mousePosWorld;
    Tile* tile = get_tile(mousePosWorld);
    if(tile){
        tile->isVisible = true;
    }
}

if(is_down(MOUSE_RIGHT)){
    IVec2 mousePosWorld = input->mousePosWorld;
    Tile* tile = get_tile(mousePosWorld);
    if(tile){
        tile->isVisible = false;
    }
}

// Dibujar Tileset
{
    //Tiles vecinos             Top     Izq         Der         Abajo
    int neighbourOffsets[24] = {0,-1,   -1,0,       1,0,        0,1,
    //                          TopIzq  TopDer      AbajoIzq    AbajoDer
                               -1,-1,   1,-1,       -1,1,       1,1,
    //                          Top2    Izq2        Der2        Abajo2
                                0,-2,   -2,0,       2,0,        0,2};

    // TopIzq ( topLeft )   = BIT(4) = 12
    // TopDer ( topRight )   = BIT(5) = 32
    // BottomIzq ( bottomLeft )   = BIT(6) = 64
    // BottomDer ( bottomRight )   = BIT(7) = 128


    for(int y = 0; y < WORLD_GRID.y; y++){
        for(int x = 0; x < WORLD_GRID.x; x++)
        {
            Tile* tile = get_tile(x, y);

            if(!tile->isVisible){
                continue;
            }

            tile->neighbourMask = 0;
            int neighbourCount = 0;
            int extendedNeighbourCount = 0;
            int emptyNeighbourSlot = 0;

            // Mira a los alrededores de los 12 vecinos
            for(int n = 0; n < 12; n++){
                Tile* neighbour = get_tile(x + neighbourOffsets[n * 2],
                                           y + neighbourOffsets[n * 2 + 1]);

                // Si no hay vecinos, significa el borde del mundo
                if(!neighbour || neighbour->isVisible)
                {
                    tile->neighbourMask |= BIT(n);
                    if(n < 8) // contamos los vecinos directos
                    {
                        neighbourCount++;
                    }
                    else{ // Contamos los vecinos 1 tile alejado
                        extendedNeighbourCount++;
                    }
                }
                else if(n < 8)
                {
                    emptyNeighbourSlot = n;
                }
            }

            if(neighbourCount == 7 && emptyNeighbourSlot >= 4) // tenemos una esquina
            {
                tile->neighbourMask = 16 + (emptyNeighbourSlot - 4);
            }
            else if(neighbourCount == 8 && extendedNeighbourCount == 4)
            {
                tile->neighbourMask = 20;
            }
            else
            {
                tile->neighbourMask = tile->neighbourMask & 0b1111;
            }

            // Dibujamos un Tile
            Transform transform = {};
            // Dibujamos el Tile alrededor del centro
            transform.pos = {x * (float)TILESIZE, y * (float)TILESIZE};
            transform.size = {8,8};
            transform.spriteSize = {8,8};
            transform.atlasOffset = gameState->tileCoords[tile->neighbourMask];
            draw_quad(transform);
        }
    }
}


draw_sprite(SPRITE_DICE, gameState->playerPos);

if(is_down(MOVE_LEFT))
{
    gameState->playerPos.x -= 1;
}
if(is_down(MOVE_RIGHT))
{
    gameState->playerPos.x += 1;
}
if(is_down(MOVE_UP))
{
    gameState->playerPos.y -= 1;
}
if(is_down(MOVE_DOWN))
{
    gameState->playerPos.y += 1;
}
}