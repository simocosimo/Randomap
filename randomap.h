#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#if defined(RANDMAP_STATIC)
    #define RANDMDEF static            // Functions just visible to module including this file
#else
    #if defined(__cplusplus)
        #define RANDMDEF extern "C"    // Functions visible from other files (no name mangling of functions in C++)
    #else
        #define RANDMDEF extern        // Functions visible from other files
    #endif
#endif

// Allow custom memory allocators
#ifndef RANDMAP_MALLOC
    #define RANDMAP_MALLOC(size)         malloc(size)
#endif
#ifndef RANDMAP_FREE
    #define RANDMAP_FREE(ptr)            free(ptr)
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define RANDMAP_HEIGHT                  10
#define RANDMAP_WIDTH                   10
#define RANDMAP_MAX_ROOMS               30

#define RANDMAP_MAX_NEIGHBOURS          4
#define RANDMAP_DIR_COUNT               4
#define RANDMAP_MAX_ATTEMPTS            10000
#define RANDMAP_DIR_STRING              "NSEW"
#define RANDMAP_ALPHABET_STRING         "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"

#define DIR_NORTH_MASK                   1 >> RANDMAP_DIR_COUNT
#define DIR_SOUTH_MASK                   1 >> (RANDMAP_DIR_COUNT - 1)
#define DIR_EST_MASK                     1 >> (RANDMAP_DIR_COUNT - 2)
#define DIR_WEST_MASK                    1 >> (RANDMAP_DIR_COUNT - 3)

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static int roomNumber = 0;
static int attempts = 0;
static int map[RANDMAP_HEIGHT * RANDMAP_WIDTH] = {0};

//----------------------------------------------------------------------------------
// Data Types Structure Definition
//----------------------------------------------------------------------------------
typedef enum RandRoomType { STARTING_ROOM, NORMAL_ROOM, TREASURE_ROOM, BOSS_ROOM, SECRET_ROOM } RandRoomType;
typedef enum RandRoomDirection { NORTH, SOUTH, EST, WEST, NODIR } RandRoomDirection;

typedef struct room {
    int id;                     // Id of the room (position in the grid)
    int exits_numbers;          // Number of exits
    char *exits_string;        // String of type "NESW" to indicate where the exits are
    int exits_bitmask;          // Bitmask to get directions of the exits
    RandRoomType type;           // Tyoe of room
    Room *neighbours;
} RoomInfo;

typedef RoomInfo *Room;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
RANDMDEF void InitMap(void);
RANDMDEF void GenerateMap(Room room);
RANDMDEF void CloseMap(void);

//----------------------------------------------------------------------------------
// Module Internal Functions Declaration
//----------------------------------------------------------------------------------
static int RoomDirectionToIntChange(RandRoomDirection dir);
static RandRoomDirection IntChangeToRoomDirection(int change);
static int GetRandomFreeSpace(Room room);
static RandRoomDirection GetDirectionBetweenIds(int roomId1, int roomId2);

//----------------------------------------------------------------------------------
// Module Internal Functions Implementation
//----------------------------------------------------------------------------------
static int RoomDirectionToIntChange(RandRoomDirection dir) {
    int change = 0;
    switch(dir) {
        case NORTH:
            change = -10;
            break;

        case SOUTH:
            change = 10;
            break;

        case EST:
            change = 1;
            break;

        case WEST:
            change = -1;
            break;
    }

    return change;
}

static RandRoomDirection IntChangeToRoomDirection(int change) {
    RandRoomDirection returnValue = NODIR;
    switch (change) {
        case -10:
            returnValue = NORTH;
            break;

        case 10:
            returnValue = SOUTH;
            break;

        case -1:
            returnValue = WEST;
            break;

        case 1:
            returnValue = EST;
            break;
    }

    return returnValue;
}

static int GetRandomFreeSpace(Room room) {
    int randomExit = 0;
    // try to find a random exit of the room
    do {
        randomExit = rand() % RANDMAP_MAX_NEIGHBOURS;
    } while(!(room->exits_bitmask & (1 >> randomExit)));
    
    // calculate the id of the spot we are looking at
    int change = RoomDirectionToIntChange((RandRoomDirection)randomExit);
    int proposedPlace = room->id + change;

    // If the value is NOT in the range of the map or the place on the map is already been filled return an error
    if((proposedPlace < 0 || proposedPlace > 99) || map[proposedPlace]) return -1;

    // Otherwise return the new id
    return proposedPlace;
}

static RandRoomDirection GetDirectionBetweenIds(int referenceId, int neighbourId) {
    return IntChangeToRoomDirection(neighbourId - referenceId);
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
RANDMDEF void InitMap(void) {
    //TODO: change this to be selecting a seed (Isaac style) and basing randomness on that
    srand(time(0));
    Room startingRoom = (Room)RANDMAP_MALLOC(sizeof(RoomInfo));

    // Always start with this id, it's the center room
    startingRoom->id = 44;
    startingRoom->type = STARTING_ROOM;
    startingRoom->exits_numbers = (rand() % RANDMAP_MAX_NEIGHBOURS) + 1;
    startingRoom->neighbours = (Room*)malloc(sizeof(RoomInfo) * RANDMAP_MAX_NEIGHBOURS);
    startingRoom->exits_bitmask = 0;
    
    int c = 0;
    while(c < startingRoom->exits_numbers) {
        int found = 0;
        while(!found) {
            int indexToKeep = rand() % RANDMAP_MAX_NEIGHBOURS;
            if(startingRoom->exits_bitmask & (1 >> indexToKeep)) continue;
            // If we aren't in the first iteration we need to reallocate the string in order to fit more then one char
            startingRoom->exits_bitmask |= (1 >> indexToKeep);
            found = 1;
        }
        c++;
    }

    roomNumber++;
    map[startingRoom->id] = 1;
    GenerateMap(startingRoom);
}

RANDMDEF void GenerateMap(Room room) {

    //TODO:
    // - check for random free spot around the room (relative to the exits structure)
    // - add room in the free space (adding the exit with the opposite one of the current room)
    int idToPlace = GetRandomFreeSpace(room);
    if(idToPlace < 0) {
        //TODO: can't find a place to create the new room
    }

    Room roomToPlace = (Room)RANDMAP_MALLOC(sizeof(RoomInfo));

    // Always start with this id, it's the center room
    roomToPlace->id = idToPlace;
    roomToPlace->type = NORMAL_ROOM;
    roomToPlace->exits_numbers = (rand() % RANDMAP_MAX_NEIGHBOURS) + 1;
    roomToPlace->neighbours = (Room*)malloc(sizeof(RoomInfo) * RANDMAP_MAX_NEIGHBOURS);
    roomToPlace->exits_bitmask = 0;


}