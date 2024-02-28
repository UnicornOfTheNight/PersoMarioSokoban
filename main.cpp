#ifdef __cplusplus
    #include <cstdlib>
#else
    #include <stdlib.h>
#endif

#include <SDL/SDL.h>
#include <SDL_image.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>

#include "main.h"

void afficherMenu();
void editeurDeNiveaux();
void sauvegardeNiveaux(int carte[][12], bool brouillon);
void jouer();
void lireFichierNiveau(int carte[][12], FILE *fichier, SDL_Rect *positionMario, SDL_Surface *images[], int niveau);
void chargerCarte(int carte[][12], SDL_Rect *positionMario, SDL_Surface *images[6], int mode);
bool verificationCase(int carte[][12], char direction);

int main(int argc, char *argv[]){
    afficherMenu();
    return EXIT_SUCCESS;
}

/* Fonction permettant d'afficher le menu principal */
void afficherMenu(){
    bool continuer = true;
    SDL_Event event;
    SDL_Surface *ecran = NULL, *imageMenu = NULL;
    imageMenu = IMG_Load("sprites/menu.jpg");
    SDL_Rect position;
    position.x = 0;
    position.y = 0;

    /* AFFICHAGE DE LA FENETRE */
    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetIcon(IMG_Load("sprites/caisse.jpg"), NULL);
    ecran = SDL_SetVideoMode(476, 408, 32, SDL_HWSURFACE);
    SDL_WM_SetCaption("TP Mario Sokoban (menu)", NULL);

    SDL_BlitSurface(imageMenu, NULL, ecran, &position);
    SDL_Flip(ecran);

    while (continuer)
    {
        SDL_WaitEvent(&event); /* Récupération de l'événement dans event */
        switch(event.type) /* Test du type d'événement */
        {
            case SDL_QUIT:
                continuer = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)//Savoir quelle touche a ete utilisée
                {
                    case SDLK_ESCAPE:
                        continuer = false;
                        break;

                    case SDLK_KP1:
                        continuer = false;
                        jouer();
                        break;

                    case SDLK_KP2:
                        continuer = false;
                        editeurDeNiveaux();
                        break;
                }
                break;
        }
    }
    SDL_FreeSurface(imageMenu);
    SDL_Quit();
}

/* Fonction permettant d'afficher l'éditeur de niveaux */
void editeurDeNiveaux(){
    bool continuer = true;
    int element = 1, carte[15][12] = {0}, nbCaisses = 0, nbObjectifs = 0;

    SDL_Event event;
    SDL_Surface *ecran = NULL, *imageFond = NULL, *elementDeplace = NULL;
    SDL_Surface *imageCaisse = NULL, *imageMur = NULL, *imageObjectif = NULL, *imageMario = NULL;

    SDL_Rect position, positionElement;
    position.x = 0;
    position.y = 0;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetIcon(IMG_Load("sprites/caisse.jpg"), NULL);
    ecran = SDL_SetVideoMode(972, 408, 32, SDL_HWSURFACE);
    SDL_WM_SetCaption("TP Mario Sokoban (editeur)", NULL);

    imageFond = IMG_Load("sprites/editeur.png");
    imageCaisse = IMG_Load("sprites/caisse.jpg");
    imageMario = IMG_Load("sprites/mario_bas.gif");
    imageObjectif = IMG_Load("sprites/objectif.png");
    imageMur = IMG_Load("sprites/mur.jpg");

    elementDeplace = imageMur;

    SDL_Surface *img[6] = {imageCaisse, imageMario, imageMur, imageObjectif, ecran, NULL};

    FILE  *fichier = NULL;
    int x = 0, y= 0;
    fichier = fopen("brouillon.txt", "r"); //On ouvre le brouillon en lecture

    if(fichier != NULL){
        int caractereActuel = 0;
        // Boucle de lecture des caractères un à un
        do
        {
            caractereActuel = fgetc(fichier); // On lit le caractère

            switch (caractereActuel){ //Selon le caractère on remplit le tableau
                case '0':
                    carte[x][y] = VIDE;
                    break;

                case '1':
                    carte[x][y] = MUR;
                    break;

                case '2':
                    carte[x][y] = CAISSE;
                    break;

                case '3':
                    carte[x][y] = OBJECTIF;
                    break;

                case '4':
                    carte[x][y] = MARIO;
                    break;
            }

            x++;

            if(x==LARGEUR){
                x = 0;
                y++;
            }
        } while (caractereActuel != EOF); // On continue tant que fgetc n'a pas retourné EOF (fin de fichier)

        fclose(fichier);
    }

    chargerCarte(carte, &position, img, 0); //0 est le mode pour indiquer qu'on est dans l'éditeur et non en train de jouer

    bool drag = false;
    bool marioPlace = false;

    while (continuer){
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
                break;

            case SDL_MOUSEMOTION:
                //L'élément suit la souris
                positionElement.x = event.motion.x;
                positionElement.y = event.motion.y;

                //On verifie que le joueur a bien cliquer pour placer l'élément
                if(drag == true){
                    int x = round(event.motion.x / 34);
                    int y = round(event.motion.y / 34);
                    carte[x][y] = element;
                }
                chargerCarte(carte, &position, img, 0);
                SDL_BlitSurface(elementDeplace, NULL, ecran, &positionElement);
                break;

            case SDL_MOUSEBUTTONUP:
                drag = false;
                break;

            case SDL_MOUSEBUTTONDOWN:
                //Si on fait un clique gauche c'est pour poser un élément
                if (event.button.button == SDL_BUTTON_LEFT){
                    if(element != MARIO || !marioPlace){ // Mario ne peut pas être placé 2 fois
                        //On arrondit la position sachant que les images font 34x34
                        int x = round(event.motion.x / 34);
                        int y = round(event.motion.y / 34);
                        carte[x][y] = element;
                        chargerCarte(carte, &position, img, 0);
                        drag = true;
                        if(element == MARIO) marioPlace = true;
                    }

                }else if(event.button.button == SDL_BUTTON_RIGHT){ //Clic gauche = effacer l'element
                    int x = round(event.motion.x / 34);
                    int y = round(event.motion.y / 34);
                    carte[x][y] = VIDE;
                    chargerCarte(carte, &position, img, 0);
                    if(carte[x][y] == MARIO) marioPlace = false;

                }
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        continuer = false;
                        afficherMenu();
                        break;

                    case SDLK_KP1:
                        element = MUR;
                        elementDeplace = imageMur;
                        break;

                    case SDLK_KP2:
                        element = CAISSE;
                        elementDeplace = imageCaisse;
                        break;

                    case SDLK_KP3:
                        element = OBJECTIF;
                        elementDeplace = imageObjectif;
                        break;

                    case SDLK_KP4:
                        element = MARIO;
                        elementDeplace = imageMario;

                        break;

                    case SDLK_s: //touche pour sauvegarder

                        for(int j = 0; j < HAUTEUR; j++){
                            for(int i = 0; i < LARGEUR; i++){
                                if(carte[i][j] == OBJECTIF) nbObjectifs++;
                                else if(carte[i][j] == CAISSE) nbCaisses++;
                            }
                        }

                        if(nbCaisses == nbObjectifs && nbCaisses > 0)
                            sauvegardeNiveaux(carte, false); // Niveau sauvegarder en temps que brouillon
                        else
                            sauvegardeNiveaux(carte, true); // Niveau sauvegarder en temps que niveau a jouer

                        continuer = false;
                        afficherMenu();
                        break;
                }
        }
        position.x = 0;
        position.y = 0;

        SDL_Flip(ecran);
    }


    SDL_FreeSurface(imageCaisse);
    SDL_FreeSurface(imageFond);
    SDL_FreeSurface(imageMario);
    SDL_FreeSurface(imageMur);
    SDL_FreeSurface(imageObjectif);
    SDL_Quit();
}

/* Fonction pour sauvegarder le niveau dans un fichier */
void sauvegardeNiveaux(int carte[][12], bool brouillon){
    bool continuer = true;

    FILE* fichier = NULL;
    SDL_Event event;
    SDL_Surface *ecran = NULL, *imageFond = NULL;
    SDL_Rect position;
    position.x = 0;
    position.y = 0;

    SDL_Init(SDL_INIT_VIDEO);
    ecran = SDL_SetVideoMode(554, 155, 32, SDL_HWSURFACE);
    SDL_WM_SetCaption("TP Mario Sokoban (sauvegarde)", NULL);

    imageFond = IMG_Load("sprites/fondSauvegarde.png");
    SDL_BlitSurface(imageFond, NULL, ecran, &position);
    SDL_Flip(ecran);

    while (continuer){
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        continuer = false;
                        break;

                    case SDLK_KP1:
                        fichier = fopen("brouillon.txt", "w");
                        continuer = false;
                        break;

                    case SDLK_KP2:
                        if(brouillon){
                            fichier = fopen("brouillon.txt", "w");
                        }else{
                            fichier = fopen("levels.txt", "a");
                        }
                        continuer=false;
                        break;
                }
        }
    }


    if(fichier != NULL){
        fprintf(fichier, "\n");
        for(int y = 0; y < HAUTEUR; y++){
            for(int x = 0; x < LARGEUR; x++){
                    int elementInt = carte[x][y];
                    char elementString[1];
                    itoa(elementInt, elementString, 10);

                    fprintf(fichier, elementString);
            }
        }
        fclose(fichier);
    }

    //Si le niveau a été enregistrer comme niveau a jouer alors on l'efface du brouillon
    if(!brouillon){
        fichier = fopen("brouillon.txt", "w");

        if(fichier != NULL){
            fprintf(fichier, "");
        }
        fclose(fichier);
    }

    SDL_FreeSurface(imageFond);
    SDL_FreeSurface(ecran);
    SDL_Quit();
}

/* Fonction permettant d'afficher la console de jeu */
void jouer(){
    int carte[15][12] = {0}, carteTmp[LARGEUR][HAUTEUR] = {0}, i =0, j=0;
    bool continuer = true;

    SDL_Event event;
    SDL_Surface *ecran = NULL, *imageMario = NULL, *imageMur = NULL, *imageBoite = NULL, *imageBoiteOk = NULL, *imageObjectif = NULL;
    SDL_Rect position, positionMario, positionMarioTmp, positionMur, positionBoite, positionObjectif;

    FILE* fichier = NULL;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_WM_SetIcon(IMG_Load("sprites/caisse.jpg"), NULL);
    ecran = SDL_SetVideoMode(510, 408, 32, SDL_HWSURFACE);
    SDL_WM_SetCaption("Mario Sokoban (jeu)", NULL);

    //Chargement des images
    imageMur = IMG_Load("sprites/mur.jpg");
    imageBoite = IMG_Load("sprites/caisse.jpg");
    imageObjectif = IMG_Load("sprites/objectif.png");
    imageMario = IMG_Load("sprites/mario_bas.gif");
    imageBoiteOk = IMG_Load("sprites/caisse_ok.jpg");

    SDL_Surface *img[6] = {imageBoite, imageMario, imageMur, imageObjectif, ecran, imageBoiteOk};

    int numeroNiveau = 1;
    lireFichierNiveau(carte, fichier, &positionMario,img, numeroNiveau);
    chargerCarte(carte, &positionMario, img, 1);

    /*GESTION DES DEPLACEMENTS*/
    bool avancer = false;
    int nbNvx = NB_NVX;
    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = false;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        continuer = false;
                        afficherMenu();
                        break;

                    case SDLK_DOWN:
                        //On enregistre l'état du tableau dans un tableau tmp
                        for(int j=0; j<HAUTEUR; j++){
                            for(int i = 0; i <LARGEUR; i++){
                                carteTmp[i][j] = carte[i][j];
                            }
                        }
                        positionMarioTmp = positionMario; //On enregistre sa position avant qu'elle change

                        avancer = verificationCase(carte, 'b');
                        imageMario = IMG_Load("sprites/mario_bas.gif");
                        img[1] =imageMario; //on met a jour l'image du mario dans le tableau
                        if(avancer){
                            positionMario.y += 34;
                            chargerCarte(carte, &positionMario, img,1);
                        }
                        break;

                    case SDLK_UP:
                        for(int j=0; j<HAUTEUR; j++){
                            for(int i = 0; i <LARGEUR; i++){
                                carteTmp[i][j] = carte[i][j];
                            }
                        }
                        positionMarioTmp = positionMario;

                        avancer = verificationCase(carte, 'h');
                        imageMario = IMG_Load("sprites/mario_haut.gif");
                        img[1] = imageMario;
                        if(avancer){
                            positionMario.y -= 34;
                            chargerCarte(carte, &positionMario, img, 1);
                        }
                        break;

                    case SDLK_LEFT:
                        for(int j=0; j<HAUTEUR; j++){
                            for(int i = 0; i <LARGEUR; i++){
                                carteTmp[i][j] = carte[i][j];
                            }
                        }
                        positionMarioTmp = positionMario;

                        avancer = verificationCase(carte, 'g');
                        imageMario = IMG_Load("sprites/mario_gauche.gif");
                        img[1] = imageMario;
                        if(avancer){
                            positionMario.x -= 34;
                            chargerCarte(carte, &positionMario, img, 1);
                        }
                        break;

                    case SDLK_RIGHT:
                        for(int j=0; j<HAUTEUR; j++){
                            for(int i = 0; i <LARGEUR; i++){
                                carteTmp[i][j] = carte[i][j];
                            }
                        }
                        positionMarioTmp = positionMario;

                        avancer = verificationCase(carte, 'd');
                        imageMario = IMG_Load("sprites/mario_droite.gif");
                        img[1] = imageMario;

                        if(avancer){
                            positionMario.x += 34;
                            chargerCarte(carte, &positionMario, img, 1);
                        }
                        break;

                    case SDLK_r: //Permet au joueur de remettre la carte a 0
                        lireFichierNiveau(carte, fichier, &positionMario, img, numeroNiveau);
                        imageMario = IMG_Load("sprites/mario_bas.gif");
                        img[1] = imageMario;
                        chargerCarte(carte, &positionMario, img, 1);
                        break;

                    case SDLK_BACKSPACE: //Permet au joueur de revenir en arrière d'un coup
                        for(int j=0; j<HAUTEUR; j++){
                            for(int i = 0; i <LARGEUR; i++){
                                carte[i][j] = carteTmp[i][j];
                            }
                        }
                        positionMario = positionMarioTmp;
                        chargerCarte(carte, &positionMario, img,1);
                        break;
                }
                break;
        }

        //On vérifie si le niveau est fini ou non
        bool gagner = true;
        for(int y = 0; y<HAUTEUR; y++){
            for(int x=0; x<LARGEUR;x++){
                if(carte[x][y]==OBJECTIF){ // Si il reste des objectif le niveau n'est pas fini
                    gagner = false;
                }
            }
        }

        if(gagner){//si le niveau est fini on verifie si il existe d'autres niveau a faire
            if(numeroNiveau == nbNvx){
                continuer = false;
                afficherMenu();
            }else{
                numeroNiveau++;
                imageMario = IMG_Load("sprites/mario_bas.gif");
                img[1] = imageMario;
                lireFichierNiveau(carte, fichier, &positionMario, img, numeroNiveau);
                chargerCarte(carte, &positionMario, img, 1);
            }
        }

    }

    SDL_FreeSurface(imageMur);
    SDL_FreeSurface(imageBoite);
    SDL_FreeSurface(imageMario);
    SDL_FreeSurface(imageObjectif);
    SDL_Quit();
}

/* Fonction permettant de lire le fichier contenant les niveaux de jeu */
void lireFichierNiveau(int carte[][12], FILE *fichier, SDL_Rect *positionMario, SDL_Surface *images[],int niveau){
    SDL_Rect position;
    position.x = 0;
    position.y = 0;
    int i = 0, j= 0, niveauLu = 1, nbNvx = 1;

    fichier = fopen("levels.txt", "r");

    if(fichier != NULL){
        int caractereActuel = 0;
        do
        {
            caractereActuel = fgetc(fichier);

            if(caractereActuel == '\n'){ //Si le caractere est un retour a la ligne on change de niveau
                niveauLu++;
                i= 0;
                j = 0;
                position.x = 0;
                position.y = 0;
                nbNvx++;
            }
            else if(niveauLu == niveau){ //Si le niveau lu correspond au niveau demander on remplit le tableau
                switch (caractereActuel){
                    case '0':
                        carte[i][j] = VIDE;
                        break;

                    case '1':
                        carte[i][j] = MUR;
                        break;

                    case '2':
                        carte[i][j] = CAISSE;
                        break;

                    case '3':
                        carte[i][j] = OBJECTIF;
                        break;

                    case '4':
                        carte[i][j] = MARIO;
                        *positionMario = position;
                        break;
                }

                i++;
                if(i==LARGEUR){
                    i = 0;
                    j++;
                    position.x = 0;
                    position.y += 34;
                }else{
                    position.x += 34;
                }
            }

        } while (caractereActuel != EOF); // On continue tant que fgetc n'a pas retourné EOF (fin de fichier)

        fclose(fichier); //On ferme le fichier qui a été ouvert
    }
    else
        printf("Impossible d'ouvrir le fichier.");

    NB_NVX = nbNvx;
}

/* Fonction permettant de charger la carte */
void chargerCarte(int carte[][12], SDL_Rect *positionMario, SDL_Surface *images[6], int mode){
    SDL_Rect position;
    position.x = 0;
    position.y = 0;

    SDL_Surface *fond = NULL;
    fond = IMG_Load("sprites/editeur.png");

    //Si on est en train de jouer on met un fond blanc et si on edite on met l'image en fond
    if(mode == 1) SDL_FillRect(images[4], NULL, SDL_MapRGB(images[4]->format, 255, 255, 255));
    else SDL_BlitSurface(fond, NULL, images[4], &position);

    for(int y = 0; y<HAUTEUR; y++){
        for(int x=0; x<LARGEUR;x++){
            switch (carte[x][y]){
                case MUR:
                    SDL_BlitSurface(images[2], NULL, images[4], &position);
                    break;

                case CAISSE:
                    SDL_BlitSurface(images[0], NULL, images[4], &position);
                    break;

                case OBJECTIF:
                    SDL_BlitSurface(images[3], NULL, images[4], &position);
                    break;

                case MARIO:
                    if(mode == 1) SDL_BlitSurface(images[1], NULL, images[4], positionMario);
                    else SDL_BlitSurface(images[1], NULL, images[4], &position);
                    break;

                case CAISSE_OK:
                    SDL_BlitSurface(images[5], NULL, images[4], &position);
                    break;
            }
            position.x += 34;
        }
        position.x = 0;
        position.y += 34;
    }
    SDL_Flip(images[4]);
}

/* Fonction pour verifier que mario peut avancer */
bool verificationCase(int carte[][12], char direction){
    bool avancer = false;
    bool verifFait = false;

    //on parcours le tableau
    for(int y = 0; y<HAUTEUR; y++){
        for(int x=0; x<LARGEUR;x++){
            if(carte[x][y] == MARIO){
                verifFait = true;
                switch(direction){ //Selon la direction on fait des verifications
                    case 'h':
                        if(carte[x][y-1] == VIDE){ //Si la prochaine case est vide le joueur peut avancer
                            carte[x][y-1] = MARIO;
                            carte[x][y] = VIDE;
                            avancer = true;
                        }else if(carte[x][y-1] == CAISSE){ //Si la prochaine case est une caisse
                            if(carte[x][y-2] == VIDE){ //On verifie que la case après la caisse est vide
                                carte[x][y-2] = CAISSE;
                                carte[x][y-1] = MARIO;
                                carte[x][y] = VIDE;
                                avancer = true;
                            }else if(carte[x][y-2] == OBJECTIF){ //Si elle n'est pas vide on verifie si c'est un objectif
                                carte[x][y-2] = CAISSE_OK;
                                carte[x][y-1] = MARIO;
                                carte[x][y] = VIDE;
                                avancer = true;
                            }
                        }
                        break;

                    case 'b':
                        if(carte[x][y+1] == VIDE){
                            carte[x][y+1] = MARIO;
                            carte[x][y] = VIDE;
                            avancer = true;
                        }else if(carte[x][y+1] == CAISSE){
                            if(carte[x][y+2] == VIDE){
                                carte[x][y+2] = CAISSE;
                                carte[x][y+1] = MARIO;
                                carte[x][y] = VIDE;
                                avancer = true;
                            }else if(carte[x][y+2] == OBJECTIF){
                                carte[x][y+2] = CAISSE_OK;
                                carte[x][y+1] = MARIO;
                                carte[x][y] = VIDE;
                                avancer = true;
                            }
                        }
                        break;

                    case 'd':
                        if(carte[x+1][y] == VIDE){
                            carte[x+1][y] = MARIO;
                            carte[x][y] = VIDE;
                            avancer = true;
                        }else if(carte[x+1][y] == CAISSE){
                            if(carte[x+2][y] == VIDE){
                                carte[x+2][y] = CAISSE;
                                carte[x+1][y] = MARIO;
                                carte[x][y] = VIDE;
                                avancer = true;
                            }else if(carte[x+2][y] == OBJECTIF){
                                carte[x+2][y] = CAISSE_OK;
                                carte[x+1][y] = MARIO;
                                carte[x][y] = VIDE;
                                avancer = true;
                            }
                        }
                        break;

                    case 'g':
                        if(carte[x-1][y] == VIDE){
                            carte[x-1][y] = MARIO;

                            carte[x][y] = VIDE;
                            avancer = true;
                        }else if(carte[x-1][y] == CAISSE){
                            if(carte[x-2][y] == VIDE){
                                carte[x-2][y] = CAISSE;
                                carte[x-1][y] = MARIO;
                                carte[x][y] = VIDE;
                                avancer = true;
                            }else if(carte[x-2][y] == OBJECTIF){
                                carte[x-2][y] = CAISSE_OK;
                                carte[x-1][y] = MARIO;
                                carte[x][y] = VIDE;
                                avancer = true;
                            }
                        }
                        break;
                }

                break;
            }
            if(verifFait)//si on a passer les verification pas la peine de continuer
                break;
        }
        if(verifFait)//si on a passer les verification pas la peine de continuer
            break;

    }
    return avancer;
}
