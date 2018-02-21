#include "Auto_functions.h"
#include "Comm_functions.h"
#include "Drive_functions.h"
#include "SPI.h"
#include "Stepper_functions.h"
//globals
#define aref_voltage 3.3
uint8_t SPI_Send_Buffer[10];
uint8_t SPI_Recv_Buffer[10]={0};
float PV_RPM_Buffer[20];
uint8_t SPI_send_value = 0;
uint16_t PIC_Timerexpr;
uint16_t SPI_Return;
bool SPI_AckReq;
bool SPI_Master;
int AI_Tension = 0;
int PulsesPerSecond = 0;
uint16_t fieldwidth = 0;
uint16_t Scalingfactor;
uint32_t RPM_pulse_time;
unsigned long t=0;
unsigned long ot=0;
unsigned long tms=0;
unsigned long otms=0;
float PV_RPM;
float PV_RPM_New;
int PV_RPM_Bufferlength = (sizeof(PV_RPM_Buffer)/sizeof(float));
uint16_t PV_RPM_Bufferexpr;
//inputs
bool I_Hallstate[5] = {0};
bool HallStateOld[5] = {0};
bool I_Endswitch_front;
bool I_Endswitch_rear;
//codeForTask1
  unsigned long i, j;
  bool SensorState_old;
//  uint16_t PulseCounter_RPM = 1;
//  long TimerCalcRPM;
//codeForTask2
  uint16_t fieldwidthcounter;
  bool SensorState_old2;
  uint32_t Pulsewidth;
  uint16_t Pulse_expr[5] = {0};
  bool PulseOutput[5] = {0};
  bool BrakeOutput = 0;
  bool PulseDone[5] = {0};
  long TimerSpeedControl;
  float Ramp=10000.0;//
  float RPM_band=5.0;//rpm
  float Auto_speed;
  float Manual_speed;
  float SP_RPM=0.0;//rpm
  float SP_RPM_old=0.0;//rpm
  float OPV_RPM;
  int Progress_percentage = 25;
  bool SPI_Ack=false;
  bool ReadAnalog;
  uint16_t Temp;
  float ControlValue;
  long start;
  bool kickstart;
  bool kickstart_direction = true;
  uint32_t kickstart_timer;
  uint32_t kickstart_done_timer;
  uint8_t kickstartnumber;
  bool ksnumber_found;
  uint32_t kickstart_factor=3;
  bool kickstart_range[5];
  bool Running;
  bool P_Running;
  bool Ready;
  bool Standstill;
  uint32_t StandstillCounter;
  bool Actual_direction;
  bool TurnAroundBrake;
  bool Automatic;
  long Actual_position=0L;
//positioning
  long Requested_Nr_of_Turns; 
  long Requested_position;
  long position_error;
  long position_error_abs;
  long distance;
  bool Start_positioning;
  bool P_Start_positioning;
  long position_factor=5;
  long position_speed;
  bool In_position;
  long Position_band=20;
  int Actual_windings;
  bool Active_stop;
  bool Passive_stop;
  
  uint32_t Active_stop_timer;
  uint8_t SP_Status;

  uint16_t Param_Nr_of_turns;
  uint16_t Calc_Stepper_Stepsize;
  uint16_t Param_coil_length;
  bool SPI_AckReq_Old;
  bool Resetrun;
  bool JogStepper_Forward;
  bool JogStepper_Reverse;
  bool JogDrive_Forward;
  bool JogDrive_Reverse;
  bool StpEnOutput;
  bool StpStpOutput;
  bool StpDirOutput;
  uint16_t Step_Timeexpr;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(250000);
  delay(200);
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
//  SPI.beginTransaction(SPISettings(140000, MSBFIRST, SPI_MODE0));
  SPI.begin();
  pinMode(5, OUTPUT);//brake relay
  pinMode(17, OUTPUT);//Drive 0
  pinMode(16, OUTPUT);//Drive 1
  pinMode(4, OUTPUT);//Drive 2
  pinMode(2, OUTPUT);//Drive 3
  pinMode(15, OUTPUT);//Drive 4
  pinMode(22, OUTPUT); //PIC SPI Ack
  pinMode(21, INPUT); //PIC SPI AckReq
  pinMode(27, OUTPUT);//Stepper En
  pinMode(14, OUTPUT);//Stepper Step
  pinMode(12, OUTPUT);//Stepper Dir
  
  pinMode(36, INPUT);//Hall 0
  pinMode(39, INPUT);//Hall 1
  pinMode(32, INPUT);//Hall 2
  pinMode(33, INPUT);//Hall 3
  pinMode(34, INPUT);//Hall 4
  pinMode(35, INPUT_PULLUP);//Endswitch front
  pinMode(25, INPUT_PULLUP);//Endswitch rear
//  pinMode(26,INPUT);//Tension or potmeter
//  attachInterrupt(digitalPinToInterrupt(36), hall0, FALLING);
  TaskHandle_t Task1, Task2;
  
  xTaskCreatePinnedToCore(
    codeForTask1,
    "commTask",
    1500,
    NULL,
    1,
    &Task1,
    0);
  delay(500);  // needed to start-up task1

  xTaskCreatePinnedToCore(
    codeForTask2,
    "driveTask",
    1000,
    NULL,
    1,
    &Task2,
    1);
      SPI_Recv_Buffer[0]=1;
  SPI_Recv_Buffer[1]=2;
  SPI_Recv_Buffer[2]=3;
  SPI_Recv_Buffer[3]=4;
  SPI_Recv_Buffer[4]=5;
  SPI_Recv_Buffer[5]=6;
  SPI_Recv_Buffer[6]=7;
  SPI_Recv_Buffer[7]=8;
  SPI_Recv_Buffer[8]=9;
 SPI_Recv_Buffer[9]=10;
}

//The communication
void codeForTask1( void * parameter ) 
{
  for (;;) 
      {
        
        //input
        I_Endswitch_front=digitalRead(35);
        I_Endswitch_rear=digitalRead(25);
        SPI_AckReq=digitalRead(21);
        ReadAnalog=true;//prevent RPM interrupt to read wrong values
        AI_Tension = analogRead(26);
        ReadAnalog=false;//reenable RPM interrupt
        //throughput
//         SPI_Comm(SPI_Send_Buffer, SPI_Recv_Buffer, SP_RPM, PV_RPM, Progress_percentage, AI_Tension, &SPI_Ack, &SPI_AckReq, &SPI_Master, &Running, &Ready, &Start_positioning, &SPI_Return, &SPI_Recv_expr, &SPI_send_value);
        SPI_Comm2(SPI_Send_Buffer, SP_RPM, PV_RPM, Actual_windings, Progress_percentage, AI_Tension, &SPI_Ack, SPI_AckReq, &SPI_AckReq_Old, 
                  &Running, &Ready, &Start_positioning, &Resetrun, &PIC_Timerexpr,&SPI_send_value, &Param_Nr_of_turns,&Param_coil_length, &Calc_Stepper_Stepsize);
        delay(5);
        CalcRPM(fieldwidth, &PV_RPM_New);
        Ringbuffer(PV_RPM_Buffer, PV_RPM_Bufferlength, PV_RPM_New, &PV_RPM_Bufferexpr, &PV_RPM);
        if (distance>0)
        {     
          Progress_percentage=(int)(100.0-(100.0/distance*position_error_abs));
        }
        if ((Requested_Nr_of_Turns > 0)&(distance>0))
        {
          Actual_windings=(int)(Requested_Nr_of_Turns-(Requested_Nr_of_Turns/distance*position_error_abs));
        }
        Debug();
        //output
        digitalWrite(22, SPI_Ack);
    }
}

void codeForTask2( void * parameter ) // the driver
{  
  for (;;) {
//    long start = millis();
    //input
    I_Hallstate[0]=digitalRead(36);
    I_Hallstate[1]=digitalRead(39);
    I_Hallstate[2]=digitalRead(32);
    I_Hallstate[3]=digitalRead(33);
    I_Hallstate[4]=digitalRead(34);
    //throughput
    Manual_speed=(uint16_t)AI_Tension / 4;
    Automatic=true;
    //Stepper
//    if (Running & PV_RPM > 0)
//    {
//      
//    }
    JogStepper_Forward=Resetrun;
    StpDirOutput=true;
    if (JogStepper_Forward)
    {
      StpEnOutput=false;
      Step_Timeexpr=Step_Timeexpr+1;
      if (Step_Timeexpr < 100)
      {
        StpStpOutput=true;
      }
      else if (Step_Timeexpr > 500)
      {
        Step_Timeexpr=0;
      }
      else
      {
        StpStpOutput=false;
      }
    }
    else
    {
      StpEnOutput=true;
    }
    //Drive
    RunStatus(I_Hallstate,HallStateOld, &StandstillCounter, &Actual_direction, &Standstill, &Actual_position);
    Positioning(Running, Resetrun, Start_positioning, kickstart, Manual_speed, Param_Nr_of_turns,  Position_band, 
                     PV_RPM, &Passive_stop, &Active_stop, &P_Start_positioning, &In_position, &kickstart_direction, &Actual_position, &position_error, 
                    &Requested_Nr_of_Turns, &Requested_position,&distance, &position_error_abs,&position_speed, &Auto_speed);
                 
    Select_SP(Running, Automatic, Active_stop,Passive_stop, In_position, Manual_speed, Auto_speed, PV_RPM,&SP_Status, &SP_RPM, &TurnAroundBrake, &kickstart, 
              &BrakeOutput, &Scalingfactor);
//    ActiveStop(PulseOutput, kickstart_factor,&Active_stop, &Active_stop_timer);
    ActiveStop(PulseOutput,I_Hallstate, kickstart_direction, kickstart_factor,&Active_stop, &Active_stop_timer);
    Kickstart_calc(I_Hallstate, BrakeOutput, Running, Standstill, Actual_direction, kickstart_direction, 
                   kickstart_factor, SP_RPM, &kickstart, &P_Running, &SP_RPM_old, &ksnumber_found, 
                   &kickstartnumber, &kickstart_timer, &kickstart_done_timer);
    Kickstart(I_Hallstate, kickstart_range, PulseDone, Pulse_expr, kickstart,
              kickstartnumber, kickstart_factor, &ksnumber_found, &kickstart_timer);
    if (Active_stop&Standstill)
    {
      Active_stop=false;
    }
    if (!kickstart&!Active_stop)
    {
      if ((Actual_direction != kickstart_direction)&(!Automatic|!In_position))
      {
        TurnAroundBrake=true;
      }
      calcfieldwidth(&fieldwidthcounter, &I_Hallstate[0],SensorState_old2, &fieldwidth);
      calcPulsewidth(&fieldwidth, &I_Hallstate[0], &SensorState_old2, &Scalingfactor, &Pulsewidth);
      CoilMath(&Pulsewidth, I_Hallstate[0], &Pulse_expr[0], &PulseOutput[0], &PulseDone[0]);
      CoilMath(&Pulsewidth, I_Hallstate[3], &Pulse_expr[1], &PulseOutput[1], &PulseDone[1]);
      CoilMath(&Pulsewidth, I_Hallstate[1], &Pulse_expr[2], &PulseOutput[2], &PulseDone[2]);
      CoilMath(&Pulsewidth, I_Hallstate[4], &Pulse_expr[3], &PulseOutput[3], &PulseDone[3]);
      CoilMath(&Pulsewidth, I_Hallstate[2], &Pulse_expr[4], &PulseOutput[4], &PulseDone[4]);
    }   
    //output
    digitalWrite(17,PulseOutput[0]);
    digitalWrite(16,PulseOutput[1]);
    digitalWrite(4,PulseOutput[2]);
    digitalWrite(2,PulseOutput[3]);
    digitalWrite(15,PulseOutput[4]);
    digitalWrite(5,BrakeOutput);
    StpEnOutput=true;
    digitalWrite(27,StpEnOutput);
//    digitalWrite(14,StpStpOutput);
//    digitalWrite(12,StpDirOutput);
  }
}

void loop() {
  delay(1000);
}


