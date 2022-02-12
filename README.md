# Object Oriented Ray Tracing 
# Projet d'IN204 - Orienté Objet: Programmation et génie Logiciel

Auteurs: Alexis OLIVEIRA DA SILVA, 
         Bruno MACEDO SANCHES

## Instalation dans linux

Pour installer les dépendances nécessaires au projet dans Linux, exécutez les commandes

```
sudo apt-get update
sudo apt-get install make
make install
```

## Compilation

Pour compiler le projet exécutez

`make all`

Le executable génére est localisé sur **bin/ray_tracing.exe**

## Exécution

Pour exécuter le projet il faut lancer l'exécutable en utilisant ./bin/ray_tracing.exe, le terminal sera affiche avec les options disponibles.

## Options
**Enter** - Cette option lance le rendu de la scène, dans le cas qui aucune scène est chargé ou crée, le programme éxecute une scène default

**c** - Cette option lance le tutorial dans lequel le utilisateur pourra définir les paramètres et itens présents dans la scène

**r** - Cette option demande au utilisateur de charger un fichier XML dans le programme avec les paramètres et itens de la scène. Il y a deux fichiers d'example sur le dossier *data*

**p** - Charge une scène d'example pré-definit dans le programme

**s** - Sauvegarde la scène crée ou chargé sur un format XML qui peut être chargé après

**i** - Sauvegarde l'image génére lors de rendu de la scène

**q** - Quitte le programme

## Paramètres de la scène

### Paramètres d'image
    1 Image Width en pixels
    2 Image Height en pixels
    3 Samples per pixel (Rayions capturés dans chaque pixel)
    4 Max depth (Quantité maximale de colisions d'un rayon, après il ne porte plus de lumière)

### Paramètres de la caméra
    1 Camera origin point 3D (Ex: 0, 0, 0)
    2 Point in the space that the camera is looking at
    3 View-up-vector (Définit le rotation de la camera au tour de l'axis Origin-Point looking at)
    4 Field of view en degrèes
    5 Aperture (diaprahgme ou ouverture de la caméra, plus de lumière reçu si ouverture est plus grande)
    6 Distace focus
    7 Temps initiale
    8 Temps finale (La diference définit la vitesse de capture de la camera, plus de mouvement de objets mouvants)
    
### Paramètres de objets
Les objets sont de deux types, spheres et spheres mouvants, les deux ont un radius e une position, les spheres mouvants on deux positions et deux temps, un pour chaque position.

Les matériels disponibles pour les objets sont de trois types
    
    1 Diélectrique
    2 Metalique
    3 Lambertien

# Réferences 
[1] Ray tracing in one weekend. https://raytracing.github.io/books/RayTracingInOneWeekend.html

[2] NCURSES Programming HOW TO. https://tldp.org/HOWTO/NCURSES-Programming-HOWTO/

[3] TinyXML2 tutorial. https://shilohjames.wordpress.com/2014/04/27/tinyxml2-tutorial/

[4] SFML Documentation v2.5.1. https://www.sfml-dev.org/documentation/2.5.1/

