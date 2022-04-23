
@@ -0,0 +1,126 @@
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "fa_cache.h"
#include "main_mem.h"
#include "main_mem_log.h"

int main(){
  uint32_t word_index_bits = 1;
  uint32_t num_cache_lines = 4;

  MainMem *mem = createMainMem(6);
  // Test: Abides by constraints
  // KMP solution won't actually pass this, may go back and add that for posterity -Ryan
    MainMem *mem2 = createMainMem(3);
    FACache* cache2 = createFACache(mem2, word_index_bits, num_cache_lines);
    if (cache2 == NULL) {
      printf("Cowabunga\n");
    }

  // Load up the main men with known contents
  writeWord(mem, 0, 0x00000000);
  writeWord(mem, 4, 0x11111111);
  writeWord(mem, 8, 0x22222222);
  writeWord(mem, 12, 0x12341234);
  writeWord(mem, 16, 0x56785678);
  writeWord(mem, 20, 0x99889988);
  writeWord(mem, 24, 0x66666666);
  writeWord(mem, 28, 0x77777777);
  writeWord(mem, 32, 0x85493741);
  writeWord(mem, 36, 0x99999999);

  clearLog(mem->op_log);

  FACache* cache = createFACache(mem, word_index_bits, num_cache_lines);
  if (cache == NULL) {
    printf("Cache unsuccessfully created.");
    exit(-1);
  }

  // Test: not constantly read from mem
    uint8_t value = 0;
    for (uint32_t i=0; i<15; i++) {
      readByte(cache, 9, &value);
    }
    if (mem->op_log->readCounts[2] == 1) {
      printf("Radical\n");
    }

  // Test: replacement policy
    readByte(cache, 4, &value);
    readByte(cache, 28, &value);
    readByte(cache, 20, &value);
    readByte(cache, 32, &value);
    // cache is full, goes back to memory for address 9
    //   again, index for op_log is 9 >> 2
    readByte(cache, 9, &value);
    if (mem->op_log->readCounts[2] == 2) {
      printf("Gnarly\n");
    }

  // Test: returns correct values
    uint8_t value2 = 0;
    uint8_t value3 = 0;
    uint8_t value4 = 0;
    readByte(cache, 32, &value);
    readByte(cache, 33, &value2);
    readByte(cache, 34, &value3);
    readByte(cache, 35, &value4);
    if(mem->op_log->readCounts[8] == 1 && value == 0x41 && value2 == 0x37
            && value3 == 0x49 && value4 == 0x85) {
      printf("Spectacular\n");
    }
    // 0x56781234 is the new, complete word
    writeByte(cache, 32, 0x34);
    writeByte(cache, 33, 0x12);
    writeByte(cache, 34, 0x78);
    writeByte(cache, 35, 0x56);
    // Pull it straight from memory to check
    // that it's actually being written through
    uint32_t pineapple = 0;
    readWord(mem, 32, &pineapple);
    uint32_t val1 =  pineapple & 0x000000FF;
    uint32_t val2 =  (pineapple & 0x0000FF00) >> 8;
    uint32_t val3 =  (pineapple & 0x00FF0000) >> 16;
    uint32_t val4 =  (pineapple & 0xFF000000) >> 24;
    if(mem->op_log->writeCounts[32 >> 2] == 4 && val1 == 0x34 && val2 == 0x12
            && val3 == 0x78 && val4 == 0x56) {
      readByte(cache, 32, &value);
      readByte(cache, 33, &value2);
      readByte(cache, 34, &value3);
      readByte(cache, 35, &value4);
      // readCount is actually 2 here as opposed to 1 because
      // we had to read straight from memory to check its value
      if(mem->op_log->readCounts[32>>2] == 2 && value == 0x34 && value2 == 0x12
              && value3 == 0x78 && value4 == 0x56) {
        printf("Wondrous\n");
      }
    }

    // Recreate cache to start fresh.
    clearLog(mem->op_log);
    cache = createFACache(mem, word_index_bits, num_cache_lines);
    readByte(cache, 0, &value);
    readByte(cache, 8, &value);
    readByte(cache, 16, &value);
    readByte(cache, 24, &value);
    // Words 0-7 of the mem should have read count of 1 and now fill all cache lines.
    for (uint32_t i=0; i<1000; i++) {
        for (uint32_t j=0; j<64; j++) {
            // Go through memory but read byte 9 and 25 every time also to update LRU
            // on those blocks to prevent them from being ousted.
            readByte(cache, 9, &value);
            readByte(cache, 25, &value);
            readByte(cache, j, &value);
        }
    }
    if (mem->op_log->readCounts[2] == 1 &&
            mem->op_log->readCounts[3] == 1 &&
            mem->op_log->readCounts[6] == 1 &&
            mem->op_log->readCounts[7] == 1 &&
            mem->op_log->readCounts[0] > 900 &&
            mem->op_log->readCounts[15] > 900) {
        printf("BatChest\n");
    }
}
