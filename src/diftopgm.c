///-----------------------------------------------------------
/// @file   diftopgm.c
/// @brief  Decodeur d'images au format DIF vers PGM
/// @author Loic Rakotoniary
///-----------------------------------------------------------
#include <g2x.h>
#include "codec.h"

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

static G2Xpixmap* img = NULL;
static char* filename_in = NULL;

void init(void) {
    if (img) g2x_PixmapPreload(img);
}

void draw(void) {
    if (img) g2x_PixmapRecall(img, true);
    if (img) g2x_StaticPrint(10, 20, G2Xg, "DECODEUR: %dx%d - [Q]uitter", img->width, img->height);
}

void action_quit(void) {
    g2x_Quit();
}

int main(int argc, char* argv[]) {
    //par étape :
    //on va d'abord décoder le fichier .DIF
    //puis on initialise et configure la fenêtre graphique
    //on ajuste le ratio d'affichage
    //on lance la boucle principale
    //on associe la touche 'q' à l'action de quitter
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <image.dif>\n", argv[0]);
        return 1;
    }
    filename_in = argv[1];
    img = load_dif(filename_in);
    if (!img) {
        fprintf(stderr, "Erreur fatale : Impossible de decoder %s\n", filename_in); 
        return 1; 
    }
    int w = MIN(img->width, 1600);
    int h = MIN(img->height, 800);
    g2x_InitWindow("Decodeur DIF -> PGM", w, h);
    double ratio = (double)img->width / (double)img->height;
    g2x_SetWindowCoord(-1., -1./ratio, +1., +1./ratio);
    g2x_SetInitFunction(init);
    g2x_SetDrawFunction(draw);
    g2x_SetKeyAction('q', action_quit, "Quitter l'application");
    return g2x_MainStart();
}