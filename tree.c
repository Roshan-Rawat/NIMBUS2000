#include "tree.h"


Tree root = { .n={

        .tag = (TagNode | TagRoot) ,
        .north = (Tree*)&root ,
        .west = 0 ,
        .east = 0 ,
        .path = "/"
        }
};


void zeroOutStructure(int8 *str , int16 size){
    int8 *p ; 
    int16 i ;

    for(i = 0 , p = str ; i < size ; ++p , ++i){
        *p = 0 ; 
    }
    
}

Node *create_Node(Node *parent , int8* path){
    Node *n ; 
    int16 size ; 


    errno = NoError ; 
    assert(parent) ;
    size = (sizeof(Node)) ; 
    n = (Node *)malloc(size) ;
    zeroOutStructure((int8*)n , size) ; 

    parent->west = n ; 
    n->tag = TagNode ; 
    n->north = (Tree *)parent ; 

    strncpy((char *)n->path , (char *)path , 255) ; 
    
    return n ; 
}


Leaf *find_last_leaf(Node *parent){
    Leaf *l ;

    errno = NoError ; 
    assert(parent) ; 
    if(!parent->east)
        reterr(NoError) ; 
    for(l = (Leaf *)parent->east ; l->east ; l = l->east);

    assert(l) ; 

    return l ; 
}

Leaf *create_Leaf(Node *parent, int8 *key , int16 count , int8* value){
    Leaf *l , *new; 
    int16 size ;

    assert(parent) ; 
    l = find_last(parent) ;

    size = sizeof(Leaf) ; 
    new = (Leaf*)malloc(size) ; 
    assert(new) ; 
    if(!l) parent->east = new ;  //directly connected
    else l->east = new ;  //last one is leaf

    zeroOutStructure((int8 *)new , size) ; 

    new->tag = TagLeaf ; 
    new->west = (!l) ? (Tree *)parent : (Tree *)l ;

    strncpy((char *)new->key , (char *)key , 128) ;
    new->key[127] = '\0' ;  
    new->value = (int8 *)malloc(count) ; 
    zeroOutStructure(new->value , count) ; 
    assert(new->value) ;
    strncpy((char *)new->value , (char *)value , count) ;
    new->value[count] = '\0' ;  
    new->size = count ; 

    return new ; 
}





int main(){

    Node *n , *n2 ;
    Leaf *l1 , *l2 ; 

    int8 *key , *value ; 
    int16 size ;
    n = create_Node((Node *)&root , (int8 *)"/Users") ; 
    assert(n) ; 
    n2 = create_Node(n , (int8 *)"/Users/login") ; 
    assert(n2) ;  

    key = (int8 *)"devendra" ; 
    value = (int8 *)"oo074u39dns" ; 
    size = (int16)strlen((char *)value) ; 
    l1 = create_Leaf(n2 , key , size , value ) ; 

    assert(l1) ; 


    
    key = (int8 *)"priyanka" ; 
    value = (int8 *)"oo03u43qir" ; 
    size = (int16)strlen((char *)value) ; 
    l2 = create_Leaf(n2 , key , size , value ) ; 
    printf("%s %s\n" ,l1->value ,  l2->value) ; 
    printf("%p %p\n", n , n2) ;
    free(n2) ; 
    free(n) ; 
    return 0 ; 
}