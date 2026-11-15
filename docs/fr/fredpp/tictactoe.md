# TIC-TAC-TOE — morpion interactif en FREDPP

La procédure [library/tictactoe.fredpp](../../../library/tictactoe.fredpp)
propose un jeu de **morpion (tic-tac-toe)** entièrement écrit sous forme
de procédure FREDPP, sans bibliothèque externe ni accès réseau.

## Lancement

~~~powershell
fredpp tictactoe
fredpp tictactoe solo
~~~

Depuis les sources sur Windows, après compilation :

~~~powershell
& "$env:LOCALAPPDATA\FREDPP\build\x64-Debug\Debug\fredpp.exe" .\library\tictactoe.fredpp
~~~

Deux modes sont disponibles :

- **Deux joueurs** : `fredpp tictactoe` (comme précédemment), X et O sont humains.
- **Solo** : `fredpp tictactoe solo` ; vous jouez **X** et FREDPP joue **O**.
  L'ordinateur utilise une stratégie **Minimax optimale** : il peut gagner
  ou faire match nul, mais il ne peut pas perdre lorsque le jeu commence
  sur une grille vide.

X commence dans les deux modes. Entrez le numéro d'une case libre, de
**1 à 9**, ou **FIN** (insensible à la casse) pour interrompre la partie.
En solo, l'ordinateur joue automatiquement après votre coup.

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

### Stratégie Minimax du mode solo

La procédure reste **100 % FREDPP au moment du jeu**. Les décisions de
l'ordinateur sont calculées *à l'avance* par un générateur Python (hors jeu),
puis stockées dans le fichier FREDPP sous forme de motifs `T`, suivis de
sauts `J` et de substitutions `S`.

Le générateur reproduit l'algorithme Minimax : l'ordinateur maximise le
résultat (+1 victoire, 0 nul, -1 défaite), en supposant des réponses humaines
optimales. À résultat égal, il privilégie le centre, puis les coins, puis
les bords. Les **285 positions distinctes atteignables au tour de l'ordinateur**
sont encodées dans des motifs compacts, sans moteur de jeu en C++ et sans
dépendance Python à l'exécution.

La vérification exhaustive de tous les choix humains contre cette stratégie
produit **482 victoires de FREDPP, 153 matchs nuls et aucune victoire humaine**
(635 séquences complètes, dont plusieurs peuvent atteindre la même position).

Reproduire la vérification, avec Python 3 :

~~~powershell
python .\scripts\generate_tictactoe_policy.py --check
~~~

Reconstruire la table si le jeu est modifié :

~~~powershell
python .\scripts\generate_tictactoe_policy.py --write
~~~

Le générateur et ses résultats sont transparents ; aucun code externe n'est
appelé pendant une partie. Il s'agit de recherche exhaustive et d'une table
de décisions déterministe, et non d'une IA conversationnelle.

## Validation

Le test CTest cli_library_tictactoe lance réellement la procédure via
son nom dans une copie temporaire de library, et vérifie les scénarios :
victoire de X, victoire de O, match nul, saisie invalide, case occupée et
interruption par FIN, plus les scénarios du mode solo (coups optimaux,
victoire informatique, nul, erreurs de saisie, abandon). Aucun fichier utilisateur n'est écrit.
