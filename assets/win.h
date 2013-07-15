
//{{BLOCK(win)

//======================================================================
//
//	win, 256x192@8, 
//	+ palette 256 entries, not compressed
//	+ 264 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 512 + 16896 + 1536 = 18944
//
//	Time-stamp: 2013-07-15, 20:14:59
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_WIN_H
#define GRIT_WIN_H

#define winTilesLen 16896
extern const unsigned short winTiles[8448];

#define winMapLen 1536
extern const unsigned short winMap[768];

#define winPalLen 512
extern const unsigned short winPal[256];

#endif // GRIT_WIN_H

//}}BLOCK(win)
