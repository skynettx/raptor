#pragma once

// HELPWIN defines

#define HELP_WINDOW 0x0000  // WINDOW
#define HELP_0000 0x0000    // BUMPIN
#define HELP_0001 0x0001    // BUMPIN
#define HELP_0002 0x0002    // BUMPOUT
#define HELP_0003 0x0003    // BUMPOUT
#define HELP_0004 0x0004    // ICON
#define HELP_UP 0x0005      // BUTTON FIELD
#define HELP_DOWN 0x0006    // BUTTON FIELD
#define HELP_DONE 0x0007    // BUTTON FIELD
#define HELP_TEXT 0x0008    // TEXT FIELD
#define HELP_HEADER 0x0009  // TEXT FIELD
#define HELP_000A 0x000a    // TEXT FIELD

// LOAD defines

#define LOAD_WINDOW 0x0000   // WINDOW
#define LOAD_0000 0x0000     // BUMPIN
#define LOAD_IDPIC 0x0001    // ICON
#define LOAD_NEXT 0x0002     // BUTTON FIELD
#define LOAD_PREV 0x0003     // BUTTON FIELD
#define LOAD_DEL 0x0004      // BUTTON FIELD
#define LOAD_LOAD 0x0005     // BUTTON FIELD
#define LOAD_CANCEL 0x0006   // BUTTON FIELD
#define LOAD_0007 0x0007     // TEXT FIELD
#define LOAD_0008 0x0008     // TEXT FIELD
#define LOAD_NAME 0x0009     // TEXT FIELD
#define LOAD_CALL 0x000a     // TEXT FIELD
#define LOAD_000B 0x000b     // TEXT FIELD
#define LOAD_CREDITS 0x000c  // TEXT FIELD

// STORE defines

#define STOR_WINDOW 0x0000   // WINDOW
#define STOR_ID 0x0000       // ICON
#define STOR_CALLSIGN 0x0001 // DRAGBAR
#define STOR_PREV 0x0002     // BUTTON FIELD
#define STOR_NEXT 0x0003     // BUTTON FIELD
#define STOR_BUY 0x0004      // BUTTON FIELD
#define STOR_BUYIT 0x0005    // BUTTON FIELD
#define STOR_SELL 0x0006     // BUTTON FIELD
#define STOR_COMP 0x0007     // TEXT FIELD
#define STOR_TEXT 0x0008     // TEXT FIELD
#define STOR_COST 0x0009     // TEXT FIELD
#define STOR_SCORE 0x000a    // TEXT FIELD
#define STOR_STATS 0x000b    // TEXT FIELD
#define STOR_TEXTCOST 0x000c // TEXT FIELD
#define STOR_NUM 0x000d      // TEXT FIELD
#define STOR_VBUY 0x000e     // VIEW AREA
#define STOR_VPREV 0x000f    // VIEW AREA
#define STOR_VNEXT 0x0010    // VIEW AREA
#define STOR_VSCREEN 0x0011  // VIEW AREA
#define STOR_VACCEPT 0x0012  // VIEW AREA
#define STOR_VSELL 0x0013    // VIEW AREA
#define STOR_VEXIT 0x0014    // VIEW AREA

// MSG defines

#define INFO_WINDOW 0x0002   // WINDOW
#define INFO_0000 0x0000     // BUMPIN
#define INFO_0001 0x0001     // BUMPOUT
#define INFO_0002 0x0002     // BUMPOUT
#define INFO_0003 0x0003     // BUMPOUT
#define INFO_0004 0x0004     // BUMPOUT
#define INFO_MSG 0x0005      // DRAGBAR

// SHIPCOMP defines

#define COMP_WINDOW 0x0000   // WINDOW
#define COMP_LITE1 0x0000    // ICON
#define COMP_LITE2 0x0001    // ICON
#define COMP_LITE3 0x0002    // ICON
#define COMP_CLOSE 0x0003    // CLOSE GADGET
#define COMP_GAME1 0x0004    // BUTTON FIELD
#define COMP_GAME2 0x0005    // BUTTON FIELD
#define COMP_SECRET 0x0006   // BUTTON FIELD
#define COMP_B1 0x0007       // BUTTON FIELD
#define COMP_B2 0x0008       // BUTTON FIELD
#define COMP_B3 0x0009       // BUTTON FIELD
#define COMP_GAME3 0x000a    // BUTTON FIELD
#define COMP_AUTO 0x000b     // BUTTON FIELD

// LOADCOMP defines

#define LCOMP_WINDOW 0x0000  // WINDOW
#define LCOMP_LITE1 0x0000   // ICON
#define LCOMP_LITE2 0x0001   // ICON
#define LCOMP_LITE3 0x0002   // ICON
#define LCOMP_SCREEN 0x0003  // ICON
#define LCOMP_CLOSE 0x0004   // CLOSE GADGET
#define LCOMP_SECRET 0x0005  // BUTTON FIELD
#define LCOMP_B1 0x0006      // BUTTON FIELD
#define LCOMP_B2 0x0007      // BUTTON FIELD
#define LCOMP_B3 0x0008      // BUTTON FIELD
#define LCOMP_SECTOR 0x0009  // TEXT FIELD
#define LCOMP_WAVE 0x000a    // TEXT FIELD
#define LCOMP_LEVEL 0x000b   // VIEW AREA

// MAIN defines

#define MAIN_WINDOW 0x0000   // WINDOW
#define MAIN_0000 0x0000     // ICON
#define MAIN_NEW 0x0001      // BUTTON FIELD
#define MAIN_LOAD 0x0002     // BUTTON FIELD
#define MAIN_OPTS 0x0003     // BUTTON FIELD
#define MAIN_ORDER 0x0004    // BUTTON FIELD
#define MAIN_CREDITS 0x0005  // BUTTON FIELD
#define MAIN_QUIT 0x0006     // BUTTON FIELD
#define MAIN_RETURN 0x0007   // BUTTON FIELD
#define MAIN_DEMO 0x0008     // TEXT FIELD
#define MAIN_0009 0x0009     // TEXT FIELD

// HANGAR defines

#define HANG_WINDOW 0x0000   // WINDOW
#define HANG_PIC 0x0000      // ICON
#define HANG_TEXT 0x0001     // TEXT FIELD
#define HANG_MISSION 0x0002  // VIEW AREA
#define HANG_SUPPLIES 0x0003 // VIEW AREA
#define HANG_MAIN_MENU 0x0004// VIEW AREA
#define HANG_QSAVE 0x0005    // VIEW AREA

// LOCKER defines

#define LOCK_WINDOW 0x0000   // WINDOW
#define LOCK_CLOSE 0x0000    // CLOSE GADGET
#define LOCK_TEXT 0x0001     // TEXT FIELD
#define LOCK_CHAR 0x0002     // VIEW AREA
#define LOCK_TOHANGER 0x0003 // VIEW AREA
#define LOCK_REGISTER 0x0004 // VIEW AREA

// ASK defines

#define ASK_WINDOW 0x0002    // WINDOW
#define ASK_0000 0x0000      // BUMPIN
#define ASK_0001 0x0001      // BUMPOUT
#define ASK_0002 0x0002      // BUMPOUT
#define ASK_0003 0x0003      // BUMPOUT
#define ASK_0004 0x0004      // BUMPOUT
#define ASK_DRAGBAR 0x0005   // DRAGBAR
#define ASK_YES 0x0006       // BUTTON FIELD
#define ASK_NO 0x0007        // BUTTON FIELD

// REGIST defines

#define REG_WINDOW 0x0000    // WINDOW
#define REG_IDPIC 0x0000     // ICON
#define REG_NAME 0x0001      // INPUT FIELD
#define REG_CALLSIGN 0x0002  // INPUT FIELD
#define REG_TEXT 0x0003      // TEXT FIELD
#define REG_VIEWEXIT 0x0004  // VIEW AREA
#define REG_VIEWID 0x0005    // VIEW AREA
#define REG_VIEWREG 0x0006   // VIEW AREA

// OKREG defines

#define OKREG_WINDOW 0x0002  // WINDOW
#define OKREG_0000 0x0000    // BUMPIN
#define OKREG_0001 0x0001    // BUMPIN
#define OKREG_0002 0x0002    // ICON
#define OKREG_0003 0x0003    // ICON
#define OKREG_0004 0x0004    // ICON
#define OKREG_0005 0x0005    // ICON
#define OKREG_TRAIN 0x0006   // BUTTON FIELD
#define OKREG_EASY 0x0007    // BUTTON FIELD
#define OKREG_MED 0x0008     // BUTTON FIELD
#define OKREG_HARD 0x0009    // BUTTON FIELD
#define OKREG_ABORT 0x000a   // BUTTON FIELD
#define OKREG_000B 0x000b    // TEXT FIELD

// WINGAME defines

#define WIN_WINDOW 0x0000    // WINDOW
#define WIN_0000 0x0000      // BUMPIN
#define WIN_0001 0x0001      // CLOSE GADGET
#define WIN_TEXT 0x0002      // TEXT FIELD

// OPTS defines

#define OPTS_WINDOW 0x0002   // WINDOW
#define OPTS_0000 0x0000     // BUMPIN
#define OPTS_0001 0x0001     // BUMPIN
#define OPTS_0002 0x0002     // BUMPIN
#define OPTS_PIC1 0x0003     // ICON
#define OPTS_PIC2 0x0004     // ICON
#define OPTS_PIC3 0x0005     // ICON
#define OPTS_DETAIL 0x0006   // BUTTON FIELD
#define OPTS_EXIT 0x0007     // BUTTON FIELD
#define OPTS_0008 0x0008     // TEXT FIELD
#define OPTS_0009 0x0009     // TEXT FIELD
#define OPTS_000A 0x000a     // TEXT FIELD
#define OPTS_VMUSIC 0x000b   // VIEW AREA
#define OPTS_VFX 0x000c      // VIEW AREA
