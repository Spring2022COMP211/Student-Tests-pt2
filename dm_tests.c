#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "dm_cache.h"
#include "main_mem.h"
#include "main_mem_log.h"

int main(){
    uint32_t word_index_bits = 1;
    uint32_t set_index_bits = 1;
    // NUM of LINES is 1 << set_index_bits


    MainMem *mem2 = createMainMem(4);

    // This should not work because sum of
    // word, set, and byte index bit counts (1, 1, and 2 in this case)
    // needs to be strictly greater than number of address bits
    // in main memory (4 in this case).

    // Test: Abides by constraints
      DMCache *cache2 = createDMCache(mem2, set_index_bits, word_index_bits);

      if (cache2 == NULL) {
          printf("Here we go!\n");
      }


    // ADRESS WIDTH is equal to the num createMainMem takes in
    MainMem *mem = createMainMem(5);

    // Load up the main men with known contents
    writeWord(mem, 0, 0x00000000);
    writeWord(mem, 4, 0x11111111);
    writeWord(mem, 8, 0x22222222);
    writeWord(mem, 12, 0x12341234);
    writeWord(mem, 16, 0x56788765);
    writeWord(mem, 20, 0x99889988);
    writeWord(mem, 24, 0x66666666);
    writeWord(mem, 28, 0x77777777);
    writeWord(mem, 32, 0x88888888);
    writeWord(mem, 36, 0x99999999);

    clearLog(mem->op_log);

    DMCache* cache = createDMCache(mem, set_index_bits, word_index_bits);
    if (cache == NULL) {
      printf("Cache unsuccessfully created.");
      exit(-1);
    }

    // Test: not constantly read from mem
      uint8_t value = 0;
      for (uint32_t i=0; i<10; i++) {
        readByte(cache, 9, &value);
      }
      // THE INDEX FOR READCOUNTS is Address (9) >> 2
      if (mem->op_log->readCounts[2] == 1) {
        printf("Yay\n");
      }

    // Test: replace differing tags in same set
      readByte(cache, 26, &value);
      readByte(cache, 25, &value);
      readByte(cache, 9, &value); // these indices are 25/26 >> 2 and 9 >> 2
      if (mem->op_log->readCounts[6] == 1 && mem->op_log->readCounts[2] == 2) {
        printf("Yipee\n");
      }

    // Test: returns correct values
      uint8_t value2 = 0;
      uint8_t value3 = 0;
      uint8_t value4 = 0;
      readByte(cache, 16, &value);
      readByte(cache, 17, &value2);
      readByte(cache, 18, &value3);
      readByte(cache, 19, &value4);
      if(mem->op_log-> readCounts[4] == 1 && value == 0x65 && value2 == 0x87 && value3 == 0x78 && value4 == 0x56) {
        printf("Wahoo\n");
      }
}
