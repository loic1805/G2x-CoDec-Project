///---------------------------------------------------------------------------
/// @file   codec.h
/// @brief  Prototypes des fonctions d'encodage et décodage DIF <-> PGM
/// @author Loic Rakotoniary
///---------------------------------------------------------------------------

#ifndef CODEC_H
#define CODEC_H

#include <g2x.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAGIC_GRAY 0xD1FF
#define QSIZE 4

//le Quantificateur
typedef struct {
    uchar binf;
    ushort bsup;
    uchar nbit;
    uchar off;
    uchar prfx;
    uchar plen;
} QuantLevel;

//le BitStream
typedef struct {
    uchar* data;   
    size_t size;
    size_t index;
    int capa;
} BitStream;

//la gestion du BitStream
void bs_init(BitStream* bs, size_t size);

//les fonctions principales du CoDec
bool save_dif(const char* filename, G2Xpixmap* img);
G2Xpixmap* load_dif(const char* filename);

//l'allocation du pixmap
G2Xpixmap* g2x_PixmapNew(int w, int h, int layer);

//les méthodes de repliement / dépliement
int fold(int diff);
int unfold(int val);

#endif