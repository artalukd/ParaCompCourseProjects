#include "_TRIE.h"
#include "_LINKED_LIST.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void plug(TrieNode *cnode, char* doc_name, unsigned int frequency){
    /*******************************************************************
    * DESCRIPTION : Insert a node corresponding to the doc_name and frequency to the linked_list of the cnode.
    *               Used in simultaneous dfs of doc_trie and clus_trie.
    * INPUT :     
    *           [1] cnode: Pointer to the root of clus_trie.
    *           [2] Other arguments are self explainatory.
    * 
    */

    Element* temp = (Element*) malloc(sizeof(Element));
    temp -> doc_name = (char*) malloc((strlen(doc_name) + 1) * sizeof(char));
    temp -> frequency = frequency;
    strcpy(temp -> doc_name, doc_name);
    cnode -> list = insert_in_order(cnode -> list, temp);
    cnode -> end = true;
}


TrieNode* insert_doc_in_clus(TrieNode *cnode, TrieNode *dnode, char* doc_name ){   
    /*******************************************************************
    * DESCRIPTION : Merge the doc_trie corresponding to the doc_name into clus_trie.
    * INPUT :     
    *           [1] cnode : Pointer to the root of clus_trie.
    *           [2] dnode : Pointer to the root of the doc_trie.
    *           [3] Other arguments are self explainatory.
    * OUTPUT :
    *           [1] Returns pointer to the root of the clus_trie.
    */

    int i = 0;  
    if(dnode -> end)
        plug(cnode, doc_name, dnode -> frequency);
    
    for(i = 0; i < ALPHABET_SIZE; i++){
        if(dnode -> children[i]){
            if (!cnode -> children[i])
                cnode -> children[i] = get_clus_Node();
            cnode -> children[i] = insert_doc_in_clus(cnode -> children[i], dnode -> children[i], doc_name );
        }     
    }
    return cnode;
}

TrieNode* cluster_merge(TrieNode *croot1,TrieNode *croot2){
    /*******************************************************************
    * DESCRIPTION : Merge operation on two cluster level tries.
    * Input :     
    *           [1] croot1: Pointer to the root of first clus_trie.
    *           [2] croot2: Pointer to the root of second clus_trie.
    * OUTPUT : 
    *           [1] Returns pointer to the root of the merged clus_trie.
    */
    int i = 0;  
    if(croot2 -> end)
        croot1-> list = mergeLists( croot1->list , croot2->list );

    for(i = 0; i < ALPHABET_SIZE; i++){
        if(croot2 -> children[i]){
            if (!croot1 -> children[i])
                croot1 -> children[i] = get_clus_Node();
            croot1 -> children[i] = cluster_merge( croot1 -> children[i],croot2 -> children[i] );
        }
    }
    return croot1;
}


int doc_free(TrieNode* dnode){
    /*******************************************************************
    * DESCRIPTION : Recursive free operation on doc_trie.
    * INPUT :
    *           [1] dnode: Pointer to the root of doc_trie.
    * OUTPUT :
    *           [1] Returns total number of nodes freed.
    */
    int i, k = 1;
    for(i = 0;i < ALPHABET_SIZE; i++){
        if(dnode -> children[i])
            k += doc_free( dnode -> children[i]);
    }
    free(dnode);
    return k;
}