# L — Liste le contenu d'un fichier ( List )

> La commande L permet de lister un fichier au terminal.

## Syntaxe

```fred
L[{d|d-f|d,w}][<bl><nom de fichier>]
```

## Paramètres

| Élément | Description |
|---|---|
| `<bl>` | Un espace. |
| `<nom de fichier>` | Nom du fichier concerné. |

## Description

La commande L permet de lister un fichier au terminal. Cette commande ne modifie par l'adresse des pointeurs du buffer courant.

## Exemples

```fred
l2-4 /fred/parexemple
$      program fred
$      limits  10,40k
$      prmfl   **,q,r,cmdlib/etc/qstar
l2,2 /fred/parexemple
$      program fred
$      limits  10,40k
```

## Options

d	: Lister le fichier à partir de l'adresse de (d)ébut de ligne.

d-f	: Lister le fichier de l'adresse de (d)ébut de ligne à l'adresse de (f)in de ligne.

d,w	: Lister le fichier à partir de l'adresse de (d)ébut de ligne sur une fenêtre de (w) lignes.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

- [P](p.md)
- [R](r.md)
- [W](w.md)
