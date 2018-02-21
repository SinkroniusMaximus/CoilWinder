#ifndef EEPROM_H
#define	EEPROM_H

int ReadEEPROM(unsigned int ee_addr_offset);
void WriteEEPROM(unsigned int ee_addr_offset, unsigned int ee_data);

#endif	/* EEPROM_H */

