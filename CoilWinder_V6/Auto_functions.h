//Auto_functions.h
#ifndef AUTO_FUNCTIONS_H
#define  AUTO_FUNCTIONS_H

  void Debug(void);
  void CalcRPM(uint16_t fieldwidth,float* PV_RPM);
  void Ringbuffer(float Buffer[], int Bufferlength, float ActualValue, uint16_t* Bufferexpr, float* Average);

#endif  //AUTO_FUNCTIONS_H


