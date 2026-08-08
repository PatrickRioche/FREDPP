# Notes de version FREDPP

La version exacte et le commit de construction se consultent avec `?version`.

## Sprint 2.15 — Options et informations sur les buffers

- `O+I(` / `O-I(` et noms courts de buffers ;
- `FO` pour afficher les options actives ;
- `FB` pour afficher l'état des buffers et leur fichier associé ;
- suppression de `:buffers` ;
- aides `?fb`, `?fo` et `?oi(` ;
- tableau de couverture de la référence ;
- passage à 29 tests.

## Sprint 2.14 — Première distribution officielle

- publication automatique des versions depuis un tag Git `vX.Y.Z` ;
- kit portable Windows x64 ;
- archive portable Debian 13 amd64 ;
- paquet Debian installable ;
- manifeste SHA-256 ;
- cohérence obligatoire entre le tag, la version CMake et le binaire ;
- compilation Release et exécution des tests avant publication ;
- suivi séparé des téléchargements Windows et Debian ;
- passage à 27 tests avec contrôle du manifeste de release.

Les commandes historiques et les formats de fichiers disponibles restent ceux de la version précédente.

Consulter `ROADMAP.md`, `CHANGELOG.md`, `docs/project/RELEASES.md` et `LIVRABLE_SPRINT_2.14.md` pour le détail.
