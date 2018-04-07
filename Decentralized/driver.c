#include "_TOKENIZER.h"
#include "_TRIE.h"
#include "_LINKED_LIST.h"
#include "_BFILTER.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <mpi.h>

void indexing();

void decompose_domain(int domain_size, int world_rank, int world_size, int* subdomain_start, int* subdomain_end) {
    /*******************************************************************
    * DESCRIPTION : Assign the subdomain to given process
    * INPUT:     
    *           [1] domain_size: Size of the over all domain (In this case value of filecount)
    *           [2] Other arguments are self explainatory.
    * 
    */

    *subdomain_start = (int) (domain_size / world_size )  * world_rank;    
    
    if ( world_rank != world_size - 1)
        *subdomain_end = (int) (domain_size / world_size )  * (world_rank+1);
    else 
        *subdomain_end = domain_size;
    
}


int main(int argc, char* argv[]){
    /*******************************************************************
    * DESCRIPTION : Main function 
    * NOTE : 
    *           [1] clus_root contains the final Global Trie.
    * 
    */
    MPI_Init(NULL, NULL);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    TrieNode** clus_root;
    char* folder_name;
    if(argc > 1 )
        folder_name = argv[1];
    else{ 
        printf("Kindly provide path to the dataset as argument");
        return -1;
    }

    char* query_folder = "/home/mpiuser/query";

    indexing(clus_root, folder_name, query_folder,  world_rank, world_size);

    MPI_Finalize();
    return 0;
}


void indexing(TrieNode** clus_root, char* folder_name,  char* query_folder , int world_rank, int world_size){

    // Getting path of the files 
    int file_count;
    char** old_file_list = list_dir(folder_name, &file_count);
    char** file_list = append_paths(folder_name, old_file_list, file_count);

    // Getting queries
    int query_count;
    char** query_list = list_dir(query_folder, &query_count);
    query_list = append_paths(query_folder, query_list, query_count);

    int subdomain_start, subdomain_end;
    decompose_domain(file_count, world_rank, world_size, &subdomain_start, &subdomain_end);

    int i;
    
    // Initilize varibles used in the document reading    
    buffer b = (buffer) malloc(4096 * sizeof(char));
    buffer word_buffer;
    int k = 4096;
    memset(b, 0, k);

    // Initialization of Global Trie
    *clus_root = get_clus_Node();

    // Reading Stopwords and storing them into the list_of_stopwords
    FILE* fptr = fopen("stopwords", "r");
    if(fptr == NULL){
        printf("StopWord File can not be opened.\n");
        return -1;
    }
    int num_stop;
    char** list_of_words = read_arr(fptr, &num_stop);
    fclose(fptr);

    // Processing files from the range of [subdomain_start, subdomain_end) of the file_list
    for(i = subdomain_start; i < subdomain_end ; i ++){
        FILE* fp = fopen(file_list[i], "r");
        if(!fp){
            perror("");
            printf("File:: %s cannot be opened, please check it", file_list[i]);
            exit(1);
        }

        // Initilization of doc level trie
        TrieNode* doc_root = get_doc_Node();
        
        // Initialization realted to reading doc        
        offset = 0;
        memset(b, 0, k);
        eof = false;

        // Reading the doc and adding all the words into the doc level trie. 
        while(1){
            word_buffer = getWord(fp, b, k);
            if(search(list_of_words, word_buffer, 0, num_stop - 1) == 1){
                free(word_buffer);   
                continue;  
            }
            doc_root = doc_insert(doc_root, word_buffer);
            free(word_buffer);
            if( offset >= strlen(b) && eof ) break;                       
        }

        fclose(fp);
        *clus_root = insert_doc_in_clus(*clus_root, doc_root, old_file_list[i]);
        doc_free(doc_root);
    }


    int max_deser = 40 * (file_count - ((file_count / world_size )  * (world_size - 1))) + 60;
    char* deser_data = (char*) calloc(max_deser, sizeof(char));

    // Initialization related to Round Robin master slave model
    int master = 0;
    int finish = 0;
    List** all_lists = (List**) malloc(world_size * sizeof(List*));
    List* Final_list;
    int finish_words = 0, remaining_cycle_work = 0;

    i = 0;
    
    // Reading queries
    int num_words;
    FILE* fpt = fopen(query_list[world_rank], "r");
    char** all_words = read_arr(fpt, &num_words);
    fclose(fpt);


    // Initialization corresponding to slave
    char* query = (char*) malloc(140 * sizeof(char));
    int size2, iter2;
    char* str2;
    List* list;

    int pending_master[world_size];
    memset(pending_master, 1, world_size * sizeof(int));

    while(!finish){
        MPI_Barrier(MPI_COMM_WORLD);
        
        // MASTER NODE
        if(world_rank == master){
            remaining_cycle_work = 10;
            while( remaining_cycle_work-- ){
                
                word_buffer = convert_to_lower(all_words[finish_words++]);
                Final_list = create_list();

                // Sending the query to each of the nodes
                for(i = 0; i < world_size; i++){
                    if(i != world_rank){
                        MPI_Send(word_buffer, strlen(word_buffer) + 1, MPI_CHAR, i, 1, MPI_COMM_WORLD);
                    }
                }

                // Searching the query in it's own index.
                List* lst = clus_search(*clus_root, word_buffer);
                if(lst){
                    Final_list = mergeLists(Final_list, lst);
                }
                
                // Receiving input from each of the nodes
                for(i = 0; i < world_size; i++){
                    if(i != world_rank){
                        MPI_Recv(deser_data, max_deser, MPI_CHAR, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        iter2 = 0;
                        all_lists[i] = deserialize_list(&deser_data, &iter2);
                        memset(deser_data, 0,max_deser );
                    }
                }
                
                // Merging all the lists.
                for(i=0; i< world_size; i++){
                    if(i != world_rank){
                        Final_list = mergeLists(Final_list, all_lists[i]);
                    }
                }
                printf("Process %d ::  :: %s\n",world_rank, word_buffer);
                print_list(Final_list);
                
                // Finishing work condintion of master
                if(finish_words >= num_words){
                    strcpy(word_buffer, "$");
                    for(i = 0; i < world_size; i++ ){
                        if(i != world_rank){
                            MPI_Send(word_buffer, strlen(word_buffer) + 1, MPI_CHAR, i, 1, MPI_COMM_WORLD);
                        }
                    }

                    pending_master[master] = 0;
                    finish = 1;
                    i = 0;
                    master = (master + 1) % world_size;
                    while( i < world_size){
                        if(pending_master[(master + i) % world_size]){
                            master = master + i;
                            finish = 0;
                            break;
                        }
                        i++;
                    }

                    free(word_buffer);
                    break;
                }
                
                // Breaking condition of the loop and transffering the master access to the next node 
                if( ! remaining_cycle_work ) {
                    strcpy(word_buffer, "!");
                    for(i = 0; i < world_size; i++ ){
                        if(i != world_rank){
                            MPI_Send(word_buffer, strlen(word_buffer) + 1, MPI_CHAR, i, 1, MPI_COMM_WORLD);
                        }
                    }
                    master = (master + 1) % world_size;
                    free(word_buffer);
                }
            }
        }

        // SLAVE NODES
        else{
            while(1){
                memset(query, 0, 140 * sizeof(char));
                
                // Recieve a query from the master
                MPI_Recv(query, 140, MPI_CHAR, master, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE );
                
                // Query realted to change of the master.
                if(strcmp(query, "!") == 0 ){
                    master = (master + 1) % world_size;
                    break;
                }
                
                // Query realted to the end of work of the master.
                else if(strcmp(query, "$") == 0 ){
                    pending_master[master] = 0;
                    finish = 1;
                    i = 0;
                    master = (master + 1) % world_size;
                    while( i < world_size){
                        if(pending_master[(master + i) % world_size]){
                            master = master + i;
                            finish = 0;
                            break;
                        }
                        i++;
                    }
                    break;
                }

                // Searching the Index for the query.
                list = clus_search(*clus_root, query);
                if(list){
                    size2 = max_deser, iter2 = 0;
                    str2 = (char*) malloc(size2 * sizeof(char));
                    memset(str2, 0, size2 * sizeof(char));
                    serialize_list(list, &str2, &iter2, &size2);
                }else{
                    str2 = (char*) malloc(4 * sizeof(char));
                    memset(str2, '\0', 4 * sizeof(char));
                    memcpy(str2, "-1\n", 4 * sizeof(char));
                    size2 = 4;
                }
                MPI_Send(str2, size2, MPI_CHAR, master, 2, MPI_COMM_WORLD);
                free(str2);
            }
        }
    }
}
