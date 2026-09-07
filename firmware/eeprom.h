#ifndef EEPROM_H
#define EEPROM_H

#include <stdint.h>

void EEPROM_Write(uint8_t addr, uint8_t data);
uint8_t EEPROM_Read(uint8_t addr);

#endif // EEPROM_H
