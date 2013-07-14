
//{{BLOCK(lose)

//======================================================================
//
//	lose, 256x192@8, 
//	+ palette 32 entries, not compressed
//	+ 256 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 64 + 16384 + 1536 = 17984
//
//	Time-stamp: 2013-07-14, 22:33:01
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_LOSE_H
#define GRIT_LOSE_H

#define loseTilesLen 16384
extern const unsigned char loseTiles[16384];

#define loseMapLen 1536
extern const unsigned char loseMap[1536];

#define losePalLen 64
extern const unsigned char losePal[64];

#endif // GRIT_LOSE_H

//}}BLOCK(lose)
