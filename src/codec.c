#include "codec.h"

G2Xpixmap* g2x_PixmapNew(int w, int h, int layer) {
    G2Xpixmap* p = (G2Xpixmap*)calloc(1, sizeof(G2Xpixmap));
    if (!p) return NULL;
    p->width = w;
    p->height = h;
    p->layer = layer;
    p->map = (uchar*)calloc(w * h * layer, sizeof(uchar));
    if (p->map) p->end = p->map + (w * h * layer);
    return p;
}

static const QuantLevel Q[QSIZE] = {
    { 0, 2, 1, 0, 0, 1 }, 
    { 2, 6, 2, 2, 2, 2 }, 
    { 6,  22,  4, 6, 6, 3 }, 
    { 22, 256, 8, 22, 7, 3 }
};

void bs_init(BitStream* bs, size_t size) {
    bs->data = (uchar*)calloc(size, 1);
    bs->size = size;
    bs->index = 0;
    bs->capa = 8;
}

static void pushbits(BitStream* bs, uchar val, int nbit) {
    if (bs->index >= bs->size) return;
    val &= ((1 << nbit) - 1);
    if (nbit <= bs->capa) {
        bs->data[bs->index] |= (val << (bs->capa - nbit));
        bs->capa -= nbit;
        if (bs->capa == 0) { bs->index++; bs->capa = 8; }
    } else {
        int r = nbit - bs->capa;
        bs->data[bs->index] |= (val >> r);
        bs->index++;
        if (bs->index < bs->size) {
            bs->capa = 8;
            pushbits(bs, val, r);
        }
    }
}

static uchar pullbits(BitStream* bs, int nbit) {
    if (bs->index >= bs->size) return 0;
    uchar res = 0;
    if (nbit <= bs->capa) {
        res = (bs->data[bs->index] >> (bs->capa - nbit)) & ((1 << nbit) - 1);
        bs->capa -= nbit;
        if (bs->capa == 0) { bs->index++; bs->capa = 8; }
    } else {
        int r = nbit - bs->capa;
        res = (bs->data[bs->index] & ((1 << bs->capa) - 1)) << r;
        bs->index++;
        bs->capa = 8;
        res |= pullbits(bs, r);
    }
    return res;
}

int fold(int x)   { return (x >= 0) ? 2 * x : -2 * x - 1; }
int unfold(int y) { return (y % 2 == 0) ? y / 2 : -(y + 1) / 2; }

bool save_dif(const char* filename, G2Xpixmap* img) {
    FILE* f = fopen(filename, "wb");
    if (!f) return false;
    ushort magic = MAGIC_GRAY;
    ushort w = img->width;
    ushort h = img->height;
    if(fwrite(&magic, 2, 1, f)) {};
    if(fwrite(&w, 2, 1, f)) {};
    if(fwrite(&h, 2, 1, f)) {};    
    uchar q_count = 4;
    uchar q_bits[4] = {1, 2, 4, 8};
    if(fwrite(&q_count, 1, 1, f)) {};
    if(fwrite(q_bits, 1, 4, f)) {};
    size_t buf_size = (size_t)(1.4 * w * h);
    BitStream bs;
    bs_init(&bs, buf_size);
    uchar* p = img->map;
    uchar first = p[0] >> 1;
    if(fwrite(&first, 1, 1, f)) {};
    int prev = first;
    int total = w * h;
    for (int i = 1; i < total; i++) {
        int curr = p[i] >> 1;
        int diff = curr - prev;
        int val_folded = fold(diff);
        int k = 3;
        for(int j=0; j<QSIZE; j++) {
            if (val_folded < Q[j].bsup) { k = j; break; }
        }
        pushbits(&bs, Q[k].prfx, Q[k].plen);
        pushbits(&bs, val_folded - Q[k].off, Q[k].nbit);
        prev = curr;
    }
    if(fwrite(bs.data, 1, bs.index + 1, f)) {};
    fclose(f);
    free(bs.data);
    return true;
}

G2Xpixmap* load_dif(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) return NULL;
    ushort magic, w, h;
    if (fread(&magic, 2, 1, f) != 1 || magic != MAGIC_GRAY) { fclose(f); return NULL; }
    if (fread(&w, 2, 1, f) != 1) {};
    if (fread(&h, 2, 1, f) != 1) {};
    uchar trash[5];
    if (fread(trash, 1, 5, f) != 5) {};
    G2Xpixmap* img = g2x_PixmapNew(w, h, 1);
    if (!img) { fclose(f); return NULL; }
    uchar first;
    if (fread(&first, 1, 1, f) != 1) {};
    img->map[0] = first << 1;
    long pos = ftell(f);
    fseek(f, 0, SEEK_END);
    long len = ftell(f) - pos;
    fseek(f, pos, SEEK_SET);
    BitStream bs;
    bs.data = (uchar*)malloc(len);
    if (fread(bs.data, 1, len, f) != len) {};
    bs.size = len; bs.index = 0; bs.capa = 8;
    fclose(f);
    int prev = first;
    int total = w * h;
    for (int i = 1; i < total; i++) {
        int k = 0;
        if (pullbits(&bs, 1) == 0) k = 0;
        else {
            if (pullbits(&bs, 1) == 0) k = 1;
            else {
                if (pullbits(&bs, 1) == 0) k = 2;
                else k = 3;
            }
        }
        int val_folded = Q[k].off + pullbits(&bs, Q[k].nbit);
        int diff = unfold(val_folded);
        int curr = prev + diff;
        if (curr < 0) curr = 0;
        if (curr > 127) curr = 127;
        img->map[i] = (uchar)(curr << 1);
        prev = curr;
    }
    free(bs.data);
    return img;
}