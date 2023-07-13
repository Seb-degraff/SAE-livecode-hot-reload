#include <stdio.h>

struct GameState {
    int frame_count;
    v
};

GameState* game_state = 0;

extern "C" {

#if _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

EXPORT void game_update(void* memory)
{
    game_state = (GameState*) memory;

    printf("Test 22 %i\n", game_state->frame_count);
    game_state->frame_count++;
}

}
