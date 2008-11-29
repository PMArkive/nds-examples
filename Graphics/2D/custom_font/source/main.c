//---------------------------------------------------------------------------------
#include <nds.h>

#include <stdio.h>


//Include the font header generated by grit
#include "font.h"

//---------------------------------------------------------------------------------
int main(void) {
//---------------------------------------------------------------------------------

	const int char_base = 0;
	const int screen_base = 20;
	videoSetMode(0);	
	videoSetModeSub(MODE_0_2D | DISPLAY_BG0_ACTIVE);	
	vramSetBankC(VRAM_C_SUB_BG); 

	REG_BG0CNT_SUB = BG_COLOR_16 | BG_TILE_BASE(char_base) | BG_MAP_BASE(screen_base);

	u16* sub_tile = (u16*)CHAR_BASE_BLOCK_SUB(char_base);
	u16* sub_map = (u16*)SCREEN_BASE_BLOCK_SUB(screen_base);

	//95 and 32 show how many characters there are and 32 shows which ASCII character to start, respectively
	//95 is the smaller set of ACSII characters. It usually will start with 32
	consoleInit((u16 *)fontTiles, sub_tile, 95, 32, sub_map, 0, 16);
    
	//Load the Font Data and Palette stuff here

	dmaCopy(fontTiles, sub_tile, fontTilesLen);
	dmaCopy(fontPal,BG_PALETTE_SUB,fontPalLen);

	iprintf("Custom Font Demo\n");
	iprintf("   by Poffy\n");
	iprintf("modified by WinterMute\n");
	iprintf("for libnds examples\n");

	while(1) {
		swiWaitForVBlank();
	}

	return 0;
}
