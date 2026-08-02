# SPEC-021 — Commande B (buffers)

- Projet : FREDPP
- Sprint : S2.7
- Statut : implémenté, à homologuer
- Source historique principale : `b.pdf`, page 1
- Documentation utilisateur : `docs/fr/b.md`

## Règles couvertes

- FR-0001 : création implicite d'un buffer.
- FR-0002 : ligne courante propre à chaque buffer.
- FR-0003 : buffer `(0)` courant au démarrage.
- FR-0004 : sélection d'un buffer existant et restauration de `.`.
- FR-0005 : nom parenthésé, espaces conservés.
- FR-0006 : suppression d'un buffer provisoire vide lorsqu'il est quitté.

## Grammaire

```ebnf
buffer-command = "B", "(", buffer-name, ")" ;
buffer-name    = 1..14 caractères ;
```

La commande n'accepte aucune adresse de ligne.

## AST

`BufferCommandNode` contient le nom du buffer et aucune adresse.

## Exécution

`BufferManager::create_or_select` crée le buffer lorsqu'il n'existe pas, puis le rend courant.

Le contenu et la ligne courante des buffers existants sont conservés.

## Limites du sprint

Les tabulations par buffer et l'association de fichiers ne sont pas encore implémentées. Elles devront être ajoutées dans les sprints correspondant à ces fonctions historiques.
