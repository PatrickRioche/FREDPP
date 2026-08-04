# FB — Informations sur les buffers

> Affiche la liste des buffers ouverts et leurs attributs.

## Syntaxe

```fred
FB
```

## Description

La commande **FB** affiche l'ensemble des buffers ouverts par l'utilisateur.

Les buffers sont présentés par ordre décroissant de dernière utilisation : le dernier buffer utilisé apparaît en premier.

## Exemples

```fred
fb
```

Résultat possible :

```text
b(buf) 4,4 /fred/parex ?
b(bufi) 4,4
b(fo) 15,15
```

## Options

Aucune option spécifique n'est documentée.

## Remarques

L'indicateur `?` signale un buffer modifié par rapport à son fichier associé.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | Compatible si l'énumération des buffers est implémentée |

## Voir aussi

- [B — Buffer](b.md)
- [F — Facts / File](f.md)
- [FF — Informations sur les fichiers](ff.md)
