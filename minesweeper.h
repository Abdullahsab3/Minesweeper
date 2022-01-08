#ifndef MINESWEEPER_H
#define MINESWEEPER_H

enum INIT_GAME_VALUES
{
    NUMBEROFROWS = 10,
    NUMBEROFCOLS = 10,
    NUMBEROFMINES = 25,
    NUMBEROFNOTMINES = 0,
    NUMBEROFFLAGS = 0
};

enum WITH_GAME_INITIALISATION
{
    WITHOUT_INITIALISATION,
    WITH_INITIALISATION
};

enum GIVEN_FLAGS
{
    NO_FLAGS_GIVEN = 1,
    F_FLAG_GIVEN = 3,
    DIMENSIONS_FLAGS_GIVEN = 7,
};


enum input
{
    INPUT_ROW,
    INPUT_COL
};


typedef struct game_fields {
    int height;
    int width;
    int **fields;
} Game_fields;



#define UNREVEALED 9
#define INIT_NOT_MINE 0
#define FLAG 10
#define MINE 11


#define BUFFER_SIZE 100

#define DEFAULT_SAVED_GAME_PATH "field.txt"

#endif
