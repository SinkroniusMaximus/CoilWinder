#include "HMI_Global.h"
void nextion_int(char nxt_cmd[25],unsigned int nxt_valor)
{
    unsigned char nxt_dmlh, nxt_mlh, nxt_cnt, nxt_dzn, nxt_und;
    unsigned char index = 0;
    unsigned char nxt_tex;
    nxt_dmlh = nxt_valor/10000;     // 
    nxt_valor -= (nxt_dmlh*10000);  // 
    nxt_mlh = nxt_valor/1000;       //converts nxt_valor
    nxt_valor -= (nxt_mlh*1000);    //
    nxt_cnt = nxt_valor/100;        // into
    nxt_valor -= (nxt_cnt*100);     //
    nxt_dzn = nxt_valor/10;         // BCD
    nxt_valor -= (nxt_dzn*10);      //
    nxt_und = nxt_valor;            //

    nxt_dmlh += 0x30;            //converte dezena de milhar para carctere ASCII
    nxt_mlh  += 0x30;            //converte milhar para caractere ASCII
    nxt_cnt  += 0x30;            //converte centena para caractere ASCII
    nxt_dzn  += 0x30;            //converte dezena para caractere ASCII
    nxt_und  += 0x30;            //converte unidade para caractere ASCII
    
    // Send command via UART
    nxt_tex = nxt_cmd[index];
    while (nxt_tex > 0)
    {
      index++;
      if(nxt_tex == 0) break;
      U1TXREG = nxt_tex;
      while(!U1STAbits.TRMT);
      nxt_tex = nxt_cmd[index];
    }
    U1TXREG = nxt_dmlh;
    while(!U1STAbits.TRMT)
    {}
    U1TXREG = nxt_mlh;
    while(!U1STAbits.TRMT)
    {}
    U1TXREG = nxt_cnt;
    while(!U1STAbits.TRMT)
    {}
    U1TXREG = nxt_dzn;
    while(!U1STAbits.TRMT)
    {}
    U1TXREG = nxt_und;
    while(!U1STAbits.TRMT)
    {}

    U1TXREG = 0xFF;               //
    while(!U1STAbits.TRMT)        //
    {}                            //
    U1TXREG = 0xFF;               //estas linhas mandam
    while(!U1STAbits.TRMT)        // os
    {}                            //stop bytes 0xFF 0xFF 0xFF
    U1TXREG = 0xFF;               //
    while(!U1STAbits.TRMT)        //
    {}                            //
}

void nextion_cmd(char nxt_cmd[25])
{
    unsigned char index = 0;
    unsigned char nxt_tex;
    // Send command via UART
    nxt_tex = nxt_cmd[index];
    while (nxt_tex > 0)
    {
      index++;
      if(nxt_tex == 0) break;
      U1TXREG = nxt_tex;
      while(!U1STAbits.TRMT);
      nxt_tex = nxt_cmd[index];
    }
    U1TXREG = 0xFF;               //
    while(!U1STAbits.TRMT)        //
    {}                            //
    U1TXREG = 0xFF;               //estas linhas mandam
    while(!U1STAbits.TRMT)        // os
    {}                            //stop bytes 0xFF 0xFF 0xFF
    U1TXREG = 0xFF;               //
    while(!U1STAbits.TRMT)        //
    {}                            //
}


void nextion_init_page(uint16_t Pagenr)
{
    switch (Pagenr)
    {
        case 4://calculate
            nextion_int("n0.val=",Param_coil_length);
            nextion_int("n1.val=",Param_Wire_diameter);
            nextion_int("n2.val=",Param_Wire_offset);
            nextion_int("n3.val=",Param_Nr_of_turns);
            nextion_int("n4.val=",Stat_nr_of_layers);
            nextion_int("n5.val=",Stat_dc_resistance);
            nextion_int("n6.val=",Stat_wire_length);
            break;
        //case 2://settings page
        
        //case 3://dashboard
        default:
            break;
    }
}