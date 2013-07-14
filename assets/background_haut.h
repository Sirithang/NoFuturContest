
//{{BLOCK(background_haut)

//======================================================================
//
//	background_haut, 512x256@8, 
//	+ palette 256 entries, not compressed
//	+ 495 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 2x1 
//	Metatiled by 32x32
//	Total size: 512 + 31680 + 4096 + 4 = 36292
//
//	Time-stamp: 2013-07-14, 19:39:50
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_BACKGROUND_HAUT_H
#define GRIT_BACKGROUND_HAUT_H

#define background_hautTilesLen 31680
extern const unsigned char background_hautTiles[31680];

#define background_hautMetaTilesLen 4096
extern const unsigned char background_hautMetaTiles[4096];

#define background_hautMetaMapLen 4
extern const unsigned char background_hautMetaMap[4];

#define background_hautPalLen 512
extern const unsigned char background_hautPal[512];

#endif // GRIT_BACKGROUND_HAUT_H

//}}BLOCK(background_haut)
