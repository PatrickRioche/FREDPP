# ZC — Efface l'indicateur "buffer modifié // fichier associé"

> La commande ZC efface l'indicateur "buffer modifié par rapport au fichier associé".

## Syntaxe

```fred
ZC
```

## Paramètres

Aucun paramètre explicite n'est documenté.

## Description

La commande ZC efface l'indicateur "buffer modifié par rapport au fichier associé".

Le registre de condition est chargé à "FAUX" si l'indicateur n'était pas positionné.

Le registre de condition est positionné à "VRAI" (True) si l'indicateur était positionné, c'est à dire si le buffer n'était pas l'exact reflet du fichier associé.

## Exemples

```fred
b(buf)
fb(buf) 4,4 /fred/parexb ? BCD card
zc
f
b(buf) 4,4 /fred/parexb BCD card
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
