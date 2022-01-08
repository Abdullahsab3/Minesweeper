#include <stdlib.h>
#include <time.h>

#include "GUI.h"

#define NUMBEROFTEXTURES 12

/*
 * Deze renderer wordt gebruikt om figuren in het venster te tekenen.
 */
static SDL_Renderer *renderer;

/* De afbeeldingen met de vakjes*/
static SDL_Texture *textures[NUMBEROFTEXTURES];

/*
 * Onderstaande twee lijnen houden de laatste positie bij waar de gebruiker geklikt heeft.
 */
int mouse_x = 0;
int mouse_y = 0;


/*
 * Dit is het venster dat getoond zal worden en waarin het speelveld weergegeven wordt.
 * Dit venster wordt aangemaakt bij het initialiseren van de GUI en wordt weer afgebroken
 * wanneer het spel ten einde komt.
 */
static SDL_Window *window;

// deze twee procedures gaan na of de ingedrukte knop de linker/rechtermuisknop is
int is_lmbutton(Uint32 buttons)
{
	return (buttons == SDL_BUTTON_LEFT);
}

int is_rmbutton(Uint32 buttons)
{
	return (buttons == SDL_BUTTON_RIGHT);
}

// filtert de niet-relevante events weg.
int is_relevant_event(SDL_Event *event)
{
	if (event == NULL)
	{
		return 0;
	}
	return (event->type == SDL_MOUSEBUTTONDOWN) ||
		   (event->type == SDL_KEYDOWN) ||
		   (event->type == SDL_QUIT);
}

/*
 * Vangt de input uit de GUI op. 
 */
void read_input(char *operator, int operands[], int *input_given)
{
	SDL_Event event;

	while (!SDL_PollEvent(&event) || !is_relevant_event(&event))
	{
		// een poging om het probleem van het transparente venster op te lossen (niet gelukt :())
		SDL_PumpEvents();
	}
	switch (event.type)
	{
	case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_p)
			*operator= 'P';
		else if (event.key.keysym.sym == SDLK_q)
			*operator= 'Q';
		else
			break;
		*input_given = 1;
		break;
	case SDL_QUIT:
		/* De gebruiker heeft op het kruisje van het venster geklikt om de applicatie te stoppen. */
		*operator= 'Q';
		*input_given = 1;
		break;
	case SDL_MOUSEBUTTONDOWN:
		/*
		 * De speler heeft met de muis geklikt: met de onderstaande lijn worden de coördinaten in het
		 * het speelveld waar de speler geklikt heeft bewaard in de variabelen mouse_x en mouse_y.
		 */
		mouse_x = event.button.x;
		mouse_y = event.button.y;
		if (is_lmbutton(event.button.button))
		{
			*operator= 'R';
			operands[INPUT_ROW] = mouse_y / IMAGE_WIDTH;
			operands[INPUT_COL] = mouse_x / IMAGE_HEIGHT;
			*input_given = 1;
		}
		else if (is_rmbutton(event.button.button))
		{
			*operator= 'F';
			operands[INPUT_ROW] = mouse_y / IMAGE_WIDTH;
			operands[INPUT_COL] = mouse_x / IMAGE_HEIGHT;
			*input_given = 1;
		}
		break;
	}
}

// maakt een vakje voor het speelveld.
SDL_Rect draw_field(int row, int col, int el)
{

	SDL_Rect rectangle = {col * IMAGE_WIDTH, row * IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT};
	return rectangle;
}


// tekent het speelveld.
void draw_window(Game_fields *fields)
{
	int rows = fields->height;
	int cols = fields->width;
	int **fields_matrix = fields->fields;

	SDL_RenderClear(renderer);

	SDL_Rect field;
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			field = draw_field(i, j, fields_matrix[i][j]);
			SDL_RenderCopy(renderer, textures[fields_matrix[i][j]], NULL, &field);
		}
	}
	SDL_RenderPresent(renderer);
}

/*
 * Initialiseert het venster en alle extra structuren die nodig zijn om het venster te manipuleren.
 */
void initialize_window(const char *title, int window_height, int window_width)
{
	/*
	 * Code o.a. gebaseerd op:
	 * http://lazyfoo.net/tutorials/SDL/02_getting_an_image_on_the_screen/index.php
	 */
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("Could not initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	/* Maak het venster aan met de gegeven dimensies en de gegeven titel. */
	window = SDL_CreateWindow(title, 0, 0, window_width, window_height, SDL_WINDOW_SHOWN);

	if (window == NULL)
	{
		/* Er ging iets verkeerd bij het initialiseren. */
		printf("Couldn't set screen mode to required dimensions: %s\n", SDL_GetError());
		exit(1);
	}

	/* Initialiseert de renderer. */
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC);
	/* Laat de default-kleur die de renderer in het venster tekent wit zijn. */
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

/*
 * Dealloceert alle SDL structuren die geïnitialiseerd werden.
 */
void free_gui()
{
	for(int i = 0; i < NUMBEROFTEXTURES; i++) {
		SDL_DestroyTexture(textures[i]);
	}
	/* Dealloceert de SDL_Textures die werden aangemaakt. */
	//SDL_DestroyTexture(digit_1_texture);
	/* Dealloceert de renderer. */
	SDL_DestroyRenderer(renderer);
	/* Dealloceert het venster. */
	SDL_DestroyWindow(window);


	/* Sluit SDL af. */
	SDL_Quit();
}

/*
 * Laadt alle afbeeldingen die getoond moeten worden in.
 */

void initialize_texture(int n)
{
	/*
	 * Laadt een afbeelding in.
	 */
	char pathTobmp[PATH_LENGTH];
	sprintf(pathTobmp, "Images/%d.bmp", n);

	SDL_Surface *digit_surface = SDL_LoadBMP(pathTobmp);
	/*
	 * Zet deze afbeeldingen om naar een texture die getoond kan worden in het venster.
	 * Indien de texture niet kon omgezet worden, geeft de functie een NULL-pointer terug.
	 */
	if((textures[n] = SDL_CreateTextureFromSurface(renderer, digit_surface)) == NULL)
		{
			printf("Error: Unable to make the texture.\n");
			exit(1);
		}
	/* Dealloceer het SDL_Surface dat werd aangemaakt. */
	SDL_FreeSurface(digit_surface);
}

// laadt alle afbeeldingen in en initialsieert de textures.
void initialize_textures()
{
	for (int ctr = 0; ctr < NUMBEROFTEXTURES; ctr++)
	{
		initialize_texture(ctr);
	}
}

/*
 * Initialiseert onder het venster waarin het spelveld getoond zal worden, en de texture van de afbeelding die getoond zal worden.
 */
void initialize_gui(int rows, int cols)
{
	int window_height = rows * IMAGE_HEIGHT;
	int window_width = cols * IMAGE_WIDTH;
	initialize_window("Minesweeper", window_height, window_width);
	initialize_textures();
}