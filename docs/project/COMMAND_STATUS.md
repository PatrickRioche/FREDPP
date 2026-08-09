# État des commandes FRED

Ce document suit l'état d'implémentation. Les pages situées sous `docs/fr/reference/commandes/` restent inchangées et seules celles des commandes disponibles sont embarquées.

## Couverture de la référence historique

| Famille | Total référence | Implémenté / homologué | Reste |
|---|---:|---:|---:|
| Commandes principales | 22 | 15 | 7 |
| Commandes spéciales | 7 | 1 (`\"`) | 6 |
| Famille F | 7 | 2 (`FB`, `FO`) | 5 |
| Famille J | 5 | 2 (`JM`, `JP`) | 3 |
| Famille N | 21 | 0 | 21 |
| Famille O | 24 | 2 (`OI(`, `OM`) | 22 |
| Famille Z | 17 | 0 | 17 |
| **Total** | **103** | **22** | **81** |

La couverture fonctionnelle de la référence est de **22 / 103**, soit environ **21,4 %**.

| Commande | État dans FREDPP | Aide embarquée |
|---|---|:---:|
| A | Disponible | Oui |
| B | Disponible, fonctions avancées encore à compléter | Oui |
| C | Disponible | Oui |
| D | Disponible | Oui |
| FB | Disponible; ordre décroissant de dernière utilisation, ligne courante, nombre de lignes, fichier associé et état modifié | Oui (`?fb`) |
| FO | Disponible; affiche les options effectivement en service | Oui (`?fo`) |
| G | Disponible avec une commande imbriquée P, D, Z ou S | Oui |
| I | Disponible | Oui |
| `\"` | Disponible : tout le reste de la ligne est ignoré comme commentaire | Oui (`?\"`) |
| JM | Disponible : message avec retour à la ligne, formes `JM texte` et `JM/texte/` | Oui (`?jm`) |
| JP | Disponible : message sans retour à la ligne, formes `JP texte` et `JP/texte/` | Oui (`?jp`) |
| L | Disponible avec un nom de fichier | Oui |
| M | Disponible sous forme provisoire, réalignement historique requis | Oui |
| P | Disponible | Oui |
| Q | Disponible pour `Q` et `QQ`; Q protège désormais les buffers modifiés; `Q!` reporté | Oui |
| R | Disponible pour un fichier ASCII ou UTF-8 dans un buffer vide et non associé | Oui |
| S | Disponible pour les substitutions globales, les plages, `&`, le suffixe P et l'emploi dans G | Oui |
| T | Disponible sous forme provisoire, réalignement historique requis | Oui |
| W | Disponible avec W, WA et l'extension WU; WB reconnu mais GCOS/BCD non pris en charge | Oui (`?w`, `?wu`) |
| Z | Disponible | Oui |

## Limite actuelle de JM / JP

Le REPL exécute toujours une commande `JM` ou `JP` par ligne saisie. Dans une procédure, les formes délimitées `JM/.../` et `JP/.../` peuvent désormais être enchaînées sur la même ligne.

## Famille O — Options

| Option | État dans FREDPP | Aide embarquée |
|---|---|:---:|
| `OI(` | Disponible : `O+I(` / `O-I(`; valeur implicite `O+I(`; noms courts de buffer autorisés avec `O-I(` | Oui (`?oi(`) |
| `OM` | Disponible : `O+M` / `O-M`; valeur implicite `O-M`; le moteur de procédures trace les commandes exécutées avec `O+M` | Oui (`?om`) |

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

La substitution porte sur toutes les occurrences non chevauchantes. `&` réinsère le texte trouvé. Les options historiques `OS&` et `OS/` ne sont pas encore configurables.

## Limites actuelles de Q

`Q` demande un arrêt normal et refuse de quitter si un buffer est modifié. `QQ` force l'arrêt immédiat. `Q!` est reporté au chantier d'exécution externe.

## Limites actuelles de R et W

`R` lit un fichier unique dans un buffer vide, non associé et non modifié. `W` écrit le buffer complet ou une plage. `WA` force l'ASCII et `WU` force l'UTF-8 sans BOM. `WB` reste réservé au GCOS/BCD historique et n'est pas implémenté. `WX`, les listes de fichiers et les encodages autres que l'ASCII ou l'UTF-8 sont reportés.

## Alias historiques

| Alias | État dans FREDPP | Aide embarquée |
|---|---|:---:|
| `*` | Disponible comme équivalent de `1,$`; utilisé seul, équivaut à `1,$P` | Oui (`?*`) |
