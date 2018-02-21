void SPI_Comm2(uint8_t SPI_Send_Buffer[], float SP_RPM, float PV_RPM,int Actual_windings, int Progress_percentage, int AI_Tension, bool* SPI_Ack, 
               bool SPI_AckReq, bool* SPI_AckReq_Old, bool* Running, bool* Ready, bool* Start_positioning, bool* Resetrun, uint16_t* PIC_Timerexpr, uint8_t* SPI_send_value,
               uint16_t* Param_Nr_of_turns, uint16_t* Param_coil_length, uint16_t* Calc_Stepper_Stepsize)
{
          int StatusBits;
          int expr;
          SPI_Send_Buffer[0]=0xFA;
          SPI_Send_Buffer[1]=highByte((int)position_error_abs);
          SPI_Send_Buffer[2]=lowByte((int)position_error_abs);
          SPI_Send_Buffer[3]=highByte((int)PV_RPM);
          SPI_Send_Buffer[4]=lowByte((int)PV_RPM);
          SPI_Send_Buffer[5]=highByte(Progress_percentage);
          SPI_Send_Buffer[6]=lowByte(Progress_percentage);
          SPI_Send_Buffer[7]=highByte(AI_Tension);
          SPI_Send_Buffer[8]=lowByte(AI_Tension);
          SPI_Send_Buffer[9]=0xFE;
          
          //
        if ((SPI_AckReq != *SPI_AckReq_Old))
        {
          if (*PIC_Timerexpr < 10)
          {
              SPI_Recv_Buffer[*PIC_Timerexpr] =(uint8_t)SPI.transfer(0);
              *SPI_send_value=SPI_Send_Buffer[*PIC_Timerexpr];
          }
            if (SPI_Recv_Buffer[9] == 0xFF)
            {
              StatusBits=SPI_Recv_Buffer[1]* 256 + SPI_Recv_Buffer[2];
              *Running = (bool)(StatusBits & 0x01);
              *Ready=(bool)((StatusBits >> 1) & 0x01);
              *Start_positioning=(bool)((StatusBits >> 2) & 0x01);
              *Resetrun=(bool)((StatusBits >> 3) & 0x01);
              *Param_Nr_of_turns=SPI_Recv_Buffer[3]* 256 + SPI_Recv_Buffer[4]; 
              *Param_coil_length=SPI_Recv_Buffer[5]* 256 + SPI_Recv_Buffer[6];
              *Calc_Stepper_Stepsize=SPI_Recv_Buffer[7]* 256 + SPI_Recv_Buffer[8];  
              SPI_Recv_Buffer[0]=0;
              SPI_Recv_Buffer[1]=0;
              SPI_Recv_Buffer[2]=0;
              SPI_Recv_Buffer[3]=0;
              SPI_Recv_Buffer[4]=0;
              SPI_Recv_Buffer[5]=0;
              SPI_Recv_Buffer[6]=0;
              SPI_Recv_Buffer[7]=0;
              SPI_Recv_Buffer[8]=0;
              SPI_Recv_Buffer[9]=0;
            }       
            if (SPI_Recv_Buffer[0] != 0xFB)
            {       
              *PIC_Timerexpr=0;
            }
            else
            {
              *PIC_Timerexpr=*PIC_Timerexpr+1;
            }
            if (*PIC_Timerexpr >= 10)
            {
                *PIC_Timerexpr = 0;
            }
            *SPI_Ack=!*SPI_Ack;
            *SPI_AckReq_Old=SPI_AckReq;            
        }
        SPI.write(*SPI_send_value);
}
