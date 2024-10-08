#pragma once
#include "SDL_endian.h"

#define LE_USHORT(x) SDL_SwapLE16(x)
#define LE_SHORT(x) (signed short) SDL_SwapLE16(x)
#define LE_ULONG(x) SDL_SwapLE32(x)
#define LE_LONG(x) (signed int) SDL_SwapLE32(x)