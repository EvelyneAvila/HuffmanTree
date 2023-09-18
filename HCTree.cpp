#include "HCTree.hpp"
#include <stack>
#include <iostream>


int HCTree::headersize = 0;
/**
 * Helper function for destructor. Post order traversal (LRV) on Huffman Tree
 */
void HCTree::clear(HCNode* x){
    if(x == nullptr){
        return;
    }

    //For each node in tree, traverse all the way left and all the way right
    clear(x->c0);
    clear(x->c1);

    //"visit" node
    delete x; //for every new, write a delete
}


/*
 * Destructor. Deleting anything allocated dynamically
 */
HCTree::~HCTree(){
    clear(root);
    root = nullptr;
}

/**
* Helper function to serialize Huffman tree. build() has been called
*/
void HCTree::serialPreOrder(HCNode* curr, FancyOutputStream & out){
    if(curr == nullptr){
        out.write_byte(0);
        headersize += 1;
        return;
    }
    //'visit' node
    out.write_byte(curr->symbol);
    headersize += 1;
    //iterate
    serialPreOrder(curr->c0, out); //going all the way left
    serialPreOrder(curr->c1, out); //going all the way right
}

void HCTree::serialize(FancyOutputStream & out){
    serialPreOrder(root, out);
}

/**
* Helper function to deserialize file header
*/
HCNode* HCTree::deserialPreOrder(FancyInputStream & input, int idx, int total){
    int pass = 0;
    if(idx == total){
        //stopping when reading the number needed for header
        //HOW? can i pass header size created above?
        return nullptr;
    }
    int byte = input.read_byte();
    idx += 1;
    if(byte == 0){
        return nullptr;
    }else{ 
        //encountered symbol
        HCNode* node = new HCNode(1, byte);
        leaves[byte] = node;
        if(pass == 0){
            root = node;
            pass += 1;
        }
        node->c0 = deserialPreOrder(input, idx, total);
        node->c1 = deserialPreOrder(input, idx, total);
        return node;
    }

}

void HCTree::deserialize(FancyInputStream & input, int total){
    int idx = 0;
    deserialPreOrder(input, idx, total);
}

/**
* Use the Huffman algorithm to build a Huffman coding tree.
* PRECONDITION: freqs is a vector of ints, such that freqs[i] is the frequency of occurrence of byte i/letter i in the input file.
* POSTCONDITION: root points to the root of the trie, and leaves[i] points to the leaf node containing byte i/letter.
*/
void HCTree::build(const vector<int>& freqs){
    //Using priority queue to order forest of trees from smallest to largest
    priority_queue<HCNode*, vector<HCNode*>, HCNodePtrComp> pq;

    //iterating through all frequencies
    for(long unsigned int i = 0; i < freqs.size(); i++){
        if(freqs[i] != 0){
            //only counting non-zero frequencies for building
            HCNode* letter = new HCNode(freqs[i], i);
            leaves.insert(leaves.begin() + i, letter);
            pq.push(letter);
        }
    }

    //while there is more than one tree in forest
    while(pq.size()!= 1){
        HCNode* smallest = pq.top();
        pq.pop(); //because there is no return type for pop()
        HCNode* secondsmallest = pq.top();
        pq.pop();
        int totalcount = (smallest->count) + (secondsmallest->count);
        //For tie count, we break off with symbol and will use symbol with children with lowest count
        HCNode* newroot = new HCNode(totalcount, smallest->symbol);
        newroot->c0 = smallest;
        newroot->c1 = secondsmallest;
        smallest->p = newroot;
        secondsmallest->p = newroot;
        pq.push(newroot);   
    }

    root = pq.top();
}

/**
* Write to the given FancyOutputStream the sequence of bits coding the given symbol.
* Output sequence of bits given by 1s and 0s
* PRECONDITION: build() has been called, to create the coding tree, and initialize root pointer and leaves vector.
*/
void HCTree::encode(unsigned char symbol, FancyOutputStream & out) const{
    int letter = (int)symbol;
    HCNode* curr = leaves[letter]; //pointing to symbols node

    //Using stack to traverse from leaf to root in HCTree
    stack<int> reverse;
    while(curr!= root){
        if(curr->p->c0 == curr){
            reverse.push(0);
        }else{
            reverse.push(1);
        }
        curr = curr->p;
    }

    //writing to output stream by reversing stack to get correct encoding
    while(!reverse.empty()){
        out.write_bit(reverse.top());
        reverse.pop();
    }
}


/**
* Return symbol uncoded/ASCII letter
* PRECONDITION: build() has been called, to create the coding tree, and initialize root pointer and leaves vector.
*/
unsigned char HCTree::decode(FancyInputStream & in) const{
    HCNode* curr = root;
    //Iterating until reaching leaf node (letters/symbol guaranteed to be a leaf node)
    while((curr->c0 != nullptr) & (curr->c1 != nullptr)){
        int edge = in.read_bit();
        if(edge == 0){
            curr = curr->c0; //traverse to 0 edge
        }else{
           curr = curr->c1; //traverse to 1 edge
        }
    }

    return curr->symbol; //leaf node with decoded symbol, i.e. ASCII character
}
