//Comm_functions.h
#ifndef COMM_FUNCTIONS_H
#define  COMM_FUNCTIONS_H
void SPI_Comm2(uint8_t SPI_Send_Buffer[], float SP_RPM, float PV_RPM,int Actual_windings, int Progress_percentage, int AI_Tension, bool* SPI_Ack, 
               bool SPI_AckReq, bool* SPI_AckReq_Old, bool* Running, bool* Ready, bool* Start_positioning, bool* Resetrun, uint16_t* PIC_Timerexpr, uint8_t* SPI_send_value,
               uint16_t* Param_Nr_of_turns, uint16_t* Param_coil_length, uint16_t* Calc_Stepper_Stepsize);
#endif  //COMM_FUNCTIONS_H



