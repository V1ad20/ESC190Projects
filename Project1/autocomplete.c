#include "autocomplete.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cmp_terms_by_name(const void *term1, const void *term2)
{
    return strcmp(((term *)term1)->term, ((term *)term2)->term);
}

int cmp_terms_by_weight(const void *term1, const void *term2)
{
    return ((term *)term2)->weight - ((term *)term1)->weight;
}

void read_in_terms(struct term **terms, int *pnterms, char *filename)
{
    FILE *fp;
    int len = 200;
    char str[len];
    int j;
    for (j = 0; j < len; j++)
    {
        str[j] = ' ';
    }
    str[j - 1] = '\0';

    /* opening file for reading */
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error opening file");
        return;
    }
    if (fgets(str, len, fp))
    {
        *pnterms = atof(str);
        *terms = (term *)malloc(sizeof(term) * (*pnterms));
    }

    for (j = 0; j < len; j++)
    {
        str[j] = ' ';
    }
    str[j - 1] = '\0';

    for (int term_num = 0; term_num < *pnterms; term_num++)
    {
        int i = 0;
        int term_start = 0;
        int state = 0;
        char str1[len];
        strcpy(str1, str);
        fgets(str1, len, fp);

        int weight = 0;
        char obj_term[len];
        strcpy(obj_term, str);

        while (str1[i] != '\n' && str1[i] != '\0')
        {
            if (state == 0 && (str1[i] < '0' || str1[i] > '9'))
            {
                state = 1;
            }
            else if (state == 1 && (str1[i] != ' ' && str1[i] != '\t'))
            {
                state = 2;
                term_start = i;
            }

            if (state == 0)
            {
                weight = weight * 10 + (str1[i] - '0');
            }
            else if (state == 2)
            {
                obj_term[i - term_start] = str1[i];
            }
            i++;
        }
        obj_term[i - term_start] = '\0';
        struct term obj;

        // bad example
        /*
        obj = {obi_term,weight} //this kind of stuff seems to only work when the string
        is in the form of a literal
        */
        strcpy(obj.term, obj_term); // u need
        obj.weight = (double)weight;

        (*terms)[term_num] = obj;
    }
    fclose(fp);

    qsort(*terms, *pnterms, sizeof(term), cmp_terms_by_name);
}

int cmp_first_letters(char *pattern, term term1)
{
    if (strlen(pattern) > strlen(term1.term))
        return strcmp(pattern, term1.term); // might have to change

    int len = strlen(pattern);
    char str[len + 1];
    for (int i = len - 1; i >= 0; i--)
    {
        str[i] = term1.term[i];
    }
    str[len] = '\0';
    return strcmp(pattern, str);
}

int binary_search_left(term *arr, char *pattern, int low, int high, int mid)
{
    if (mid == low)
        return low;

    int cmp_res = cmp_first_letters(pattern, arr[mid]);

    if (cmp_res == 0)
    {
        if (cmp_first_letters(pattern, arr[mid - 1]) != 0)
            return mid;
        return binary_search_left(arr, pattern, low, mid, low + (mid - low) / 2);
    }
    if (cmp_res < 0)
        return binary_search_left(arr, pattern, low, mid, low + (mid - low) / 2);
    return binary_search_left(arr, pattern, mid, high, mid + (high - mid) / 2);
}

int lowest_match(term *terms, int nterms, char *substr)
{
    return binary_search_left(terms, substr, 0, nterms, nterms / 2);
}

int binary_search_right(term *arr, char *pattern, int low, int high, int mid)
{
    if (mid == low)
        return low;

    int cmp_res = cmp_first_letters(pattern, arr[mid]);
    if (cmp_res == 0)
    {
        if (cmp_first_letters(pattern, arr[mid + 1]) != 0)
            return mid;
        return binary_search_right(arr, pattern, mid, high, mid + (high - mid) / 2);
    }
    if (cmp_res > 0)
        return binary_search_right(arr, pattern, mid, high, mid + (high - mid) / 2);
    return binary_search_right(arr, pattern, low, mid, low + (mid - low) / 2);
}

int highest_match(term *terms, int nterms, char *substr)
{
    return binary_search_right(terms, substr, 0, nterms, nterms / 2);
}

void autocomplete(term **answer, int *n_answer, term *terms, int nterms, char *substr)
{
    int low = lowest_match(terms, nterms, substr);
    int high = highest_match(terms, nterms, substr);
    int res = high - low + 1;

    if (res > 0)
    {
        *n_answer = res;
    }
    else
    {
        if (cmp_first_letters(substr, terms[low]) == 0)
        {
            *n_answer = 1;
        }
        else
        {
            *n_answer = 0;
        }
    }

    if (*n_answer > 0)
    {
        *answer = (term *)malloc(sizeof(term) * (*n_answer));
        for (int i = 0; i < *n_answer; i++)
        {
            (*answer)[i] = terms[low + i];
        }
        qsort(*answer, *n_answer, sizeof(term), cmp_terms_by_weight);
    }
    else
    {
        *answer = (term *)malloc(0);
    }
}

// int main()
// {
//     term *terms;
//     int nterms;
//     read_in_terms(&terms, &nterms, "cities.txt");

//     term *matches;
//     int n_matches;
//     autocomplete(&matches, &n_matches, terms, nterms, "Shang");

//     for (int i = 0; i < n_matches; i++)
//     {
//         printf("%s: %lf\n", matches[i].term, matches[i].weight);
//     }
// }