
// Undefine the problematic macros from gba.h
#undef IWRAM_DATA
#undef EWRAM_DATA

// Redefine them using sub-sections. 
// The GBA linker script will still put these in the correct RAM banks
// because it looks for "*(.iwram*)" and "*(.ewram*)".
#define IWRAM_DATA __attribute__((section(".iwram.data")))
#define EWRAM_DATA __attribute__((section(".ewram.data")))
