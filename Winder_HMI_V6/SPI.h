#ifndef SPI_H
#define	SPI_H

void EspSpiComm(uint8_t SPI_SEND_Buffer[], int StatusBits, int Param_Nr_of_turns, int Param_coil_length, int Calc_Stepper_Stepsize, bool ESP_SPI_ACK, int* Actual_windings, int* PV_RPM, int* Progress_percentage, int* AI_Tension, uint16_t* SPI_Clock, uint8_t* SPI_timeout, uint8_t* ESP_Timerexpr, uint8_t* SPI_Send, bool* ESP_SPI_ACK_Old, bool* ESP_SPI_ACKREQ);

#endif	/* SPI_H */

