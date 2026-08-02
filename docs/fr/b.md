# B — Positionnement sur un buffer

## Syntaxe

```text
B(nom du buffer)
```

## Description

La commande `B` sélectionne un buffer comme buffer courant.

Lorsqu'un nom est référencé pour la première fois, FREDPP crée automatiquement le buffer. Au démarrage, le buffer courant est `(0)`.

Chaque buffer conserve sa propre ligne courante, désignée par `.`. Lorsqu'un buffer vient d'être créé, `.` vaut zéro. Lorsqu'on revient dans un buffer existant, sa ligne courante précédente est restaurée.

Un buffer vide et sans fichier associé est provisoire : s'il est quitté sans avoir reçu de texte, il est supprimé automatiquement. L'association de fichiers sera ajoutée dans un sprint ultérieur ; dans l'état actuel, ce comportement s'applique aux buffers vides.

## Exemples

```text
B(travail)
B(mon buffer)
B(0)
```

## Noms

Pour FREDPP 1.x, le nom est limité à 14 caractères. La limite est centralisée afin de pouvoir évoluer sans modifier le modèle interne.

Les espaces sont acceptés dans la forme parenthésée.

## État actuel

La gestion des tabulations propres à chaque buffer et l'association d'un fichier sont documentées historiquement mais ne sont pas encore implémentées.

## Voir aussi

- `?index`
- `?m`
- `?t`
