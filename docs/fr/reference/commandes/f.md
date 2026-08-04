# F — Informations sur le buffer et association de fichier (Facts / File)

> Affiche les informations du buffer courant ou lui associe un fichier.

## Syntaxe

Afficher les informations du buffer courant :

```fred
F
```

Associer un fichier au buffer courant :

```fred
F <nom de fichier>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom de fichier>` | Nom du fichier à associer au buffer courant. |

## Description

Sans argument, la commande **F** affiche :

- le nom du buffer courant ;
- l'adresse de la ligne courante ;
- le nombre total de lignes ;
- le nom du fichier associé, s'il existe ;
- un indicateur `?` si le buffer a été modifié.

Avec un nom de fichier, la commande associe ce fichier au buffer courant ou remplace l'association existante.

## Exemples

Afficher les informations :

```fred
f
```

Résultat :

```text
b(buf) 4,4 ?
```

Associer un fichier :

```fred
f /fred/parex
f
```

Résultat :

```text
b(buf) 4,4 /fred/parex ?
```

## Options

Aucune option spécifique n'est documentée.

## Remarques

La présence de `?` indique que le buffer a été modifié par rapport à son fichier associé.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider pour l'association de fichiers |

## Voir aussi

- [FB — Informations sur les buffers](fb.md)
- [FF — Informations sur les fichiers](ff.md)
- [F? — Buffers modifiés](fq.md)
