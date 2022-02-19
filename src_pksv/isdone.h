#ifndef PKSV_ISDONE_H
#define PKSV_ISDONE_H 1

void initDoneProcs(void);

unsigned int Do(unsigned int proc);
unsigned int Done(unsigned int proc);
unsigned char AllDone(void);
unsigned int FindNotDone(void);

unsigned int DoText(unsigned int proc);
unsigned int DoneText(unsigned int proc);
unsigned char AllDoneText(void);
unsigned int FindNotDoneText(void);

unsigned int DoMove(unsigned int proc);
unsigned int DoneMove(unsigned int proc);
unsigned char AllDoneMove(void);
unsigned int FindNotDoneMove(void);

unsigned int DoMart(unsigned int proc);
unsigned int DoneMart(unsigned int proc);
unsigned char AllDoneMart(void);
unsigned int FindNotDoneMart(void);

unsigned int DoThumb(unsigned int proc);
unsigned int DoneThumb(unsigned int proc);
unsigned char AllDoneThumb(void);
unsigned int FindNotDoneThumb(void);

unsigned int DoDword(unsigned int proc);
unsigned int DoneDword(unsigned int proc);
unsigned char AllDoneDword(void);
unsigned int FindNotDoneDword(void);

unsigned int DoLevel(unsigned int proc);
unsigned int DoneLevel(unsigned int proc);
unsigned char AllDoneLevel(void);
unsigned int FindNotDoneLevel(void);

#endif
