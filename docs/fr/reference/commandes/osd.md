# OSD — Différence majuscule et minuscule ( Option String Defaut )

> L'option OSD permet de valider ou d'invalider la différentiation entre les majuscules et les minsucules dans la recherche de chaîne de caractères.

## Syntaxe

```fred
O[+|-]SD
```

## Paramètres

Aucun paramètre explicite n'est documenté.

## Description

L'option OSD permet de valider ou d'invalider la différentiation entre les majuscules et les minsucules dans la recherche de chaîne de caractères.

## Exemples

```fred
o-sd
b(bufo) a>MAJUSCULE>\f/maj/? search failed/MAJ/MAJUSCULEo+sd/maj/MAJUSCULE/MAJ/MAJUSCULE
```

## Options

Valeur implicite O+SD.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

Aucun lien associé n'est encore défini.
