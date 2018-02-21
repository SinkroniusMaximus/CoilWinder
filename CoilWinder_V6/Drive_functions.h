//Drive_functions.h
#ifndef DRIVE_FUNCTIONS_H
#define  DRIVE_FUNCTIONS_H
  void calcfieldwidth(uint16_t* fieldwidthcounter, bool* SensorState, bool SensorState_old, uint16_t* fieldwidth);
  void calcPulsewidth(uint16_t* fieldwidth, bool* SensorState, bool* SensorState_old, uint16_t* Scalingfactor, uint32_t* Pulsewidth);
  void CoilMath(uint32_t* Pulsewidth, bool SensorState, uint16_t* Pulse_expr, bool* PulseOutput, bool* PulseDone);
  void RunStatus(bool HallState[],bool HallStateOld[],uint32_t* StandstillCounter , bool* Actual_direction, bool* Standstill,long* Actual_position);
  void Select_SP(bool Running, bool Automatic,bool Active_stop, bool Passive_stop, bool In_position, float Manual_speed, float Auto_speed, float PV_RPM,
                 uint8_t* SP_Status, float* SP_RPM, bool* TurnAroundBrake,bool* kickstart, bool* BrakeOutput, uint16_t* Scalingfactor);

  void Kickstart_calc(bool I_Hallstate[], bool BrakeOutput, bool Running, bool Standstill, bool Actual_direction, 
                    bool kickstart_direction, uint32_t kickstart_factor, float SP_RPM, bool* kickstart, bool* P_Running, 
                    float* SP_RPM_old, bool* ksnumber_found,uint8_t* kickstartnumber, uint32_t* kickstart_timer, uint32_t* kickstart_done_timer);
  void Kickstart(bool I_Hallstate[], bool kickstart_range[], bool PulseDone[],uint16_t Pulse_expr[], bool kickstart,
               uint8_t kickstartnumber, uint32_t kickstart_factor, bool* ksnumber_found,uint32_t* kickstart_timer);
  void ActiveStop(bool PulseOutput[],bool I_Hallstate[], bool kickstart_direction, uint32_t kickstart_factor, bool *Active_stop, uint32_t* Active_stop_timer);
  void Positioning(bool Running, bool Resetrun, bool Start_positioning,bool kickstart, uint16_t Manual_speed, uint16_t Param_Nr_of_turns, long Position_band, 
                    float PV_RPM, bool* Passive_stop, bool* Active_stop, bool* P_Start_positioning, bool* In_position, bool* kickstart_direction, long* Actual_position, long* position_error, 
                    long* Requested_Nr_of_Turns, long* Requested_position,long* distance, long* position_error_abs,long* position_speed, float* Auto_speed);
#endif  //DRIVE_FUNCTIONS_H


