#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "helpers.h"


// Dit zijn de hulpfuncties die geen deel uitmaken van de spellogica.


// een functie om een willekeurig getal van een gegeven interval terug te geven.
int myrandom(int min, int max)
{
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

// een predikaat die nagaat of een string een getal is.
int is_number(char *str)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

// een eigen versie van strcmp om te vermijden dat ik overal == 0 moet zetten.
int strcmp1(char *str1, char *str2)
{
    return strcmp(str1, str2) == 0;
}
