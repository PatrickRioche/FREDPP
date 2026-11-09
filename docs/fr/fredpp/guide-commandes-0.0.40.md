# FREDPP 0.0.40 — Guide pratique des 40 commandes disponibles

> **Périmètre :** 40 commandes historiques reconnues par FREDPP en
> développement, sur 103 recensées. Ce document décrit le lot **31 à 40**,
> à valider par l'utilisateur sous Windows avant tout tag `dev-0.0.40`.

Le [guide détaillé des 30 premières commandes](guide-commandes-0.0.30.md)
reste applicable aux commandes déjà prises en charge jusqu'à la version
`0.0.30`. Les dix commandes supplémentaires appartiennent à la famille des
**registres numériques N** et s'appuient sur les mêmes registres que `N:` et
`FN`. Les [fiches historiques](../reference/commandes/index.md) restent
conservées à l'identique ; lorsqu'un exemple contient plusieurs commandes
sur une même ligne, les lancer ici **sur des lignes distinctes dans le REPL**.

## Les dix nouvelles commandes

| N° | Opération | Syntaxe interactive | Effet |
|---:|---|---|---|
| 31 | Addition `N+` | `N(x)+5` | Ajoute 5 au registre `x`. |
| 32 | Soustraction `N-` | `N(x)-3` | Soustrait 3. |
| 33 | Multiplication `N*` | `N(x)*4` | Multiplie par 4. |
| 34 | Division entière `N/` | `N(x)/2` | Divise par 2, troncature vers zéro. |
| 35 | Modulo `N%` | `N(x)%3` | Conserve le reste signé. |
| 36 | Égalité `N=` | `N(x)=10` | Positionne le booléen de condition. |
| 37 | Infériorité `N<` | `N(x)<11` | Teste strictement « x inférieur à 11 ». |
| 38 | Supériorité `N>` | `N(x)>9` | Teste strictement « x supérieur à 9 ». |
| 39 | ET bit à bit `N&` | `N(x)&7` | Applique un masque binaire. |
| 40 | Valeur absolue ou OU `N|` | `N(x)|` / `N(x)|8` | Abs sans opérande, OU bit à bit avec opérande. |

Les opérandes acceptés sont les entiers décimaux signés sur 64 bits, `#`
(compteur partagé), `.` (ligne courante), `$` (dernière ligne du buffer) et
`\N(autre)` (valeur d'un autre registre). Les noms des registres sont
limités à 14 caractères ; une valeur d'un registre absent est lue comme zéro.

Les opérations qui modifient un registre y enregistrent leur résultat et
recopient celui-ci dans le compteur `#`. Les comparaisons `N=`, `N<` et `N>`
mettent à jour l'indicateur de condition sans modifier le registre comparé ;
le compteur reçoit la valeur du registre. La commande `FN` affiche les
registres, ce qui permet de vérifier les calculs. Aucun de ces opérateurs ne
modifie directement le texte ni le curseur des buffers.

## Scénario reproductible — terminal Windows

Entrer les lignes suivantes une à une :

```fred
N(a):20
N(a)+5
N(a)-3
N(a)*4
N(a)/8
N(a)%4
N(a)&6
N(a)|8
N(b):-5
N(b)|
N(copy)+\N(a)
N(a)=10
N(a)<11
N(a)>9
FN
```

La fin de la sortie de `FN` doit être exactement :

```text
n(a):10
n(b):5
n(copy):10
```

Les tests de comparaison pilotent le booléen utilisé par les sauts
conditionnels des procédures, mais n'impriment pas eux-mêmes `TRUE/FALSE`.
La forme de chaînage de procédures `N(x):5+2>6` est reconnue ; le mode
interactif attend **une commande N par ligne**.

## Gestion des erreurs et compatibilité

- Les divisions/modulos par zéro sont refusés.
- Les additions, soustractions, multiplications et valeurs absolues qui
  dépasseraient les limites de l'entier signé 64 bits sont refusées.
- Le couple `INT64_MIN / -1` (division ou modulo) est refusé pour éviter un
  débordement.
- En cas d'erreur arithmétique, le registre et le compteur conservent leur
  valeur antérieure.
- Les opérateurs non encore implémentés (`N^`, `N~`, `N{`, `N}`, etc.) restent
  hors du périmètre de cette version et ne doivent pas être comptabilisés.
- Les nouveaux détails sont vérifiés par des tests C++ de parsing et
  d'exécution, des scénarios de procédures et un test de l'invite interactive.
  L'homologation Windows par l'utilisateur est une étape distincte.

## Aides disponibles

Chaque commande possède une fiche historique embarquée accessible par :

```text
?n+  ?n-  ?n*  ?n/  ?n%  ?n=  ?n<  ?n>  ?n&  ?n|
```

Le mode `?n:` et la commande `FN` restent disponibles. L'aide historique peut
décrire des combinaisons non encore implémentées dans le programme.
