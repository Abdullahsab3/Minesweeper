#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "helpers.h"
#include "minesweeper.h"
#include "GUI.h"

/*

          __  ____                                                   
         /  |/  (_)___  ___  ______      _____  ___  ____  ___  _____
        / /|_/ / / __ \/ _ \/ ___/ | /| / / _ \/ _ \/ __ \/ _ \/ ___/
       / /  / / / / / /  __(__  )| |/ |/ /  __/  __/ /_/ /  __/ /    
      /_/  /_/_/_/ /_/\___/____/ |__/|__/\___/\___/ .___/\___/_/     
                                                 /_/                 
                 
                          Abdullah Sabaa Allil
                      abdullah.sabaa.allil@vub.be
                           Rolnummer: 0575278
                       Vrije Universiteit Brussel
                               2021-2022


*/

void decode_game_values(char *str, int *NumberOfRows, int *NumberOfCols, int *AmountOfMines, int *AmountOfNotMines, int *AmountOfFlags);
void get_formatted_field(FILE *fp, int *field, int *outfield);
void decode_fields(FILE *fp, int **outfields, int **fields, int rows, int cols);

void put_game_variables(FILE *stream, int NumberOfRows, int NumberOfCols, int AmountOfMines, int AmountOfNotMines, int AmountOfFlags);
void put_formatted_field(FILE *stream, int first, int second);
void save_fields(FILE *fp, int **fields, int **outfields, int rows, int cols);
void save_game(int **fields, int **outfields, int NumberOfRows, int NumberOfCols, int AmountOfMines, int AmountOfNotMines, int AmountOfFlags);

void print_usage(void);
void print_intro(void);
void print_nrflags(int AmountOfFlags);

int is_w_flag(char *arg);
int is_h_flag(char *arg);
int is_m_flag(char *arg);
int is_f_flag(char *arg);
int convert_arg_to_number(char *str);
void process_dimensions_flags(char *argv[], int *rows, int *cols, int *aom);
int is_amountofmines_valid(int rows, int cols, int aom, int aonm);

Game_fields *make_game_fields(int height, int width);
void free_game_fields(Game_fields *fields);

int is_mine(int el);
int is_init_not_mine(int el);
int is_flag(int el);
int is_unrevealed(int el);
int is_revealed(int el);
int all_not_mines_revealed(int AmountOfNotMines);
int all_mines_flagged(int AmountOfMines);

int is_gamewon(int AmountOfNotMines, int AmountOfMines);
void gameover();
void gameWon();

void set_flag(Game_fields *fields, Game_fields *outfields, int row, int col, int *AmountOfFlags, int *AmountofMines);
int is_out_of_boundaries(int operands[], int rows, int cols);
void process_command(int *print_all_fields, int *input_given, char *operator, int operands[], Game_fields *fields, Game_fields *outfields, int *AmountOfNotMines, int *AmountofFlags, int *AmountOfMines);
void process_first_command(int *first_input, int *print_all_fields, int *input_given, char *operator, int operands[], Game_fields *fields, Game_fields *outfields, int *AmountOfNotMines, int *AmountofFlags, int *AmountOfMines);

void swap_mine_with_not_mine(Game_fields *fields, int row, int col);
void reveal_surroundings(Game_fields *fields, Game_fields *outfields, int row, int col, int *AmountOfNotMines, int *AmountOfFlags, int *AmountOfMines);
void reveal(Game_fields *fields, Game_fields *outfields, int row, int col, int *AmountOfNotMines, int *AmountOfFlags, int *AmountOfMines);
int how_many_surrounding_mines(Game_fields *fields, int row, int col);

void fill_fields(Game_fields *fields, int AmountOfMines);
void initialize_fields(Game_fields *fields);
void fill_outfields(Game_fields *outfields);

void first_round(int *print_all_fields, int *input_given, char *operator, int operands[], Game_fields *fields, Game_fields *outfields, int *AmountOfNotMines, int *AmountOfFlags, int *AmountOfMines);
void initialize_game(int *print_all_fields, int *input_given, char *operator, int operands[], Game_fields *fields, Game_fields *outfields, int *AmountOfNotMines, int *AmountOfFlags, int *AmountOfMines);

void draw_game(int print_all_fields, Game_fields *fields, Game_fields *outfields);
void free_game(Game_fields *fields, Game_fields *outfields);

void minesweeper(int with_initializing, int NumberOfRows, int NumberOfCols, int AmountOfNotMines, int AmountOfFlags, int AmountOfMines, Game_fields *fields, Game_fields *outfields);

/*
 * Geeft aan of de applicatie moet verdergaan.
 */
int should_continue = 1;

int main(int argc, char *argv[])
{
    //seed voor de rand functie.
    srand(time(NULL));

    // initiele variabelen voor minesweeper.
    int rows = NUMBEROFROWS;
    int cols = NUMBEROFCOLS;
    int amount_of_mines = NUMBEROFMINES;
    enum WITH_GAME_INITIALISATION is_with_initialisation = WITH_INITIALISATION;
    int amount_of_not_mines = NUMBEROFNOTMINES;
    int amount_of_flags = NUMBEROFFLAGS;
    Game_fields *fields;
    Game_fields *outfields;

    switch (argc)
    {
    // ./minesweeper
    case NO_FLAGS_GIVEN:
        fields = make_game_fields(rows, cols);
        outfields = make_game_fields(rows, cols);
        break;
    // ./minesweeper -w x -h y -m z
    case DIMENSIONS_FLAGS_GIVEN:
        process_dimensions_flags(argv, &rows, &cols, &amount_of_mines);

        // error als het gevraagde aantal mijnen groter is dan het aantal vakjes van het spel
        if (!is_amountofmines_valid(rows, cols, amount_of_mines, amount_of_not_mines))
        {
            printf("Error: Minesoverflow\nThe requested amount of mines is more than the available squares.\n");
            exit(1);
        }

        fields = make_game_fields(rows, cols);
        outfields = make_game_fields(rows, cols);
        break;

    // ./minesweeper -f file.txt
    case F_FLAG_GIVEN:
        if (is_f_flag(argv[1]))
        {
            is_with_initialisation = WITHOUT_INITIALISATION;

            char buffer[BUFFER_SIZE];
            FILE *fp;
            char *file_path = argv[2];
            // error handling
            if ((fp = fopen(argv[2], "r")) == NULL)
            {
                fprintf(stderr, "Error: can't open %s\n", file_path);
                exit(1);
            }

            // de spelwaarden inlezen naar de buffer.
            if (fgets(buffer, BUFFER_SIZE, fp) == NULL)
            {
                fprintf(stderr, "Error: File might not be correctly formatted.\n");
                exit(1);
            }

            decode_game_values(buffer, &rows, &cols, &amount_of_mines, &amount_of_not_mines, &amount_of_flags);
            // Het spel was al gedaan en de speler heeft gewonnen.
            if (is_gamewon(amount_of_not_mines, amount_of_mines))
            {
                printf("Error: Loaded game is already done.\n");
                exit(1);
            }
            fields = make_game_fields(rows, cols);
            outfields = make_game_fields(rows, cols);

            decode_fields(fp, outfields->fields, fields->fields, rows, cols);
            fclose(fp);
            break;
        }
        else
        {
            print_usage();
            exit(1);
        }
        break;

    default:
        print_usage();
        exit(1);
    }
    // het spel starten
    minesweeper(is_with_initialisation, rows, cols, amount_of_not_mines, amount_of_flags, amount_of_mines, fields, outfields);
    return 0;
}

// gaat na of een argumentflag -w is
int is_w_flag(char *arg)
{
    return strcmp1(arg, "-w");
}

// gaat na of een argumentflag -h is
int is_h_flag(char *arg)
{
    return strcmp1(arg, "-h");
}

// gaat na of een argumentflag -m is
int is_m_flag(char *arg)
{
    return strcmp1(arg, "-m");
}

// gaat na of een argumentflag f is
int is_f_flag(char *arg)
{
    return strcmp1(arg, "-f");
}

// zal een string omzetten naar een getal (wordt gebruikt bij het inlezen van argumenten)
int convert_arg_to_number(char *str)
{
    if (is_number(str))
    {
        int number = atoi(str);
        if(number > 0)
            return number;
        else {
            printf("Error: Expected a positive integer greater than zero.\n");
            exit(1);
        }
    }
    printf("Error: Wrong type of arguments. Expected: integers.\n");
    exit(1);
}

// zal de flags en hun waarden processen.
void process_dimensions_flags(char *argv[], int *rows, int *cols, int *aom)
{
    int w_flag_processed = 0, h_flag_processed = 0, m_flag_processed = 0;
    for (int i = 1; i < 7; i++)
    {
        if (is_w_flag(argv[i]))
        {
            *cols = convert_arg_to_number(argv[++i]);
            w_flag_processed = 1;
        }
        else if (is_h_flag(argv[i]))
        {
            *rows = convert_arg_to_number(argv[++i]);
            h_flag_processed = 1;
        }
        else if (is_m_flag(argv[i]))
        {
            *aom = convert_arg_to_number(argv[++i]);
            m_flag_processed = 1;
        }
        // als de flag geen van de vorige was, dan was het een foute flag.
        // dus wordt de usage van hoe het spel opstarten geprint.
        else
        {
            print_usage();
            exit(1);
        }
    }

    if (!(w_flag_processed && h_flag_processed && m_flag_processed))
    {
        print_usage();
        exit(1);
    }
}

// een procedure om de spelwaarden (#rijen, #kolommen, #aanwezige mijnen, #niet_mijnen die nog niet gerevealed zijn, #aanwezige flags)
void decode_game_values(char *str, int *NumberOfRows, int *NumberOfCols, int *AmountOfMines, int *AmountOfNotMines, int *AmountOfFlags)
{
    // {%d %d %d %d %d}
    if (sscanf(str,
               "{%d "
               "%d "
               "%d "
               "%d "
               "%d",
               NumberOfRows, NumberOfCols, AmountOfMines, AmountOfNotMines, AmountOfFlags) != 5)
    {
        printf("Wrong formatted file\n");
        exit(1);
    }
}

// een geformatteerd speelveldvakje inlezen.
void get_formatted_field(FILE *fp, int *field, int *outfield)
{
    if (fscanf(fp, "{ %d"
                   "%d} ",
               field,
               outfield) != 2)
    {
        fprintf(stderr, "Error: File might not be correctly formatted.\n");
        exit(1);
    }
}

// de speelvelden invullen met de velden uit het bestand.
void decode_fields(FILE *fp, int **outfields, int **fields, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            get_formatted_field(fp, &fields[i][j], &outfields[i][j]);
}

// informatie over het spel geformatteerd wegschrijven naar een bestand (file pointer)
void put_game_variables(FILE *stream, int NumberOfRows, int NumberOfCols, int AmountOfMines, int AmountOfNotMines, int AmountOfFlags)
{
    fprintf(stream, "{%d %d %d %d %d}\n", NumberOfRows, NumberOfCols, AmountOfMines, AmountOfNotMines, AmountOfFlags);
}

// een speelveldvakje formatteren en wegschrijven naar een bestand (file pointer)
void put_formatted_field(FILE *stream, int first, int second)
{
    fprintf(stream, "{%d %d} ", first, second);
}

// de speelveleden opslaan in een file (file pointer)
void save_fields(FILE *fp, int **fields, int **outfields, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            put_formatted_field(fp, fields[i][j], outfields[i][j]);
}

// het spel opslaan in field.txt
void save_game(int **fields, int **outfields, int NumberOfRows, int NumberOfCols, int AmountOfMines, int AmountOfNotMines, int AmountOfFlags)
{
    FILE *fp = fopen(DEFAULT_SAVED_GAME_PATH, "w");
    put_game_variables(fp, NumberOfRows, NumberOfCols, AmountOfMines, AmountOfNotMines, AmountOfFlags);
    save_fields(fp, fields, outfields, NumberOfRows, NumberOfCols);
    fclose(fp);
}

// printen welke flags en args het spelprogramma verwacht in de CLI
void print_usage(void)
{

    printf("Usage:\n");
    printf("./minesweeper to run the game with an 10x10 game field and 25 mines\n");
    printf("./minesweeper -w x -h y -m z to specify the size of the game field and the amount of mines.\n");
    printf("./minesweeper -f file.txt to load a saved game from a file\n");
}

void print_intro(void)
{
    printf("\nWelcome to Minesweeper!\n");
    printf("\nUsage:\n\n");
    printf(" Left mouse button to reveal a square.\n");
    printf(" Right mouse button to (de)flag a square.\n");
    printf(" P to print the revealed game (for testing purposes, obviously).\n");
    printf(" Q to exit the game.\n\n");
    printf(" Your game gets saved each round in fields.txt\n\n");
}
// het aantal resterende flags printen
void print_nrflags(int AmountOfFlags)
{
    printf("Remaining flags: %d\n", AmountOfFlags);
}

// checken of het gevraagde aantal mijnen al dan niet kleiner is dan het aantal vakjes.
int is_amountofmines_valid(int rows, int cols, int aom, int aonm)
{
    return (aom < rows * cols);
}

// maakt een game_field aan (dat bevat: #rijen, #kolommen, speelveld.)
Game_fields *make_game_fields(int height, int width)
{
    Game_fields *new = (Game_fields *)malloc(sizeof(Game_fields));
    if (new == NULL)
    {
        fprintf(stderr, "Error: not enough memory\n");
        exit(1);
    }
    new->height = height;
    new->width = width;

    int **fields = (int **)calloc(height, sizeof(int *));
    if (fields == NULL)
    {
        fprintf(stderr, "Error: not enough memory\n");
        free(new);
        exit(1);
    }
    for (int i = 0; i < height; i++)
    {
        *(fields + i) = (int *)calloc(width, sizeof(int));
        if (*(fields + i) == NULL)
        {
            fprintf(stderr, "Error: not enough memory\n");
            free(fields);
            free(new);
            exit(1);
        }
    }

    new->fields = fields;
    return new;
}

// zal de dynamisch gealloceerde memory freeen.
void free_game_fields(Game_fields *fields)
{
    for (int i = 0; i < fields->height; i++)
        free((fields->fields)[i]);
    free(fields->fields);
    free(fields);
}

// predikaat om na te gaan of een element een mijn is.
int is_mine(int el)
{
    return (el == MINE);
}

// het vakje is initieel (= voor dat we de vakjes die geen mijnen bevatten gaan invullen) geen mijn.
int is_init_not_mine(int el)
{
    return (el == INIT_NOT_MINE);
}
// predikaat om na te gaan of een element een vlag is.
int is_flag(int el)
{
    return (el == FLAG);
}

// predikaat om na te gaan of een element nog niet onthuld is.
// niet onthulde elementen worden voorgesteld door -1
int is_unrevealed(int el)
{
    return (el == UNREVEALED);
}

// predikaat om na te gaan of het vakje al reeds gereveald is, en geen vlag is (zie set_flag )
int is_revealed(int el)
{
    return (!(is_unrevealed(el) || is_flag(el)));
}

// predikaat om na te gaan of alle vakjes die geen mijn bevatten onthuld zijn.
int all_not_mines_revealed(int AmountOfNotMines)
{
    return (AmountOfNotMines == 0);
}

// predikaat om na te gaan of alle mijnen geflagged zijn.
int all_mines_flagged(int AmountOfMines)
{
    return (AmountOfMines == 0);
}

// een predikaat om na te gaan of de speler gewonnen heeft.
int is_gamewon(int AmountOfNotMines, int AmountOfMines)
{
    return (all_not_mines_revealed(AmountOfNotMines) || all_mines_flagged(AmountOfMines));
}

// het spel verlaten wanneer de speler verliest.
void gameover()
{
    printf("You stepped on a mine, whoops!\n");
    should_continue = 0;
}

// het spel verlaten wanneer de speler wint.
void gameWon()
{
    printf("Congrats! You are a true minesweeper. Consider a job at the army.\n");
    should_continue = 0;
}

void set_flag(Game_fields *fields, Game_fields *outfields, int row, int col, int *AmountOfFlags, int *AmountofMines)
{
    int **fields_matrix = fields->fields;
    int **outfields_matrix = outfields->fields;

    // als het element reeds gevreveald is, dan moet je hem niet kunnen flaggen.
    if (is_revealed(outfields_matrix[row][col]))
    {
        printf("This field is already revealed.\n");
    }
    // als er al een flag is, deflag het vakje.
    else if (is_flag(outfields_matrix[row][col]))
    {
        outfields_matrix[row][col] = UNREVEALED;
        (*AmountOfFlags)++;

        // als het geflagde vakje een mijn bevat, dan wordt het aantal mijnen opnieuw vermeerderd.
        if (is_mine(fields_matrix[row][col]))
            (*AmountofMines)++;

        print_nrflags(*AmountOfFlags);
    }
    else if (*AmountOfFlags == 0)
        // als de gebruiker al zijn vlaggen heeft gebruikt, kan hij geen vlaggen meer zetten.
        printf("You don't have any more flags\n");
    else
    {
        outfields_matrix[row][col] = FLAG;
        (*AmountOfFlags)--;
        // als het vakje inderdaad een mijn bevat, verminder het aantal nog aanwezige mijnen.
        if (is_mine(fields_matrix[row][col]))
        {
            (*AmountofMines)--;
        }

        print_nrflags(*AmountOfFlags);
    }
}

int is_out_of_boundaries(int operands[], int rows, int cols)
{
    return (operands[INPUT_ROW] > (rows - 1) || operands[INPUT_COL] > (cols - 1));
}

// de gebruikerinput toepassen op het spel
void process_command(int *print_all_fields, int *input_given, char *operator, int operands[], Game_fields *fields, Game_fields *outfields, int *AmountOfNotMines, int *AmountofFlags, int *AmountOfMines)
{
    // input_given is een flag die aangeeft dat de speler input heeft gegeven.
    // alleen dan wordt de input verwerkt.
    if (*input_given)
        switch (*operator)
        {

        case 'R':
            if (is_out_of_boundaries(operands, fields->height, fields->width))
            {
                printf("Error: The given coordinates are too good to be true.\n");
                break;
            }
            // als het vakje een mijn bevatte is het spel klaar en heeft de speler verloren.
            int **fields_matrix = fields->fields;
            if (is_mine(fields_matrix[operands[INPUT_ROW]][operands[INPUT_COL]]))
            {
                gameover();
            }
            else
            {
                reveal(fields, outfields, operands[INPUT_ROW], operands[INPUT_COL], AmountOfNotMines, AmountofFlags, AmountOfMines);
                print_nrflags(*AmountofFlags);
            }

            break;

        case 'F':
            if (is_out_of_boundaries(operands, fields->height, fields->width))
            {
                printf("Error: The given coordinates are too good to be true.\n");
                break;
            }
            set_flag(fields, outfields, operands[INPUT_ROW], operands[INPUT_COL], AmountofFlags, AmountOfMines);
            break;

        case 'P':
            *print_all_fields = !(*print_all_fields);
            break;

        case 'Q':
            should_continue = 0;
            break;
        }
    save_game(fields->fields, outfields->fields, fields->height, fields->width, *AmountOfMines, *AmountOfNotMines, *AmountofFlags);

    *input_given = 0;
}

// de eerste input verwerken (die moet apart verwerkt worden om te vermijden dat de speler meteen verliest als hij direct een mijn onthult)
void process_first_command(int *first_input, int *print_all_fields, int *input_given, char *operator, int operands[], Game_fields *fields, Game_fields *outfields, int *AmountOfNotMines, int *AmountofFlags, int *AmountOfMines)
{
    if (*input_given)
    {
        switch (*operator)
        {
        case 'R':
            *first_input = 0;
            *input_given = 0;
            if (is_out_of_boundaries(operands, fields->height, fields->width))
            {
                printf("Error: The given coordinates are too good to be true.\n");
                break;
            }
            int **fields_matrix = fields->fields;
            if (is_mine(fields_matrix[operands[INPUT_ROW]][operands[INPUT_COL]]))
                swap_mine_with_not_mine(fields, operands[INPUT_ROW], operands[INPUT_COL]);
            print_nrflags(*AmountofFlags);
            break;

        // Q wordt hier apart afgehandeld om ervoor te zorgen dat het spel niet leeg wordt opgeslagen.
        case 'Q':
            *first_input = 0;
            *input_given = 0;
            should_continue = 0;
            break;
        case 'F':
        case 'P':
            *first_input = 0;
            process_command(print_all_fields, input_given, operator, operands, fields, outfields, AmountOfNotMines, AmountofFlags, AmountOfMines);
            break;
        }
    }
}

// swap een vakje dat een mijn bevat met een ander vakje dat geen bevat. Dit wordt gebruikt bij eerste commando van de gebruiker
// indien de gebruiker R r c ingeeft.
void swap_mine_with_not_mine(Game_fields *fields, int row, int col)
{
    int rows = fields->height;
    int cols = fields->width;
    int **fields_matrix = fields->fields;
    int found = 0;
    // we beginnen op een willekeurige rij van het spelveld.
    for (int i = myrandom(0, rows - 1); i < rows; i++)
    {
        if (found)
            break;
        else
            for (int j = 0; j < cols; j++)
            {
                if (is_init_not_mine(fields_matrix[i][j]))
                {
                    fields_matrix[i][j] = MINE;
                    fields_matrix[row][col] = INIT_NOT_MINE;
                    found = 1;
                    break;
                }
            }
    }
}

// als er geen naburige mijnen zijn, worden de naburige vakjes onthuld en wordt hetzelfde toegepast op de naburige vakjes.
void reveal_surroundings(Game_fields *fields, Game_fields *outfields, int row, int col, int *AmountOfNotMines, int *AmountOfFlags, int *AmountOfMines)
{
    int rows = fields->height;
    int cols = fields->width;
    int **outfields_matrix = outfields->fields;
    // voor alle rijen vanaf de rij boven het element tot de rij onder het element.
    for (int row_offset = -1; row_offset < 2; row_offset++)
    {
        // voor alle kolommen vanaf de kolom links van het element tot de kolom rechts van het element.
        for (int col_offset = -1; col_offset < 2; col_offset++)
        {
            // de randgevallen: wanneer het vakje aan de rand van het spel zit
            if ((row_offset == col_offset) && (row_offset == 0))
                continue;
            else if ((row_offset + row) < 0 || (row_offset + row >= rows))
                continue;
            else if ((col_offset + col) < 0 || (col_offset + col >= cols))
                continue;

            // als er een vlag is in het vakje, dan moet je het eerst deflaggen.
            if (is_flag(outfields_matrix[row_offset + row][col_offset + col]))
                set_flag(fields, outfields, row_offset + row, col_offset + col, AmountOfFlags, AmountOfMines);
            // als het vakje al onthuld is, dan moet je het niet opnieuw onthullen.
            if (is_unrevealed(outfields_matrix[row_offset + row][col_offset + col]))
                reveal(fields, outfields, row_offset + row, col_offset + col, AmountOfNotMines, AmountOfFlags, AmountOfMines);
        }
    }
}

// de inhoud van het vakje tonen.
void reveal(Game_fields *fields, Game_fields *outfields, int row, int col, int *AmountOfNotMines, int *AmountOfFlags, int *AmountOfMines)
{
    int **fields_matrix = fields->fields;
    int **outfields_matrix = outfields->fields;

    // als er een flag op het vakje stond, dan wordt de flag eerst verwijderd.
    if (is_flag(outfields_matrix[row][col]))
    {
        set_flag(fields, outfields, row, col, AmountOfFlags, AmountOfMines);
    }

    if (is_unrevealed(outfields_matrix[row][col]))
    {
        outfields_matrix[row][col] = fields_matrix[row][col];
        (*AmountOfNotMines)--;
    }

    // onthul naburige vakjes als het vakje geen naburige mijnen heeft.
    if (fields_matrix[row][col] == 0)
    {
        reveal_surroundings(fields, outfields, row, col, AmountOfNotMines, AmountOfFlags, AmountOfMines);
    }
}

// tellen hoeveel mijnen er zijn rond elk vakje dat geen mijn bevat.
int how_many_surrounding_mines(Game_fields *fields, int row, int col)
{
    int rows = fields->height;
    int cols = fields->width;
    int **fields_matrix = fields->fields;
    int res = 0;
    for (int row_offset = -1; row_offset < 2; row_offset++)
    {
        for (int col_offset = -1; col_offset < 2; col_offset++)
        {
            // je hebt het element zelf vast => skip
            if ((row_offset == col_offset) && (row_offset == 0))
                continue;
            // randgeval voor de rijen
            else if ((row_offset + row) < 0 || (row_offset + row >= rows))
                continue;
            // randgeval voor de kolommen
            else if ((col_offset + col) < 0 || (col_offset + col >= cols))
                continue;

            if (is_mine(fields_matrix[row_offset + row][col_offset + col]))
                res++;
        }
    }
    return res;
}

// het spelveld vullen met mijnen.
void fill_fields(Game_fields *fields, int AmountOfMines)
{
    int rows = fields->height;
    int cols = fields->width;
    int **fields_matrix = fields->fields;
    int *curr;
    while (AmountOfMines)
    {
        // een willekeurig vakje nemen.
        curr = &fields_matrix[myrandom(0, rows - 1)][myrandom(0, cols - 1)];
        // als het vakje al een mijn bevat moet je verder zoeken naar een ander vakje.
        if (is_mine(*curr))
            continue;
        *curr = MINE;
        AmountOfMines--;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            // de rest van de vakjes vullen met niet-mijnen
            if (!is_mine(fields_matrix[i][j]))
                fields_matrix[i][j] = INIT_NOT_MINE;
        }
    }
}

// alle vakjes die geen mijnen bevatten invullen met het aantal naburige mijnen.
void initialize_fields(Game_fields *fields)
{
    int rows = fields->height;
    int cols = fields->width;
    int **fields_matrix = fields->fields;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (is_init_not_mine(fields_matrix[i][j]))
                fields_matrix[i][j] = how_many_surrounding_mines(fields, i, j);
        }
    }
}

// het spelveld dat geprint gaat worden initializeren.
void fill_outfields(Game_fields *outfields)
{
    int rows = outfields->height;
    int cols = outfields->width;
    int **outfields_matrix = outfields->fields;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            outfields_matrix[i][j] = UNREVEALED;
        }
    }
}

// de eerste spelbeurt
void first_round(int *print_all_fields, int *input_given, char *operator, int operands[], Game_fields *fields, Game_fields *outfields, int *AmountOfNotMines, int *AmountOfFlags, int *AmountOfMines)
{
    int first_input = 1;
    while (first_input)
    {
        draw_window(outfields);

        read_input(operator, operands, input_given);

        process_first_command(&first_input, print_all_fields, input_given, operator, operands, fields, outfields, AmountOfNotMines, AmountOfFlags, AmountOfMines);
    }
    initialize_fields(fields);
    if (*operator== 'R')
    {
        reveal(fields, outfields, operands[INPUT_ROW], operands[INPUT_COL], AmountOfNotMines, AmountOfFlags, AmountOfMines);
    }
}

// het spel initialiseren.
void initialize_game(int *print_all_fields, int *input_given, char *operator, int operands[], Game_fields *fields, Game_fields *outfields, int *AmountOfNotMines, int *AmountOfFlags, int *AmountOfMines)
{
    fill_outfields(outfields);
    fill_fields(fields, *AmountOfMines);
    *AmountOfNotMines = (fields->height * fields->width) - *AmountOfMines;
    *AmountOfFlags = *AmountOfMines;
    print_intro();
    first_round(print_all_fields, input_given, operator, operands, fields, outfields, AmountOfNotMines, AmountOfFlags, AmountOfMines);
}

// het spel tekenen.
void draw_game(int print_all_fields, Game_fields *fields, Game_fields *outfields)
{
    if (print_all_fields)
        draw_window(fields);
    else
        draw_window(outfields);
}

void free_game(Game_fields *fields, Game_fields *outfields)
{

    free_game_fields(fields);
    free_game_fields(outfields);
}

/* with_initializing is een variable die aangeeft of het spel vanaf het begin geinitialiseerd moet worden
   Als het spel geinitialiseerd moet worden, d.w.z. als het een nieuw spel is, dan is deze variabele 1.
   Als het spel niet geinitialiseerd moet worden, d.w.z. Een bestaand spel dat ingeladen wordt uit een bestand, dan is deze variabele 0.
*/
void minesweeper(int with_initializing, int NumberOfRows, int NumberOfCols, int AmountOfNotMines, int AmountOfFlags, int AmountOfMines, Game_fields *fields, Game_fields *outfields)
{
    char operator;
    int operands[2];
    int input_given = 0;
    int print_all_fields = 0;

    initialize_gui(NumberOfRows, NumberOfCols);
    if (with_initializing)
        initialize_game(&print_all_fields, &input_given, &operator, operands, fields, outfields, &AmountOfNotMines, &AmountOfFlags, &AmountOfMines);

    while (should_continue)
    {
        draw_game(print_all_fields, fields, outfields);
        read_input(&operator, operands, &input_given);
        process_command(&print_all_fields, &input_given, &operator, operands, fields, outfields, &AmountOfNotMines, &AmountOfFlags, &AmountOfMines);
        if (is_gamewon(AmountOfNotMines, AmountOfMines))
        {
            gameWon();
        }
    }

    // Dealloceer al het geheugen dat werd aangemaakt door SDL zelf.
    free_gui();
    // Dealloceer het geheugen van het spel
    free_game(fields, outfields);
}
