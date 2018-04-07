#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "_TOKENIZER.h"

char** read_arr(FILE* fptr, int* size){
    /*******************************************************************
    * DESCRIPTION : Reading the stopword file
    * INPUT :     
    *           [1] fptr : File pointer.
    *           [2] size : Pointer to the size array being returned.
    * OUTPUT : 
    *           [1] Returns an array of strings
    */

    *size = 32;
    char* temp = (char*) malloc(25 * sizeof(char));
    char** list_of_words = (char**) malloc((*size) * sizeof(char*));
    int i = 0, garbage, len;
    for(i = 0; ; i++){
        memset(temp, '\0', 25 * sizeof(char));
        garbage = fscanf(fptr, "%s", temp);
        len = strlen(temp);

        if(garbage == 0 || feof(fptr) || garbage == EOF){
            break;
        }

        list_of_words[i] = (char*) malloc((len + 1) * sizeof(char));
        memcpy(list_of_words[i], temp, (len + 1) * sizeof(char));
        // printf("%s\n", list_of_words[i]);

        if(i >= (*size) - 2){
            list_of_words = (char**) realloc(list_of_words, (*size) * 2 * sizeof(char**));
            (*size) = (*size) * 2;
        }
    }

    *size = i ;
    return list_of_words;
}

bool search(char** arr, char* key, int lo, int hi){
    /*******************************************************************
    * DESCRIPTION : Checking for the existance of the key in the array
    * INPUT :     
    *           [1] fp : Pointer to the string that stores the serialized LinkedList.
    *           [2] fp_count : Pointer to the size of fp.
    * OUTPUT : 
    *           [1] Returns a boolean.
    */
    int what;
    while(lo <= hi){
        int mid = (lo + hi)/2;
        if((what = strcmp(arr[mid], key)) == 0){
            return true;
        }else if(what > 0){
            hi = mid - 1;
        }else{
            lo = mid + 1;
        }
    }
    return false;
}

// int main(){

//     FILE* fptr = fopen("stopwords", "r");
//     if(fptr != NULL){
//         printf("File opened correctly\n");
//     }

//     int size;
//     char** list_of_words = read_arr(fptr, &size);
    
//     fclose(fptr);

//     printf("%d\n", search(list_of_words, "zerot", 0, 570));

//     return 0;
// }