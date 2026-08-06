# État des commandes FRED

Ce document suit l'état d'implémentation. Les pages situées sous `docs/fr/reference/commandes/` restent inchangées et seules celles des commandes disponibles sont embarquées.

| Commande | État dans FREDPP | Aide embarquée |
|---|---|:---:|
| A | Disponible | Oui |
| B | Disponible, fonctions avancées encore à compléter | Oui |
| C | Disponible | Oui |
| D | Disponible | Oui |
| G | Disponible avec une commande imbriquée P, D, Z ou S | Oui |
| I | Disponible | Oui |
| L | Disponible avec un nom de fichier | Oui |
| M | Disponible sous forme provisoire, réalignement historique requis | Oui |
| P | Disponible | Oui |
| Q | Disponible pour `Q` et `QQ`; Q protège désormais les buffers modifiés; `Q!` reporté | Oui |
| R | Disponible pour un fichier ASCII ou UTF-8 dans un buffer vide et non associé | Oui |
| S | Disponible pour les substitutions globales, les plages, `&`, le suffixe P et l'emploi dans G | Oui |
| T | Disponible sous forme provisoire, réalignement historique requis | Oui |
| W | Disponible avec W, WA et l'extension WU; WB reconnu mais GCOS/BCD non pris en charge | Oui (`?w`, `?wu`) |
| Z | Disponible | Oui |

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
