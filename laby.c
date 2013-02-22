
/**
 * Copyright 2012 Eric Tremblay TIandSE@gmail.com.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * INF5071-20
 * INFOGRAPHIE
 * TP1 :: LABYRINTHE 
 * HIVER 2012
 * 12 septembre
 *
 * Prend en entre un fichier decrivant un labyrinthe et l'affiche.
 * 
 *  N.B ::
 *  Ce logiciel n'effectue aucune validation du fichier d'entre, il se doit d'etre de forme correcte sinon 
 *  rien ne garantie le bon fonctionmement du logiciel, aucun traitement d'erreur n'a ete fait en ce qui 
 *  concerne d'eventuels problemes qui pourraient etre provoques par l'utilisation du logiciel avec un 
 *  fichier d'entre incorrect.
 *
 * Exemple de fichier d'entre ( ne doit pas avoir  de * et de commentaires )
 * mursNord.ppm
 * mursOuest.ppm
 * mursSud.ppm
 * mursEst.ppm
 * pancarte.ppm
 * 1 2 0 0 0 //cette ligne correspond a une case, une grille contien 100 cases et elles doivent toutes etre decrite.
 *
 * 
 * Auteur       : Eric Tremblay
 * 
 */

// commande de compilation: "g++ -Wall -o laby laby.cpp -L/usr/X11R6/lib -lX11 -lGL -lm -lGLU -lglut"

#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <ostream>
#include <math.h>
#include <GL/freeglut.h>
#include <GL/glext.h>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

#define hauteur 64
#define largeur 32

/**
 * Une image
 */
struct StructureImage {
    GLubyte image[hauteur][largeur][4]; //definition de tableau hXl * 4

};

/**
 * GLOBAL
 */
static GLuint tableauDeTexNom[100];
GLsizei nombreDeTextures;
vector<int> vecteurDeMur; //vecteur contenant le numero des textures a appliquer sur les murs.
vector<string> vecteurNomTexture; //vecteur contenant les nom de texture en entre
vector< StructureImage *> vecteurDimage; //vecteur contenant des pointeurs vers les image

enum TypeDeMur {
    NORD, OUEST, PANCARTE, EST, SUD
};

typedef void (*Ptrfonction)(TypeDeMur, int);

/**
 * 
 */
int traitement_erreur(const string nom_procedure_concerne, const unsigned int code_erreur) {

    switch (code_erreur) {
        case 1:
        {

            cerr << nom_procedure_concerne << " Erreur dans le  nombre d'arguments." << endl;
            break;
        }
        case 2:
        {
            cerr << nom_procedure_concerne << " Erreur en ouvrant le fichier d'entree" << endl;
            break;
        }
        default:
        {
            cerr << nom_procedure_concerne << " Erreur inconnue rencontree." << endl;
            break;
        }
    }

    return -1;

}

/**
 * Procedure qui va charger chaque textures a partir du (ou des) nom(s)
 * contenu(s) dans le vecteurTexture chaque texture sera charger dans une
 * StructureImage dont l'adresse sera conserve dans le vecteurDimage.
 * Erreur: pas de traitement d'erreur, il revient a l'utilisateur de
 * fournir des variable globale correct.
 */
void chargerTexture() {
    /**
     * + 2 textures hardcode qui serviront pour le plancher et les cotes de
     * de mur.
     */
    for (int k = 0; k < nombreDeTextures + 2; ++k) {

        StructureImage * lImage;
        StructureImage uneImage;
        lImage = &uneImage;
        vecteurDimage.push_back(lImage);

        if (k < nombreDeTextures) {

            ifstream fichier(vecteurNomTexture[k].c_str());
            string s;
            int p;
            fichier >> s >> p >> p >> p; //entrer des parametre

            int i, j;

            for (i = hauteur - 1; i >= 0; --i) {
                for (j = 0; j < largeur; ++j) {
                    int c;
                    fichier >> c;
                    uneImage.image[i][j][0] = (GLubyte) c;
                    fichier >> c;
                    uneImage.image[i][j][1] = (GLubyte) c;
                    fichier >> c;
                    uneImage.image[i][j][2] = (GLubyte) c;
                    uneImage.image[i][j][3] = (GLubyte) 255;
                }
            }

        } else if (k == nombreDeTextures) {

            ifstream fichier;
            string s;
            int p;
            fichier >> s >> p >> p >> p; //entrer des parametre


            int i, j;

            for (i = hauteur - 1; i >= 0; --i) {
                for (j = 0; j < largeur; ++j) {

                    //texture harcode #1 gris a 50%
                    uneImage.image[i][j][0] = (GLubyte) 128;
                    uneImage.image[i][j][1] = (GLubyte) 128;
                    uneImage.image[i][j][2] = (GLubyte) 128;
                    uneImage.image[i][j][3] = (GLubyte) 255;
                }
            }

        } else {

            ifstream fichier;
            string s;
            int p;
            fichier >> s >> p >> p >> p; //entrer des parametre


            int i, j;

            for (i = hauteur - 1; i >= 0; --i) {
                for (j = 0; j < largeur; ++j) {

                    //texture hardcode #2 gris a 30%
                    uneImage.image[i][j][0] = (GLubyte) 77;
                    uneImage.image[i][j][1] = (GLubyte) 77;
                    uneImage.image[i][j][2] = (GLubyte) 77;
                    uneImage.image[i][j][3] = (GLubyte) 255;

                }
            }

        }

        glBindTexture(GL_TEXTURE_2D, tableauDeTexNom[k]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, largeur, hauteur, 0, GL_RGBA, GL_UNSIGNED_BYTE, vecteurDimage[k]);

    }
}

/**
 * Initialisation de l'eclairage
 */
void initLights() {

    GLfloat ambient[] = {0.4, 0.2, 0.1, 1.0};
    GLfloat position[] = {5.0, -6.0, 2.5, 1.0};
    GLfloat mat_diffuse[] = {0.6, 0.6, 0.6, 1.0};
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = {50.0};
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

}

/**
 * Quelques Initialisations : entre autre : la couleur de fond, de la profondeur, du shading, des lumiere ( appel d'un fonction ),
 * des textures leur chargements ( via une fonction ).  
 */
void init() {

    glClearColor(0.1, 0.3, 0.5, 0.0);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
    initLights();
    glEnable(GL_COLOR_MATERIAL);
    glGenTextures(nombreDeTextures + 2, tableauDeTexNom);
    chargerTexture();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

}

/**
 *
 * Ce logiciel ne construit pas des murs en tant que tel, en fait il
 * construit des moitiees de mur sur lesquels il applique ou non une
 * texture, le dos d'une moitie de mur est legerement incline ( comme si
 * le mur avec ete trancher en deux triangle rectangle d'une arrete
 * superieur en haut vers l'arrete inferieur opposee en bas ) cela permet
 * de traiter les entrees ( donnees du fichier ) dans l'ordre
 * ( N, W, C, E, S ).
 * Soit une case a la fois de la case 1 a la centieme case.
 *
 */

/**
 * Procedure qui construit un mur ( demi- mur ) sur l'axe des X et va lui
 * coller la texture correspondant sur la face appropriee
 * Si aucune texture alors la couleurs sera celle par defaut ( gris ).
 * @param: oriention, Type enumere TypeDeMur NORD ou SUD
 * @param: positionTexture type entier correspond a la postion du mur sur
 * l'axe des X dans le labyrhinte ( 5 par case X 100 cases )
 */
void murAxeDesX(TypeDeMur orientation, int positionTexture) {

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, tableauDeTexNom[vecteurDeMur[ positionTexture ] - 1]);

    glBegin(GL_QUADS);


    if (orientation == NORD) {

        glTexCoord2f(1, 1);
        glVertex3f(-4.0, 2.0, 0.05);
        glTexCoord2f(0, 1);
        glVertex3f(-5.0, 2.0, 0.05);
        glTexCoord2f(0, 0);
        glVertex3f(-5.0, 0.0, 0.05);
        glTexCoord2f(1, 0);
        glVertex3f(-4.0, 0.0, 0.05);

    } else {

        glVertex3f(-4.0, 2.0, 0.05);
        glVertex3f(-5.0, 2.0, 0.05);
        glVertex3f(-5.0, 0.0, 0.0);
        glVertex3f(-4.0, 0.0, 0.0);

    }

    if (orientation == SUD) {

        glTexCoord2f(0, 0);
        glVertex3f(-4.0, 0.0, 0.0);
        glTexCoord2f(1, 0);
        glVertex3f(-5.0, 0.0, 0.0);
        glTexCoord2f(1, 1);
        glVertex3f(-5.0, 2.0, 0.0);
        glTexCoord2f(0, 1);
        glVertex3f(-4.0, 2.0, 0.0);



    } else {


        glVertex3f(-4.0, 0.0, 0.0);
        glVertex3f(-5.0, 0.0, 0.0);
        glVertex3f(-5.0, 2.0, 0.05);
        glVertex3f(-4.0, 2.0, 0.05);

    }

    glEnd();

    glBindTexture(GL_TEXTURE_2D, nombreDeTextures + 2);
    glBegin(GL_QUADS);

    //top

    glTexCoord2f(0, 0);
    glVertex3f(-4.0, 2.0, 0.05);
    glTexCoord2f(1, 0);
    glVertex3f(-5.0, 2.0, 0.05);
    glTexCoord2f(1, 1);
    glVertex3f(-5.0, 2.0, 0.0);
    glTexCoord2f(0, 1);
    glVertex3f(-4.0, 2.0, 0.0);

    //cote droit

    glTexCoord2f(0, 0);
    glVertex3f(-4.0, 2.0, 0.05);
    glTexCoord2f(1, 0);
    glVertex3f(-4.0, 0.0, 0.05);
    glTexCoord2f(1, 1);
    glVertex3f(-4.0, 0.0, 0.0);
    glTexCoord2f(0, 1);
    glVertex3f(-4.0, 2.0, 0.0);

    glTexCoord2f(0, 0);
    glVertex3f(-5.0, 2.0, 0.05);
    glTexCoord2f(1, 0);
    glVertex3f(-5.0, 0.0, 0.05);
    glTexCoord2f(1, 1);
    glVertex3f(-5.0, 0.0, 0.0);
    glTexCoord2f(0, 1);
    glVertex3f(-5.0, 2.0, 0.0);



    glEnd();
    glFlush();

}

/**
 * Procedure qui construit un mur ( demi- mur ) sur l'axe des Y et va lui
 * coller la texture correspondant sur la face appropriee
 * Si aucune texture alors la couleurs sera celle par defaut ( gris ).
 * @param: oriention, Type enumere TypeDeMur NORD ou SUD
 * @param: positionTexture type entier correspond a la postion du mur sur
 * dans le labyrhinte ( 5 par case X 100 cases )
 * @erreur: pas de traitement d'erreur, il revient a l'utilisateur de
 * fournir un fichier d'entre valide lors de l'executuion du logiciel.
 */
void murAxeDesZ(TypeDeMur orientation, int positionTexture) {


    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, tableauDeTexNom[vecteurDeMur[ positionTexture ] - 1]);


    glBegin(GL_QUADS);

    //cote droit
    if (orientation == OUEST) {

        glTexCoord2f(0, 1);
        glVertex3f(-4.975, 2.0, 1.0);
        glTexCoord2f(0, 0);
        glVertex3f(-4.975, 0.0, 1.0);
        glTexCoord2f(1, 0);
        glVertex3f(-4.975, 0.0, 0.0);
        glTexCoord2f(1, 1);
        glVertex3f(-4.975, 2.0, 0.0);

    } else {


        glVertex3f(-4.975, 2.0, 1.0);
        glVertex3f(-5.025, 0.0, 1.0);
        glVertex3f(-5.025, 0.0, 0.0);
        glVertex3f(-4.975, 2.0, 0.0);


    }
    //cote gauche
    if (orientation == EST) {

        glTexCoord2f(0, 1);
        glVertex3f(-5.025, 2.0, 0.0);
        glTexCoord2f(0, 0);
        glVertex3f(-5.025, 0.0, 0.0);
        glTexCoord2f(1, 0);
        glVertex3f(-5.025, 0.0, 1.0);
        glTexCoord2f(1, 1);
        glVertex3f(-5.025, 2.0, 1.0);

    } else {


        glVertex3f(-5.025, 2.0, 0.0);
        glVertex3f(-4.975, 0.0, 0.0);
        glVertex3f(-4.975, 0.0, 1.0);
        glVertex3f(-5.025, 2.0, 1.0);

    }

    glEnd();

    glBindTexture(GL_TEXTURE_2D, nombreDeTextures + 2);
    glBegin(GL_QUADS);

    //face du devant
    glTexCoord2f(0, 1);
    glVertex3f(-4.975, 2.0, 1.0);
    glTexCoord2f(0, 0);
    glVertex3f(-5.025, 2.0, 1.0);
    glTexCoord2f(1, 0);
    glVertex3f(-5.025, 0.0, 1.0);
    glTexCoord2f(1, 1);
    glVertex3f(-4.975, 0.0, 1.0);

    //face du fond
    glTexCoord2f(0, 1);
    glVertex3f(-4.975, 0.0, 0.0);
    glTexCoord2f(0, 0);
    glVertex3f(-5.025, 0.0, 0.0);
    glTexCoord2f(1, 0);
    glVertex3f(-5.025, 2.0, 0.0);
    glTexCoord2f(1, 1);
    glVertex3f(-4.975, 2.0, 0.0);

    //top
    glTexCoord2f(0, 1);
    glVertex3f(-4.975, 2.0, 1.0);
    glTexCoord2f(0, 0);
    glVertex3f(-5.025, 2.0, 1.0);
    glTexCoord2f(1, 0);
    glVertex3f(-5.025, 2.0, 0.0);
    glTexCoord2f(1, 1);
    glVertex3f(-4.975, 2.0, 0.0);


    glEnd();
    glFlush();

}

/**
 * Procedure  qui construit une pancarte
 * @param: orientation de type TypeDeMur
 * @param: position de type entier
 * @erreur: pas de traitement d'erreur, il revient a l'utilisateur de
 * fournir un fichier d'entre valide lors de l'executuion du logiciel.
 */
void unePancarte(TypeDeMur orientation, int positionTexture) {

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, nombreDeTextures);

    glBegin(GL_QUADS);

    //face texture
    glTexCoord2f(0, 0);
    glVertex3f(-4.5, 0.0, 0.0);
    glTexCoord2f(1, 0);
    glVertex3f(-5.0, 0.0, 0.0);
    glTexCoord2f(1, 1);
    glVertex3f(-5.0, 1.0, 0.0);
    glTexCoord2f(0, 1);
    glVertex3f(-4.5, 1.0, 0.0);

    glEnd();
    // le reste sera gris
    glBindTexture(GL_TEXTURE_2D, nombreDeTextures + 2);
    glBegin(GL_QUADS);

    //derriere de la pancarte
    glTexCoord2f(1, 1);
    glVertex3f(-4.5, 1.0, 0.05);
    glTexCoord2f(0, 1);
    glVertex3f(-5.0, 1.0, 0.05);
    glTexCoord2f(0, 0);
    glVertex3f(-5.0, 0.0, 0.05);
    glTexCoord2f(1, 0);
    glVertex3f(-4.5, 0.0, 0.05);



    //top
    glTexCoord2f(1, 1);
    glVertex3f(-4.5, 1.0, 0.05);
    glTexCoord2f(0, 1);
    glVertex3f(-5.0, 1.0, 0.05);
    glTexCoord2f(0, 0);
    glVertex3f(-5.0, 1.0, 0.0);
    glTexCoord2f(1, 0);
    glVertex3f(-4.5, 1.0, 0.0);

    //cote droit
    glTexCoord2f(1, 1);
    glVertex3f(-4.5, 1.0, 0.05);
    glTexCoord2f(0, 1);
    glVertex3f(-4.5, 0.0, 0.05);
    glTexCoord2f(0, 0);
    glVertex3f(-4.5, 0.0, 0.0);
    glTexCoord2f(1, 0);
    glVertex3f(-4.5, 1.0, 0.0);

    //cote gauche
    glTexCoord2f(1, 1);
    glVertex3f(-5.0, 0.0, 0.0);
    glTexCoord2f(0, 1);
    glVertex3f(-5.0, 0.0, 0.05);
    glTexCoord2f(0, 0);
    glVertex3f(-5.0, 1.0, 0.05);
    glTexCoord2f(1, 0);
    glVertex3f(-5.0, 1.0, 0.0);

    glEnd();
    glFlush();

}

/**
 * Procedure qui va effectuer la translation d'un mur en fonction des
 * coordonnees tx, tz et ty fournit
 * cette procedure recoit aussi deux arametre necessaire a l'appel de la
 * fonction ( entree comme dernier parametre )
 * @param:tx de type flottant correspond a la translation sur l'axe des X
 * @param:ty de type flottant correspond a la translation sur l'axe des Y
 * @param:tz de type flottant correspond a la translation sur l'axe des Z
 * @param: orientation de type TypeDeMur face de mur sur laquelle devra
 * etre applique la texture ( si texture il doit y avoir )
 * @param: laFonction un pointeur de fonction , fonction appele soit:
 * murAxeDesX ou murAxeDesZ ou regardeCamera
 * @erreur: pas de traitement d'erreur, il revient a l'utilisateur de
 * fournir un fichier d'entre valide lors de l'executuion du logiciel.
 */
void placerMur(float tx, float ty, float tz, TypeDeMur orientation, int texture, Ptrfonction laFonction) {

    glPushMatrix();
    glTranslatef(tx, ty, tz);
    (*laFonction)(orientation, texture);
    glPopMatrix();

}

/**
 * Procedure qui cree un beau plancher de couleur grise ( 20% ) de
 * dimension 10 X 10 aux  coorconnees ( -5, 0,  0 ), ( 5, 0,  0 )
 *                                    ( -5, 0, 10 ), ( 5, 0, 10 )
 */
void lePlancher() {

    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, nombreDeTextures + 1);


    glBegin(GL_QUADS);

    glTexCoord2f(1, 1);
    glVertex3f(-5.0, 0.0, 0.0);
    glTexCoord2f(0, 1);
    glVertex3f(-5.0, 0.0, 10.0);
    glTexCoord2f(0, 0);
    glVertex3f(5.0, 0.0, 10.0);
    glTexCoord2f(1, 0);
    glVertex3f(5.0, 0.0, 0.0);

    glEnd();
    glFlush();

}

/**
 * Fonction d'affichage.
 * Parcour toute la grille de plancher sur l'axe des x de -oo vers +oo ( sans jamais sortir du plancher du labyrinthe)
 * en allant vers ++oo sur l'axe des z lorsque l'extremite du plancher ( sur les x ) est atteinte (sans encore
 * une fois sortir de la grille). 
 * Pour chaque case [x,y] les quatre mur(N,S,E,O) ainsi que la pancarte du millieu est traite. 
 * Toutes les textures sont contenues dans le vecteurDeMur et dans l'ordre. 
 */
void display() {

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    TypeDeMur compteur = NORD; //sert a indiquer quel mur ou pancarte est traiter ( dans l'appel au pointeur de fonctions )
    unsigned int texture = 0;

    Ptrfonction pointeurMurNord = murAxeDesX;
    Ptrfonction pointeurMurSud = murAxeDesX;
    Ptrfonction pointeurMurEst = murAxeDesZ;
    Ptrfonction pointeurMurOuest = murAxeDesZ;
    Ptrfonction pointeurPancarte = unePancarte;

    lePlancher();
    //parcour sur l'axe des X
    for (float z = 0; z < 10; ++z) {
        //Parcours sur l'axe des Y
        for (float x = 0; x < 10; ++x) {
            //traitement des murs et de la pancarte.
            for (int i = 0; i < 5; ++i) {

                if (texture < vecteurDeMur.size() && vecteurDeMur[texture] != 0) {

                    if (compteur == NORD) {

                        placerMur(x, 0.0, z, NORD, texture, pointeurMurNord);

                    } else if (compteur == OUEST) {

                        placerMur(x, 0.0, z, OUEST, texture, pointeurMurOuest);

                    } else if (compteur == PANCARTE) {

                        placerMur(x + 0.25, 0.0, z + 0.25, PANCARTE, texture, pointeurPancarte);

                    } else if (compteur == EST) {

                        placerMur(x + 1, 0.0, z, EST, texture, pointeurMurEst);

                    } else if (compteur == SUD) {

                        placerMur(x, 0.0, z + 1, SUD, texture, pointeurMurSud);

                    }

                }
                ++texture;

                if (compteur == NORD) {

                    compteur = OUEST;
                    ;

                } else if (compteur == OUEST) {

                    compteur = PANCARTE;

                } else if (compteur == PANCARTE) {

                    compteur = EST;

                } else if (compteur == EST) {

                    compteur = SUD;

                } else {

                    compteur = NORD;

                }

            }
        }
    }

    glFlush();

}

/**
 * Creation de la scene.
 */
void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei) w, (GLsizei) h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    double angleOuverture = 45.0;
    double ratioEcran = (GLfloat) w / (GLfloat) h;
    double near = 1.5;
    double far = 50.0;

    gluPerspective(angleOuverture, ratioEcran, near, far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 10, 0, 0, 2, 5, 0, 1, 0);

}

int main(int argc, char ** argv) {

    //le char dans lequel sera stoke un char lu dans le fichier.
    char c;
    string leNomTexture;
    const string main_erreur = "Main() :: ERREUR :: ";
    const char * nom_de_la_fenetre = " UQAM :: Hiver2012 :: INF5071 :: TP1 :: Labyrinthe :: Eric Tremblay TREE13057906 ";

    //il doit il avoir 2 argument sur la ligne de commande sinon Erreur.
    if (argc != 2) {
        traitement_erreur(main_erreur, 1);
        return -1;
    }
    //ouverture du canal vers le fichier  en entree
    ifstream ficIn(argv[1]);


    //s'il y a un probleme lors de l'ouverture du fichier Erreur()
    if (!ficIn) {
        traitement_erreur(main_erreur, 2);
        return -1;
    }

    //lit le fichier caractere par caractere jursqu'a la fin du fichier et recupere les donees utiles a
    // l'execution du programme.
    while (ficIn.get(c)) {

        if (c >= '0' && c <= '9') {


            vecteurDeMur.push_back(atoi(&c));

        } else if (c >= 'a' && c <= 'z') {

            string temporaire;
            getline(ficIn, temporaire);
            leNomTexture += c;
            leNomTexture += temporaire;
            vecteurNomTexture.push_back(leNomTexture);
            leNomTexture = "";

        }

    }

    nombreDeTextures = (GLsizei) vecteurNomTexture.size();

    //fermeture du canal d'entre.
    ficIn.close();

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(1000, 1000);
    glutInitWindowPosition(100, 150);
    glutCreateWindow(nom_de_la_fenetre);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMainLoop();

    return 0;
}


