# FREDPP 0.0.103 — Guide de recette du lot 91–103

Le dernier lot vise les **treize entrées** restant à documenter comme prises en
charge dans la référence de 103 commandes : `E`, `J`, `U`, `V`, `!`, `FE`,
`JB`, `OB`, `OO`, `OR`, `OS`, `ZG` et `ZV`. Les commandes `!` et `ZG` étaient
déjà opérationnelles ; ce lot consolide leurs tests et leur comptabilisation.

> **Important : 103/103 désigne la couverture des entrées de commande, pas
> l'exactitude historique de toutes les variantes GCOS.** Les comportements
> spécifiques au système d'origine sont remplacés, lorsqu'une adaptation est
> sûre, par des équivalents C++20 portables.

| Commande | Exemple | Fonctionnement / limite du portage |
|---|---|---|
| `E` | `E(mot)/Alpha/` | Enregistre un modèle nommé, nom de 1 à 15 caractères. |
| `FE` | `FE` | Affiche les modèles nommés, par dernière définition en premier. |
| `J` | `J(fin)` ; `JT`, `JF` | En procédure, sauts vers `@(fin)` ; `JT/JF` abandonnent la suite de la ligne si la condition est respectivement vraie/fausse. |
| `U` | `U3 JM/Bonjour/` ; `UT T/Alpha/` | Répète la suite de commandes avec limite numérique ou condition `T`, `F`, `E`. Limite historique de 262 143 répétitions ; sans compte, plafond de sécurité de 10 000. Pas de saisie de bloc A/I/C imbriqué. |
| `JB` | `U3 JB` ; `JBT`, `JBF` | Interrompt la boucle active ; les suffixes dépendent du registre de condition. |
| `V` | `1D` puis `V` | Annule une modification de l'état **en mémoire** ; au plus dix états précédents. Les options de terminal, les effets externes `!` et les écritures sur disque ne sont pas annulés. |
| `ZV` | `V` puis `ZV` | Rétablit l'état qui vient d'être annulé. |
| `OB` | `O+B2` / `O-B2` | Ajuste un compteur de pages de travail (initialement 3, bornes 1–65 535). Pas de réservation de pages GCOS réelle. |
| `OO` | `O+O\F` / `O-O\F` | Affiche certains caractères de contrôle sous la forme `\F`, `\R`, etc. **Différent de `O+OO`**, option `OOO` d'affichage octal. |
| `OR` | `O-R` / `O+R` | Choix de visibilité publique. Sur POSIX, le programme règle les bits de lecture groupe/autres **sur les nouveaux fichiers créés par W**. Sur Windows, le drapeau est mémorisé mais les ACL NTFS ne sont pas modifiées. |
| `OS` | `O-S*` / `O+S*` | Désactive/réactive le sens spécial d'un sous-ensemble de métacaractères des modèles FRED ; ne change pas la ponctuation de commande. `OS&` et `OS/` conservent leurs interprétations distinctes. |
| `!` | `!echo bonjour` | Commande du shell de l'hôte ; n'exécuter que du texte de confiance. |
| `ZG` | `ZG(capture)JM/bonjour/` | Ajoute la sortie de la commande imbriquée au buffer désigné. |

Les modèles `E` se référencent avec `\E(nom)` dans les commandes saisies :

```fred
E(salut)/Bonjour/
FE
B(exemple)
O-I\F
A Bonjour
O+I\F
1T/\E(salut)/
```

L'expansion des modèles est limitée à seize passes et à 1 Mio de texte final.
Une référence à un nom absent est une erreur. La définition de modèles et la
modification des buffers sont enregistrées dans l'historique `V` ; les
consultations `FE` et `FO` n'y ajoutent pas d'état.

## Recette interactive Windows

Lancer `fredpp.exe`, puis copier :

```fred
FV
B(test103)
O-I\F
A Alpha
A Bravo
O+I\F
E(mot)/Alpha/
FE
1T/\E(mot)/
U3 JM/BOUCLE/
1D
V
1,$P
ZV
1,$P
O+B2
O-B2
O+O\F
O-O\F
O-R
FO
O+R
QQ
```

Résultats attendus :

1. `FV` annonce `0.0.103` (la date et le commit dépendent de la compilation).
2. `FE` affiche `e(mot)/Alpha/` et le test `1T` réussit.
3. Le message `BOUCLE` apparaît **trois fois**.
4. Après `V`, `1,$P` affiche `Alpha`, puis `Bravo`. Après `ZV`,
   la même impression ne montre plus que `Bravo`.
5. `FO` contient `o-r` entre `O-R` et `O+R`.

## Validation avant tag

Sous Windows, reconstruire le projet et exécuter **tous** les tests CTest.
Le lot ajoute `test_lot_91_103` et `cli_lot_91_103`. Le tag `dev-0.0.103`
ne doit être créé qu'après un résultat CTest complet et la recette manuelle.
La branche `main` et les releases stables restent inchangées.
