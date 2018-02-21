#include "uart1.h"
#include "HMI_Global.h"

void (*UART1_RxDefaultInterruptHandler)(void);


void UART1_Initialize (void)
{
   // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; RXINV disabled; UEN TX_RX; 
   U1MODE = (0x8008 & ~(1<<15));  // disabling UARTEN bit   
   // UTXISEL0 TX_ONE_CHAR; UTXINV enabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
   U1STA = 0x0000;
   // BaudRate = 9600; Frequency = 4000000 Hz; BRG 103; 
   U1BRG = 0x00CF; 
   
   IEC0bits.U1RXIE = 1;
   UART1_SetRxInterruptHandler(UART1_Receive_ISR);

    //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
   U1MODEbits.UARTEN = 1;  // enabling UART ON bit
   U1STAbits.UTXEN = 1;
}

void UART1_SetRxInterruptHandler(void* handler){
    UART1_RxDefaultInterruptHandler = handler;
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U1RXInterrupt( void )
{
    (*UART1_RxDefaultInterruptHandler)();
}

void UART1_Receive_ISR(void)
{
    while((U1STAbits.URXDA == 1))
    {
        UARTBuffer[numBytes] = U1RXREG;
        Read_busy=true;
        reccounter++;
        if (reccounter > 100)
        {
            reccounter = 0;
        }
        if ((UARTBuffer[0] != 0x0B) && (UARTBuffer[0] != 0x0A) && (UARTBuffer[0] != 0x71))
        {
            numBytes = 0;
        }
        else if (numBytes < 8)
        {
            numBytes++;
        } 
        else
        {
            numBytes = 0;
        }
    }

    IFS0bits.U1RXIF = false;
}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _U1ErrInterrupt ( void )
{
    if ((U1STAbits.OERR == 1))
    {
        U1STAbits.OERR = 0;
    }

    IFS4bits.U1ERIF = false;
}

void ReadHMIBuffer (uint8_t UARTBuffer[], uint8_t* HMI_Timerexpr, uint16_t* Param_coil_length, uint16_t* Param_Wire_diameter, uint16_t* Param_Wire_offset, uint16_t* Param_Nr_of_turns, uint16_t* HMI_Pagenr, uint16_t* numBytes,bool* Read_busy, bool* HMI_Button_Reset_Run, bool* HMI_Button_Reset_Alarm)
{
    uint16_t RecvVal;
    if  (((UARTBuffer[4] == 0xff)) && (UARTBuffer[5] == 0xff) && (UARTBuffer[6] == 0xff))
        {
            if (UARTBuffer[0] == 0x0A)//page events   
            {
                *HMI_Pagenr=UARTBuffer[3];
            }            
            if (UARTBuffer[0] == 0x0B)//button events
            {
                switch (UARTBuffer[1])// pagenumber
                { 
                    case 0: //Menu page
                        if (UARTBuffer[2] == 0x01)//first button
                        {
                            *HMI_Button_Reset_Run=(bool)UARTBuffer[3];
                        }
                        break;
                    case 3: //Alarm page
                        if (UARTBuffer[2] == 0x01)//first and only button
                        {
                            *HMI_Button_Reset_Alarm = (bool)UARTBuffer[3];
                        }
                        break;
                    //case 2: //Settings page                 
                    //case 3: //Dashboard   
                    default:
                        break;
                }
            }
                    *numBytes = 0;
                    *Read_busy=false;
        //clear UARTBuffer
        UARTBuffer[0]=0;
        UARTBuffer[1]=0;
        UARTBuffer[2]=0;
        UARTBuffer[3]=0;
        UARTBuffer[4]=0;
        UARTBuffer[5]=0;
        UARTBuffer[6]=0;
        UARTBuffer[7]=0;
        }
        if (((UARTBuffer[5] == 0xff) | (UARTBuffer[8] == 0xff)) && (UARTBuffer[6] == 0xff) && (UARTBuffer[7] == 0xff))
        {

            if (UARTBuffer[0] == 0x71)//return value events
            {
               RecvVal = UARTBuffer[1] + UARTBuffer[2] * 256;
               switch(*HMI_Timerexpr)
               {
                   case 1:
                       *Param_coil_length=RecvVal;
                       *HMI_Timerexpr=*HMI_Timerexpr+1;
                       break;
                   case 2:
                       *Param_Wire_diameter=RecvVal;
                       *HMI_Timerexpr=*HMI_Timerexpr+1;
                       break;
                   case 3:
                       *Param_Wire_offset=RecvVal;
                       *HMI_Timerexpr=*HMI_Timerexpr+1;                  
                       break;
                   case 4:
                       *Param_Nr_of_turns=RecvVal; 
                       *HMI_Timerexpr=*HMI_Timerexpr+1;                 
                       break;
                   default:
                       break;              
               }
            }
        *Read_busy=false;
        *numBytes = 0;
        //clear UARTBuffer
        UARTBuffer[0]=0;
        UARTBuffer[1]=0;
        UARTBuffer[2]=0;
        UARTBuffer[3]=0;
        UARTBuffer[4]=0;
        UARTBuffer[5]=0;
        UARTBuffer[6]=0;
        UARTBuffer[7]=0;
        //memset(UARTBuffer, 0, sizeof UARTBuffer);
        }
}
void HmiUartComm(int PV_RPM, int Progress_percentage, int Actual_windings, int Runstatus, uint16_t Stat_nr_of_layers, uint16_t Stat_dc_resistance, uint16_t Stat_wire_length, uint8_t HMI_Pagenr, uint16_t* HMI_Clock, uint16_t* Temperature_T1, uint16_t* Temperature_T2, uint16_t* Temperature_T3, uint16_t* Temperature_T4, uint16_t* Temperature_T5, uint16_t* Temperature_Overall, uint16_t* Velocity_Reduction, uint8_t* HMI_Timerexpr, bool* Read_busy)
{
    *HMI_Clock=*HMI_Clock+1;
    if (*HMI_Clock > 1000)
    {
        if (HMI_Pagenr == 0)//Menu
        {
            *HMI_Timerexpr = 100;
        }
        if (HMI_Pagenr == 1)//Dashboard
        {
            if (*HMI_Timerexpr >= 7)
            {
                *HMI_Timerexpr=1;            
            }
                switch(*HMI_Timerexpr)
                {
                    case 1:
                        nextion_int("n0.val=", PV_RPM);                  
                        break;
                    case 2:
                        nextion_int("n1.val=",Progress_percentage); //AI_Tension);
                        break;
                    case 3:
                        nextion_int("n2.val=", *Temperature_Overall);
                        break;
                    case 4:
                        nextion_int("n3.val=", Actual_windings);
                        break;
                    case 5:
                        nextion_int("j0.val=",Progress_percentage);
                        break;   
                    case 6:
                        nextion_int("va0.val=",Runstatus);
                        break;                               
                }
                *HMI_Timerexpr=*HMI_Timerexpr+1;
        }
        if (HMI_Pagenr == 2)//Temperature
        {
            if (*HMI_Timerexpr >= 8)
            {
                *HMI_Timerexpr=1;            
            }
                switch(*HMI_Timerexpr)
                {
                    case 1:
                        *Temperature_T1=SPI_RECV_Buffer[0];//(bool)ESP_SPI_ACK;
                        nextion_int("n0.val=", *Temperature_T1);                  
                        break;
                    case 2:
                        *Temperature_T2=SPI_RECV_Buffer[1];//(bool)ESP_SPI_ACK_Old;
                        nextion_int("n1.val=", *Temperature_T2);
                        break;
                    case 3:
                        *Temperature_T3=SPI_RECV_Buffer[2];//(bool)ESP_SPI_ACKREQ;
                        nextion_int("n2.val=", *Temperature_T3);
                        break;
                    case 4:
                        *Temperature_T4=SPI_RECV_Buffer[3];
                        nextion_int("n3.val=",*Temperature_T4);
                        break;   
                    case 5:
                        *Temperature_T5=SPI_RECV_Buffer[4];
                        nextion_int("n4.val=",*Temperature_T5);
                        break;   
                    case 6:
                        *Temperature_Overall=SPI_RECV_Buffer[5];
                        nextion_int("n5.val=",*Temperature_Overall);
                        break;     
                    case 7:
                        *Velocity_Reduction=SPI_RECV_Buffer[6];
                        nextion_int("n6.val=",*Velocity_Reduction);
                        break;     
                }
                *HMI_Timerexpr=*HMI_Timerexpr+1;
        }
        if (HMI_Pagenr == 3)//Alarm
        {
            nextion_int("va3.val=",Stat_alarm);
        }
        if (HMI_Pagenr == 4)//Calculate

        {
            if (*HMI_Timerexpr >= 8)
            {
                *HMI_Timerexpr=1;            
            }
            if (!*Read_busy)
            {
                switch(*HMI_Timerexpr)
                {
                    case 1:
                        nextion_cmd("get n0.val");  
                        *Read_busy=true;
                        break;
                    case 2:
                        nextion_cmd("get n1.val");
                        *Read_busy=true;
                        break;
                    case 3:
                        nextion_cmd("get n2.val");
                        *Read_busy=true;
                        break;
                    case 4:
                        nextion_cmd("get n3.val");
                        *Read_busy=true;
                        break;   
                    case 5:
                        nextion_int("n4.val=", Stat_nr_of_layers);
                        *HMI_Timerexpr=*HMI_Timerexpr+1;
                        break;  
                    case 6:
                        nextion_int("n5.val=", Stat_dc_resistance);
                        *HMI_Timerexpr=*HMI_Timerexpr+1;
                        break;  
                    case 7:
                        nextion_int("n6.val=", Stat_wire_length);
                        *HMI_Timerexpr=*HMI_Timerexpr+1;
                        break; 
                }
            }
        }
        *HMI_Clock=0;
    }
}