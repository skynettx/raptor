#pragma once

extern int num_gshadows;
extern int num_shadows;

void SHADOW_Init(void);
void SHADOW_MakeShades(void);
void SHADOW_GAdd(int item, int x, int y);
void SHADOW_Add(int item, int x, int y);
void SHADOW_DisplayGround();
void SHADOW_DisplaySky();
