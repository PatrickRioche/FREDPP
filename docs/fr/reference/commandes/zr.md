# ZR — Positionnement sur un buffer affecté à un fichier ( Zap Read )

> La commande ZR permet de charger des buffers avec le contenu des fichiers en affectant automatiquement le nom du buffer.

## Syntaxe

```fred
ZR <nom de fichier>
```

## Paramètres

| Élément | Description |
|---|---|
| `<nom de fichier>` | Nom du fichier concerné. |

## Description

La commande ZR permet de charger des buffers avec le contenu des fichiers en affectant automatiquement le nom du buffer.

Si un buffer est déjà associé au <nom de fichier>, il y a positionnement sur ce buffer. Si aucun buffer n'est associé au <nom de fichier>, il y a création d'un nouveau buffer avec lecture du fichier.

Les nouveaux buffers sont : b(a), .. b(b), etc...  ou b(0), .. b(2). Si une lecture est effectuée, ZR affiche les statistiques du fichier selon le même format que R; si c'est simplement un positionnement les statistiques seront celles de F.

## Exemples

```fred
zr library/fred/time
40,1291 b(a) library/fred/timezr library/fred/index77,2421 b(b) library/fred/indexfbb(b) 77,77 library/fred/indexb(a) 40,40 library/fred/time
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
