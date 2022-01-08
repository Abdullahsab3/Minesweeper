#ifndef GUI_H_
#define GUI_H_

#include <stdio.h>
#include <stdlib.h>
#include "minesweeper.h"

/*
 * Importeer de benodigde functies uit SDL2.
 */
#include <SDL2/SDL.h>

void draw_window(Game_fields *fields); //[NUMBEROFROWS][NUMBEROFCOLS]);
void initialize_gui(int rows, int cols);
void initialize_textures();
void free_gui();
void read_input(char *operator, int operands[], int *input_given);


/*
 * De hoogte en breedte van het venster (in pixels).
 * Deze dimensies zijn arbitrair gekozen. Deze dimensies hangen mogelijk af van de grootte van het speelveld.
 */
#define WINDOW_HEIGHT 500
#define WINDOW_WIDTH 500

/*
 * De hoogte en breedte (in pixels) van de afbeeldingen voor de vakjes in het speelveld die getoond worden.
 * Als je andere afbeelding wil gebruiken in je GUI, zorg er dan voor dat deze
 * dimensies ook aangepast worden.
 */
#define IMAGE_HEIGHT 50
#define IMAGE_WIDTH 50

#define PATH_LENGTH 13

#endif /* GUI_H_ */
