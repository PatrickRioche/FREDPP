# NL — Affectation du nombre de caractère au registre

> La commande NL affecte le nombre de caractères total des lignes désignées au registre.

## Syntaxe

```fred
ld,lfN<nom registre>L
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom registre>` | Nom du registre numérique. |

## Description

La commande NL affecte le nombre de caractères total des lignes désignées au registre.

## Exemples

```fred
b(buf)
1,$n(reg1)l  n(reg1)p
76
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
