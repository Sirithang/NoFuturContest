
//{{BLOCK(tsar)

//======================================================================
//
//	tsar, 256x192@4, 
//	+ palette 16 entries, not compressed
//	+ 224 tiles (t|f reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 32 + 7168 + 1536 = 8736
//
//	Time-stamp: 2013-07-15, 21:05:07
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_TSAR_H
#define GRIT_TSAR_H

#define tsarTilesLen 7168
extern const unsigned char tsarTiles[7168];

#define tsarMapLen 1536
extern const unsigned char tsarMap[1536];

#define tsarPalLen 32
extern const unsigned char tsarPal[32];

#endif // GRIT_TSAR_H

//}}BLOCK(tsar)
