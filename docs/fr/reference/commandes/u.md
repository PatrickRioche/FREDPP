# U — Répétition jusqu'à ... ( Until )

> Documentation de la commande FRED.

## Syntaxe

```fred
(.)U[<n>][T|F|E]<commandes><nl>
```

## Paramètres

| Élément | Description |
|---|---|
| `(.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |
| `<n>` | Nombre entier. |
| `<commandes>` | Commandes FRED à exécuter. |
| `<nl>` | Fin de ligne. |

## Description

La commande "U<n>" répète les commandes  n  fois, les commandes listées sont copiées dans un buffer secret, qui  est ensuite  exécuté à plusieurs reprises, jusqu'à ce que la condition d'arrêt soit satisfaite. La commande "UT" répète les commandes jusqu'à ce que le registre de condition soit "VRAI" à la fin d'une itération. La commande "UF" répète les commandes jusqu'à ce que le registre de condition soit "FAUX" à la fin d'une itération. La commande "UE" répète les commandes jusqu'à ce qu'une erreur se produise. - si <n> est spécifié avec "UT", "UF", ou "UE", les commandes sont répétées  n  fois, à moins qu'une condition ne soit satisfaite avant ( Limite <n> = 262143 ).

## Exemples

```fred
o-i\F (bufu) a Ajout de ligne dans buffer
b(bufa)
u3 $za(bufu)
*
Ajout de ligne dans buffer
Ajout de ligne dans buffer
Ajout de ligne dans buffer
u1e 20d
n(err):#=-6 jf jm/La ligne n'existe pas!/
La ligne n'existe pas!
```

## Options

Liste des erreurs possibles :
 0   -- internal error			        -10   -- file error
-1   -- context search failed		    -11   -- too many alternatives
-2   -- unknown command		            -12   -- internal table overflow
-3   -- syntax error in pattern 	    -14   -- callss not allowed
-4   -- incorrectly constructed address -15   -- add/cmd conflict
-5   -- address wrap around		        -16   -- cannot grow workfile
-6   -- value out of range			    -17   -- invalid parameter
-7   -- buff/reg name invalid		    -18   -- missing closing delimiter
-9   -- command syntax error		    -19   -- BREAK key was pushed


## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
