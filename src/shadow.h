#pragma once

extern int num_gshadows;
extern int num_shadows;

void SHADOW_Init(void);
void SHADOW_MakeShades(void);
void SHADOW_GAdd(int a1, int a2, int a3);
void SHADOW_Add(int a1, int a2, int a3);
void SHADOW_DisplayGround();
void SHADOW_DisplaySky();
