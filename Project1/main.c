#include "autocomplete.h"
#include "autocomplete.c"
#include <stdlib.h>

int main(void)
{
    struct term *terms;
    int nterms;
    read_in_terms(&terms, &nterms, "cities.txt");
    lowest_match(terms, nterms, "Torasdfkljhgfdsfghjkl");
    highest_match(terms, nterms, "Torasdfghjkljhgfdsfghjkl");

    struct term *answer;
    int n_answer;
    autocomplete(&answer, &n_answer, terms, nterms, "Toronto, Ontario, Canada");

    for (int i = 0; i < n_answer; i++)
    {
        printf("%s: %lf\n", answer[i].term, answer[i].weight);
    }

    free(terms);
    free(answer);
    // free allocated blocks here -- not required for the project, but good practice
    return 0;
}