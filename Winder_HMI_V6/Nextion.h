#ifndef NEXTION_H
#define	NEXTION_H

void nextion_init_page(uint16_t Pagenr);
void nextion_int(char nxt_cmd[25],unsigned int nxt_valor);
void nextion_cmd(char nxt_cmd[25]);

#endif	/* NEXTION_H */

