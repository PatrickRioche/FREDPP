# État des commandes FRED

Ce document suit l'état d'implémentation. Les pages situées sous `docs/fr/reference/commandes/` restent inchangées et seules celles des commandes disponibles sont embarquées.

Pour un emploi pratique des **90 commandes prises en charge en développement
v0.0.90** (syntaxe, effet, exemple, limites), consulter le
[guide de recette de FREDPP 0.0.90](../fr/fredpp/guide-commandes-0.0.90.md),
ainsi que le [guide 0.0.80](../fr/fredpp/guide-commandes-0.0.80.md).
Il complète les fiches historiques embarquées dans l'aide sans les altérer.

## Couverture de la référence historique

| Famille | Total référence | Implémenté / homologué | Reste |
|---|---:|---:|---:|
| Commandes principales | 22 | 18 | 4 |
| Commandes spéciales | 7 | 6 (`\"`, `=`, `#`, `:`, `@`, `\\`) | 1 |
| Famille F | 7 | 6 (`FB`, `FF`, `FN`, `FO`, `F?`, `FV`) | 1 |
| Famille J | 5 | 4 (`JB` restant ; `JE`, `JM`, `JO`, `JP` disponibles) | 1 |
| Famille N | 21 | 21 (famille complète en développement) | 0 |
| Famille O | 24 | 20 (`OC`, `OI(`, `OI\\F`, `OIA`, `OIC`, `OIE`, `OIF`, `OIL`, `OIR`, `OIU`, `OM`, `OOL`, `OOO`, `OOU`, `OP`, `OQ`, `OS&`, `OS/`, `OSD`, `OT`) | 4 |
| Famille Z | 17 | 15 (`ZA`, `ZB`, `ZC`, `ZD`, `ZI`, `ZK`, `ZL`, `ZM`, `ZO`, `ZP`, `ZR`, `ZS`, `ZT`, `ZU`, `ZW`) | 2 |
| **Total** | **103** | **90** | **13** |

La couverture fonctionnelle provisoire de la référence est de **90 / 103**, soit environ **87,4 %**. Elle inclut les commandes en validation Windows ; elle ne signifie pas une homologation historique complète.

**Numérotation de `dev` :** la version `0.0.N` reprend le nombre `N` de commandes historiques référencées et prises en charge dans ce tableau. Ainsi, **90 commandes = `0.0.90`**. Les corrections internes sans nouvelle commande ne changent pas ce numéro. CMake vérifie l'accord entre la version et le total à chaque configuration. Le tag `dev-0.0.80` a été publié après **67/67 tests Windows** et la recette interactive ; **`dev-0.0.90` attend la validation Windows par l'utilisateur**, directement sur `dev` et sans modifier `main`.

| Commande | État dans FREDPP | Aide embarquée |
|---|---|:---:|
| A | Disponible | Oui |
| B | Disponible, fonctions avancées encore à compléter | Oui |
| C | Disponible | Oui |
| D | Disponible | Oui |
| F | Disponible : sans argument, affiche nom, curseur, nombre de lignes, fichier associé et état modifié du buffer courant ; avec un nom de fichier, modifie uniquement son association, sans lecture/écriture ni effacement de l'indicateur modifié | Oui (`?f`) |
| FB | Disponible; ordre décroissant de dernière utilisation, ligne courante, nombre de lignes, fichier associé et état modifié | Oui (`?fb`) |
| FF | Disponible; liste de tous les buffers associés à un fichier, triée par nom de buffer, avec indication de modification (`?`) | Oui (`?ff`) |
| FN | Disponible : affiche les registres numériques créés, triés par nom, sous la forme `n(nom):valeur`, sans modifier leur état | Oui (`?fn`) |
| `F?` | Disponible : affiche toujours le buffer courant, puis les autres buffers modifiés triés par nom, avec chemin associé éventuel et marque `?` pour les buffers modifiés ; lecture seule | Oui (`?f?`, alias `?fq`) |
| FO | Disponible; affiche les options effectivement en service | Oui (`?fo`) |
| FV | Affiche la version compilée FREDPP, la date de compilation et le commit source, en remplacement de l'ancien « file-code » GCOS | Oui (`?fv`) |
| G | Disponible avec une commande imbriquée P, D, Z ou S | Oui |
| JE | Message d'erreur utilisateur sur une ligne ; `O+Q` demande l'arrêt après le message | Oui (`?je`) |
| JO | `JO`, `JOT`, `JOF` interrompent la procédure courante (respectivement sans condition, si vraie, si fausse). La sortie d'une itération G/U reste à homologuer | Oui (`?jo`) |
| I | Disponible | Oui |
| K | Copie vers un buffer avec remplacement ; candidat en validation historique | Oui (`?k`) |
| `\"` | Disponible : tout le reste de la ligne est ignoré comme commentaire | Oui (`?\"`) |
| `=` | Disponible : affiche le numéro de la ligne courante ou de la ligne adressée (`2=`, `$=`) sans déplacer le curseur | Oui (`?=`) |
| `#` | Affiche la valeur du compteur partagé sans modifier son état | Oui (`?#`) |
| `:` | `:n` affiche les n lignes voisines de la ligne courante (n=5 par défaut) ; le curseur se place à la dernière ligne imprimée | Oui (`?window`) |
| `@` | `@(nom)` définit une étiquette de procédure, déjà reconnue par les sauts de procédure ; sans effet direct dans le REPL | Oui (`?@`) |
| `\\` | `\\n` attend n secondes (de 0 à 86 400), `\\0` est immédiat | Oui (`?wait`) |
| JM | Disponible : message avec retour à la ligne, formes `JM texte` et `JM/texte/` | Oui (`?jm`) |
| JP | Disponible : message sans retour à la ligne, formes `JP texte` et `JP/texte/` | Oui (`?jp`) |
| L | Disponible avec un nom de fichier | Oui |
| M | Disponible sous forme provisoire, réalignement historique requis | Oui |
| `N:` | Disponible en mode interactif et en procédure : affectation d'un registre numérique (entier signé ou `#`, `.` et `$`) et mise à jour du compteur | Oui (`?n:`) |
| `N+`, `N-`, `N*` | Addition, soustraction et multiplication d'un registre, avec vérification des dépassements d'entiers signés ; opérandes littéraux, `#`, `.`, `$` ou `\N(nom)` | Oui (`?n+`, `?n-`, `?n*`) |
| `N/`, `N%` | Division entière vers zéro et reste signé, avec refus du diviseur nul et du cas `INT64_MIN / -1` | Oui (`?n/`, `?n%`) |
| `N=`, `N<`, `N>` | Comparaison avec la valeur du registre ; modifie la condition sans changer la valeur du registre | Oui (`?n=`, `?n<`, `?n>`) |
| `N&`, `N|` | ET bit à bit ; `N|` sans opérande calcule la valeur absolue, avec opérande calcule le OU bit à bit | Oui (`?n&`, `?n|`) |
| `N~`, `N^` | Complément et OU exclusif sur les 64 bits du registre | Oui (`?n~`, `?n^`) |
| `N{`, `N}` | Décalages binaires de 0 à 63 positions ; refus des décalages négatifs ou supérieurs à 63 | Oui (`?n{`, `?n}`) |
| `NA` | Stocke le numéro de la ligne courante dans un registre numérique | Oui (`?na`) |
| `NB`, `ND`, `NF` | Paramètres d'affichage propres à chaque registre : base 2–36, minimum de 0–1024 chiffres, caractère de remplissage ASCII imprimable | Oui (`?nb`, `?nd`, `?nf`) |
| `NL` | Compte les caractères UTF-8 des lignes adressées (ligne courante par défaut), hors séparateurs de ligne | Oui (`?nl`) |
| `NP` | Affiche la valeur formatée d'un registre sur une ligne ; ne modifie ni le compteur ni le buffer | Oui (`?np`) |
| P | Disponible | Oui |
| Q | Disponible pour `Q` et `QQ`; Q protège désormais les buffers modifiés; `Q!` reporté | Oui |
| R | Lecture ASCII/UTF-8, insertion adressée et liste ordonnée de fichiers, y compris les noms individuels entre guillemets avec espaces (formes avancées restantes) | Oui |
| S | Disponible pour les substitutions globales, les plages, `&`, le suffixe P et l'emploi dans G | Oui |
| T | Test de modèle `T`/`T~` et registre de condition ; homologation historique restante | Oui |
| W | Disponible avec W, WA et l'extension WU; WB reconnu mais GCOS/BCD non pris en charge | Oui (`?w`, `?wu`) |
| Y | Affiche le dernier diagnostic de parsing ou d'exécution mémorisé ; ne modifie pas l'erreur | Oui (`?y`) |
| Z | Disponible | Oui |
| ZA | Copie tout un buffer après l'adresse courante, avec protection de l'auto-copie | Oui (`?za`) |
| ZB | Tri décroissant de la plage ; condition vraie si lignes identiques | Oui (`?zb`) |
| ZC | Efface le drapeau de modification ; condition = état antérieur | Oui (`?zc`) |
| ZD | Supprime définitivement le buffer nommé et son contenu non sauvegardé | Oui (`?zd`) |
| ZI | Convertit des alignements d'espaces en tabulations selon les taquets `OT` | Oui (`?zi`) |
| ZK | Copie avec insertion après la ligne courante du destinataire ; tests Windows locaux et CI réussis, homologation historique restante | Oui (`?zk`) |
| ZL | Passage en minuscules ASCII sur plage ou modèle, mise à jour du compteur et du curseur | Oui (`?zl`) |
| ZM | Déplace des lignes entre buffers ou dans le même buffer hors ligne courante | Oui (`?zm`) |
| ZO | Convertit les tabulations en espaces selon les taquets `OT` | Oui (`?zo`) |
| ZP | Affiche un buffer entier sans le sélectionner | Oui (`?zp`) |
| ZR | Sélectionne un buffer associé au fichier ou charge le fichier dans le premier buffer libre `a` à `z` | Oui (`?zr`) |
| ZS | Tri croissant de la plage ; condition vraie si lignes identiques | Oui (`?zs`) |
| ZT | Traduction de caractères ASCII avec jeux de taille égale, sans doublon source | Oui (`?zt`) |
| ZU | Passage en majuscules ASCII sur plage ou modèle, mise à jour du compteur et du curseur | Oui (`?zu`) |
| ZW | Ajoute une plage de lignes en fin de fichier sans effacer le contenu existant ; variantes `ZWA` et `ZWU` | Oui (`?zw`) |

## Limite actuelle de JM / JP

Le REPL exécute toujours une commande `JM` ou `JP` par ligne saisie. Dans une procédure, les formes délimitées `JM/.../` et `JP/.../` peuvent désormais être enchaînées sur la même ligne.

## Famille O — Options

| Option | État dans FREDPP | Aide embarquée |
|---|---|:---:|
| `OI(` | Disponible : `O+I(` / `O-I(`; valeur implicite `O+I(`; noms courts de buffer autorisés avec `O-I(` | Oui (`?oi(`) |
| `OM` | Disponible : `O+M` / `O-M`; valeur implicite `O-M`; le moteur de procédures trace les commandes exécutées avec `O+M` | Oui (`?om`) |
| `OQ` | Disponible : `O+Q` demande l'arrêt sur erreur dans le REPL/procédure, `O-Q` le désactive | Oui (`?oq`) |
| `OS&` | Disponible : `O+S&` active `&` comme texte trouvé dans `S`, `O-S&` le rend littéral | Oui (`?os&`) |
| `OSD` | Disponible : `O+SD` recherche sans distinction de casse ASCII, `O-SD` sensible à la casse ; défaut provisoire `O-SD` pour compatibilité avec les tests FREDPP existants | Oui (`?osd`) |
| `OT` | Disponible : `O+T` définit des colonnes de tabulation ordonnées, `O-T` désactive leur rendu ; les conversions ZI/ZO utilisent les taquets enregistrés | Oui (`?ot`) |
| `OC` | `O+C/ancien/nouveau/` remplace les occurrences exactes dans les lignes A/I/C au moment de l'entrée ; `O-C` désactive | Oui (`?oc`) |
| `OI\\F` | `O-I\\F` accepte la forme courte `A texte`, `I texte`, `C texte` ; `O+I\\F` restaure la saisie à terminateur | Oui (`?oi\\f`) |
| `OIE` | `O+IE!` choisit `!F` comme terminateur de saisie, `O-IE` restaure `\\F` | Oui (`?oie`) |
| `OIF` | `O-IF` supprime le saut de ligne ajouté au message `OIR` dans le REPL ; `O+IF` le rétablit | Oui (`?oif`) |
| `OOL`, `OOU` | Rendu de secours des minuscules/majuscules avec les préfixes `OIL`/`OIU`, appliqué à P et L ; modes exclusifs | Oui (`?ool`, `?oou`) |
| `OOO` | `O+OO` imprime les octets de contrôle en notation octale `\\nnn` pour P et L (option active par défaut) ; `O-OO` restaure le rendu direct | Oui (`?ooo`) |
| `OP` | `O+Pn` insère une séparation visible toutes les n lignes de P et L, sans pause interactive ; `O-P` désactive | Oui (`?op`) |
| `OS/` | `O-S/` autorise la substitution S sans délimiteur final ; `O+S/` le rétablit | Oui (`?os/`) |
| `OIA` | `O+IA/texte/` définit le prompt de saisie A/I/C ; `O-IA` restaure `text> ` | Oui (`?oia`) |
| `OIC` | `O+IC/texte/` définit le prompt de commande ; `O-IC` restaure le nom du buffer suivi de `> ` | Oui (`?oic`) |
| `OIR` | `O+IR/texte/` affiche un message au retour clavier ; `O-IR` le désactive | Oui (`?oir`) |
| `OIL` | `O+IL%` transforme en minuscule ASCII le caractère suivant `%` dans la saisie texte ; `O-IL` désactive | Oui (`?oil`) |
| `OIU` | `O+IU^` transforme en majuscule ASCII le caractère suivant `^` dans la saisie texte ; `O-IU` désactive | Oui (`?oiu`) |

## Moteur de procédures — Sprint 2.17

Le lancement :

```text
fredpp nomscript.fredpp
```

charge le fichier dans `B(.)`, puis exécute son contenu selon le bootstrap minimal documenté par `?procedure`.

Une directive autonome :

```fred
\B(buffer)
```

exécute un buffer de commandes sans changer le buffer courant par elle-même. Les appels peuvent être imbriqués et sont protégés contre la récursion excessive.

Les blocs `A`, `I` et `C` consomment les lignes suivantes jusqu'à `\F`. Le bootstrap historique complet (`B(0)` pour les arguments, `.init`, environnement utilisateur/date/heure et bibliothèques de procédures) reste reporté.

## Directive de flot `\S`

`\S(buffer)` est disponible dans le moteur de flot : le contenu du buffer est
injecté littéralement dans l'entrée courante et tous ses retours de ligne sont
supprimés. Les autres directives de flot DNB11A restent à implémenter.

## Commandes `!` et `ZG` — Sprint 2.19

La commande historique `!` exécute une commande du système hôte. Sa sortie
standard et sa sortie d'erreur passent par la sortie FREDPP et peuvent être
capturées par `ZG`.

`ZG(buffer)<commande>` exécute la commande imbriquée et place la production
destinée à la visualisation dans le buffer désigné, après sa ligne courante.
Si le buffer n'existe pas, FREDPP le crée.

Exemple Windows :

```fred
zg(buf)!dir
```

Sous Linux/Raspberry Pi OS, l'équivalent peut utiliser `!ls`.

## Limites actuelles de G

La forme initiale est :

```fred
(1,$)G[~]/<modèle>/<commande>
```

Les commandes imbriquées prises en charge sont `P`, `D`, `Z` et `S`, sans adresse propre. Le compteur interne reçoit le nombre de lignes sélectionnées. Les suites générales de plusieurs commandes et le rappel du modèle courant avec `//` restent à compléter.

## Limites actuelles de S

Sont disponibles :

```fred
(.,.)S<séparateur><modèle><séparateur><chaîne><séparateur>[P]
```

La substitution porte sur toutes les occurrences non chevauchantes. `&` réinsère le texte trouvé. Les options historiques `OS&` et `OS/` sont configurables ; avec `O-S/`, le dernier séparateur peut être omis.

## Limites actuelles de Q

`Q` demande un arrêt normal et refuse de quitter si un buffer est modifié. `QQ` force l'arrêt immédiat. `Q!` est reporté au chantier d'exécution externe.

## Limites actuelles de R et W

`R` charge un fichier dans un buffer vide ou insère ses lignes à une adresse. Il parcourt une liste de noms en ordre, avec guillemets possibles pour les espaces ; les formes historiques avancées de `R` restent à étudier. `W` écrit le buffer complet ou une plage. `WA` force l'ASCII et `WU` force l'UTF-8 sans BOM. `WB` reste réservé au GCOS/BCD historique et n'est pas implémenté. `WX` et les encodages autres que l'ASCII ou l'UTF-8 restent reportés.

## Alias historiques

| Alias | État dans FREDPP | Aide embarquée |
|---|---|:---:|
| `*` | Disponible comme équivalent de `1,$`; utilisé seul, équivaut à `1,$P` | Oui (`?*`) |
