# État des commandes FREDPP

Ce fichier suit l'état d'implémentation sans modifier les pages historiques sous
`docs/fr/reference/commandes/`.

| Commande | État dans FREDPP | Aide embarquée |
|---|---|:---:|
| A | Disponible | Oui |
| B | Disponible, fonctions avancées encore à compléter | Oui |
| C | Disponible | Oui |
| D | Disponible | Oui |
| G | Disponible pour une commande imbriquée P, D ou Z | Oui |
| I | Disponible | Oui |
| L | Disponible avec un nom de fichier | Oui |
| M | Disponible sous forme provisoire, réalignement historique requis | Oui |
| P | Disponible | Oui |
| T | Disponible sous forme provisoire, réalignement historique requis | Oui |
| Z | Disponible | Oui |

## Limites actuelles de G

La première implémentation accepte :

```fred
(1,$)G[~]/<modèle>/<commande>
```

Les commandes imbriquées prises en charge sont `P`, `D` et `Z`, sans adresse
propre. Le compteur interne reçoit le nombre de lignes sélectionnées. Les suites
de plusieurs commandes et le rappel du modèle courant avec `//` seront ajoutés
dans un sprint ultérieur.
