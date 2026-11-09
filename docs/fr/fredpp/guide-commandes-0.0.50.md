# FREDPP 0.0.50 — Guide pratique des 50 commandes disponibles

> **Périmètre de développement :** 50 commandes historiques sur 103, soit
> 48,5 %. Les dix commandes 41–50 complètent la famille numérique N. Elles
> attendent la recette Windows de l'utilisateur avant le tag `dev-0.0.50`.

Les [commandes 1–30](guide-commandes-0.0.30.md) et les
[opérations 31–40](guide-commandes-0.0.40.md) conservent leur comportement.
Les fiches du [manuel historique](../reference/commandes/index.md) sont
conservées sans modification. Dans l'invite interactive, saisir **une commande
N par ligne** ; les procédures acceptent aussi les enchaînements N.

## Commandes 41–50

| N° | Commande | Syntaxe | Effet |
|---:|---|---|---|
| 41 | `N~` | `N(x)~` | Complément bit à bit du registre (ex. `~1 = -2`). |
| 42 | `N^` | `N(x)^8` | OU exclusif bit à bit. |
| 43 | `N{` | `N(x){2` | Décalage des bits vers la gauche. |
| 44 | `N}` | `N(x)}2` | Décalage arithmétique des bits vers la droite. |
| 45 | `NA` | `N(x)A` | Affecte à `x` le numéro de ligne courante. |
| 46 | `NB` | `N(x)B16` | Définit la base d'affichage de 2 à 36. |
| 47 | `ND` | `N(x)D4` | Affiche au minimum quatre chiffres, avec remplissage. |
| 48 | `NF` | `N(x)F.` | Choisit ici `.` comme caractère de remplissage. |
| 49 | `NL` | `1,$N(x)L` | Compte les caractères des lignes désignées. |
| 50 | `NP` | `N(x)P` | Imprime la valeur formatée suivie d'un retour à la ligne. |

Comme pour les commandes 31–40, `N^`, `N{`, `N}`, `NB` et `ND` acceptent un
entier signé, `#`, `.`, `$` ou `\N(autre)`. Les opérations qui changent une
valeur mettent également à jour le compteur `#` ; `NB`, `ND`, `NF` et `NP`
ne le modifient pas. Les paramètres d'affichage sont attachés à un registre,
indépendamment de sa valeur, et restent actifs après une nouvelle affectation.
`FN` respecte également la représentation configurée de chaque registre.

## Scénario à tester sous Windows

Entrer ces commandes dans `fredpp.exe` :

```fred
N(bit):15
N(bit)^8
N(bit)P
N(bit)~
N(bit)P
N(bit):7
N(bit){2
N(bit)}1
N(bit)P
N(bit)D5
N(bit)F.
N(bit)B2
N(bit)P
B(buf)
A
café
xy
\F
N(chars)A
N(chars)P
1,$N(chars)L
N(chars)P
FN
```

Les valeurs imprimées par `NP`, dans cet ordre, doivent être :

```text
7
-8
14
.1110
2
6
```

L'affichage final de `FN` doit être :

```text
n(bit):.1110
n(chars):6
```

La chaîne `café` comporte quatre points de code UTF-8 ; `xy` en comporte
deux. `NL` ne compte pas les séparateurs de lignes et ne déplace pas le
curseur. Sans adresse, `NL` porte sur la ligne courante.

## Bornes et erreurs

- Les décalages acceptent un nombre de positions de **0 à 63**. Pour `N{`,
  les bits dépassant la largeur de 64 bits sont abandonnés ; `N}` conserve le
  signe des valeurs négatives.
- `NB` accepte les bases **2 à 36** (chiffres `0–9` puis `A–Z`).
- `ND` accepte **0 à 1 024** chiffres minimum. `0` désactive le remplissage
  supplémentaire. Le signe négatif n'est pas inclus dans ce minimum.
- `NF` accepte **un caractère ASCII imprimable non blanc**, comme `0`, `.`
  ou `_`. Les caractères blancs et les caractères Unicode multioctets ne sont
  pas encore définis pour cette opération.
- La représentation de `INT64_MIN` fonctionne dans toutes les bases acceptées.
- `NL` dénombre les points de code UTF-8, pas les *graphèmes* composés, et
  n'inclut pas les terminaisons de ligne. Le buffer doit contenir les lignes
  visées ; un comptage débordant l'entier signé est refusé.
- En cas d'opérande invalide, les valeurs numériques, les paramètres de
  représentation et le compteur restent inchangés.

## Aide embarquée

```text
?n~  ?n^  ?n{  ?n}  ?na  ?nb  ?nd  ?nf  ?nl  ?np
```

Les dix commandes s'ajoutent aux 40 précédentes, sans publication officielle
de FREDPP ni changement de la branche stable `main`.
