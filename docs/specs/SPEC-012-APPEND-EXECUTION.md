# SPEC-012 — Commande A (Append)

## Portée

Cette Step introduit uniquement l'exécution interactive de `A`. Les commandes `I` et `C` restent hors périmètre.

## Syntaxe

```text
[address]A
text
...
\F
```

La lettre de commande est insensible à la casse.

## Adresse

- Sans adresse, `A` utilise `$`.
- Une seule adresse est acceptée.
- La ligne `0` est acceptée et représente la position précédant la première ligne stockée.
- Une plage est refusée.

## Effet

Le texte saisi est inséré immédiatement après la ligne adressée.

Après une insertion non vide, la ligne courante devient la dernière ligne insérée.

Si la saisie ne contient aucune ligne, le buffer reste inchangé et la ligne courante devient la ligne adressée.

## Mode de saisie CLI

Après reconnaissance de `A`, le CLI affiche :

```text
-- text input; finish with \F --
text>
```

Chaque nouvelle ligne affiche à nouveau `text> `. Une ligne contenant exactement `\F` termine la saisie et n'est pas insérée.

Une fin de fichier avant `\F` annule l'opération et affiche une erreur.

## Séparation des responsabilités

- Le parser produit `AppendCommandNode`.
- Le CLI collecte les lignes de texte sans les interpréter comme des commandes.
- `CommandExecutor::execute_append` applique l'opération au buffer.
- Le runtime ne reparcourt pas la chaîne de commande.
