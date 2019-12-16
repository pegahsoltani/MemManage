CS342 Project#3

Can Demirel: 21401521

Pegah Soltani: 21500559

##About
This program takes a size from the user and allocates a chunk of memory as large as the size. It also allocates memory in the chunk according to one of the 3 algorithms defined for managing the memory. This document contains a makefile so just running the command "make" will compile all the necessary files. In order to run the program please try the command below:
./app "sizeOfTheChunk" "MethodForAllocation"

##Usage
./app <chunk_size> <holesearch_method>


For instance the command below will allocate a 1024 KB chunk and allocate memory based on first fit algorithm
./app 1024 0


