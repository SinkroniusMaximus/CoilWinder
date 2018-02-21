#include "HMI_Global.h"
int __attribute__ ((space(eedata))) eeData[25] = {0};

int ReadEEPROM(unsigned int ee_addr_offset){
    
    unsigned int data, offset;
    
    TBLPAG = __builtin_tblpage(&eeData);
    offset = __builtin_tbloffset(&eeData);
    data = __builtin_tblrdl(offset + ee_addr_offset);
    return (data);
}

void WriteEEPROM(unsigned int ee_addr_offset, unsigned int ee_data){
    unsigned int offset;
    /* Do single word erase first */
    NVMCON = 0x4058; // Set up NVMCON to erase one word of data EEPROM
    TBLPAG = __builtin_tblpage(&eeData);
    offset = __builtin_tbloffset(&eeData) + ee_addr_offset;
    __builtin_tblwtl(offset, 0);
    asm volatile ("disi #5"); // disable interrupts for 5 instructions
    __builtin_write_NVM();
    while(NVMCONbits.WR);
    
    /* Now write a single word to the erased location */
    NVMCON = 0x4004; // Set up NVMCON to write one word of data EEPROM
    TBLPAG = __builtin_tblpage(&eeData);
    offset = __builtin_tbloffset(&eeData) + ee_addr_offset;
    __builtin_tblwtl(offset, ee_data);
    asm volatile ("disi #5"); // disable interrupts for 5 instructions
    __builtin_write_NVM();
    while(NVMCONbits.WR);
}