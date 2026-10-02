#include "clonceleste_lib.h"

#include "input.h"

#include "game.h"

#include "platform.h"

#define WIN32_LEAN_AND_MEAN
#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include "glcorearb.h"

static KeyCodeID KeyCodeLookupTable[KEY_COUNT];

#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.cpp"

// ###
// Game DLL STUFF( HOT CODE RELOADING )
// ###
// Esta es la funcion puntero, hacia el update_game() en game.cpp
typedef decltype(update_game) update_game_type;
static update_game_type* update_game_ptr;

// ###
// Cross Platform Functions
// ###
void reload_game_dll(BumpAllocator* transientStorage);


int main()
{
BumpAllocator transientStorage = make_bump_allocator(MB(50)); // 10 MB
BumpAllocator persistentStorage = make_bump_allocator(MB(50));

input = (Input*)bump_alloc(&persistentStorage, sizeof(Input));
if(!input)
{
    SM_ERROR(false, "Failed to allocate Input");
    return -1;
}

renderData = (RenderData*)bump_alloc(&persistentStorage, sizeof(RenderData));
if(!renderData)
{
    SM_ERROR(false, "Failed to allocate RenderData");
    return -1;
}

gameState = (GameState*)bump_alloc(&persistentStorage, sizeof(GameState));
if(!gameState)
{
    SM_ERROR(false, "Failed to allocate GameState");
    return -1;
}

platform_fill_keycode_lookup_table();
platform_create_window(1280, 720, "Baldegon Motor");

gl_init(&transientStorage);

while(running)
{
    reload_game_dll(&transientStorage);
    // Update
    platform_update_window();
    update_game(gameState, renderData, input);
    gl_render(&transientStorage);
    platform_swap_buffers();

    transientStorage.used = 0; // Reset the transient storage for the next frame
}

return 0;
}

void update_game(GameState* gameStateIn,RenderData* renderDataIn, Input* inputIn)
{
    update_game_ptr(gameStateIn, renderDataIn, inputIn);
}

void reload_game_dll(BumpAllocator* transientStorage)
{
    static void* gameDLL;
    static long long lastEditTimestampGameDLL;

    long long currentTimestampGameDLL = get_timestamp("game.dll");
    if(currentTimestampGameDLL > lastEditTimestampGameDLL)
    {
        if(gameDLL)
        {
            bool freeResult = platform_free_dynamic_library(gameDLL);
            SM_ASSERT(freeResult, "Failed to free game.dll");
            gameDLL = nullptr;
            SM_TRACE("Freed game.dll");
        }

        while(!copy_file("game.dll", "game_load.dll", transientStorage))
        {
            Sleep(10);
        }
        SM_TRACE("Copied game.dll to game_load.dll");

        gameDLL = platform_load_dynamic_library("game_load.dll");
        SM_ASSERT(gameDLL, "Failed to load game.dll");

        update_game_ptr = (update_game_type*)platform_load_dynamic_function(gameDLL, "update_game");
        SM_ASSERT(update_game_ptr, "Failed to load update_game function");
        lastEditTimestampGameDLL = currentTimestampGameDLL;
    }
}