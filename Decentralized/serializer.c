#include "_TRIE.h"
#include "_LINKED_LIST.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


void MAY_REALLOC(char** fp, int* fp_count, int* fp_size){
    /******************************************************************* 
    * DESCRIPTION : Reallocs the fp if fp_count has reach the fp_size.
    * INPUT :     
    *           [1] fp : Pointer to the string
    *           [2] fp_count : length of the fp
    *           [3] fp_size : capacity of the fp
    * 
    */

    if((*fp_count) >= (*fp_size) - 100){
        int size = (*fp_size);
        *fp = (char*) realloc(*fp, 2 * size * sizeof(char));
        memset((*fp) + (*fp_size), '\0', *fp_size);
        (*fp_size) = 2 * size;
    }
}

void serialize_trie(TrieNode* root, char** fp1, char** fp2, int child_index, int* fp1_count, int* fp2_count, int* fp1_size, int* fp2_size){
    /*******************************************************************
    * DESCRIPTION : Serializing the trie
    * INPUT :     
    *           [1] root : root of the trie.
    *           [2] fp1 : Pointer to the string that stores the serialized trie.
    *           [3] fp2 : Pointer to the string that stores the serialized linkedlist.
    *           [4] child_index : used in recursion to know the state of dfs. (Initially -1)
    *           [5] fp1_count : Pointer to the size of fp1.
    *           [6] fp2_count : Pointer to the size of fp2.
    *           [7] fp1_size : Pointer to the capacity of fp1.
    *           [8] fp2_size : Pointer to the capacity of fp2.
    * 
    */

    if(root == NULL){
    	return;
    }

    if(child_index == -1){
        *(*fp1 + *fp1_count) = '$';
        (*fp1_count) ++;
        MAY_REALLOC(fp1, fp1_count, fp1_size);
 	}else{
 		if(root -> end){
            *(*fp1 + *fp1_count) = index_to_char(child_index);
            (*fp1_count) ++;
            MAY_REALLOC(fp1, fp1_count, fp1_size);
            *(*fp1 + *fp1_count) = '(';
            (*fp1_count) ++;
            MAY_REALLOC(fp1, fp1_count, fp1_size);
    		serialize_list(root -> list, fp2, fp2_count, fp2_size);
    	}else{
            *(*fp1 + *fp1_count) = index_to_char(child_index);
            (*fp1_count) ++;
            MAY_REALLOC(fp1, fp1_count, fp1_size);
    	}
 	}

    int i;
 	for(i = 0; i < ALPHABET_SIZE; i++){
       	serialize_trie(root -> children[i], fp1, fp2, i, fp1_count, fp2_count, fp1_size, fp2_size);
    }
 
    *(*fp1 + *fp1_count) = '|';
    (*fp1_count) ++;
    MAY_REALLOC(fp1, fp1_count, fp1_size);
}


TrieNode* deserialize_trie(TrieNode* root, char** fp1, char** fp2, int* fp1_count, int* fp2_count){
    /*******************************************************************
    * DESCRIPTION : Deserializing the trie. 
    * INPUT :     
    *           [1] root :Pointer to the root of the trie.
    *           [2] fp1 : Pointer to the string that stores the serialized trie.
    *           [3] fp2 : Pointer to the string that stores the serialized linkedlist.
    *           [4] fp1_count : Pointer to the size of fp1.
    *           [5] fp2_count : Pointer to the size of fp2.
    * OUTPUT : 
    *           [1] Returns the Root node of the trie.
    */
    char val;
	int garbage;

    while(1){
        val = *(*fp1 + *fp1_count);
        (*fp1_count) ++;
	    if(val == '|'){
			return root;
	    }else if(val == '$'){
	    	root = get_clus_Node();
	    	root = deserialize_trie(root, fp1, fp2, fp1_count, fp2_count);
	    	break;
	    }else if(val == '('){
		   	root -> end = 1;
		   	root -> list = deserialize_list(fp2, fp2_count);
	    	root = deserialize_trie(root, fp1, fp2, fp1_count, fp2_count);
	    	break;
	    }else{
		   	int index = CHAR_TO_INDEX(val);
	    	root -> children[index] = get_clus_Node();
	    	root -> children[index] = deserialize_trie(root -> children[index], fp1, fp2, fp1_count, fp2_count);
	    }
	}
	return root;
}

char* itoa(int data){
    /*******************************************************************
    * DESCRIPTION : Converting interger to the corresponding string. (reverse of atoi)
    * 
    */
	char* ret = (char*) malloc(10*sizeof(char));
	memset(ret, '\0', 10 * sizeof(char));
	sprintf(ret, "%d", data);
	return ret;
}

void serialize_list(List* list, char** fp, int* fp_count, int* fp_size){
    /*******************************************************************
    * DESCRIPTION : Serializing the LinkedList. 
    * INPUT :     
    *           [1] list :Pointer to the head of the LinkedList.
    *           [2] fp : Pointer to the string that stores the serialized LinkedList.
    *           [3] fp_count : Pointer to the size of fp.
    *           [4] fp_size : Pointer to the capacity of fp. 
    * 
    */
	Node* temp = list -> head;
	char* temp_fp = *fp;
	while(temp != NULL){

		char* buf = itoa(temp -> ele -> frequency);
		int len = strlen(buf);
		memcpy(temp_fp + *fp_count, buf, len);
		*fp_count += len;
		memcpy(temp_fp + *fp_count, "\t", sizeof(char));
		*fp_count += 1;
		len = strlen(temp -> ele -> doc_name);
		memcpy(temp_fp + *fp_count, temp -> ele -> doc_name, len);
		*fp_count += len;
		memcpy(temp_fp + *fp_count, "\t", sizeof(char));
		*fp_count += 1;

		if((*fp_count) >= (*fp_size) - 50){
			MAY_REALLOC(fp, fp_count, fp_size);
			temp_fp = *fp;
		}
		temp = temp -> next;
	}
    memcpy(temp_fp + *fp_count, "-1\n", strlen("-1\n"));
    *fp_count += strlen("-1\n");
}

List* deserialize_list(char** fp, int* fp_count){
    /*******************************************************************
    * DESCRIPTION : Deserializing the LinkedList. 
    * INPUT :     
    *           [1] fp : Pointer to the string that stores the serialized LinkedList.
    *           [2] fp_count : Pointer to the size of fp.
    * OUTPUT : 
    *           [1] Returns the head of the LinkedList.
    */
	List* list = create_list();
	int garbage, len;
    unsigned int frequency;
	char* temp_str = (char*) malloc(100 * sizeof(char));
	memset(temp_str, '\0', 100 * sizeof(char));

	while(1){
		Element* data = (Element*) malloc(sizeof(Element));
		
		garbage = sscanf((*fp + *fp_count), "%d", &frequency);
		char* freq_str = itoa(frequency);
		len = strlen(freq_str);
		(*fp_count) += len + 1;
		if(frequency == -1){
			break;
		}
		garbage = sscanf((*fp + *fp_count), "%s", temp_str);
		len = strlen(temp_str);
		data -> doc_name = (char*) malloc((len + 1) * sizeof(char));
        memset(data -> doc_name, '\0', len);
		(*fp_count) += len + 1;

		data -> frequency = frequency;
		memcpy(data -> doc_name, temp_str, (len) * sizeof(char));
		memset(temp_str, '\0', 100 * sizeof(char));

		list = add_to_end(list, data);
	}

	return list;
}