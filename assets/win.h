
//{{BLOCK(win)

//======================================================================
//
//	win, 256x192@8, 
//	+ palette 48 entries, not compressed
//	+ 264 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 96 + 16896 + 1536 = 18528
//
//	Time-stamp: 2013-07-14, 18:39:09
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_WIN_H
#define GRIT_WIN_H

#define winTilesLen 16896
extern const unsigned char winTiles[16896];

#define winMapLen 1536
extern const unsigned char winMap[1536];

#define winPalLen 96
extern const unsigned char winPal[96];

#endif // GRIT_WIN_H

//}}BLOCK(win)
