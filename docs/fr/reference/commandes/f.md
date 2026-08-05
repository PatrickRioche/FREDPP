# F — Informations sur le buffer et association de fichier (Facts / File)

> La commande F imprime le nom du buffer courant ainsi que tous ses attributs : l'adresse de la ligne courante, le nombre total de lignes du buffer, le nom du fichier associé (s'il existe), et un ?

## Syntaxe

```fred
F

F <nom de fichier>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom de fichier>` | Nom du fichier concerné. |

## Description

La commande F imprime le nom du buffer courant ainsi que tous ses attributs : l'adresse de la ligne courante, le nombre total de lignes du buffer, le nom du fichier associé (s'il existe), et un ? si le buffer a été modifié.

La commande F avec un nom de fichier permet d'associer un nom de fichier à un buffer ou de le modifier (s'il en existe déjà un).

## Exemples

```fred
f
b(buf) 4,4 ?

f /fred/parex
fb(buf) 4,4 /fred/parex ?
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

- [FB](fb.md)
- [FF](ff.md)
- [F?](fq.md)
