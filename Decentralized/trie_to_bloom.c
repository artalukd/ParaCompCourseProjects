#include "_TRIE_BLOOM.h"

unsigned long* traverse(TrieNode *cnode, unsigned long* vec, char *key, int depth){
    /*******************************************************************
    * DESCRIPTION : Traversing the Trie and adding each node into the bloom filter
    * INPUT :     
    *           [1] cnode : Pointer to the root of the Trie.
    *           [2] vec : Pointer to the bloom filter.
    *           [3] key : key to add (used in  recurive call, initially passed as pointer to empty buffer)
    *           [4] depth : depth of the trie (Used in recursive call. Initially passed as zero)
    * OUTPUT : 
    *           [1] Returns the pointer of the updated bloom filter.
    */
    int i = 0, d = depth + 1;
    if( cnode -> end)
        vec = addB( key, vec);   
    for( i = 0; i < ALPHABET_SIZE; i++ )
    {   if( cnode -> children[i] ){
            key[depth] = i +'a';
            key[depth + 1] = '\0'; 
            cnode -> children[i] = traverse(cnode -> children[i], vec , key, d );
        }
    }
    return cnode;
}

unsigned long* clusToBloom(TrieNode *root){
    /*******************************************************************
    * DESCRIPTION : Add each word from the clus_trie to the bloom filter
    * INPUT :     
    *           [1] root : Pointer to the root of the Trie.
    * OUTPUT : 
    *           [1] Returns the pointer of the updated bloom filter.
    */
    int depth = 0;
    unsigned long* vec = getVector();
    char* key = (char*) malloc( 26 * sizeof(char));
    memset(key, 0, 25);
    return traverse(root, vec, key, depth);
}
