/*******************************************************
                          main.cc
********************************************************/

#include <stdlib.h>
#include <assert.h>
#include <fstream>
using namespace std;

#include "cache.h"

int main(int argc, char *argv[])
{
    
    ifstream fin;
    FILE * pFile;

    if(argv[1] == NULL)
    {
        printf("input format: ");
        printf("./smp_cache <cache_size> <assoc> <block_size> <num_processors> <protocol> <trace_file> \n");
        exit(0);
    }

    ulong cache_size     = atoi(argv[1]);
    ulong cache_assoc    = atoi(argv[2]);
    ulong blk_size       = atoi(argv[3]);
    ulong num_processors = atoi(argv[4]);
    ulong protocol       = atoi(argv[5]); /* 0:MODIFIED_MSI 1:DRAGON*/
    char *fname        = (char *) malloc(20);
    fname              = argv[6];

    printf("===== 506 Personal information =====\n");
    printf("Po-Hsun Lin\n");
    printf("plin8\n");
    printf("ECE492 Students? NO\n");
    printf("===== 506 SMP Simulator configuration =====\n");
    // print out simulator configuration here
    printf("L1_SIZE:                %lu\n", cache_size);
    printf("L1_ASSOC:               %lu\n", cache_assoc);
    printf("L1_BLOCKSIZE:           %lu\n", blk_size);
    printf("NUMBER OF PROCESSORS:   %lu\n", num_processors);
    if (protocol == 0)
        printf("COHERENCE PROTOCOL:     %s\n", "MSI");
    else if (protocol == 1)
        printf("COHERENCE PROTOCOL:     %s\n", "DRAGON");
    printf("TRACE FILE: 	%s\n", fname);
    
    // Using pointers so that we can use inheritance */
    Cache** cacheArray = (Cache **) malloc(num_processors * sizeof(Cache));
    for(ulong i = 0; i < num_processors; i++) {
        cacheArray[i] = new Cache(cache_size, cache_assoc, blk_size);
    }

    pFile = fopen (fname,"r");
    if(pFile == 0)
    {   
        printf("Trace file problem\n");
        exit(0);
    }
    
    ulong proc;
    char op;
    ulong addr;

    // int line = 1;
    while(fscanf(pFile, "%lu %c %lx", &proc, &op, &addr) != EOF)
    {
        // check copy exist status (only for dragon)
        bool copy = false;
        bool send = cacheArray[proc]->checkCopy(addr, false);
        for(ulong i = 0; i < num_processors; i++)
        {
            if(i == proc || protocol == 0)   continue;
            if (op == 'w' && send == false)
            {
                if (cacheArray[i]->checkCopy(addr, true))
                    copy = true;
            }
            else if (cacheArray[i]->checkCopy(addr, false))
                copy = true;

        }
        // propagate request down through memory hierarchy
        // by calling cachesArray[processor#]->Access(...)
        ulong bus_signal = cacheArray[proc]->Access(addr, op, protocol, copy);
        for(ulong i = 0; i < num_processors; i++)
        {
            if(i != proc) 
            {
                cacheArray[i]->recvCore(addr, bus_signal, protocol);
            }
        }
    }

    fclose(pFile);

    //********************************//
    //print out all caches' statistics //
    for(ulong i = 0; i < num_processors; i++)
    {
        cacheArray[i]->printStats(i, protocol);
    }
    //********************************//

}
