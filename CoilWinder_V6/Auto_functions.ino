void Debug(void)
{
//        Serial.print(SPI_Ack);
//        Serial.print("  ");
//        Serial.print(SPI_AckReq);
//        Serial.print("  ");
//        Serial.print(SPI_AckReq_Old);
//        Serial.print("  ");
//        Serial.print(Ready);
//        Serial.print("  ");
//        Serial.print(Running);      
//        Serial.print("  ");
        Serial.print(Resetrun);  
        Serial.print("  ");
        Serial.print(Param_Nr_of_turns);
        Serial.print("  ");
        Serial.print(Param_coil_length);
        Serial.print("  ");
        Serial.print(Calc_Stepper_Stepsize);   
      Serial.print("  ");
      Serial.println(Actual_windings);
//      Serial.print("  ");
//      Serial.print(distance);
//      Serial.print("  ");
//      Serial.print(position_error);      
//      Serial.print("  ");
//      Serial.print((Actual_position/6)/6);
//        Serial.print("  ");
////        Serial.print(PV_RPM_Bufferexpr);
////        Serial.print("  ");
////        Serial.print(PV_RPM_New);
//        Serial.print("  ");
//        Serial.print(PV_RPM);
//      Serial.print("  ");
//      Serial.print(SP_RPM);
//      Serial.print("  ");
//      Serial.print(TurnAroundBrake);
//      Serial.print("  ");
//      Serial.print(In_position);
//      Serial.print("  ");
//      Serial.print(Actual_direction);
//      Serial.print("  ");
//      Serial.print(kickstart);
//      Serial.print("  ");
//      Serial.print(SP_Status);
//      Serial.print("  ");
//      Serial.print(Passive_stop);
//      Serial.print("  ");
//      Serial.print(Active_stop);
//      Serial.print("  ");
//      Serial.print(PulseOutput[0]);
//      Serial.print(PulseOutput[1]);
//      Serial.print(PulseOutput[2]);
//      Serial.print(PulseOutput[3]);
//      Serial.print(PulseOutput[4]);
//      Serial.print("  ");
//      Serial.print(I_Hallstate[0]);
//      Serial.print(I_Hallstate[1]);
//      Serial.print(I_Hallstate[2]);
//      Serial.print(I_Hallstate[3]);
//      Serial.println(I_Hallstate[4]);
//
//              Serial.print(PIC_Timerexpr); 
//              Serial.print("  ");
//              Serial.print(SPI_send_value); 
//              Serial.print("  ");
//              Serial.print(SPI_Recv_Buffer[0]); 
//              Serial.print("  ");
//              Serial.print(SPI_Recv_Buffer[1]); 
//              Serial.print("  ");
//              Serial.print(SPI_Recv_Buffer[2]); 
//              Serial.print("  ");
//              Serial.print(SPI_Recv_Buffer[3]); 
//              Serial.print("  ");
//              Serial.print(SPI_Recv_Buffer[4]); 
//              Serial.print("  ");
//              Serial.print(SPI_Recv_Buffer[5]); 
//              Serial.print("  ");
//              Serial.print(SPI_Recv_Buffer[6]); 
//              Serial.print("  ");
//              Serial.print(SPI_Recv_Buffer[7]); 
//              Serial.print("  ");
//              Serial.print(SPI_Recv_Buffer[8]); 
//              Serial.print("  ");
//              Serial.println(SPI_Recv_Buffer[9]); 
}

void CalcRPM(uint16_t fieldwidth, float* PV_RPM)
{
    if((millis()-t)>500) 
    {
    *PV_RPM=0;
    }
    else 
    {
      *PV_RPM=(60000.0/(((float)fieldwidth)/66.72))/6.0;
    }
}

void Ringbuffer(float Buffer[], int Bufferlength,float ActualValue, uint16_t* Bufferexpr, float* Average)
{
  long Sum = 0.0;
  Buffer[*Bufferexpr]=ActualValue;
  *Bufferexpr=*Bufferexpr +1;
  if (*Bufferexpr >= Bufferlength)
  {
    *Bufferexpr = 0;
  }
  
  for (int i = 0; i < Bufferlength; i++)
  {
    Sum = Sum + Buffer[i];
  }
  *Average=(float)(Sum / (long)Bufferlength); 
}


