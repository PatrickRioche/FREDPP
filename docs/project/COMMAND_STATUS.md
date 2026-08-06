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
| Q | Disponible pour `Q` et `QQ`; `Q!` reporté | Oui |
| S | Disponible pour les substitutions globales, les plages, `&`, le suffixe P et l'emploi dans G | Oui |
| T | Disponible sous forme provisoire, réalignement historique requis | Oui |
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

`Q` demande un arrêt normal et `QQ` un arrêt immédiat. Comme FREDPP ne possède pas encore de liaison complète entre buffers et fichiers, le contrôle historique des fichiers modifiés sera finalisé avec les commandes de lecture et d'écriture. `Q!` est reporté au chantier d'exécution externe.

## Alias historiques

| Alias | État dans FREDPP | Aide embarquée |
|---|---|:---:|
| `*` | Disponible comme équivalent de `1,$`; utilisé seul, équivaut à `1,$P` | Oui (`?*`) |
