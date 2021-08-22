#pragma once

extern int keyboard[256];
extern int lastscan;
extern int lastascii;
extern int kbd_ack;
extern int capslock;

void KBD_Clear(void);
void KBD_Install(void);
void KBD_End(void);
int KBD_IsKey(int a1);
void KBD_Wait(int a1);
