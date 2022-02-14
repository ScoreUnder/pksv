#ifndef PKSV_ISDONE_H
#define PKSV_ISDONE_H 1

void initDoneProcs();

unsigned int Do(unsigned int proc);
unsigned int Done(unsigned int proc);
unsigned char AllDone();
unsigned int FindNotDone();

unsigned int DoText(unsigned int proc);
unsigned int DoneText(unsigned int proc);
unsigned char AllDoneText();
unsigned int FindNotDoneText();

unsigned int DoMove(unsigned int proc);
unsigned int DoneMove(unsigned int proc);
unsigned char AllDoneMove();
unsigned int FindNotDoneMove();

unsigned int DoMart(unsigned int proc);
unsigned int DoneMart(unsigned int proc);
unsigned char AllDoneMart();
unsigned int FindNotDoneMart();

unsigned int DoThumb(unsigned int proc);
unsigned int DoneThumb(unsigned int proc);
unsigned char AllDoneThumb();
unsigned int FindNotDoneThumb();

unsigned int DoDword(unsigned int proc);
unsigned int DoneDword(unsigned int proc);
unsigned char AllDoneDword();
unsigned int FindNotDoneDword();

unsigned int DoLevel(unsigned int proc);
unsigned int DoneLevel(unsigned int proc);
unsigned char AllDoneLevel();
unsigned int FindNotDoneLevel();

#endif
