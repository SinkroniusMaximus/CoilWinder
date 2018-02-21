#include "HMI_Global.h"
 
   
        
void EspSpiComm(uint8_t SPI_SEND_Buffer[], int StatusBits, int Param_Nr_of_turns, int Param_coil_length, int Calc_Stepper_Stepsize, bool ESP_SPI_ACK, int* Actual_windings, int* PV_RPM, int* Progress_percentage, int* AI_Tension, uint16_t* SPI_Clock, uint8_t* SPI_timeout, uint8_t* ESP_Timerexpr, uint8_t* SPI_Send, bool* ESP_SPI_ACK_Old, bool* ESP_SPI_ACKREQ)
{
    *SPI_Clock=*SPI_Clock+1;
    if (*SPI_Clock > 100)
    {
        SPI_SEND_Buffer[0]=0xFB;
        SPI_SEND_Buffer[1]=(uint8_t)(StatusBits >> 8);
        SPI_SEND_Buffer[2]=(uint8_t)(StatusBits);
        SPI_SEND_Buffer[3]=(uint8_t)(Param_Nr_of_turns >> 8);
        SPI_SEND_Buffer[4]=(uint8_t)(Param_Nr_of_turns);
        SPI_SEND_Buffer[5]=(uint8_t)(Param_coil_length >> 8);
        SPI_SEND_Buffer[6]=(uint8_t)(Param_coil_length);
        SPI_SEND_Buffer[7]=(uint8_t)(Calc_Stepper_Stepsize >> 8);
        SPI_SEND_Buffer[8]=(uint8_t)(Calc_Stepper_Stepsize);
        SPI_SEND_Buffer[9]=0xFF;
        *SPI_timeout=*SPI_timeout+1;
        if ((ESP_SPI_ACK != *ESP_SPI_ACK_Old)|(*SPI_timeout > 10))
        {
            if (*ESP_Timerexpr<10)
            {
                SPI_RECV_Buffer[*ESP_Timerexpr]=Read_SPI();
                delay(20);
                *SPI_Send=SPI_SEND_Buffer[*ESP_Timerexpr];
            }
            if (SPI_RECV_Buffer[9]==0xFE)
            {
                    *Actual_windings=SPI_RECV_Buffer[1]* 256 + SPI_RECV_Buffer[2];  
                    *PV_RPM=SPI_RECV_Buffer[3]* 256 + SPI_RECV_Buffer[4]; 
                    *Progress_percentage=SPI_RECV_Buffer[5]* 256 + SPI_RECV_Buffer[6];
                    *AI_Tension=SPI_RECV_Buffer[7]* 256 + SPI_RECV_Buffer[8];  
                    SPI_RECV_Buffer[0]=0;
                    SPI_RECV_Buffer[1]=0;
                    SPI_RECV_Buffer[2]=0;
                    SPI_RECV_Buffer[3]=0;
                    SPI_RECV_Buffer[4]=0;
                    SPI_RECV_Buffer[5]=0;
                    SPI_RECV_Buffer[6]=0;
                    SPI_RECV_Buffer[7]=0;
                    SPI_RECV_Buffer[8]=0;
                    SPI_RECV_Buffer[9]=0;
            }      
            if (SPI_RECV_Buffer[0]!=0xFA)
            {
                *ESP_Timerexpr=0;
            }
            else
            {
                *ESP_Timerexpr=*ESP_Timerexpr+1;                    
            }

            if (*ESP_Timerexpr >= 10)
            {
                *ESP_Timerexpr = 0;
            }

            *ESP_SPI_ACKREQ=!*ESP_SPI_ACKREQ;
            *ESP_SPI_ACK_Old=ESP_SPI_ACK;      
            *SPI_timeout=0;
        }
        Write_SPI(SPI_SEND_Buffer[*ESP_Timerexpr]);
        *SPI_Clock=0;
    }
}