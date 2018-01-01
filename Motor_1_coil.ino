#include<Wire.h>

int PotmeterPin = A0;    // select the input pin for the potentiometer
int PulsePin = 8;      // select the pin for the pulse output
int HallSensorPin = 2;     // the number of the input pin
long PotValue = 0;  // variable to store the value coming from the sensor
bool HallSensorState = false; //Hall sensor magnetic trigger input state
bool PulseActive = false; //Pulse detection active
bool PulseDone = false; // Pulse output done
bool PulseOutput = false; //Actual pulse sent to output
long PulseCounter = 0; // pulse length counter
int SensorCounter = 0; // read potmeter value interval counter
long CalcPulseWidth = 0; // calculated magnetic field pulse width value
long FieldWidth = 10000;
int FieldWidthCounter = 0; // Magnetic field width counter
bool HallStateCp = false;


void setup() {
  // put your setup code here, to run once:
    pinMode(PulsePin, OUTPUT);
    pinMode(HallSensorPin, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Read hall sensor
  HallSensorState = not digitalRead(HallSensorPin);
  //Read user tuner input every 100 cycles
  SensorCounter++;
  if (SensorCounter >= 100) 
  {
   SensorCounter = 0;
  }
  if (SensorCounter == 1)
  {
    PotValue = analogRead(PotmeterPin);
  }
  //Calculate field width and pulse width
  if (HallSensorState == true)
  {
    if (FieldWidthCounter < 10000)
    {
    FieldWidthCounter++; //count field pulse length
    }
    HallStateCp = true; //copy of hall effect state
  }
  else if (HallSensorState == false and HallStateCp == true)
  {
    FieldWidth = FieldWidthCounter; // copy measured field width
    CalcPulseWidth = ((FieldWidth*2)*PotValue)/1024; //calculate desired pulse width
    HallStateCp=false;// reset rest
    FieldWidthCounter=0;
  }
  
  if (PulseActive == false and PulseDone == false and HallSensorState == true) 
  {
    PulseActive = true;
  }
  if (PulseActive == true)
  {
    if (PulseDone == false)
    {
      PulseCounter++;
    }
    if (PulseCounter >= CalcPulseWidth)
    {
     PulseDone = true;
    }
  }
  if (PulseActive == true and PulseDone == true and HallSensorState == false) 
  {
    PulseActive = false;
    PulseDone = false;
    PulseCounter = 0;
  }
  PulseOutput = (PulseActive and not PulseDone); //
  digitalWrite(PulsePin,PulseOutput);
}
