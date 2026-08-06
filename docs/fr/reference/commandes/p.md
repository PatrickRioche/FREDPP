# P — Impression ( Print )

> La commande P imprime les lignes spécifiées à l'écran.

## Syntaxe

```fred
(.,.)P
```

## Paramètres

| Élément | Description |
|---|---|
| `(.,.)` | Adresse ou plage de lignes optionnelle, selon la forme indiquée. |

## Description

La commande P imprime les lignes spécifiées à l'écran.

L'adresse de la ligne courante pointe sur la dernière ligne imprimée.

NB : <nl> Impression de la ligne suivant la ligne courante( .+1 ).

## Exemples

```fred
b(buf)
1,2p
Ajout  ligne de texte numero 1
Ajout  ligne de texte numero 2
<transmit>
Ajout  ligne de texte numero 3
1,/3/p
Ajout  ligne de texte numero 1
Ajout  ligne de texte numero 2
Ajout  ligne de texte numero 3
```

## Options

Aucune option spécifique n'est documentée.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
