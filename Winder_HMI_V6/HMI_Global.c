#include "HMI_Global.h"
#include "HMI_Config_bits.h"

uint8_t UARTBuffer[10] = {0};
uint8_t SPI_RECV_Buffer[10] = {0};
uint8_t SPI_SEND_Buffer[10] = {0};
int Memorybuffer[20] = {0};
//input tags
uint16_t Temperature_T1=1;
uint16_t Temperature_T2=2;
uint16_t Temperature_T3=3;
uint16_t Temperature_T4=4;
uint16_t Temperature_T5=5;
uint16_t Temperature_Overall;
uint16_t Velocity_Reduction;

//screen tags
uint16_t Stat_alarm;
uint32_t counter = 0;
bool HMI_Button_Reset_Run = false;
bool HMI_Button_Reset_Alarm = false;
uint16_t Param_coil_length = 0;
uint16_t Param_Wire_diameter = 0;
uint16_t Param_Wire_offset = 0;
uint16_t Param_Nr_of_turns = 0;
uint16_t Stat_nr_of_layers = 0;
uint16_t Stat_dc_resistance = 0;
uint16_t Stat_wire_length = 0;
uint16_t HMI_Pagenr = 0;
uint16_t dummy = 0;
uint16_t reccounter = 0;
uint16_t numBytes = 0;
//internal
uint16_t Calc_Stepper_Stepsize = 201;

//esp32 tags
int AI_Tension = 0;
int AI_Tension_old = 0;
int PulsesPerSecond = 0;
int PV_RPM = 0;
int PV_RPM_old = 0;
int SP_RPM = 0;
int SP_RPM_old = 0;
int Actual_windings;
int Progress_percentage = 0;
int Runstatus;
int StatusBits = 0;
bool Endswitch_front = false;
bool Endswitch_rear = false;
bool Running;
bool Ready;
bool Faulted;
bool kickstart_direction;
bool requested_position;
bool Read_busy;
bool SPI_Master;

void Pin_init()
{
    //set the tris register, selects GPIO as in- or output.
    TRISA = 0xFF3F;
    TRISB = 0xDF7E;//DF7F;
    TRISAbits.TRISA2 = 0;
    ANSELAbits.ANSA0 = false;
    ANSELAbits.ANSA1 = false;
    ANSELAbits.ANSA2 = false;
    ANSELAbits.ANSA3 = false;
    ANSELBbits.ANSB0 = false;
    ANSELBbits.ANSB1 = false;
    ANSELBbits.ANSB2 = false;
    ANSELBbits.ANSB3 = false; 
    ANSELBbits.ANSB4 = false; 
    ANSELBbits.ANSB12 = false; 
    ANSELBbits.ANSB13 = false;
    ANSELBbits.ANSB14 = false; 
    ANSELBbits.ANSB15 = false;
}
void SPI_Init()
{
SSP1STAT = 0x40;    //bit 6 CKE: SPI Clock Select bit
SSP1CON1 = 0x05;    //0101 = SPI Slave mode, Clock = SCKx pin; SSx pin control is disabled, SSx can be used as an I/O pin
SSP1CON3 = 0x10;    //bit 4 BOEN: Buffer Overwrite Enable bit,
                    //1 = SSPxBUF updates every time that a new data byte is shifted in, ignoring the BF bit
PADCFG1bits.SDO1DIS = 0; // bit 9 SDO1DIS: MSSP1 SDO Pin Disable bit
                         //0 = The SPI output data (SDO1) of MSSP1 is output to the pin
SSP1CON1bits.SSPEN = 1; 
}


void INTERRUPT_Initialize (void)
{
    //    URXI: U1RX - UART1 Receiver
    //    Priority: 1
    IPC2bits.U1RXIP = 1;
    //    URXI: U1RX - UART1 Receiver
    //    Priority: 1
}

void delay(uint16_t delay_length)
    {
    uint16_t i;
    for(i=delay_length;i!=0;i--){}
    }

unsigned short Read_SPI() {
//    while (!SSP1STATbits.BF); // wait for transfer to complete
    return SSP1BUF; // read the received value
//    while (!SSP1STATbits.BF); // wait for transfer to complete
}//writeSPI1

void Write_SPI(unsigned short data) {
    SSP1BUF = data; // write to buffer for TX
//    while (!SSP1STATbits.BF); // wait for transfer to complete
}
void Ack_SPI(void) {
    SSP1BUF = 0xFF; // write to buffer for TX
    while (!SSP1STATbits.BF); // wait for transfer to complete
    delay(10);
    SSP1BUF = (0xAA + Running + (requested_position << 1) + (!SPI_Master << 2)); // write to buffer for TX
//    SSP1BUF = 0xAA;
    while (!SSP1STATbits.BF); // wait for transfer to complete
}