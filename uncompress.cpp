#include "HCTree.hpp"

int main(int argc, char* argv[]){

    if(argc != 3){
        error("Incorrect parameters");
        return 1;
    }

    //opening input file for reading
    FancyInputStream inputfile(argv[1]);

    //opening output file for writing
    FancyOutputStream outputfile(argv[2]);

    //empty file
    if(inputfile.filesize() == 0){
        return 0;
    }

    /**
    //read file header, i.e. 1024 bytes 
    int totaloriginal = 0; //counts original file size based on frequencies
    vector<int> frequencies(256, 0);
    for (long unsigned int i = 0; i < frequencies.size(); i++){
        frequencies[i] = inputfile.read_int();
        totaloriginal += frequencies[i];
    }
    */
    //build Huffman tree
    HCTree ht;
    //ht.build(frequencies);
    //REPLACING WITH deserialize
    int totaloriginal = inputfile.read_int();
    ht.deserialize(inputfile, headersize);

    //int totaloriginal = inputfile.read_int();
    //reading bits to get decoded ASCII letter and write to output file
    for(int i = 0; i < totaloriginal; i++){
        unsigned char letter = ht.decode(inputfile);
        outputfile.write_byte(letter);
    }
    
    return 0;
}
