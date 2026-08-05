# Q — Arrêt ( Quit )

> La commande Q permet l'arrêt sous réserve qu'il n'y ait pas de buffers modifiés par rapport à leurs fichiers associés respectifs.

## Syntaxe

```fred
Q<nl>				 Arrêt.
QQ<nl> 			Arrêt immédiat.
Q!<commande TSS><nl>	 Arrêt avec appel à un autre sous-système TSS.
```

## Paramètres

| Élément | Description |
|---|---|
| `<nl>` | Fin de ligne. |
| `<commande TSS>` | Élément défini par la syntaxe historique de la commande. |

## Description

La commande Q permet l'arrêt sous réserve qu'il n'y ait pas de buffers modifiés par rapport à leurs fichiers associés respectifs.

La commande QQ permet l'arrêt immédiat, sans considération de l'état des buffers (modifiés ou non, par rapport à leurs fichiers associés respectifs).

La commande Q! permet l'arrêt immédiat de FRED, sans considération pour l'état des buffers, en enchaînant sur un autre sous-système TSS.

## Exemples

```fred
fred
q*fredq!bye$$ 0400 AU REVOIR
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
