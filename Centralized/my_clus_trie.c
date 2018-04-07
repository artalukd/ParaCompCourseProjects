#include "_TRIE.h"
#include "_LINKED_LIST.h"
#include <string.h>

#define _CLUS_TRIE

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])
 
TrieNode* get_clus_Node(void){
    /*******************************************************************
    * DESCRIPTION : Retuns an initialized clus_trie node
    * 
    */
    TrieNode *temp = (TrieNode*) malloc(sizeof(TrieNode));
    temp -> children = (TrieNode**) malloc(ALPHABET_SIZE * sizeof(TrieNode*));

    if (temp){
        int i;
        temp -> end = false;
        temp -> frequency = 0;
        temp -> list = create_list();
        for (i = 0; i < ALPHABET_SIZE; i++){
            temp -> children[i] = NULL;
        }
    } 
    return temp;
}
 
TrieNode* clus_insert(TrieNode *root, char *key, unsigned int frequency, char* doc_name){
    /*******************************************************************
    * DESCRIPTION : Insert a node into the the clus_trie
    * INPUT :     
    *           [1] root: Pointer to the root of clus_trie.
    *           [2] Other arguments are self explainatory.
    * OUTPUT :
    *           [1] Returns pointer to the root of the clus_trie.
    * 
    */

    int depth, len = strlen(key), index;
    TrieNode *iter = root;
    for (depth = 0; depth < len; depth++){
        index = CHAR_TO_INDEX(key[depth]);
        if (!iter -> children[index]){
            iter -> children[index] = get_clus_Node();
        } 
        iter = iter -> children[index];
    }
    iter -> end = true;

    Element* temp = (Element*) malloc(sizeof(Element));
    temp -> frequency = frequency;
    temp -> doc_name = (char*) malloc((strlen(doc_name) + 1) * sizeof(char));
    strcpy(temp -> doc_name, doc_name);
    iter -> list = insert_in_order(iter -> list, temp);
    return root;
}
 
bool clus_search(struct TrieNode *root, const char *key){
    /*******************************************************************
    * DESCRIPTION : Search a key in the trie
    * INPUT :     
    *           [1] root: Pointer to the root of clus_trie.
    *           [2] key : key that needs to be searched.
    * OUTPUT :
    *           [1] Returns a boolean 
    */

    int depth, len = strlen(key), index;
    TrieNode* iter = root;
 
    for (depth = 0; depth < len; depth++){
        index = CHAR_TO_INDEX(key[depth]);
        if (!iter -> children[index]){
            return false;
        }
        iter = iter -> children[index];
    }
    print_list(iter -> list);
    return (iter != NULL && iter -> end);
}


char index_to_char(int index){
    /*******************************************************************
    * DESCRIPTION : Returns index'th character
    * 
    */
    return index + 'a';
}


// int main(){
//     // Input keys (use only 'a' through 'z' and lower case)
//     // char keys[][8] = {"the", "a", "there", "answer", "any", "by", "bye", "their"};
//     char output[][32] = {"Not present in trie", "Present in trie"};
//     char keys[][4] = {"a", "any", "by", "bye"};

//     TrieNode *root = get_clus_Node();
//     int i;
//     for (i = 0; i < ARRAY_SIZE(keys); i++){
//         clus_insert(root, keys[i], i, "lol");
//     }

//     int size1 = 100, size2 = 100;
//     char* str1 = (char*) malloc(size1 * sizeof(char));
//     char* str2 = (char*) malloc(size2 * sizeof(char));
//     int iter1 = 0, iter2 = 0;
//     serialize_trie(root, &str1, &str2, -1, &iter1, &iter2, &size1, &size2);

//     printf("%s\n", str1);
//     printf("%s\n", str2);

//     TrieNode* de_root;
//     iter1 = 0;
//     iter2 = 0;
//     de_root = deserialize_trie(de_root, &str1, &str2, &iter1, &iter2);


//     // FILE* fptr1 = fopen("serial_trie.txt", "w+");
//     // FILE* fptr2 = fopen("serial_lists.txt", "w+");
//     // serialize_trie(root, &fptr1, &fptr2, -1);
//     // fclose(fptr1);
//     // fclose(fptr2);

//     // fptr1 = fopen("serial_trie.txt", "r");
//     // fptr2 = fopen("serial_lists.txt", "r");
//     // // FILE* fptr_temp = fopen("test.txt", "w+");
//     // TrieNode* de_root;
//     // de_root = deserialize_trie(de_root, &fptr1, &fptr2);
//     // // serialize_trie(de_root, fptr_temp, -1);
//     // fclose(fptr1);
//     // fclose(fptr2);
//     // // fclose(fptr_temp);    

//     printf("%d\n\n", clus_search(de_root, "answer"));
//     printf("%d\n", clus_search(de_root, "a"));
//     printf("%d\n", clus_search(de_root, "any"));
//     printf("%d\n", clus_search(de_root, "by"));
//     printf("%d\n", clus_search(de_root, "bye"));

//     // fptr2 = fopen("serial_lists.txt", "r");
//     // List* list = deserialize_list(fptr2);
//     // print_list(list);
//     // List* list1 = deserialize_list(fptr2);
//     // print_list(list1);
//     // List* list2 = deserialize_list(fptr2);
//     // print_list(list2);
//     // List* list3 = deserialize_list(fptr2);
//     // print_list(list3);
//     // List* list4 = deserialize_list(fptr2);
//     // print_list(list4);
//     // fclose(fptr2);
//     // Search for different keys
//     // printf("%s --- %s\n", "the", output[search(root, "the")] );
//     // printf("%s --- %s\n", "these", output[search(root, "these")] );
//     // printf("%s --- %s\n", "their", output[search(root, "their")] );
//     // printf("%s --- %s\n", "thaw", output[search(root, "thaw")] );
// //     // printf("%s --- %s\n", "these", output[search(root, "a")] );
// //     // printf("%s --- %s\n", "these", output[search(root, "there")] );
 
//     return 0;
// }
