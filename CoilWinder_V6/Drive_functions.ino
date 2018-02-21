void calcfieldwidth(uint16_t* fieldwidthcounter, bool* SensorState, bool SensorState_old, uint16_t* fieldwidth)
{
    if(*SensorState)
    {
        if (*fieldwidthcounter < 60000)
        {
            *fieldwidthcounter=*fieldwidthcounter+1;
        }
    }
    else if (*fieldwidthcounter > 2)
    {
        *fieldwidth=*fieldwidthcounter;
        *fieldwidthcounter=0;
    }
}

void calcPulsewidth(uint16_t* fieldwidth, bool* SensorState, bool* SensorState_old, uint16_t* Scalingfactor, uint32_t* Pulsewidth)
{
    uint32_t temp_1;
    uint32_t temp_2;
    uint32_t temp_3;
    if (!*SensorState & !*SensorState_old)
    {
        temp_1= *fieldwidth * 2;
        temp_2= temp_1 * (uint32_t)*Scalingfactor;
        temp_3 = (temp_2/4096);
        *Pulsewidth = temp_3;
        t=millis();
        *SensorState_old = true;
        }
    if (*SensorState)
    {
        *SensorState_old= false;
    }
    
}

void CoilMath(uint32_t* Pulsewidth, bool SensorState, uint16_t* Pulse_expr, bool* PulseOutput, bool* PulseDone)
{
   if(!SensorState & *Pulse_expr == 0)
    {
        *Pulse_expr = *Pulse_expr + 1;
        *PulseDone =false;
    }   
    if (*Pulse_expr > 0 & *Pulse_expr < *Pulsewidth)      
    {
        *PulseOutput=true;
        *Pulse_expr = *Pulse_expr + 1;
    } 
    else
    {
        *PulseOutput =false;
        *PulseDone = true;
    }
    if (SensorState & *PulseDone)
    {
        *Pulse_expr= 0;
    }
}

void RunStatus(bool HallState[],bool HallStateOld[], uint32_t* StandstillCounter, bool* Actual_direction, bool* Standstill, long* Actual_position)
{
  uint8_t Hallold;
  uint8_t Hallnew;
  uint8_t Halloff=99;
  uint8_t Hallon=99;
  uint8_t HallCore=99;
  int HallDiff=0;

  if (*StandstillCounter < 5000)
  {
      *StandstillCounter=*StandstillCounter+1;
  }
  else
  {
    *Standstill=true;
  }
  for (uint8_t i=0;i<5;i++)
  {
    if (HallState[i] != HallStateOld[i])
    {
      *Standstill=false;
      *StandstillCounter=0;
    }
//    Hallold=*HallStateOld[i] << i;
//    Hallnew=HallState[i] << i;
    if (!HallStateOld[i]&HallState[i])
    {
      Halloff=i;
    }
    if (HallStateOld[i]&!HallState[i])
    {
      Hallon=i;
    }
    if (HallStateOld[i]&HallState[i])
    {
      HallCore=i;
    }
    HallStateOld[i]=HallState[i];
  }
  if (HallCore != 99)
  {
    if ((Halloff != 99)&(Hallon == 99))
    {
      HallDiff=(int)HallCore-(int)Halloff;
      
      if (HallDiff < 2 & HallDiff > -2)
      {
        if (HallDiff < 0)
        {
          *Actual_direction=true;
        }
        else if (HallDiff > 0)
        {
          *Actual_direction=false;
        }
      }
    }
    else if (Hallon != 99)
    {
      HallDiff=(int)HallCore-(int)Hallon;
      
      if (HallDiff < 2 & HallDiff > -2)
      {
        if (HallDiff > 0)
        {
          *Actual_direction=true;
        }
        else if (HallDiff < 0)
        {
          *Actual_direction=false;
        }
      }  
    }
    if (Hallon != 99)
    {
        if (*Actual_direction)
        {
          *Actual_position=*Actual_position+1;
        }
        else
        {
          *Actual_position=*Actual_position-1;
        }
    }
  }
}
void Select_SP(bool Running, bool Automatic,bool Active_stop, bool Passive_stop, bool In_position, float Manual_speed, float Auto_speed,float PV_RPM,uint8_t* SP_Status,float* SP_RPM, bool* TurnAroundBrake,bool* kickstart, bool* BrakeOutput, uint16_t* Scalingfactor)
{ 
    if (Automatic)
    {
      *SP_RPM=Auto_speed;
    }
    else
    {
      *SP_RPM=Manual_speed;
    }
    if (Running)
    {
      if (Passive_stop)
      {
        if (*SP_RPM < 0)
        {
        *Scalingfactor=*SP_RPM;
        }
        else
        {
         *Scalingfactor=0; 
        }
        *BrakeOutput=false;  
      }
      else if ((!*TurnAroundBrake)&((!In_position)|(Active_stop)))
      {
        *Scalingfactor=*SP_RPM;
        *BrakeOutput=true; 
        *SP_Status=2;
      }
      else if ((*TurnAroundBrake)&((!In_position)|(*kickstart)))
      {
        *Scalingfactor= 4000;
        *BrakeOutput=false;    
        *SP_Status=3;
        if (PV_RPM < 10)
        {
          *kickstart=true;
          *TurnAroundBrake=false;
        }
      }
      else if (In_position)
      {
        *Scalingfactor= 4000;
        *BrakeOutput=false; 
        *SP_Status=4;
      }
    }
    else if (PV_RPM > 0)
    {
      *Scalingfactor= 4000;
      *BrakeOutput=false;
      *SP_Status=5;
    }
    else
    {
      *Scalingfactor= 0;
      *BrakeOutput=false;
      *SP_Status=6;
    }

}

void Kickstart_calc(bool I_Hallstate[5], bool BrakeOutput, bool Running, bool Standstill, bool Actual_direction, 
                    bool kickstart_direction, uint32_t kickstart_factor, float SP_RPM, bool* kickstart, bool* P_Running, 
                    float* SP_RPM_old, bool* ksnumber_found,uint8_t* kickstartnumber, uint32_t* kickstart_timer, uint32_t* kickstart_done_timer)
{
    *ksnumber_found=false;
    *kickstartnumber=99;
      for(int i = 0; i < 5; i++)
      {
        if (kickstart_direction)
        {
          if (!I_Hallstate[i] & !*ksnumber_found)
          {
            *kickstartnumber=i;
          }
          if (((I_Hallstate[i] & !*ksnumber_found)& *kickstartnumber == i-1)| (i == 4))
          {
            *ksnumber_found=true;
          }
        }
        else
         {
            if (!I_Hallstate[4-i] & !*ksnumber_found)
            {
              *kickstartnumber=4-i;
            }
            if (((I_Hallstate[4-i] & !*ksnumber_found)& *kickstartnumber == (4-i)+1)| (i==4))
            {
              *ksnumber_found=true;
            }
         }
      } 
     if (*kickstart_timer > (1000*kickstart_factor))//50000)
      { 
        *kickstart_timer=0;  
      }
     if (!Standstill & (Actual_direction == kickstart_direction))
      {
        if (*kickstart_done_timer < 100000)
        {
          *kickstart_done_timer=*kickstart_done_timer+1;
        }        
        else
        {
          *kickstart=false; 
          *kickstart_done_timer=0;
        }
      }
      else
      {
        *kickstart_done_timer=0;
      }

    if (((SP_RPM>1.0 & *SP_RPM_old < 1.0 & PV_RPM < 50) & BrakeOutput)|(Running & !*P_Running))
    {
      *kickstart = true;
      *P_Running=true;
    }
    if (*P_Running & !Running)
    {
      *P_Running=false;
    }
        *SP_RPM_old=SP_RPM;
}

void Kickstart(bool I_Hallstate[], bool kickstart_range[], bool PulseDone[],uint16_t Pulse_expr[], bool kickstart,
               uint8_t kickstartnumber, uint32_t kickstart_factor, bool* ksnumber_found,uint32_t* kickstart_timer)
{
    uint32_t kickstartmodulo;
    if (kickstart)
    {
      *ksnumber_found=false;
      *kickstart_timer=*kickstart_timer+1;
      kickstartmodulo= *kickstart_timer % (1000*kickstart_factor);
      kickstart_range[0] = ((kickstartmodulo > (1*kickstart_factor))& ( kickstartmodulo < (100*kickstart_factor)));
      kickstart_range[1] = ((kickstartmodulo > (200*kickstart_factor))& ( kickstartmodulo < (300*kickstart_factor)));
      kickstart_range[2] = ((kickstartmodulo > (400*kickstart_factor))& ( kickstartmodulo < (500*kickstart_factor)));
      kickstart_range[3] = ((kickstartmodulo > (600*kickstart_factor))& ( kickstartmodulo < (700*kickstart_factor)));
      kickstart_range[4] = ((kickstartmodulo > (800*kickstart_factor))& ( kickstartmodulo < (900*kickstart_factor)));
      
      if (kickstart_direction)
      {
      PulseOutput[0]=((!I_Hallstate[0])&((kickstart_range[0]&kickstartnumber==0)));
      PulseOutput[1]=((!I_Hallstate[3])&((kickstart_range[3]&kickstartnumber==3)));
      PulseOutput[2]=((!I_Hallstate[1])&((kickstart_range[1]&kickstartnumber==1)));
      PulseOutput[3]=((!I_Hallstate[4])&((kickstart_range[4]&kickstartnumber==4)));
      PulseOutput[4]=((!I_Hallstate[2])&((kickstart_range[2]&kickstartnumber==2)));
      }
      else
      {
      PulseOutput[0]=((!I_Hallstate[0])&((kickstart_range[2]&kickstartnumber==0)));
      PulseOutput[1]=((!I_Hallstate[3])&((kickstart_range[4]&kickstartnumber==3)));
      PulseOutput[2]=((!I_Hallstate[1])&((kickstart_range[1]&kickstartnumber==1)));
      PulseOutput[3]=((!I_Hallstate[4])&((kickstart_range[3]&kickstartnumber==4)));
      PulseOutput[4]=((!I_Hallstate[2])&((kickstart_range[0]&kickstartnumber==2)));
      }
      Pulse_expr[0]=0;
      Pulse_expr[1]=0;
      Pulse_expr[2]=0;
      Pulse_expr[3]=0;
      Pulse_expr[4]=0;
      PulseDone[0]=false;
      PulseDone[1]=false;
      PulseDone[2]=false;
      PulseDone[3]=false;
      PulseDone[4]=false; 
    }
}
void ActiveStop(bool PulseOutput[], uint32_t kickstart_factor, bool *Active_stop, uint32_t* Active_stop_timer)
{
  bool Active_stop_on;
  uint32_t Active_stop_modulo;
  if (*Active_stop)
  {
      *Active_stop_timer=*Active_stop_timer+1;
      Active_stop_modulo= *Active_stop_timer % (600*kickstart_factor);
      if (*Active_stop_timer > (100000*kickstart_factor))
      {
        *Active_stop_timer=0;
      }
      Active_stop_on=(Active_stop_modulo <(180*kickstart_factor));
      PulseOutput[0]=Active_stop_on;
      PulseOutput[1]=Active_stop_on;
      PulseOutput[2]=Active_stop_on;
      PulseOutput[3]=Active_stop_on;
      PulseOutput[4]=Active_stop_on;
  }
  else
  {
    *Active_stop_timer=0;
  }
}

void ActiveStop(bool PulseOutput[],bool I_Hallstate[], bool kickstart_direction, uint32_t kickstart_factor, bool *Active_stop, uint32_t* Active_stop_timer)
{
  bool Active_stop_on;
  uint32_t Active_stop_modulo;
  if (*Active_stop)
  {
      *Active_stop_timer=*Active_stop_timer+1;
      Active_stop_modulo= *Active_stop_timer % (400*kickstart_factor);
      if (*Active_stop_timer > (100000*kickstart_factor))
      {
        *Active_stop_timer=0;
      }
      Active_stop_on=(Active_stop_modulo <(150*kickstart_factor));
      if (!kickstart_direction)
      {
        PulseOutput[0]=((!I_Hallstate[0])&(Active_stop_on));
        PulseOutput[1]=((!I_Hallstate[3])&(Active_stop_on));
        PulseOutput[2]=((!I_Hallstate[1])&(Active_stop_on));
        PulseOutput[3]=((!I_Hallstate[4])&(Active_stop_on));
        PulseOutput[4]=((!I_Hallstate[2])&(Active_stop_on));
      }
      else
      {
        PulseOutput[0]=((!I_Hallstate[0])&(Active_stop_on));
        PulseOutput[1]=((!I_Hallstate[3])&(Active_stop_on));
        PulseOutput[2]=((!I_Hallstate[1])&(Active_stop_on));
        PulseOutput[3]=((!I_Hallstate[4])&(Active_stop_on));
        PulseOutput[4]=((!I_Hallstate[2])&(Active_stop_on));
      }
  }
  else
  {
    *Active_stop_timer=0;
  }
}

//void Positioning(bool Running, bool Resetrun, bool Start_positioning,bool kickstart, uint16_t Manual_speed, uint16_t Param_Nr_of_turns, long Position_band, float PV_RPM, bool* Passive_stop, bool* Active_stop, bool* P_Start_positioning, bool* In_position, bool* kickstart_direction,long* Actual_position, long* position_error, long* Requested_Nr_of_Turns,long* Requested_position,long* distance, long* position_error_abs,long* position_speed, uint16_t* Auto_speed)
 void Positioning(bool Running, bool Resetrun, bool Start_positioning,bool kickstart, uint16_t Manual_speed, uint16_t Param_Nr_of_turns, long Position_band, 
                    float PV_RPM, bool* Passive_stop, bool* Active_stop, bool* P_Start_positioning, bool* In_position, bool* kickstart_direction, long* Actual_position, long* position_error, 
                    long* Requested_Nr_of_Turns, long* Requested_position,long* distance, long* position_error_abs,long* position_speed, float* Auto_speed)
{
      if (!Running&Resetrun)
    {
           *Actual_position=0;
           *Requested_Nr_of_Turns=Param_Nr_of_turns;
           *Requested_position = (*Requested_Nr_of_Turns*6*6)+15;
           *position_error=*Requested_position-*Actual_position;
           *distance=*position_error;
    }
    if (Start_positioning != *P_Start_positioning)
    {
      if (*In_position)
      {
       *Actual_position=0;
       *Requested_Nr_of_Turns=Param_Nr_of_turns;
       *Requested_position = (*Requested_Nr_of_Turns*6*6)+15;
       *position_error=*Requested_position-*Actual_position;
       *distance=*position_error;
      }
      *P_Start_positioning=Start_positioning;
    }
    *position_error=*Requested_position-*Actual_position;
    if (*position_error>0)
    {
      *kickstart_direction=true;
      *position_error_abs=*position_error;
    }
    else
    {
      *kickstart_direction=false;
      *position_error_abs=*position_error*-1;
    }
    *position_speed=(((*position_error_abs/position_factor)+20)-((PV_RPM-(*position_error_abs/10))/6));
    if (*position_speed < Manual_speed)
    {
      *Auto_speed=*position_speed;
      if (!*Passive_stop & *Auto_speed <0)
      {
        *Auto_speed=0;
      }
    }
    else
    {
      *Auto_speed=Manual_speed;
    }
    if ((*position_error_abs < (Position_band+200))&(*position_error_abs > (Position_band+100))&(*Auto_speed<=0))
    {
      *Passive_stop=true;
    }
    else if ((*position_error_abs < (Position_band+5))&(*position_error_abs > Position_band))
    {
      *Active_stop=true;
    }
    else if (*position_error_abs < Position_band)
    {
      *In_position=true;
    }
    else
    {
      *In_position=false;
    }
    if (!Running|kickstart|*Active_stop|(PV_RPM==0))
      {
        *Passive_stop=false;
      }
}

