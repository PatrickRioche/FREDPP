# ADR-004 — Modèle des buffers

- Statut : accepté
- Date : 2026-08-02

## Contexte

La documentation historique de `B` montre que chaque buffer conserve son contenu et sa propre ligne courante. Un buffer inexistant est créé implicitement et un buffer provisoire vide est supprimé lorsqu'il est quitté.

## Décision

`Buffer` reste propriétaire de sa ligne courante. `BufferManager` possède les buffers, maintient le buffer courant et réalise la création/sélection implicite.

La limite historique du nom est centralisée dans `fred::limits::max_buffer_name_length`.

## Conséquences

Les futures commandes inter-buffers (`M`, `K`, Flow) utiliseront `BufferManager` au lieu de recréer leur propre modèle de stockage.
