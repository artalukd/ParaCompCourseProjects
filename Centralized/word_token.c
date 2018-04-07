#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include "_TOKENIZER.h"

int offset = 0;
bool eof = false;

FILE *getStream(FILE *fp, buffer b, int k){	
    /*******************************************************************
    * DESCRIPTION : Read k Bytes from file stream and store it in buffer b.
    *               Used as subroutine in the getWord function to update the buffer when needed.
    * INPUT :     
    *           [1] fp : File pointer.
    *           [2] b : buffer that stores the data.
    *           [3] k : size of buffer
    * OUTPUT : 
    *           [1] Returns a the file pointer.
    */
    int count = -1;
	if (!eof)
        count = fread(b, 1, k, fp);
    //hack
    if(count < k) eof = true ;
    b[count] = '\0';
    return fp;
}

char* getWord(FILE* fp, buffer b, int k){
    /*******************************************************************
    * DESCRIPTION : Returns a word from the file.
    * INPUT :     
    *           [1] fp : File pointer.
    *           [2] b : buffer that stores the data.
    *           [3] k : size of buffer
    * OUTPUT : 
    *           [1] Returns a word.
    */
    buffer temp = (buffer) malloc(140 * sizeof(char));
    int i = 0;
    memset(temp, 0, 140);
    while(1){
        if (offset == k || strlen(b) == 0 || b[offset] == '\0'){
			if( eof ){
				return temp;
			}
			memset(b, 0, k);
            fp = getStream(fp, b, k);
            offset = 0;
		}
        if(('a' <= b[offset] && b[offset] <= 'z') || ('A' <= b[offset] && b[offset] <= 'Z')){
            temp[i++] = b[offset++];
            if(temp[i-1] >= 'A' && temp[i-1] <= 'Z'){
                temp[i-1] += 'a' - 'A';
            }
        }else{
            offset++;
            if(temp[0] == '\0') continue;
            else return temp;
        }
    }
}

char** list_dir(char* folder_name, int* file_count){
    /*******************************************************************
    * DESCRIPTION : List all the files present in the folder_name.
    * INPUT :     
    *           [1] folder_name : Path of the folder.
    *           [2] file_count : Pointer to file_count.
    * OUTPUT : 
    *           [1] Returns the list of files.
    */
    DIR *dir;
    struct dirent *ent;
    *file_count = 0;
    if ((dir = opendir (folder_name)) != NULL) {
        /* Count total files within directory */
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) { /* If the entry is a regular file */
                (*file_count)++;
            }
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        return NULL;
    }

    char** file_list = (char**) malloc((*file_count) * sizeof(char*));
    int i = 0;
    
    if ((dir = opendir (folder_name)) != NULL) {
        /* print all the files and directories within directory */
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type == DT_REG){
                file_list[i] =  (char*) malloc(strlen(ent->d_name)*sizeof(char));
                strcpy(file_list[i], ent->d_name);
                // printf ("%s\n", file_list[i]);
                i++;
            }
        }
        closedir (dir);
    } else {
        /* could not open directory */
        perror ("");
        return NULL;
    }
    return file_list;
}


char** append_paths(char* folder_name, char** file_list, int file_count){
    /*******************************************************************
    * DESCRIPTION : Append the path of folder_name to each file of file_list.
    * INPUT :     
    *           [1] folder_name : Path of the folder.
    *           [2] file_list : List of files.
    *           [3] file_count : Pointer to file_count.
    * OUTPUT : 
    *           [1] Returns the new list of files.
    */
    int i;
    char** new_file_list = (char**) malloc(file_count * sizeof(char*));

    for(i = 0; i < file_count; i ++){
        new_file_list[i] = (char*) malloc( (strlen(folder_name) + strlen(file_list[i]) + 2) * sizeof(char) );
        strcpy(new_file_list[i], folder_name);
        strcat(new_file_list[i], "/");
        strcat(new_file_list[i], file_list[i]);
    }
    return new_file_list;
}


// int main(){
//     buffer b = (buffer) malloc(4096 * sizeof(char));
//     int k = 4096;
//     memset(b, 0, k);
//     int i = 0;
//     char* temp;
//     FILE* fp = fopen("test", "r");

//     // while(1){
//     //     printf("%s\n", getWord(fp, b, k));
//     //     if( offset >= strlen(b) && eof ) break;        
//     // }
    
//     int file_count;
//     // char** filelist = list_dir(".", &file_count);

//     char** old_file_list = list_dir(".", &file_count);
//     char** file_list = append_paths(".", old_file_list, file_count);

//     // printf("%d", file_count);
//     for(i=0; i < file_count; i++){
//         printf("%s\n", file_list[i]);
//     }

// }