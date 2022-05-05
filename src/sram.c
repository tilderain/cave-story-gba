#include "common.h"

#include "sram.h"

void SRAM_enable()
{
	return;
    *(volatile uint8_t*)SRAM_CONTROL = 1;
}

void SRAM_enableRO()
{
	return;
    *(volatile uint8_t*)SRAM_CONTROL = 3;
}

void SRAM_disable()
{
	return;
    *(volatile uint8_t*)SRAM_CONTROL = 0;
}


uint8_t SRAM_readByte(uint32_t offset)
{
	return 1;
    return *(volatile uint8_t*)(SRAM_BASE + (offset * 2));
}

void SRAM_writeByte(uint32_t offset, uint8_t val)
{
	return;
    *(volatile uint8_t*)(SRAM_BASE + (offset * 2)) = val;
}

//GBATODO
uint16_t SRAM_readWord(uint32_t offset){ return 0; }
uint32_t SRAM_readLong(uint32_t offset) { return 0; }
void SRAM_writeWord(uint32_t offset, uint16_t val) { return; }
void SRAM_writeLong(uint32_t offset, uint32_t val) { return; }
