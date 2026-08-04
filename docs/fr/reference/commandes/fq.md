# F? — Liste des buffers modifiés

> Affiche les buffers dont le contenu diffère du fichier associé.

## Syntaxe

```fred
F?
```

## Description

La commande **F?** affiche :

- le nom du buffer courant ;
- le nom du fichier associé ;
- les noms des buffers dont le contenu est différent de celui de leur fichier associé ;
- le nom des fichiers associés à ces buffers.

## Exemples

```fred
f?
```

Résultat possible :

```text
b(buf) /fred/parex ?
```

## Options

Aucune option spécifique n'est documentée.

## Remarques

L'indicateur `?` signale un buffer modifié.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon la gestion des fichiers |

## Voir aussi

- [F — Facts / File](f.md)
- [FB — Informations sur les buffers](fb.md)
- [FF — Informations sur les fichiers](ff.md)
