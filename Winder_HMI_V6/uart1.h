
#ifndef _UART1_H
#define _UART1_H

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

void UART1_Initialize(void);
void UART1_SetRxInterruptHandler(void* handler);
void UART1_Receive_ISR(void);
void ReadHMIBuffer(uint8_t UARTBuffer[], uint8_t* HMI_Timerexpr, uint16_t* Param_coil_length, uint16_t* Param_Wire_diameter, uint16_t* Param_Wire_offset, uint16_t* Param_Nr_of_turns, uint16_t* HMI_Pagenr, uint16_t* numBytes, bool* Read_busy, bool* HMI_Button_Reset_Run, bool* HMI_Button_Reset_Alarm);
void HmiUartComm(int PV_RPM, int Progress_percentage, int Actual_windings, int Runstatus, uint16_t Stat_nr_of_layers, uint16_t Stat_dc_resistance, uint16_t Stat_wire_length, uint8_t HMI_Pagenr, uint16_t* HMI_Clock, uint16_t* Temperature_T1, uint16_t* Temperature_T2, uint16_t* Temperature_T3, uint16_t* Temperature_T4, uint16_t* Temperature_T5, uint16_t* Temperature_Overall, uint16_t* Velocity_Reduction, uint8_t* HMI_Timerexpr, bool* Read_busy);
#endif  // _UART1_H

/*
  End of File
*/

