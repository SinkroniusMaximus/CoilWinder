#ifndef HMI_GLOBAL_H
#define	HMI_GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <p24f16kl402.h>
#include <stdint.h>
#include <stdbool.h>
#include <xc.h>
#include <string.h>
#include "Nextion.h"
#include "uart1.h"
#include "SPI.h"
extern uint8_t UARTBuffer[10];
extern uint8_t SPI_RECV_Buffer[10];
extern uint8_t SPI_SEND_Buffer[10];
extern int Memorybuffer[20];
//input tags
extern uint16_t Temperature_T1;
extern uint16_t Temperature_T2;
extern uint16_t Temperature_T3;
extern uint16_t Temperature_T4;
extern uint16_t Temperature_T5;
extern uint16_t Temperature_Overall;
extern uint16_t Velocity_Reduction;
//screen tags
extern uint16_t Stat_alarm;
extern uint32_t counter;
extern bool HMI_Button_Reset_Run;
extern bool HMI_Button_Reset_Alarm;
extern uint16_t Param_coil_length;
extern uint16_t Param_Wire_diameter;
extern uint16_t Param_Wire_offset;
extern uint16_t Param_Nr_of_turns;
extern uint16_t Stat_nr_of_layers;
extern uint16_t Stat_dc_resistance;
extern uint16_t Stat_wire_length;
extern uint16_t HMI_Pagenr;
extern uint16_t dummy;
extern uint16_t reccounter;
extern uint16_t numBytes;
//internal
extern uint16_t Calc_Stepper_Stepsize;
//esp32 tags
extern int AI_Tension;
extern int AI_Tension_old;
extern int PulsesPerSecond;
extern int PV_RPM;
extern int PV_RPM_old;
extern int SP_RPM;
extern int SP_RPM_old;
extern int Progress_percentage;
extern int Actual_windings;
extern int Runstatus;
extern bool Endswitch_front;
extern bool Endswitch_rear;
extern int StatusBits;
extern bool Running;
extern bool Ready;
extern bool Faulted;
extern bool kickstart_direction;
extern bool requested_position;
extern bool Read_busy;
extern bool SPI_Master;

void Pin_init();
void SPI_Init();
void INTERRUPT_Initialize (void);
void delay(uint16_t delay_length);
unsigned short Read_SPI();
void Write_SPI(unsigned short data);
void Ack_SPI(void);
#endif	/* HMI_GLOBAL_H */

