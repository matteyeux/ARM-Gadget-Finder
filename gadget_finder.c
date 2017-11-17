//
//  gadget_finder.c
//  
//
//  Created by Billy Ellis on 26/10/2017.
//
//  This code scans ARM binaries for ROP gadgets useful for exploit developers


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

//this function checks bytes to see if they match common ARM instruction encodings
//only some instruction encodings are stored here, feel free to add others :)
char * checkInstruction(int c1, int c2, int c3, int c4){
    
    // check for 07 D0 A0 E1 / mov sp, r7
    if (c1 == 0x07 && c2 == 0xD0 && c3 == 0xA0 && c4 == 0xE1){
        return "mov sp, r7";
    }
    // check for 00 00 81 E5 / str r0, [r1]
    if (c1 == 0x00 && c2 == 0x00 && c3 == 0x81 && c4 == 0xE5){
        return "str r0, [r1]";
    }
    // check for 80 80 BD E8 / pop {r7, pc}
    if (c1 == 0x80 && c2 == 0x80 && c3 == 0xBD && c4 == 0xE8){
        return "pop {r7, pc}";
    }
    // check for 03 80 BD E8 / pop {r0, r1, pc}
    if (c1 == 0x03 && c2 == 0x80 && c3 == 0xBD && c4 == 0xE8){
        return "pop {r0, r1, pc}";
    }
    return "UNKNOWN INSTRUCTION";
}


long long get_file_size(const char *armfile){
    struct stat sb;
    int get_stat = stat(armfile, &sb);
    if (get_stat == -1) {
        perror("stat");
        return get_stat;
    }
    // return size of file in bytes
    return (long long) sb.st_size;
}

int main(int argc, char const *argv[]){
    
    const char *bin;
    long long binsize;
    unsigned char c;
    char *instruction;
    size_t bytes = 0;
    int i = 0;
    
    if (argc != 2)
    {
        printf("usage : %s [file]\n", argv[0]);
        return -1;
    }

    bin = argv[1];
    binsize = get_file_size(bin);
    unsigned char hex[binsize];

    printf("Welcome to @bellis1000's ROP Gadget Finder!\n");
    get_file_size(bin);
    FILE *f = fopen(bin,"r");

    fread(&hex, 1, binsize, f);

    printf("Searching binary for gadgets...\n\n");
    
    // search for 80 80 BD E8 / pop {r7, pc}
    // this is the common "return" instruction in the 32-bit ARM instruction set
    
    while (i < binsize){
        // if pop {r7, pc} is found...
        if (hex[i] == 0x80 && hex[i+1] == 0x80 && hex[i+2] == 0xBD && hex[i+3] == 0xE8){
            // search backwards 4 bytes for previous instruction
            instruction = checkInstruction(hex[i-4],hex[i-3],hex[i-2],hex[i-1]);
            
            printf("%s\n",instruction);
            // calculation of the address is very broken
            // the '16384' is a number I've found that works for ARM binaries compiled by me using clang
            // does not work for all binaries, need to figure this out
            printf("pop {r7, pc} found at address \x1B[32m0x%x\n\n\x1B[0m",(i+16384)-0x4);
        }
        i++;
    }
    
    i = 0;
    
    // search for 1E FF 2F E1  / bx lr
    // this is another "return" instruction in the 32-bit ARM instruction set
    
    while (i < binsize){
        if (hex[i] == 0x1E && hex[i+1] == 0xFF && hex[i+2] == 0x2F && hex[i+3] == 0xE1){
            // search backwards 4 bytes for previous instruction
            instruction = checkInstruction(hex[i-4],hex[i-3],hex[i-2],hex[i-1]);
            
            printf("%s\n",instruction);
            // calculation of the address is very broken
            // the '16384' is a number I've found that works for ARM binaries compiled by me using clang
            // does not work for all binaries, need to figure this out
            printf("bx lr found at address \x1B[32m0x%x\n\n\x1B[0m",(i+16384)-0x4);
        }
        i++;
    }
    fclose(f);
    return 0;
}
