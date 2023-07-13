#include <stdio.h>
#include <dlfcn.h>
#include <assert.h>

#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <dlfcn.h>
#include <execinfo.h>
#include <copyfile.h>
#include <sys/stat.h> // for mkdir
#include <sys/types.h>
#include <dirent.h>

#include <unistd.h>

// NOTE: copied from another project. Quick and dirty
bool rdx_file_exists(char* path) {
	return access(path, F_OK) != -1;
}

// NOTE: copied from another project. Quick and dirty
int64_t rdx_last_modified(char* path)
{
	if (!rdx_file_exists(path)) {
		return -1;
	}
	#if _WIN32
	struct _stat info;
	_stat(path, &info);
	#else
	struct stat info;
	int err = stat(path, &info);
	assert(err == 0);
	#endif
    
	time_t time = info.st_mtime; // From some documentation: "For historical reasons, it is generally implemented as an integral value representing the number of seconds elapsed since 00:00 hours, Jan 1, 1970 UTC(i.e., a unix timestamp). Although libraries may implement this type using alternative time representations.

	return (uint64_t) time;
}

#define GAME_STATE_MAX_BYTE_SIZE 4096

int main()
{
    // Allocates some memory that we'll pass to the game so that its state can persist between reloads.
    // NOTE(seb): we can also decide to serialize the game state just before unloading, and deserialize after
    //       loading the new version of the game code. I think serializing would be better for the city builder. 
    void* game_state_memory = malloc(GAME_STATE_MAX_BYTE_SIZE);
    memset(game_state_memory, 0, GAME_STATE_MAX_BYTE_SIZE);

    void (*game_update_func)(void*);

    void* lib_handle = 0;
    
    uint64_t last_mod = 0;

    printf("Hello world!\n");

    while (true) {
        // Check modification time of the library file, to know if we should reload it.
        uint64_t new_last_mod = rdx_last_modified((char*) "./libgame.so");
        if (new_last_mod != -1 && new_last_mod != last_mod) {
            last_mod = new_last_mod;

            // Close previous version of the lib (NOTE(seb): could ask the game to serialize itself just before closing).
            if (lib_handle != 0) {
                dlclose(lib_handle);
            }

            // Load new version of the lib (NOTE(seb): could ask the game to deserialize itself just after loading).
            do {
                lib_handle = dlopen("./libgame.so", RTLD_NOW);
            } while (lib_handle == 0);

            assert(lib_handle != 0 && "Couldn't load libgame.so");

            game_update_func = (void (*)(void*)) dlsym(lib_handle, "game_update");
            assert(game_update_func != 0 && "Couldn't find function game_update in libgame.so.");
        }

        // Call the game update function once per frame (here we don't limit framerate)
        game_update_func(game_state_memory);
    }

    return 0;
}
