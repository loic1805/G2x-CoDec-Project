///-----------------------------------------------------------
/// @file   pgmtodif.c
/// @brief  Encodeur d'images au format PGM vers DIF
/// @author Loic Rakotoniary
///-----------------------------------------------------------
#include <g2x.h>
#include "codec.h"

static G2Xpixmap* img = NULL;
static char* filename_in = NULL;
static char* rootname = NULL;
static char* extname = NULL;
static char  filename_out[256];

void init(void) {

}

void draw(void) {
    g2x_PixmapRecall(img, true);
    g2x_StaticPrint(10, 20, G2Xr, "ENCODEUR: [S]auvegarder | [Q]uitter");
}

void action_save(void) {
    if (save_dif(filename_out, img)) {
        fprintf(stderr, ">> SUCCES : Image sauvegardee dans %s !\n", filename_out);
    } else {
        fprintf(stderr, ">> ERREUR : Echec de la sauvegarde !\n");
    }
}

void action_quit(void) {
    g2x_Quit();
}

int main(int argc, char* argv[]) {
    //par étape :
    //on va d'abord charger le fichier PGM
    //puis on initialise et configure la fenêtre graphique
    //on associe les touches 's' et 'q' aux actions sauvegarder et quitter
    //on lance la boucle principale
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <image.pgm>\n", argv[0]);
        return 1;
    }
    strcpy(filename_out, argv[1]);
    char* ext = strrchr(filename_out, '.');
    if (ext) strcpy(ext, ".dif"); else strcat(filename_out, ".dif");
    img = g2x_InitImage(argv[1], &filename_in, &rootname, &extname);
    
    if (!img) { 
        fprintf(stderr, "Erreur chargement %s\n", argv[1]); 
        return 1; 
    }
    g2x_SetInitFunction(init);
    g2x_SetDrawFunction(draw);
    g2x_SetKeyAction('s', action_save, "Sauvegarder en .DIF");
    g2x_SetKeyAction('q', action_quit, "Quitter");
    return g2x_MainStart();
}