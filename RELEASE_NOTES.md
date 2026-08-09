# Notes de version FREDPP

La version exacte et le commit de construction se consultent avec `?version`.

## Sprint 2.17 — Procédures et bootstrap minimal

- `O+M` / `O-M`, avec `O-M` par défaut ;
- `FO` étendu à l'état de `OM` ;
- exécution de buffers avec `\B(buffer)` ;
- lancement `fredpp script.fredpp` ;
- chargement du script dans `B(.)` ;
- blocs `A`, `I`, `C` jusqu'à `\F` dans les procédures ;
- séquences délimitées `JM/JP` sur une même ligne ;
- aides `?om` et `?procedure` ;
- couverture portée à 22/103 ;
- passage attendu à 33 tests.

Cette version constitue la base de la prochaine release officielle v0.0.12, après homologation et validation multiplateforme.

## Sprint 2.16 — Commentaires et messages de procédures

- commande `"` pour les commentaires ;
- `JM` avec retour à la ligne ;
- `JP` sans retour à la ligne ;
- aides `?"`, `?jm` et `?jp` ;
- couverture portée à 21/103 ;
- passage à 30 tests.

Jalon intermédiaire : la prochaine release officielle est prévue après le Sprint 2.17.

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
