#ifndef _CSCTAPI_CARDLIST_H_
#define _CSCTAPI_CARDLIST_H_

#ifdef WITH_CARDLIST
struct atrlist { char providername[32]; char atr[80]; char info[92]; } current;
void findatr(struct s_reader *reader);
#endif // WITH_CARDLIST

#endif