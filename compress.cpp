#include "HCTree.hpp"


int main(int argc, char* argv[]){

    if(argc != 3){
        error("Incorrect parameters");
        return 1;
    }

    //open input file for reading
    FancyInputStream inputfile(argv[1]); //reading from original file

    //open output file for writing
    FancyOutputStream outputfile(argv[2]);

    //obtaining file size
    int file_size = inputfile.filesize();

    //empty file
    if(file_size == 0){
        return 0;
    }

    //read bytes from file counting ocurrance of each byte/letter and store in vector
    vector<int> frequencies(256, 0);
    for(int i = 0; i < file_size; i++){
        int byte = inputfile.read_byte(); //ASCII character as int
        frequencies[byte] += 1;  //ex. freq[65] = 2, where 65 = A is ASCII
    }

    //constructing Huffman tree
    HCTree ht;
    ht.build(frequencies);

    //TO EDIT FILE header
    //WANT TO SERIALIZE HERE USE HELPER function
    //keep track of how many bit do we have to read for file header for uncompress
    /*
    //write the ints of vector onto output for file header
    //file header 1024 bytes
    for(long unsigned int i = 0; i < frequencies.size(); i++){
        outputfile.write_int(frequencies[i]);
    }*/
    outputfile.write_int(file_size); //before header, total original size
    ht.serialize(outputfile);
    
    //moving back to start of original input file
    inputfile.reset();

    //Using huffman tree to translate each byte/letter from input file into its code
    for(int i = 0; i < file_size; i++){
        unsigned char byte = (unsigned char)inputfile.read_byte();
        ht.encode(byte, outputfile);
    }

    //output and input streams closed automatically because on stack
    //Destructor for HCTree called automatically because on stack

    return 0;
}
