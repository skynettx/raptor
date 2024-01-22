#include <stdio.h>
#include <string.h>
#include <time.h>
#include "SDL.h"
#include "common.h"
#include "glbapi.h"
#include "i_video.h"
#include "gfxapi.h"
#include "vmemapi.h"
#include "intro.h"
#include "fx.h"
#include "windows.h"
#include "swdapi.h"
#include "ptrapi.h"
#include "imsapi.h"
#include "kbdapi.h"
#include "input.h"
#include "prefapi.h"
#include "loadsave.h"
#include "help.h"
#include "objects.h"
#include "demo.h"
#include "tile.h"
#include "anims.h"
#include "bonus.h"
#include "shots.h"
#include "eshot.h"
#include "enemy.h"
#include "shadow.h"
#include "flame.h"
#include "input.h"
#include "joyapi.h"
#include "i_lastscr.h"
#include "fileids.h"

#ifdef _WIN32
#include <io.h>
#endif // _WIN32
#ifdef __GNUC__
#include <unistd.h>
#endif // __GNUC__

#define wmemcpy(dst,src,size) memmove(dst,src,size)

struct bday_t {
    int month;
    int day;
    int year;
    const char *name;
};

int wRandSeed = 1;

int wrand(void)
{
    wRandSeed = ((long long)wRandSeed * 1103515245) + 12345;
    
    return (wRandSeed >> 16) & 0x7fff;
}

void wsrand(int seed)
{
    wRandSeed = seed;
}

#define MAX_BDAY 6

bday_t bday[MAX_BDAY];

#define MONTH     2
#define DAY       20
#define YEAR      1994
#define WLENGTH   12

int bday_flag = 0;
int bday_num = -1;
char *palette;
int godmode;
int cur_diff;
int reg_flag;
int demo_mode;
int demo_flag;
int ingameflag;
int cur_game;
int game_wave[4];

int gameflag[4];
int curship[16];
int lship[8];
int dship[8];
int fship[8];
char *numbers[11];

char gdmodestr[] = "CASTLE";

player_t plr;

char* g_highmem;

char *cursor_pic;
int draw_player;
int fadeflag;
int end_fadeflag;
int debugflag;
int playerx = PLAYERINITX;
int playery = PLAYERINITY;
int player_cx = PLAYERINITX;
int player_cy = PLAYERINITY;
int playerbasepic = 3;
int startfadeflag;
int g_oldsuper = -1;
int g_oldshield = -1;
int startendwave = -1;
int gl_cnt;
int g_flash = 0;
char gpal[768];
int end_wave;
int playerpic = 4;
int g_mapleft;

int fadecnt;

#define ROTPAL_START 240 
#define FADE_FRAMES  20
#define MAX_GLOW 20

int shakes[FADE_FRAMES] = {
    -4, 4, -3, 3, -2, 2, -1, 1,
    -1, 1, -1, 1, -1, 1, -1, 1,
    -1, 1, 0, 0
};

int o_engine[8] = {
    0, 1, 2, 3, 2, 1, 0, 0
};

int o_gun1[8] = {
    1, 3, 5, 6, 5, 3, 1, 0
};

int o_gun2[8] = {
    1, 3, 6, 9, 6, 3, 2, 0
};

int o_gun3[8] = {
    2, 6, 8, 11, 8, 6, 2, 0
};

int glowtable[20] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1
};

char flatnames[4][14] = {
    "FLATSG1_ITM",
    "FLATSG2_ITM",
    "FLATSG3_ITM",
    "FLATSG4_ITM"
};

flat_t *flatlib[4];

/***************************************************************************
RAP_Bday() - Get system date
 ***************************************************************************/
void 
RAP_Bday(
    void
)
{
    time_t t = time(NULL);
    struct tm *date = localtime(&t);
    int loop;
    
    bday_flag = 0;
    bday_num = -1;
    
    for (loop = 0; loop < MAX_BDAY; loop++)
    {
        if (bday[loop].day)
        {
            if (date->tm_mon + 1 == bday[loop].month && date->tm_mday == bday[loop].day && date->tm_year + 1900 >= bday[loop].year)
            {
                bday_num = loop;
                
                if (loop == 0)
                    bday_flag = 1;
            }
        }
    }
}

/***************************************************************************
InitScreen() - Prints the Init Screen
 ***************************************************************************/
void 
InitScreen(
    void
)
{
    printf(" RAPTOR: Call Of The Shadows V1.2                        (c)1994 Cygnus Studios\n");
}

/*==========================================================================
   ShutDown () Shut Down function called by EXIT_xxx functions
 ==========================================================================*/
void 
ShutDown(
    int errcode
)
{
    char* mem;

    if (!errcode && !godmode)
        WIN_Order();

    //IPT_DeInit();
    //DMX_DeInit();
    //GFX_EndSystem();
    //PTR_End();
    //KBD_End();
    
    if (reg_flag)
        mem = GLB_GetItem(FILE002_LASTSCR2_TXT);     //Get ANSI Screen Fullversion from GLB to char*
    else
        mem = GLB_GetItem(FILE001_LASTSCR1_TXT);     //Get ANSI Screen Shareware from GLB to char*

    closewindow();                                   //Close Main Window
    I_LASTSCR(mem);                                  //Call to display ANSI Screen 
    GLB_FreeAll();
    IPT_CloJoy();                                    //Close Joystick
    SWD_End();
    SDL_Quit();
    
    free(g_highmem);
}

/***************************************************************************
RAP_ClearSides () - Clears the Sides
 ***************************************************************************/
void 
RAP_ClearSides(
    void
)
{
    GFX_ColorBox(0, 0, MAP_LEFT, 200, 0);
    GFX_ColorBox(320 - MAP_LEFT, 0, MAP_LEFT, 200, 0);
}

/***************************************************************************
RAP_GetShipPic () - Loads Correct Ship Pics for Light/Dark Waves
 ***************************************************************************/
void 
RAP_GetShipPic(
    void
)
{
    int lightflag, loop;
    lightflag = 1;
    
    // GAME 2 wave 8
    if (cur_game == 1 && game_wave[cur_game] == 7)
        lightflag = 0;
    
    // GAME 3 wave 3
    if (cur_game == 2 && game_wave[cur_game] == 2)
        lightflag = 0;

    for (loop = 0; loop < 7; loop++)
    {
        if (lightflag)
        {
            curship[loop] = lship[loop];
            curship[loop + 7] = lship[loop];
        }
        else
        {
            curship[loop] = dship[loop];
            curship[loop + 7] = fship[loop];
        }
    }
    
    for (loop = 0; loop < 14; loop++)
    {
        GLB_CacheItem(curship[loop]);
    }
}

/***************************************************************************
   Rot_Color () - Rotates color in palette
 ***************************************************************************/
void 
Rot_Color(
    char *gpal, 
    int snum, 
    int len)
{
    short pos;
    short maxloop;
    char h1[3];
    
    pos = snum * 3;
    maxloop = len * 3;

    wmemcpy(h1, &gpal[pos], 3);
    wmemcpy(&gpal[pos], &gpal[pos+3], maxloop);
    wmemcpy(&gpal[pos+maxloop-3], h1, 3);
}

/***************************************************************************
   InitMobj() - Inits an object to be moved
 ***************************************************************************/
void 
InitMobj(
    mobj_t *cur            // INPUT : pointer to MOVEOBJ
)
{
    cur->done = 0;
    cur->addx = 1;
    cur->addy = 1;
    
    cur->delx = cur->x2 - cur->x;
    cur->dely = cur->y2 - cur->y;
    
    if (cur->delx < 0)
    {
        cur->delx = -cur->delx;
        cur->addx = -cur->addx;
    }
    if (cur->dely < 0)
    {
        cur->dely = -cur->dely;
        cur->addy = -cur->addy;
    }
    
    if (cur->delx >= cur->dely)
    {
        cur->err = -(cur->dely >> 1);
        cur->maxloop = cur->delx + 1;
    }
    else
    {
        cur->err = cur->delx >> 1;
        cur->maxloop = cur->dely + 1;
    }
}

/***************************************************************************
   MoveMobj() - gets next postion for an Object
 ***************************************************************************/
void 
MoveMobj(
    mobj_t *cur            // INPUT : pointer to MOVEOBJ
)
{
    if (cur->maxloop == 0)
    {
        cur->done = 1;
        return;
    }
    
    if (cur->delx >= cur->dely)
    {
        cur->x += cur->addx;
        cur->err += cur->dely;
        
        if (cur->err > 0)
        {
            cur->y += cur->addy;
            cur->err -= cur->delx;
        }
    }
    else
    {
        cur->y += cur->addy;
        cur->err += cur->delx;
        
        if (cur->err > 0)
        {
            cur->x += cur->addx;
            cur->err -= cur->dely;
        }
    }
    
    cur->maxloop--;
}

/***************************************************************************
   MoveSobj() - gets next postion for an Object at speed
 ***************************************************************************/
int 
MoveSobj(
    mobj_t *cur,           // INPUT : pointer to MOVEOBJ
    int speed              // INPUT : speed to plot at
)
{
    if (speed == 0)
        return 0;
    
    if (cur->delx >= cur->dely)
    {
        while (speed)
        {
            speed--;
            cur->maxloop--;
            cur->x += cur->addx;
            cur->err += cur->dely;
            
            if (cur->err > 0)
            {
                cur->y += cur->addy;
                cur->err -= cur->delx;
            }
        }
    }
    else
    {
        while (speed)
        {
            speed--;
            cur->maxloop--;
            cur->y += cur->addy;
            cur->err += cur->delx;
            
            if (cur->err > 0)
            {
                cur->x += cur->addx;
                cur->err -= cur->dely;
            }
        }
    }
    
    if (cur->maxloop < 1)
        cur->done = 1;
    
    return speed;
}

/***************************************************************************
RAP_PrintNum() -
 ***************************************************************************/
void 
RAP_PrintNum(
    int x, 
    int y, 
    char *str
)
{
    int maxloop, num;

    maxloop = strlen(str);
    
    GFX_PutSprite(numbers[10], x, y);
    x += 9;
    
    while (--maxloop != -1)
    {
        num = *str - '0';
        
        if (num < 11 && num >= 0)
            GFX_PutSprite(numbers[num], x, y);
        
        x += 8;
        str++;
    }
}

/***************************************************************************
RAP_DisplayShieldLevel() - 
 ***************************************************************************/
void 
RAP_DisplayShieldLevel(
    int xpos, 
    int level
)
{
    char *outbuf;
    unsigned int curs, addx;
    int loop;

    outbuf = &displayscreen[ylookup[199] + xpos];

    addx = (SHIELD_COLOR_RUN << 16) / MAX_SHIELD;
    curs = 0;
    
    for (loop = 0; loop < MAX_SHIELD; loop++)
    {
        if (loop < level)
            memset(outbuf, 74 - (curs >> 16), 4);
        else
            memset(outbuf, 0, 4);
        
        curs += addx;
        
        outbuf -= 640;
    }
}

/***************************************************************************
RAP_DisplayStats() - 
 ***************************************************************************/
void 
RAP_DisplayStats(
    void
)
{
    char temp[24];
    int super, shield, loop, x, y;
    char* pic;
    GFX_PIC *h;
    static int damage = -1;
    static int blinkflag = 1;

    // == DISPLAY SUPER SHIELD ========================
    super = OBJS_GetAmt(S_SUPER_SHIELD);
    if (g_oldsuper != super)
    {
        RAP_DisplayShieldLevel(MAP_LEFT - 8, super);
        g_oldsuper = super;
    }
    
    // == DISPLAY NORM SHIELD ========================
    shield = OBJS_GetAmt(S_ENERGY);
    if (g_oldshield != shield)
    {
        RAP_DisplayShieldLevel(MAP_RIGHT + 4, shield);
    }

    if (shield <= 0 && !godmode)
    {
        // BLOW UP SHIP IF ! IN GOD MODE ===================
        
        ANIMS_StartAnim(A_MED_AIR_EXPLO, playerx + (wrand()%32), playery + (wrand()%32));
        ANIMS_StartAnim(A_SMALL_AIR_EXPLO, playerx + (wrand()%32), playery + (wrand()%32));
        
        if (startendwave > END_EXPLODE)
        {
            if ((wrand()%2) == 0)
                SND_Patch(FX_AIREXPLO, 30);
            else
                SND_Patch(FX_AIREXPLO, 225);
        }
        
        if (startendwave == -1)
            startendwave = END_DURATION;
        
        if (startendwave == END_EXPLODE)
        {
            draw_player = 0;
            SND_Patch(FX_AIREXPLO, 127);
            SND_Patch(FX_AIREXPLO2, 127);
            ANIMS_StartAnim(A_LARGE_AIR_EXPLO, player_cx, player_cy);
            
            for (loop = 0; loop < (PLAYERWIDTH * PLAYERHEIGHT) / 2; loop++)
            {
                x = playerx - (PLAYERWIDTH / 2) + (wrand() % 32) * 2;
                y = playery - (PLAYERWIDTH / 2) + (wrand() % 32) * 2;
                
                if (loop & 1)
                    ANIMS_StartAnim(A_LARGE_AIR_EXPLO, x, y);
                else
                    ANIMS_StartAAnim(A_MED_AIR_EXPLO2, x, y);
            }
            SND_Patch(FX_AIREXPLO2, 127);
        }
    }

    // IF END OF WAVE FLY SHIP OFF SCREEN ===================

    if (startendwave != -1 && shield > 0)
    {
        if (startendwave == END_FLYOFF)
        {
            IPT_PauseControl(1);
            SND_Patch(FX_FLYBY, 127);
        }
        
        if (startendwave < END_FLYOFF)
        {
            x = 0;
            
            if (playerx < 152)
                x = 8;
            else if (playerx > 168)
                x = -8;
            
            IPT_FMovePlayer(x, -4);
        }
    }
    
    if (shield <= SHIELD_LOW && !godmode)
    {
        if (!(gl_cnt % 8))
        {
            blinkflag ^= 1;
            
            if (blinkflag)
            {
                if (damage)
                {
                    damage--;
                    
                    if ((haptic) && (control == 2))
                    {
                        IPT_CalJoyRumbleHigh();                                                                   
                    }
                }
            }
        }
        
        if (shield < g_oldshield && super < 1)
        {
            if (OBJS_LoseObj())
            {
                SND_Patch(FX_CRASH, 127);
                damage = 2;
            }
        }
        
        if (blinkflag)
        {
            if (damage)
            {
                pic = GLB_GetItem(FILE111_WEPDEST_PIC);
                h = (GFX_PIC*)pic;
                //h = pic = (texture_t*)GLB_GetItem(FILE111_WEPDEST_PIC);
                GFX_PutSprite(pic, (320 - h->width) >> 1, MAP_BOTTOM - 9);
            }
            
            if (startendwave == -1)
                SND_Patch(FX_WARNING, 127);
            
            pic = GLB_GetItem(FILE110_SHLDLOW_PIC);
            h = (GFX_PIC*)pic;
            //h = pic = (texture_t*)GLB_GetItem(FILE110_SHLDLOW_PIC);
            GFX_PutSprite(pic, (320 - h->width) >> 1, MAP_BOTTOM);
        }
    }
    
    g_oldshield = shield;
    
    OBJS_DisplayStats();
    
    sprintf(temp, "%08u", plr.score);
    RAP_PrintNum(119, MAP_TOP, temp);
    
    if (demo_mode == DEMO_RECORD)
        DEMO_DisplayStats();
    
    if (debugflag)
    {
        sprintf(temp, "%02u", plr.diff[cur_game]);
        RAP_PrintNum(18, MAP_TOP, temp);
        
        x = MAP_LEFT + 16;
        
        for (loop = 0; loop < 16; loop++)
        {
            GFX_ColorBox(x, 0, 8, 8, 240 + loop);
            x += 8;
        }
    }
}

/***************************************************************************
RAP_PaletteStuff() - 
 ***************************************************************************/
void 
RAP_PaletteStuff(
    void
)
{
    static int wblink = 0;
    static int glow1 = 0;
    static int glow2 = 8;
    static int cnt = 0;
    static int palcnt = 0;
    static int blink = 0;
    int offset;
    char *pal1, *pal2;
    int num;
    
    if (cnt & 1)
    {
        // == COLOR 240 - 244 ======== WATER
        if (palcnt % 4 != 0)
            Rot_Color(gpal, 240, 5);
        
        // == COLOR 245 - 249 ======== FIRE
        if (palcnt % 2 != 0)
            Rot_Color(gpal, 245, 5);
        
        // == COLOR 250 ======== GLOWING FIRE 1
        offset = 250 * 3;
        pal1 = &gpal[offset];
        pal2 = &palette[offset];
        
        num = glowtable[glow1];
        glow1++;
        glow1 %= MAX_GLOW;
        
        *pal1 = *pal2 - num;
        if ((uint8_t)*pal2 < num)
            *pal1 = 0;
        pal1++;
        pal2++;
        
        *pal1 = *pal2 - num;
        if ((uint8_t)*pal2 < num)
            *pal1 = 0;
        pal1++;
        pal2++;
        
        *pal1 = *pal2 - num;
        if ((uint8_t)*pal2 < num)
            *pal1 = 0;

        // == COLOR 251 ======== GLOWING FIRE 2
        offset = 251 * 3;
        pal1 = &gpal[offset];
        pal2 = &palette[offset];
        
        num = glowtable[glow2];
        glow2++;
        glow2 %= MAX_GLOW;
        
        *pal1 = *pal2 - num;
        if ((uint8_t)*pal2 < num)
            *pal1 = 0;
        pal1++;
        pal2++;
        
        *pal1 = *pal2 - num;
        if ((uint8_t)*pal2 < num)
            *pal1 = 0;
        pal1++;
        pal2++;
        
        *pal1 = *pal2 - num;
        if ((uint8_t)*pal2 < num)
            *pal1 = 0;
        
        // == COLOR 252 & 253 ======== BLINKING RED AND GREEN
        if (palcnt % 2)
        {
            offset = 252 * 3;
            pal1 = &gpal[offset];
            pal2 = &palette[offset];
            
            if (blink & 1)
            {
                memcpy(pal1, pal2, 3);
                pal1 += 3;
                pal2 += 3;
                memset(pal1, 0, 3);
            }
            else
            {
                memset(pal1, 0, 3);
                pal1 += 3;
                pal2 += 3;
                memcpy(pal1, pal2, 3);
            }
            
            blink++;
        }
        
        // == COLOR 254 ======== BLINKING BLUE
        offset = 254 * 3;
        if ((wrand() % 3) == 0)
        {
            pal1 = &gpal[offset];
            pal2 = &palette[offset];
            memcpy(pal1, pal2, 3);
        }
        else
        {
            pal1 = &gpal[offset];
            memset(pal1, 0, 3);
        }
        
        // == COLOR 255 ======== BLINKING WHITE
        offset = 255 * 3;
        if (wblink < 3)
        {
            pal1 = &gpal[offset];
            pal2 = &palette[offset];
            memcpy(pal1, pal2, 3);
        }
        else
        {
            pal1 = &gpal[offset];
            memset(pal1, 0, 3);
        }
        
        wblink++;
        wblink %= 6;
        
        GFX_SetPalette(gpal, 240);
        palcnt++;
    }
    
    cnt++;
}

/***************************************************************************
Do_Game () - The main game thing this is it dude
 ***************************************************************************/
int            // TRUE=Aborted, FALSE = timeout
Do_Game(
    void
)
{
    int b2_flag, b3_flag, init_flag, rval, start_score, local_cnt;
    b2_flag = 0;
    b3_flag = 0;
    init_flag = 1;
    rval = 0;
    start_score = 0;
    
    draw_player = 1;

    wsrand(game_wave[cur_game] << 10);
    
    fadeflag = 0;
    end_fadeflag = 0;
    KBD_Clear();
    IMS_StartAck();
    BUT_1 = 0;
    BUT_2 = 0;
    BUT_3 = 0;
    BUT_4 = 0;
    
    playerx = PLAYERINITX;
    playery = PLAYERINITY;
    
    if (!demo_mode) 
    {
        PTR_SetPos(playerx, playery);
        IPT_Start();
    }
    
    RAP_GetShipPic();
    BONUS_Clear();
    ANIMS_Clear();
    SHOTS_Clear();
    ESHOT_Clear();
    
    memcpy(gpal, palette, 768);
    GFX_FadeOut(0, 0, 0, 2);
    
    memset(displaybuffer, 0, 64000);
    memset(displayscreen, 0, 64000);
    
    local_cnt = GFX_GetFrameCount();
    
    g_flash = 0;
    g_oldsuper = -1;
    g_oldshield = -1;
    startendwave = -1;
    
    if (demo_flag == DEMO_RECORD)
        DEMO_StartRec();

    // == CLEAR ALL BUTTONS ===========================

    start_score = plr.score;
    IMS_StartAck();
    memset(buttons, 0, sizeof(buttons));
    
    do
    {
        num_shadows = num_gshadows = 0;
        
        IPT_MovePlayer();
        
        if (KBD_IsKey(SC_F1))                                                                   
        {
            SWD_SetClearFlag(0);
            IPT_End();
            HELP_Win("OVERVW06_TXT");
            memset(displayscreen, 0, 64000);
            IPT_Start();
            g_oldsuper = -1;
            g_oldshield = -1;
            RAP_ClearSides();
            RAP_DisplayStats();
            SWD_SetClearFlag(1);
            IMS_StartAck();
            BUT_1 = 0;
            BUT_2 = 0;
            BUT_3 = 0;
            BUT_4 = 0;
            b2_flag = 0;
            b3_flag = 0;
        }
        
        if (KBD_IsKey(SC_P) || JOY_IsKeyInGameStart(Start))                                                                  
        {
            while (IMS_IsAck())
            {
            }
            SWD_SetClearFlag(0);
            RAP_ClearSides();
            WIN_Pause();
            g_oldsuper = -1;
            g_oldshield = -1;
            RAP_ClearSides();
            RAP_DisplayStats();
            SWD_SetClearFlag(1);
            IMS_StartAck();
            BUT_1 = 0;
            BUT_2 = 0;
            BUT_3 = 0;
            BUT_4 = 0;
            b2_flag = 0;
            b3_flag = 0;
        }
        
        if (KBD_IsKey(SC_F8) && godmode)
        {
            while (IMS_IsAck())
            {
            }
            debugflag ^= 1;
        }
        
        if (DEMO_Think())
            break;
        
        if (demo_mode == DEMO_PLAYBACK)
        {
            if (IMS_IsAck() && !BUT_2 && !BUT_3 && !BUT_4)
            {
                rval = 1;
                break;
            }
        }
        
        switch (KBD_LASTSCAN)
        {
        case SC_NONE:
            break;
        
        case 1:
            break;
        
        case SC_1:
            OBJS_MakeSpecial(S_DUMB_MISSLE);
            break;
        
        case SC_2:
            OBJS_MakeSpecial(S_MINI_GUN);
            break;
        
        case SC_3:
            OBJS_MakeSpecial(S_TURRET);
            break;
        
        case SC_4:
            OBJS_MakeSpecial(S_MISSLE_PODS);
            break;
        
        case SC_5:
            OBJS_MakeSpecial(S_AIR_MISSLE);
            break;
        
        case SC_6:
            OBJS_MakeSpecial(S_GRD_MISSLE);
            break;
        
        case SC_7:
            OBJS_MakeSpecial(S_BOMB);
            break;
        
        case SC_8:
            OBJS_MakeSpecial(S_ENERGY_GRAB);
            break;
        
        case SC_9:
            OBJS_MakeSpecial(S_PULSE_CANNON);
            break;
        
        case SC_0:
            OBJS_MakeSpecial(S_DEATH_RAY);
            break;
        
        case SC_MINUS:
            OBJS_MakeSpecial(S_FORWARD_LASER);
            break;
        }
        
        if (BUT_1)
        {
            OBJS_Use(S_FORWARD_GUNS);
            OBJS_Use(S_PLASMA_GUNS);
            OBJS_Use(S_MICRO_MISSLE);
            BUT_1 = 0;
            if (plr.sweapon != -1)
                OBJS_Use(plr.sweapon);
        }
        
        if (BUT_2)
        {
            BUT_2 = 0;
            if (!b2_flag)
            {
                SND_Patch(FX_SWEP, 127);
                b2_flag = 1;
                OBJS_GetNext();
            }
        }
        else
        {
            if (b2_flag == 1)
                b2_flag = 0;
        }
        
        if (BUT_3)
        {
            BUT_3 = 0;
            if (!b3_flag)
            {
                b3_flag = 1;
                OBJS_Use(S_MEGA_BOMB);
            }
        }
        else
        {
            if (b3_flag == 1)
                b3_flag = 0;
        }
        
        if (startendwave != -1)
        {
            if (startendwave == 0)
                end_wave = 1;
            
            startendwave--;
        }
        
        gl_cnt++;
        
        TILE_Think();
        ENEMY_Think();
        ESHOT_Think();
        BONUS_Think();
        SHOTS_Think();
        ANIMS_Think();
        OBJS_Think();
        
        if (draw_player)
            SHADOW_Add(curship[playerpic + g_flash], playerx, playery);
        
        TILE_Display();
        SHADOW_DisplayGround();
        ENEMY_DisplayGround();
        SHADOW_DisplaySky();
        ANIMS_DisplayGround();
        
        ENEMY_DisplaySky();
        SHOTS_Display();
        BONUS_Display();
        ANIMS_DisplaySky();
        
        if (draw_player)
        {
            FLAME_Down(player_cx - o_engine[playerpic] - 3, player_cy + 15, 4, gl_cnt % 2);
            FLAME_Down(player_cx + o_engine[playerpic] - 2, player_cy + 15, 4, gl_cnt % 2);
            GFX_PutSprite((char*)GLB_GetItem(curship[playerpic + g_flash]), playerx, playery);
            g_flash = 0;
        }
        
        ANIMS_DisplayHigh();
        ESHOT_Display();
        
        if (fadeflag)
        {
            if (fadecnt >= FADE_FRAMES - 1)
            {
                RAP_ClearSides();
                g_mapleft = MAP_LEFT;
                GFX_SetPalette(gpal, 0);
                fadeflag = 0;
                g_oldsuper = -1;
                g_oldshield = -1;
            }
            else
            {
                RAP_ClearSides();
                retraceflag = 0;
                GFX_FadeFrame(gpal, fadecnt, FADE_FRAMES);
                retraceflag = 0;
                g_mapleft = shakes[fadecnt] + MAP_LEFT;
                fadecnt++;
            }
        }
        else
        {
            if (!init_flag)
                RAP_PaletteStuff();
        }
        
        RAP_DisplayStats();
        
        while (GFX_GetFrameCount() - local_cnt < 3)
        {
        }
        
        local_cnt = GFX_GetFrameCount();
        
        if (fadeflag)
            TILE_ShakeScreen();
        else
            TILE_DisplayScreen();
        
        if (startfadeflag)
        {
            SND_Patch(FX_GEXPLO, 127);
            SND_Patch(FX_AIREXPLO, 127);
            retraceflag = 1;
            GFX_FadeOut(63, 60, 60, 1);
            GFX_FadeStart();
            startfadeflag = 0;
            fadeflag = 1;
            fadecnt = 0;
            retraceflag = 0;
        }
        
        if (reg_flag && KBD_Key(SC_BACKSPACE))
        {
            OBJS_Add(S_DEATH_RAY);
            OBJS_Add(S_ENERGY);
            OBJS_Add(S_ENERGY);
            OBJS_Add(S_ENERGY);
            plr.score = 0;
        }
        
        if (init_flag)
        {
            init_flag = 0;
            GFX_FadeIn(gpal, 64);
        }
        
        if (KBD_Key(SC_X) && KBD_Key(SC_ALT))
        {
            RAP_ClearSides();
            SWD_SetClearFlag(0);
            IPT_End();
            WIN_AskExit();
            IPT_Start();
            g_oldsuper = -1;
            g_oldshield = -1;
            RAP_ClearSides();
            RAP_DisplayStats();
            SWD_SetClearFlag(1);
            IMS_StartAck();
            BUT_1 = 0;
            BUT_2 = 0;
            BUT_3 = 0;
            BUT_4 = 0;
            b2_flag = 0;
            b3_flag = 0;
        }
        
        if (KBD_IsKey(SC_ESC) || JOY_IsKeyInGameBack(Back))                                                                       
        {
            if (godmode)
                end_wave = 1;
            
            if (!demo_mode)
            {
                SWD_SetClearFlag(0);
                IPT_End();
                RAP_ClearSides();
                if (WIN_AskBool("Abort Mission ?"))
                {
                    plr.score = start_score;
                    rval = 1;
                    break;
                }
                IPT_Start();
                g_oldsuper = -1;
                g_oldshield = -1;
                RAP_ClearSides();
                RAP_DisplayStats();
                SWD_SetClearFlag(1);
                IMS_StartAck();
                BUT_1 = 0;
                BUT_2 = 0;
                BUT_3 = 0;
                BUT_4 = 0;
                b2_flag = 0;
                b3_flag = 0;
            }
            else if (demo_mode == DEMO_PLAYBACK)
            {
                rval = 1;
                break;
            }
            else if (demo_mode == DEMO_RECORD)
                break;
        }
    } while (!end_wave);
    
    GFX_FadeOut(0, 0, 0, 32);
    
    RAP_FreeMap();
    end_wave = 0;
    
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    
    IPT_PauseControl(0);
    
    GFX_DisplayUpdate();
    GFX_SetPalette(palette, 0);
    IPT_End();
    
    if (demo_flag == DEMO_RECORD)
        DEMO_SaveFile();
    
    return rval;
}

/***************************************************************************
RAP_InitMem() - Allocates memory for VM and GLB to use
 ***************************************************************************/
void 
RAP_InitMem(
    void
)
{
    unsigned int heapsize = 0x495FF0;                       
    
    g_highmem = (char*)calloc(heapsize, 1);
    
    VM_InitMemory(g_highmem, heapsize);
    GLB_UseVM();
}

/***************************************************************************
main() -
 ***************************************************************************/
int 
main(
    int argc, 
    char *argv[]
)
{
    char *var1, *tptr, *pal;
    int loop, numfiles, ptrflag, item;

    var1 = getenv("S_HOST");

    InitScreen();

    RAP_InitLoadSave();
    
    if (access(RAP_SetupFilename(), 0))
    {
        printf("\n\n** You must run SETUP first! **\n");
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
            "Raptor", "** You must run SETUP first! **", NULL);
        exit(0);
    }

    godmode = 0;

    if (var1 != NULL && !strcmp(var1, gdmodestr))
        godmode = 1;
    else
        godmode = 0;

    if (argv[1])
    {
        if (!strcmp(argv[1], "REC"))
        {
            DEMO_SetFileName(argv[2]);
            demo_flag = DEMO_RECORD;
            printf("DEMO RECORD enabled\n");
        }
        else if (!strcmp(argv[1], "PLAY"))
        {
            if (!access(argv[2], 0))
            {
                DEMO_SetFileName(argv[2]);
                demo_flag = DEMO_PLAYBACK;
                printf("DEMO PLAYBACK enabled\n");
            }
        }
    }

    if (godmode)
        printf("GOD mode enabled\n");
    
    cur_diff = 0;

    if (!access("FILE0001.GLB", 0))
        gameflag[0] = 1;
    
    if (!access("FILE0002.GLB", 0))
        gameflag[1] = 1;
    
    if (!access("FILE0003.GLB", 0) && !access("FILE0004.GLB", 0))
    {
        gameflag[2] = 1;
        gameflag[3] = 1;
    }

    if (gameflag[1] + gameflag[2])
        reg_flag = 1;

    numfiles = 0;
    
    for (loop = 0; loop < 4; loop++)
    {
        if (gameflag[loop])
            numfiles++;
    }

    if (access("FILE0000.GLB", 0) || !numfiles)
    {
        printf("All game data files NOT FOUND cannot proceed !!\n");
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
            "Raptor", "All game data files NOT FOUND cannot proceed !!", NULL);
        exit(0);
    }
    
    printf("Init -\n");
    EXIT_Install(ShutDown);
    
    // ================================================
    // SET UP B-DAY STUFF O RAMA
    // ================================================

    memset(bday, 0, sizeof(bday));
    
    bday[0].month = 5;   // Scott
    bday[0].day = 16;
    bday[0].year = 1994;
    bday[0].name = "Scott Host";

    bday[1].month = 2;   // Dave
    bday[1].day = 27;
    bday[1].year = 1996;
    bday[1].name = "Dave T.";

    bday[2].month = 10;  // JIM
    bday[2].day = 2;
    bday[2].year = 1995;
    bday[2].name = "Jim M.";

    bday[3].month = 3;   // BOBBY P.
    bday[3].day = 12;
    bday[3].year = 1995;
    bday[3].name = "Bobby P.";

    bday[4].month = 8;   // RICH
    bday[4].day = 28;
    bday[4].year = 1995;
    bday[4].name = "Rich F.";

    bday[5].month = 4;   // Paul
    bday[5].day = 24;
    bday[5].year = 1996;
    bday[5].name = "Paul R.";

    RAP_Bday();

    if (bday_num != -1)
        printf("Birthday() = %s\n", bday[bday_num].name);

    // ================================================

    if (access(RAP_SetupFilename(), 0))
        EXIT_Error("You Must run SETUP.EXE First !!");

    if (!INI_InitPreference(RAP_SetupFilename()))
        EXIT_Error("SETUP Error");

    fflush(stdout);
    KBD_Install();
    GFX_InitSystem();
    SWD_Install(0);
    
    VIDEO_LoadPrefs();
    IPT_LoadPrefs();
    
    switch (control)
    {
    default:
        printf("PTR_Init()-Auto\n");
        fflush(stdout);
        ptrflag = PTR_Init(P_MOUSE);
        usekb_flag = 1;
        break;
        
    case I_JOYSTICK:
        printf("PTR_Init()-Joystick\n");
        fflush(stdout);
        ptrflag = PTR_Init(P_JOYSTICK);
        if (joy_ipt_MenuNew)
            usekb_flag = 1;
        else
            usekb_flag = 0;
        break;
    
    case I_MOUSE:
        printf("PTR_Init()-Mouse\n");
        fflush(stdout);
        ptrflag = PTR_Init(P_MOUSE);
        usekb_flag = 0;
        break;
    }
    
    if (reg_flag)
    {
        printf("Registered EXE!\n");
        fflush(stdout);
    }
    
    GLB_InitSystem(argv[0], 6, 0);
    
    if (reg_flag)
    {
        tptr = GLB_GetItem(FILE000_ATENTION_TXT);
        printf("%s\n", tptr);
        GLB_FreeItem(FILE000_ATENTION_TXT);
    }
    
    SND_InitSound();
    IPT_Init();
    GLB_FreeAll();
    RAP_InitMem();
    
    printf("Loading Graphics\n");
    
    pal = GLB_LockItem(FILE100_PALETTE_DAT);
    memset(pal, 0, 3);
    palette = pal;
    SHADOW_Init();
    FLAME_Init();
    
    fflush(stdout);
    
    if (ptrflag)
    {
        cursor_pic = (char*)GLB_LockItem(FILE112_CURSOR_PIC);
        PTR_SetPic(cursor_pic);
        PTR_SetPos(160, 100);
        PTR_DrawCursor(0);
    }
    
    // = SET UP SHIP PICTURES =========================
    for (loop = 0; loop < 7; loop++)
    {
        lship[loop] = FILE11a_LPLAYER_PIC + loop;
        
        if (GAME2)
        {
            dship[loop] = FILE245_DPLAYER_PIC + loop;
            fship[loop] = FILE24c_FPLAYER_PIC + loop;
        }
    }
    
    // = LOAD IN FLAT LIBS  =========================
    for (loop = 0; loop < 4; loop++)
    {
        if (gameflag[loop])
        {
            item = GLB_GetItemID(flatnames[loop]);
            flatlib[loop] = (flat_t*)GLB_LockItem(item);
        }
        else
            flatlib[loop] = NULL;
    }
    
    // = LOAD IN 0-9 $ SPRITE PICS  =========================
    for (loop = 0; loop < 11; loop++)
    {
        item = FILE105_N0_PIC + loop;
        numbers[loop] = (char*)GLB_LockItem(item);
    }

    FLAME_InitShades();
    HELP_Init();
    OBJS_Init();
    TILE_Init();
    SHOTS_Init();
    ESHOT_Init();
    BONUS_Init();
    ANIMS_Init();
    SND_Setup();
    
    GFX_SetPalRange(0, ROTPAL_START - 1);
    GFX_InitVideo(palette);
    SHADOW_MakeShades();
    
    RAP_ClearPlayer();
    
    if (!godmode)
        INTRO_Credits();
    
    if (demo_flag != DEMO_PLAYBACK)
    {
        SND_PlaySong(FILE056_RINTRO_MUS, 1, 1);
        INTRO_PlayMain();
        SND_PlaySong(FILE057_MAINMENU_MUS, 1, 1);
    }
    else if (demo_flag == DEMO_PLAYBACK)
    {
        DEMO_LoadFile();
        DEMO_Play();
    }

    cur_game = 0;
    game_wave[0] = 0;
    game_wave[1] = 0;
    game_wave[2] = 0;
    game_wave[3] = 0;
    
    do
    {
       WIN_MainMenu();
       WIN_MainLoop();
    } while (1);

    return 0;
}
