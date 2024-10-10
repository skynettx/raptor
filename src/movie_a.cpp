#include <string.h>
#include "common.h"
#include "movie.h"
#include "gfxapi.h"
#include "entypes.h"

/*************************************************************************
ANIM_Render () - Renders an ANIM FRAME
 *************************************************************************/
void 
ANIM_Render(
	ANIMLINE *inmem
)
{
	while (LE_USHORT(inmem->opt))
	{
		int l = LE_USHORT(inmem->length);
		int p = LE_USHORT(inmem->offset);
		
		inmem++;
		
		memcpy(&displaybuffer[p], inmem, l);
		
		inmem = (ANIMLINE*)((char*)inmem + l);
	}
}
