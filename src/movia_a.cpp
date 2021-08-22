#include <string.h>
#include "common.h"
#include "movie.h"
#include "gfxapi.h"

void ANIM_Render(movanim_t *a1)
{
	while (a1->f_0)
	{
		int l = a1->f_6;
		int p = a1->f_4;
		a1++;
		memcpy(&displaybuffer[p], a1, l);
		a1 = (movanim_t*)((char*)a1 + l);
	}
}
