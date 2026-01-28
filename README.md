# 🍄 Perso Mario Sokoban

![Language](https://img.shields.io/badge/Language-C-blue)
![Library](https://img.shields.io/badge/Library-SDL-orange)
![Genre](https://img.shields.io/badge/Genre-Puzzle%20Game-green)

**Perso Mario Sokoban** est une implémentation du célèbre jeu de réflexion "Sokoban" (gardien d'entrepôt en japonais), mettant en scène Mario. Ce projet a été réalisé pour maîtriser le langage C et la bibliothèque multimédia SDL.

## 🎮 Le Principe

Le but du jeu est simple mais le défi est grand :
1.  Vous contrôlez **Mario**.
2.  Vous devez pousser des **caisses** pour les placer sur des **objectifs** (points rouges/cibles).
3.  **Contraintes** :
    * Vous ne pouvez pousser qu'une seule caisse à la fois.
    * Vous ne pouvez pas tirer les caisses.
    * Attention à ne pas bloquer une caisse dans un coin !

[Image of sokoban game level layout]

## ✨ Fonctionnalités

* **Gestion des déplacements** : Haut, Bas, Gauche, Droite.
* **Collisions** : Gestion des murs et des caisses entre elles.
* **Condition de victoire** : Détection automatique lorsque toutes les caisses sont sur les objectifs.
* **Chargement de niveaux** : Lecture des niveaux depuis un fichier (ex: `niveaux.lvl`) ou un tableau codé en dur.
* **Éditeur de niveau** (Si implémenté) : Possibilité de créer ses propres casse-têtes.

## 🛠 Technologies Utilisées

* **Langage** : C
* **Bibliothèque Graphique** : SDL (Simple DirectMedia Layer) + SDL_Image.
* **Assets** : Sprites 2D (Mario, Mur, Caisse, Objectif).

## 🚀 Installation & Compilation

Pour lancer le jeu, vous devez avoir la bibliothèque SDL installée sur votre machine.

### Prérequis (Linux / Mac)
* `gcc`
* `libsdl1.2-dev` ou `libsdl2-dev` (selon la version utilisée)
* `libsdl-image1.2-dev` ou `libsdl2-image-dev`

### Compilation (Commande type)
```bash
gcc main.c jeu.c editeur.c -o sokoban -lSDL -lSDL_image
