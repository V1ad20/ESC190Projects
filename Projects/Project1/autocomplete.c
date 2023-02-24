#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "autocomplete.h"

int is_letter(char c){
    return !((c >= '0' && c <= '9') || (c == ' ')|| (c == '\t'));
}

char * get_word_str(char * str){    // returns the address in memory where the string starts
    while(!is_letter(*str)){
        str++;
    }
    return str;
}

int compare(const void *a, const void *b){
    // sort by term. if tie, sort by weight
    int term_cmp = strcmp(((term *)a)->term, ((term *)b)->term);
    int difference = ((term *)b)->weight - ((term *)a)->weight;

    if(term_cmp != 0){
        return term_cmp;
    } else{
       return difference;
    }
}

void read_in_terms(struct term **terms, int *pnterms, char *filename){
    FILE *fp = fopen("cities.txt", "r");
    char line[200];

    if(fp == NULL){ // checking if fp returns null
        printf("Error opening file %s\n", filename);
        return;
    }

    fgets(line, sizeof(line), fp);
    *(pnterms) = atof(line);    // setting the number of terms
    *terms = (term *)malloc((*(pnterms)) * sizeof(term)); // malloc for terms

    int i = 0; // counter to set values in block
    while(fgets(line, sizeof(line), fp) != NULL){
        strncpy((*terms)[i].term , get_word_str(line), 200);
        (*terms)[i].term[strlen((*terms)[i].term) - 1] = '\0';

        (*terms)[i].weight = atof(line);
        i++;
    }
    qsort(*terms, *pnterms, sizeof(term), compare); // sorting in lexicographic order

    fclose(fp);

    // // making sorted file
    // FILE *fp2 = fopen("sortedcities.txt", "w");

    // if(fp2 == NULL){ // checking if fp returns null
    //     printf("Error opening file\n");
    //     return;
    // }

    // for(int i = 0; i < 93827; i++){
    //     fprintf(fp2, "%s  %f\n",(*terms)[i].term, (*terms)[i].weight);
    // }

    // fclose(fp2);
}

int lowest_match(struct term *terms, int nterms, char *substr){
    int left = 0;
    int right = nterms - 1;
    int mid;

    while(left <= right){
        mid = (left + right) / 2;

        if(strcmp(substr, terms[mid].term) > 0){
            left = mid + 1;
        } else{
            right = mid - 1;
        }
    }
    // printf("%d, %s\n", left, terms[left].term);
    return left;
}

int highest_match(struct term *terms, int nterms, char *substr){
    int left = 0;
    int right = nterms - 1;
    int mid;
    int substrlen = strlen(substr);

    char prefix[substrlen + 1];

    while(left <= right){
        mid = (left + right) / 2;
        // memset(prefix, 0, 4);
        strncpy(prefix, terms[mid].term, substrlen);
        prefix[substrlen] = '\0';

        if(strcmp(prefix, substr) > 0){
            right = mid - 1;
        } else{
            left = mid + 1;
        }
    }

    // printf("%d, %s\n", right, terms[right].term);
    return right;
}

int compare2(const void *a, const void *b){
    int difference = ((term *)b)->weight - ((term *)a)->weight;
    int term_cmp = strcmp(((term *)a)->term, ((term *)b)->term);

    if(difference != 0){
        return difference;
    } else{
        return term_cmp;
    }
}

void autocomplete(struct term **answer, int *n_answer, struct term *terms, int nterms, char *substr){
    int lowest = lowest_match(terms, nterms, substr);
    *n_answer = highest_match(terms, nterms, substr) - lowest + 1;

    *answer = (term *)malloc((*n_answer) * sizeof(term)); // malloc for answer

    for(int i = 0; i < *n_answer; i++){
        strncpy((*answer)[i].term, terms[lowest + i].term, 200);
        (*answer)[i].weight = terms[lowest + i].weight;
    }
    qsort(*answer, *n_answer, sizeof(term), compare2);
}

int main(void)
{
    struct term *terms;
    int nterms;
    read_in_terms(&terms, &nterms, "cities.txt");
    lowest_match(terms, nterms, "Tor");
    highest_match(terms, nterms, "Tor");
    
    struct term *answer;
    int n_answer;
    autocomplete(&answer, &n_answer, terms, nterms, "Tor");

    // for(int i = 0; i < n_answer; i++){
    //     printf("%s has a weight of: %f\n",answer[i].term, answer[i].weight);
    // }

    //free allocated blocks here -- not required for the project, but good practice
    free(terms);
    free(answer);
    return 0;
}
