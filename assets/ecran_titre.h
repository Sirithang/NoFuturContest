
//{{BLOCK(ecran_titre)

//======================================================================
//
//	ecran_titre, 256x192@8, 
//	+ palette 32 entries, not compressed
//	+ 150 tiles (t|f|p reduced) not compressed
//	+ regular map (flat), not compressed, 32x24 
//	Total size: 64 + 9600 + 1536 = 11200
//
//	Time-stamp: 2013-07-13, 22:49:59
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_ECRAN_TITRE_H
#define GRIT_ECRAN_TITRE_H

#define ecran_titreTilesLen 9600
extern const unsigned char ecran_titreTiles[9600];

#define ecran_titreMapLen 1536
extern const unsigned char ecran_titreMap[1536];

#define ecran_titrePalLen 64
extern const unsigned char ecran_titrePal[64];

#endif // GRIT_ECRAN_TITRE_H

//}}BLOCK(ecran_titre)
