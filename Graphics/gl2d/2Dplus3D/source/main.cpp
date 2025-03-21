/******************************************************************************
*******************************************************************************
	Easy GL2D
	2D + 3D example
	Shows a way to combine a 3D renderer
	with the 2D capabilities of Easy GL2D

	Relminator (Richard Eric M. Lope BSN RN)
	Http://Rel.Phatcode.Net

	Thanks to:
	Adigun A. Polack for the enemies sprites and 3d Texture
	Cearn (Jasper Vijn) for the atan2 implementation
	Patater (Jaeden Amero) for the shuttle and flyer sprites

*******************************************************************************
******************************************************************************/

#include <nds.h>
#include <stdio.h>
#include <gl2d.h>

// Cearn's awesome Fixed-point ATAN2
#include "cearn_atan.h"


//This is my procedural 3D model generator
#include "Cvertexbuffer.h"

// GRIT auto-generated  files
#include "organ16.h"
#include "enemies.h"
#include "shuttle.h"
#include "flyer.h"


// Texture Packer auto-generated UV coords
#include "uvcoord_enemies.h"




// Mode7-like 3D effect parameters
static const int GRID_RINGS = 2;
static const int GRID_BANDS = 2;
static const int GRID_WIDTH = 15;
static const int GRID_HEIGHT = 12;

// PI for our binary radian measure
static const int BRAD_PI = 1 << 14;


// function to draw our mode7 fx
void draw_grid();



// Our sprites
glImage  Enemies[ENEMIES_NUM_IMAGES];	// spriteset
glImage  Shuttle[1];					// single image
glImage  Flyer[1];						// single image

// Our mode7 effect using 2 planes
Cvertexbuffer *vb;


int main()
{


	// Initialize fake mode7 planes
	vb = init_grid ( GRID_RINGS, GRID_BANDS, GRID_WIDTH, GRID_HEIGHT, 8, 8 );

	//set mode 5, enable BG0 and set it to 3D
	videoSetMode( MODE_5_3D );

	consoleDemoInit();


	// initialize gl2d
	glScreen2D();

	//set up enough texture memory for our textures
	vramSetBankA( VRAM_A_TEXTURE );

	// Very Important!!! Or you get black sprites
	vramSetBankE(VRAM_E_TEX_PALETTE);  // Allocate VRAM bank for all the palettes

	// Load our texture for our mode7 effect
	vb->load_texture( (u8*)organ16Bitmap );


	// Load our Enemies texture
	// We used glLoadSpriteSet since the texture was made
	// with my texture packer.
	int EnemiesTextureID =
		glLoadSpriteSet( Enemies,				// pointer to glImage array
						 ENEMIES_NUM_IMAGES, 	// Texture packer auto-generated #define
						 enemies_texcoords,		// Texture packer auto-generated array
						 GL_RGB256,				// texture type for glTexImage2D() in videoGL.h
						 TEXTURE_SIZE_256,		// sizeX for glTexImage2D() in videoGL.h
						 TEXTURE_SIZE_256,		// sizeY for glTexImage2D() in videoGL.h
						 GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT, // param for glTexImage2D() in videoGL.h
						 256,					// Length of the palette to use (256 colors)
						 (u16*)enemiesPal,		// Load our 256 color enemies palette
						 (u8*)enemiesBitmap	 	// image data generated by GRIT
					   );



	// Shuttle
	// Since the shuttle is just a single 64x64 image,
	// We use glLoadTileSet() giving the right dimensions.
	int ShuttleTextureID =
		glLoadTileSet( Shuttle,			// pointer to glImage array
					   64,				// sprite width
					   64,				// sprite height
					   64,				// bitmap image width
					   64,				// bitmap image height
					   GL_RGB16,		// texture type for glTexImage2D() in videoGL.h
					   TEXTURE_SIZE_64,	// sizeX for glTexImage2D() in videoGL.h
					   TEXTURE_SIZE_64,	// sizeY for glTexImage2D() in videoGL.h
					   GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT,
					   16,					// Length of the palette to use (16 colors)
					   (u16*)shuttlePal,	// Load our 16 color Shuttle palette
					   (u8*)shuttleBitmap  // image data generated by GRIT
					 );


	// Flyer
	int FlyerTextureID =
		glLoadTileSet( Flyer,
					   64,
					   64,
					   64,
					   64,
					   GL_RGB16,
					   TEXTURE_SIZE_64,
					   TEXTURE_SIZE_64,
					   GL_TEXTURE_WRAP_S|GL_TEXTURE_WRAP_T|TEXGEN_OFF|GL_TEXTURE_COLOR0_TRANSPARENT,
					   16,					// Length of the palette to use (16 colors)
					   (u16*)flyerPal,     // Load our 16 color Flyer palette
					   (u8*)flyerBitmap
					 );




	// Print some console stuff

	iprintf("\x1b[1;1HEasy GL2D + 3D");
	iprintf("\x1b[2;1HRelminator");
	iprintf("\x1b[4;1HHttp://Rel.Phatcode.Net");

	iprintf("\x1b[6;1HA demo showing a very easy");
	iprintf("\x1b[7;1Hway to combine 3D and 2D");

	iprintf("\x1b[ 9;1HSprites by:");
	iprintf("\x1b[10;1H  Adigun A. Polack and Patater");
	iprintf("\x1b[11;1HFixed Point atan2Lerp By Cearn");

	iprintf("\x1b[13;1HEnemiesTextureID = %i", EnemiesTextureID);
	iprintf("\x1b[14;1HFlyerTextureID = %i", FlyerTextureID);
	iprintf("\x1b[15;1HShuttleTextureID = %i", ShuttleTextureID);


	iprintf("\x1b[17;1HEnemies use a 256 color pal");
	iprintf("\x1b[18;1HShuttle uses a 16 color pal");
	iprintf("\x1b[19;1HFlyer uses a 16 color pal");
	iprintf("\x1b[20;1H3D plane is a 16 bit image");

	int TextureSize = enemiesBitmapLen +
					  flyerBitmapLen +
					  organ16BitmapLen +
					  shuttleBitmapLen;


	iprintf("\x1b[22;1HTotal Texture size= %i kb", TextureSize / 1024);


	// some variables for our demo
	s32 ox = 0, oy = 0;			// needed for shuttle angle calculation
	int Frame = 0;				// just the standard frame counter
	int PhoenixFrame = 0;		// animation frame for our firebird
	int BeeFrame = 0;			// animation frame for the bee
	int Rotation = 0;			// rotation value of the rotating sprites

	while( pmMainLoop() )
	{

		Frame++;

		Rotation = Frame * 240;		// speed up our rotation

		// animate some of our animated sprites
		// every 8th frame
		if ( (Frame & 7) == 0 )
		{
			BeeFrame = (BeeFrame + 1) & 1;
			PhoenixFrame++;
			if (PhoenixFrame > 2) PhoenixFrame = 0;
		}


		// calculate positions for our rotating sprites
		int x = 128 + ((cosLerp(Frame)+sinLerp(BRAD_PI+Rotation) * 70) >> 12);
		int y = 96 + ((cosLerp(Frame)+cosLerp(-Rotation) * 50) >> 12);


		// Calculate moving shuttle position
		// Get new ship position in f32 format for accuracy since
		// shifting here by >> 12 looses accuracy
		s32 sx = ( cosLerp(Frame*150) + sinLerp(Frame*70) ) * (SCREEN_WIDTH/4) ;
		s32 sy = ( sinLerp(-(Frame*80)) + sinLerp(Frame*190) ) * (SCREEN_HEIGHT/4) ;

		// get angle(binary radian) to draw the rotated shuttle
		s32 angle = atan2Lerp( ox - sx, oy - sy );

		// save positions for use on the nexgt frame to get the new angle
		ox = sx;
		oy = sy;


		// Get new shuttle position
		// Shifting by >> 12 so that we can screen coordinates to draw
		sx = SCREEN_WIDTH/2 + ( sx >> 12 );
		sy = SCREEN_HEIGHT/2 + ( sy >> 12 );




		// Drawing the fake mode7 FX in 3D mode

		draw_grid();

		// End drawing in 3D mode

		// Start 2D mode
		glBegin2D();


			// Draw our enemies
			// draw some rotated and/or animated sprites
			glSpriteRotate(    x,     y,      Rotation,          GL_FLIP_NONE, &Enemies[30+BeeFrame]);
			glSpriteRotate(255-x, 191-y,  Rotation * 4,             GL_FLIP_H, &Enemies[84]);
			glSpriteRotate(255-x,     y,     -Rotation,             GL_FLIP_V, &Enemies[32]);
			glSpriteRotate(    x, 191-y, -Rotation * 3, GL_FLIP_H | GL_FLIP_V, &Enemies[81]);


			// Some phoenix enemies on the right
			// Note the flipmodes
			// Also shows how we can draw in "color mode" and shadow mode
			glSprite(200, 30,  GL_FLIP_NONE,           &Enemies[87 + PhoenixFrame]);
			glColor( RGB15(31,0,0) );
			glSprite(200, 60,  GL_FLIP_H,              &Enemies[87 + PhoenixFrame]);

			// Make the last two sprites translucent
			glPolyFmt(POLY_ALPHA(20) | POLY_CULL_NONE | POLY_ID(1));
			glColor( RGB15(0,31,20) );
			glSprite(200, 90,  GL_FLIP_V,              &Enemies[87 + PhoenixFrame]);
			glColor( RGB15(0,0,0) );
			glSprite(200, 130, GL_FLIP_V | GL_FLIP_H , &Enemies[87 + PhoenixFrame]);

			//Restore color and translucency to normal
			glColor( RGB15(31,31,31) );
			glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE );



			// Offset the angle by -PI/2 since Patater's
			// Original sprite faces upwards
			glSpriteRotate( sx, sy, angle - (BRAD_PI/2), GL_FLIP_NONE, Shuttle );



			// Make the flyer translucent just for kicks
			glPolyFmt(POLY_ALPHA(20) | POLY_CULL_NONE | POLY_ID(2));
			glSpriteRotateScaleXY( SCREEN_WIDTH/2, SCREEN_HEIGHT/2, Frame*140, sinLerp(Frame*120) * 3, sinLerp(Frame*210) * 2,  GL_FLIP_NONE, Flyer );

			// Restore to normal rendering just to be safe
			glPolyFmt(POLY_ALPHA(31) | POLY_CULL_NONE );

		glEnd2D();



		glFlush( 0 );

		swiWaitForVBlank();
		scanKeys();
		if (keysDown()&KEY_START) break;

	}

	return 0;


}//end main



// draws 2 planes
// for a fake mode7 effect
void draw_grid()
{
	static s32 text_off_u = 0;
	static s32 text_off_v = 0;
	static int frame = 0;

	frame++;

	glPushMatrix();

		glLoadIdentity();

		glTranslate3f32( 0, 0, floattof32(3.0) );
		glScalef32(floattof32(1),floattof32(1.5),floattof32(1.5));

		// floor
		glPushMatrix();

			glTranslate3f32( 0, -1 << 12, -2 << 12 );
			glRotateXi( inttof32(244) );

			vb->render( text_off_u, text_off_v, true );

		glPopMatrix( 1 );

		// ceiling
		glPushMatrix();

			glTranslate3f32( 0, 1 << 12, -2 << 12 );
			glRotateXi( inttof32(180) );

			vb->render( text_off_u ,text_off_v, true );

		glPopMatrix( 1 );

	glPopMatrix( 1 );


	// "move" the floor and ceiling
	text_off_u = (sinLerp(frame*30)*2) & 4095;
	text_off_v = (sinLerp(-frame*50)*3) & 4095;

}



