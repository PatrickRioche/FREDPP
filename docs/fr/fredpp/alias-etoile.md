# * — Alias de la totalité du buffer

> L'astérisque remplace la plage d'adresses `1,$` au début d'une commande FRED.

## Syntaxe

```fred
*
*<commande>
```

## Équivalences

| Forme courte | Forme développée | Effet |
|---|---|---|
| `*` | `1,$P` | Afficher toutes les lignes du buffer. |
| `*P` | `1,$P` | Afficher toutes les lignes du buffer. |
| `*D` | `1,$D` | Supprimer toutes les lignes du buffer. |
| `*G/<modèle>/P` | `1,$G/<modèle>/P` | Appliquer G à la totalité du buffer. |

## Description

Lorsque `*` est placé au début d'une commande, FREDPP construit la plage complète du buffer, de la première ligne (`1`) à la dernière (`$`). La commande qui suit reçoit ensuite cette plage comme si `1,$` avait été écrit explicitement.

Utilisé seul, `*` conserve le comportement historique de raccourci pour `1,$P`.

Les lettres de commande restent insensibles à la casse : `*d` et `*D` sont équivalents.

## Limites

L'alias est reconnu uniquement au début d'une commande. Il ne modifie pas la signification de l'astérisque lorsqu'il apparaît dans un modèle ou dans du texte.

Une commande qui n'accepte pas de plage d'adresses reste invalide avec `*`.

## Exemples

```fred
b(buf)
*
*d
```

## Compatibilité

| Implémentation | Statut |
|---|---|
| FRED historique | Alias `*` et formes telles que `*D` documentés dans les exemples historiques. |
| FREDPP | Implémenté et testé. |
