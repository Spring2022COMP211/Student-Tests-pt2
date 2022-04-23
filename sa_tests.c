#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "sa_cache.h"
#include "main_mem.h"
#include "main_mem_log.h"

#define read mem->op_log->readCounts

int main(){
    uint32_t set_index_bitcount = 1;
    uint32_t word_index_bits = 1;
    uint32_t num_cache_lines = 4;

    MainMem *mem = createMainMem(7);
    // Test: Abides by constraints
    // KMP solution won't actually pass this, may go back and add that for posterity -Ryan
    MainMem *mem2 = createMainMem(3);
    SACache*  cache2 = createSACache(mem2,  set_index_bitcount, word_index_bits, num_cache_lines);
    if (cache2 == NULL) {
        // This is TEST 1
        printf("Little Dipper\n");
    }

    // Load up the main men with known contents
    writeWord(mem, 0, 0x09099000);
    writeWord(mem, 4, 0x19111111);
    writeWord(mem, 8, 0x23222222);
    writeWord(mem, 12, 0x12341234);
    writeWord(mem, 16, 0x56785678);
    writeWord(mem, 20, 0x99889988);
    writeWord(mem, 24, 0x66666666);
    writeWord(mem, 28, 0x77777777);
    writeWord(mem, 32, 0x85493741);
    writeWord(mem, 36, 0x99999999);
    writeWord(mem, 40, 0x91187524);
    writeWord(mem, 44, 0x82830323);
    writeWord(mem, 48, 0x91061096);
    writeWord(mem, 52, 0x17372737);
    writeWord(mem, 56, 0x37AE4CB);
    writeWord(mem, 60, 0x29FE12FA);
    writeWord(mem, 64, 0x11311311);
    writeWord(mem, 68, 0x13121312);
    writeWord(mem, 72, 0x10895438);
    writeWord(mem, 76, 0x66699902);
    writeWord(mem, 80, 0x81742381);


    clearLog(mem->op_log);

    SACache*  cache = createSACache(mem,  set_index_bitcount, word_index_bits, num_cache_lines);

    // Test: not constantly read from mem
    uint8_t value = 0;
    for (uint32_t i=0; i<15; i++) {
        readByte(cache, 16, &value);
    }
    if (mem->op_log->readCounts[4] == 1) {
        // This is TEST 2
        printf("Fornax\n");
    }
    // Test: reads correct values
    uint8_t value2 = 0;
    uint8_t value3 = 0;
    uint8_t value4 = 0;
    readByte(cache, 32, &value);
    readByte(cache, 49, &value2);
    // was running into memory size issues here so switched to 7 width address
    readByte(cache, 66, &value3);
    readByte(cache, 3, &value4);
    readByte(cache, 16, &value4);
    if (value == 0x41 && value2 == 0x10 && value3 == 0x31 && value4 == 0x78){
        // readByte(cache, 24, &value);
        readByte(cache, 40, &value);
        readByte(cache, 57, &value2);
        readByte(cache, 74, &value3);
        readByte(cache, 11, &value4);
        if (value == 0x24 && value2 == 0xE4 && value3 == 0x89 && value4 == 0x23) {
            // This is TEST 3
            printf("Cassiopeia\n");
        }
    }
    // check read/write for all addresses/values
    if (read[4] == 2 && read[8] == 1 && read[12] == 1 && read[16] == 1 && read[0] ==1) {
        if (read[10] == 1 && read[14] == 1 && read[18] == 1 && read[2] == 1) {
            // This is TEST 4
            printf("Ursa Minor\n");
        }
    }

    // Test: writing values
    //this makes line 4 of set 2 'dirty'
    writeByte(cache, 11, 0x24);
    uint32_t actual = 0;
    // mask off w/ pattern 1100
    readWord(mem, 11 & 8, &actual);
    if (actual == 0x23222222) {
        for (int i = 0; i < 2; i++) {
            readByte(cache, 40, &value);
            readByte(cache, 57, &value2);
            readByte(cache, 74, &value3);
        }
        readByte(cache, 24, &value4);
        // now it overwrites and pushes changes back to mem
        readWord(mem, 11 & 8, &actual);
        if (actual == 0x24222222) {
            // This is TEST 6
            printf("Draco\n");
        }
    }
    // Test: flush (2 parts)
    // this line is already cached
    // reads shouldn't go up unless flush works
    int8_t oldReads1 = read[10];
    int8_t oldReads2 = read[14];
    int8_t oldReads3 = read[18];
    int8_t oldReads4 = read[6];
    writeByte(cache, 40, 0x99);
    writeByte(cache, 56, 0x88);
    writeByte(cache, 72, 0x77);
    writeByte(cache, 24, 0x55);
    flushCache(cache);

    readByte(cache, 40, &value);
    readByte(cache, 57, &value2);
    readByte(cache, 74, &value3);
    readByte(cache, 24, &value4);
    if(read[10] != oldReads1 && read[14] != oldReads2 && read[18] != oldReads3 && read[6] != oldReads4) {
        // This is TEST 7
        printf("Hercules\n");
    }
    // since you write them all back when flushing,
    // all our writes should be in memory (and now back in cache)
    uint32_t val = 0;
    uint32_t val2 = 0;
    uint32_t val3 = 0;
    uint32_t val4 = 0;
    readWord(mem, 40, &val);
    readWord(mem, 56, &val2);
    readWord(mem, 72, &val3);
    readWord(mem, 24, &val4);
    if (val == 0x91187599 && val2 == 0x37AE488 && val3 == 0x10895477 && val4 == 0x66666655) {
        // This is TEST 8
        printf("Cygnus\n");
    }


    // This is TEST 5
    // Test: one last one to make sure people are always updated counters
    SACache*  cache3 = createSACache(mem,  set_index_bitcount, word_index_bits, num_cache_lines);
    clearLog(mem->op_log);
    readByte(cache3, 24, &value);
    readByte(cache3, 40, &value);
    readByte(cache3, 72, &value);
    readByte(cache3, 56, &value);
    for(int i = 0; i <20; i++) {
        // before this, it's technically the oldest
        // read it a bunch so it's timer goes up a lot
        readByte(cache3, 24, &value);
    }
    uint8_t oldReads = read[6];
    readByte(cache3, 8, &value);
    if (oldReads == read[6]) {
        printf("Pollux\n");
    }
}
