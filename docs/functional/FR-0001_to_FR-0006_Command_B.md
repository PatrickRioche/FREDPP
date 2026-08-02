# Règles fonctionnelles — Commande B

Source primaire : `b.pdf`, page 1.

## FR-0001 — Création implicite

La première référence à un nom par `B(nom)` crée automatiquement le buffer.

## FR-0002 — Ligne courante propre au buffer

Chaque buffer mémorise sa propre valeur de `.`.

## FR-0003 — Buffer initial

Au démarrage, le buffer courant est `(0)`.

## FR-0004 — Restauration du contexte

Lorsqu'un buffer existant redevient courant, sa ligne courante précédente est restaurée.

## FR-0005 — Nom parenthésé

La commande utilise la forme `B(nom)`. Les espaces contenus entre les parenthèses font partie du nom.

## FR-0006 — Buffer provisoire

Un buffer vide et sans fichier associé est supprimé lorsqu'il est quitté. L'association de fichiers n'étant pas encore implémentée, S2.7 applique cette règle aux buffers vides non nommés `0`.
