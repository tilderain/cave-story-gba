#include "common.h"

#include "sram.h"

#include "gba.h"

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
    return *(volatile uint8_t*)(SRAM + (offset));
}

void SRAM_writeByte(uint32_t offset, uint8_t val)
{
    *(volatile uint8_t*)(SRAM + (offset)) = val;
}

//GBATODO
uint16_t SRAM_readWord(uint32_t offset)
{ 
	return *(volatile uint8_t*)(SRAM + (offset)); 
}
uint32_t SRAM_readLong(uint32_t offset) 
{ 
	return *(volatile uint8_t*)(SRAM + (offset)); 
}
void SRAM_writeWord(uint32_t offset, uint16_t val) 
{
    *(volatile uint8_t*)(SRAM + (offset)) = val;
}
void SRAM_writeLong(uint32_t offset, uint32_t val) 
{
    *(volatile uint8_t*)(SRAM + (offset)) = val;
}
