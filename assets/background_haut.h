
//{{BLOCK(background_haut)

//======================================================================
//
//	background_haut, 512x256@8, 
//	+ palette 64 entries, not compressed
//	+ 500 tiles (t|f reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x32 
//	Total size: 128 + 32000 + 4096 = 36224
//
//	Time-stamp: 2013-07-15, 00:21:06
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BACKGROUND_HAUT_H
#define GRIT_BACKGROUND_HAUT_H

#define background_hautTilesLen 32000
extern const unsigned short background_hautTiles[16000];

#define background_hautMapLen 4096
extern const unsigned short background_hautMap[2048];

#define background_hautPalLen 128
extern const unsigned short background_hautPal[64];

#endif // GRIT_BACKGROUND_HAUT_H

//}}BLOCK(background_haut)
