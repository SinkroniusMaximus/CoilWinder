#include "HMI_Global.h"
#include "uart1.h"
#include <xc.h>
#include <stdlib.h>
int main(void) {
    Pin_init();
    SPI_Init();
    INTERRUPT_Initialize();
    UART1_Initialize();
    uint8_t SPI_timeout = 0;
    uint8_t HMI_Timerexpr = 1;
    uint8_t ESP_Timerexpr = 10;
    bool ESP_SPI_ACK;
    bool ESP_SPI_ACKREQ;
    bool ESP_SPI_ACK_Old;
    uint16_t HMI_Clock;
    uint16_t SPI_Clock;
    uint8_t SPI_Send;
    
    HMI_Pagenr = 0;
    bool Start_button;
    bool Stop_button;
    bool P_Running = false;
    SPI_Master=true;
    Ready = true;
    while (1) 
    {
        ESP_SPI_ACK=PORTBbits.RB1;
        Start_button=PORTBbits.RB14;
        Stop_button=PORTBbits.RB15;   
        StatusBits = (Running + (Ready << 1) + (requested_position << 2) + (HMI_Button_Reset_Run << 3));;
        //converting data
        ReadHMIBuffer(UARTBuffer, &HMI_Timerexpr, &Param_coil_length, &Param_Wire_diameter, &Param_Wire_offset, &Param_Nr_of_turns,
                      &HMI_Pagenr, &numBytes, &Read_busy, &HMI_Button_Reset_Run, &HMI_Button_Reset_Alarm);
        EspSpiComm(SPI_SEND_Buffer, StatusBits, Param_Nr_of_turns, Param_coil_length, Calc_Stepper_Stepsize, ESP_SPI_ACK, 
                   &Actual_windings, &PV_RPM, &Progress_percentage, &AI_Tension, &SPI_Clock, &SPI_timeout, &ESP_Timerexpr, &SPI_Send, 
                   &ESP_SPI_ACK_Old, &ESP_SPI_ACKREQ);
        HmiUartComm(PV_RPM, Progress_percentage, Actual_windings, Runstatus, Stat_nr_of_layers, Stat_dc_resistance, Stat_wire_length, 
                    HMI_Pagenr, &HMI_Clock, &Temperature_T1, &Temperature_T2, &Temperature_T3, &Temperature_T4, &Temperature_T5, 
                    &Temperature_Overall, &Velocity_Reduction, &HMI_Timerexpr, &Read_busy);
        if (Ready & Start_button)
        {
            Running = true;
            Ready = false;
            Runstatus=2;
        }
        if (Running & Stop_button)
        {
            Running = false;
            Runstatus=0;
        }
        if (!Running & (PV_RPM <= 0) & !Ready)
        {        
            Ready = true;
            Runstatus=1;
        }       
        if (Running&!P_Running)
        {
            requested_position=!requested_position;
            P_Running=true;
        }
        else if (!Running&P_Running)
        {
            P_Running=false;
        }
        
        LATAbits.LATA6=Running;            
        LATAbits.LATA7=Ready;
        LATBbits.LATB0=ESP_SPI_ACKREQ;
    }
    return (EXIT_SUCCESS);}