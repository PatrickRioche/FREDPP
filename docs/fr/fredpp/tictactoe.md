# TIC-TAC-TOE — morpion interactif en FREDPP

La procédure [library/tictactoe.fredpp](../../../library/tictactoe.fredpp)
propose un jeu de **morpion (tic-tac-toe)** entièrement écrit sous forme
de procédure FREDPP, sans bibliothèque externe ni accès réseau.

## Lancement

~~~powershell
fredpp tictactoe
~~~

Depuis les sources sur Windows, après compilation :

~~~powershell
& "$env:LOCALAPPDATA\FREDPP\build\x64-Debug\Debug\fredpp.exe" .\library\tictactoe.fredpp
~~~

Deux joueurs humains s'affrontent sur le même terminal. **X joue en premier**,
puis les joueurs alternent. Entrez le numéro d'une case libre, de **1 à 9**,
ou **FIN** (insensible à la casse) pour interrompre la partie.

## Plateau

~~~text
 +---+---+---+
 | 1 | 2 | 3 |
 +---+---+---+
 | 4 | 5 | 6 |
 +---+---+---+
 | 7 | 8 | 9 |
 +---+---+---+
~~~

Le premier joueur qui aligne trois de ses symboles sur une ligne, une colonne
ou une diagonale remporte la partie. Si les neuf cases sont prises sans
vainqueur, la partie est déclarée nulle. Une tentative de rejouer une case
occupée ou un choix non compris entre 1 et 9 ne change pas le tour de jeu.

## Exemple de partie

Pour faire gagner X sur la première ligne, saisir successivement :

~~~text
1
4
2
5
3
~~~

La grille finale est :

~~~text
 +---+---+---+
 | X | X | X |
 +---+---+---+
 | O | O | 6 |
 +---+---+---+
 | 7 | 8 | 9 |
 +---+---+---+
Le joueur X a gagne.
~~~

## Intérêt pédagogique pour FREDPP

Le programme démontre la création de neuf buffers de cases (B), la lecture
interactive d'une ligne avec \R, le déplacement du choix avec M, la
construction dynamique d'un nom de buffer avec \S, le test de motifs avec
T, la substitution S, les sauts J, les registres N, les messages JM et
JP, et la sortie de procédure JO.

Un buffer indépendant contient les deux combinaisons gagnantes XXX et
OOO. Les huit alignements sont testés en construisant dynamiquement
des motifs à partir des cases, sans modifier les cases pendant les contrôles.

Le jeu est **local et pour deux joueurs humains** : il n'inclut pas
d'adversaire informatique ni d'algorithme d'intelligence artificielle.

## Validation

Le test CTest cli_library_tictactoe lance réellement la procédure via
son nom dans une copie temporaire de library, et vérifie les scénarios :
victoire de X, victoire de O, match nul, saisie invalide, case occupée et
interruption par FIN. Aucun fichier utilisateur n'est écrit.
