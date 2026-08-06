# JE — ( Jump Exit )

> La commande JE imprime le message d'erreur utilisateur au terminal.

## Syntaxe

```fred
JE <remarques><nl> ou JE/<remarques>/
```

## Paramètres

| Élément | Description |
|---|---|
| `<remarques>` | Message à afficher. |
| `<nl>` | Fin de ligne. |

## Description

La commande JE imprime le message d'erreur utilisateur au terminal.

## Exemples

```fred
o+q
JE/Erreur Numero 5/
Erreur Numero 5
```

## Options

Si l'option O+Q a été préalablement validée, JE fait quitter l'éditeur.

## Remarques

Aucune remarque spécifique n'est documentée.

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Compatible |
| FRED++ | À valider selon l'implémentation |

## Voir aussi

- [JM](jm.md)
- [JP](jp.md)
- [OQ](oq.md)
