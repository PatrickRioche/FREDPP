## v0.0.17 — Flot de procédures et packaging macOS

### Ajouté
- packaging natif macOS Intel x64 ;
- packaging natif macOS Apple Silicon arm64 ;
- compilation et tests macOS dans GitHub Actions avant publication ;
- archives macOS intégrées à `SHA256SUMS.txt` ;
- labels et sauts de procédures `@(label)`, `J(label)`, `J(label)T/F` ;
- premier sous-ensemble des registres numériques historiques ;
- enchaînement de commandes sur une même ligne ;
- délimiteurs historiques arbitraires pour `JM` et `JP`.

### Amélioré
- `M(buffer)` suit la sémantique historique inter-buffer ;
- `\S(buffer)` est développé dans `!`, `ZG` et les modèles de `G` ;
- une erreur de procédure conserve l'état puis rend la main au mode interactif ;
- les buffers système du bootstrap sont correctement visibles dans `FB`.

### Distribution
- la release attend les builds Windows, Debian amd64, Debian arm64, macOS x64,
  macOS arm64 et VS Code avant publication ;
- la release est explicitement marquée `Latest` ;
- les exécutables macOS restent non signés et non notarisés dans cette version.

### Validation
- suite Windows : **49/49 tests réussis** avant préparation de la release ;
- chaque architecture macOS exécute la suite complète sur runner natif.

# Changelog

## v0.0.16 — Bootstrap historique des procédures

### Ajouté

- lancement d'une procédure FREDPP par son nom, avec extension `.fredpp`
  facultative ;
- recherche d'une procédure dans le répertoire courant puis dans la
  bibliothèque FREDPP ;
- transmission des paramètres de ligne de commande dans `B(0)`, un argument
  par ligne ;
- initialisation des buffers système historiques :
  - `B(d)` : date locale au format `MM/DD/YY` ;
  - `B(t)` : heure locale au format `HH:MM` ;
  - `B(u)` : identifiant utilisateur ;
- prise en charge du fichier d'initialisation utilisateur `.init.fredpp`,
  exécuté avant le programme principal ;
- possibilité de surcharger explicitement l'init avec `FREDPP_INIT` ;
- ajout du logo FREDPP au dépôt.

### Compatibilité historique

Le bootstrap des procédures se rapproche du programme `BOOTSTRAP` UWTOOLS :
les buffers système sont initialisés en premier, l'init utilisateur est
exécuté ensuite, puis `B(0)` est construit avant le chargement et l'exécution
de la procédure principale dans `B(.)`.

### Maintenance

- suppression des anciens scripts temporaires `apply_*.py` du dépôt et
  exclusion de ces scripts via `.gitignore` ;
- archivage des anciens fichiers `LIVRABLE*` et `00_README*` sous `project/`
  afin d'alléger la racine du dépôt ;
- aucune modification des fichiers historiques sous
  `docs/fr/reference/commandes`.

### Validation

- suite complète : **37/37 tests réussis** avant préparation de la release.

## v0.0.15

### Added

- Packaging Debian 13 ARM64 natif avec compilation et tests sur runner ARM64.
- Extension Visual Studio Code FREDPP version 0.1.0 sous `editors/vscode`.
- Publication du VSIX universel dans les Releases GitHub.
- Artefacts Debian ARM64 ajoutés au manifeste SHA-256.

### Changed

- Le packaging Debian détecte désormais l'architecture avec
  `dpkg --print-architecture` au lieu de forcer `amd64`.
- Le fichier `LISEZMOI-DEBIAN.txt` documente `amd64` et `arm64`.
- Le kit Windows documente le statut non signé de `fredpp.exe` et les
  protections Windows 11 susceptibles de le bloquer.

## v0.0.14

### Added

- Commande historique `!` pour exécuter une commande du système hôte.
- Commande historique `ZG(buffer)<commande>` pour capturer la production
  destinée à la visualisation dans un buffer.
- Aides embarquées `?!` et `?zg`.
- Test multiplateforme `test_system_zg`.

### Changed

- `ExecutionContext` permet une redirection temporaire de `Output`.
- `BufferManager` peut créer un buffer sans changer le buffer courant.

## v0.0.13

### Added

- Directive de flot `\S(buffer)` d'après DNB11A.
- Injection littérale d'un buffer sans retours de ligne.
- Tests couvrant la suppression des retours de ligne, la littéralité, le buffer
  vide et le buffer inexistant.

### Changed

- `InputCharacter` transporte désormais l'information de littéralité.
- `BufferInputSource` peut supprimer les retours de ligne et marquer ses
  caractères comme littéraux.

## v0.0.12
### Added

- Option historique `O+M` / `O-M` avec `O-M` comme valeur implicite.
- Aide historique `?om`.
- Moteur `ProcedureRunner` pour exécuter des buffers de commandes.
- Directive `\B(buffer)` pour appeler une procédure stockée dans un buffer.
- Lancement direct `fredpp script.fredpp` avec chargement initial dans `B(.)`.
- Aide FREDPP `?procedure`.
- Exécution des blocs `A`, `I` et `C` jusqu'à `\F` dans les procédures.
- Enchaînement de `JM/.../` et `JP/.../` sur une ligne de procédure.
- Tests `test_monitor`, `test_procedure_runner` et `cli_script`.

### Changed

- `FO` affiche les états connus de `OI(` et `OM`.
- La couverture de la référence passe à 22/103.
- La suite passe de 30 à 33 tests.

### Known limitations

- `\B(buffer)` doit être seul sur sa ligne dans ce bootstrap minimal.
- Les suites générales de commandes restent reportées, hors séquences délimitées JM/JP.
- Le bootstrap historique complet n'est pas encore reproduit.

## v0.0.11
### Added

- Commande spéciale historique `"` pour documenter les futures procédures FREDPP.
- Commande `JM` pour écrire un message au terminal avec retour à la ligne.
- Commande `JP` pour écrire un message au terminal sans retour à la ligne.
- Aides embarquées `?"`, `?jm` et `?jp`.
- Test `test_messages`.

### Changed

- Les commentaires passent désormais par le parser et l'AST au lieu d'être ignorés directement par le REPL.
- La couverture de la référence passe à 21/103.
- La suite passe de 29 à 30 tests.

### Known limitations

- Le chaînage de plusieurs commandes `JM`/`JP` sur une même ligne est reporté au moteur de procédures.

## v0.0.10
### Added

- Option historique `O+I(` / `O-I(` et noms courts de buffers à un caractère.
- Commandes historiques `FB` et `FO`.
- Aides embarquées `?fb`, `?fo` et `?oi(`.
- Tableau de couverture de la référence historique.

### Changed

- `FB` remplace la commande de développement `:buffers`.
- La suite passe de 27 à 29 tests.

## v0.0.9

### Added

- Kits portables officiels pour Windows x64 et Debian 13 amd64.
- Paquet Debian natif `fredpp_X.Y.Z_amd64.deb` généré avec CPack.
- Workflow GitHub Actions de publication déclenché par les tags `v*`.
- Vérification automatique de la cohérence entre le tag Git et la version CMake.
- Manifeste `SHA256SUMS.txt` publié avec chaque release.
- Scripts `package-release.ps1` et `package-release.sh` pour produire les kits localement.
- Scripts `release-stats.ps1` et `release-stats.sh` pour consulter les téléchargements par ressource.
- Documentation française `docs/project/RELEASES.md` et spécification `SPEC-027`.
- Test `test_release_manifest` pour vérifier les fichiers obligatoires de livraison.

### Changed

- La version des tests n'est plus dupliquée manuellement : elle provient de `PROJECT_VERSION`.
- Le binaire Windows Release utilise le runtime MSVC statique.
- La suite passe de 26 à 27 tests.
- Le workflow historique dupliqué `.github/workflows/build.yml` est supprimé ; `build-and-test.yml` reste le workflow de validation générale.

## v0.0.8

### Added

- Commande historique `R` pour charger un fichier dans un buffer vide et non associé.
- Commande historique `W` pour écrire tout le buffer ou une plage de lignes.
- Forme historique `WA` pour forcer une écriture ASCII stricte.
- Extension FREDPP `WU` pour écrire en UTF-8 sans BOM.
- Détection à la lecture des fichiers ASCII, UTF-8 et UTF-8 avec BOM.
- Conservation des fins de ligne LF ou CRLF et de l'absence ou présence d'une fin de ligne finale.
- Association d'un fichier, d'un encodage et d'un état modifié à chaque buffer.
- Aides embarquées `?r`, `?w` et `?wu`.
- Test `test_file_io` et couverture complémentaire de `Q` avec des buffers modifiés.
- Spécification `SPEC-026` consacrée à la lecture, à l'écriture et aux encodages.

### Changed

- `Q` refuse désormais l'arrêt normal lorsqu'un buffer contient des modifications non enregistrées.
- `QQ` conserve son comportement d'arrêt immédiat.
- `W` sans option conserve l'encodage connu du buffer ; un nouveau fichier utilise UTF-8.
- Une écriture complète réussie rend le buffer propre ; une écriture partielle ne le fait pas.
- La suite passe de 25 à 26 tests.

### Known limitations

- `WB` est reconnu mais non implémenté, car il correspond au format historique GCOS/BCD.
- `WX` et la lecture d'une liste de fichiers avec `R` sont reportés.
- Les encodages autres que l'ASCII et l'UTF-8 ne sont pas pris en charge.

## v0.0.7

### Added

- Commande historique `S` pour substituer un modèle dans la ligne courante ou une plage.
- Remplacement de toutes les occurrences non chevauchantes et prise en charge de `&` pour réinsérer le texte trouvé.
- Séparateurs symboliques pour `S`, notamment `/`, `?`, `!` et `;`.
- Forme `S/<modèle>/<chaîne>/P` pour afficher la dernière ligne modifiée.
- Exécution de `S` à l'intérieur de `G`.
- Commandes historiques `Q` et `QQ` pour quitter FREDPP.
- Tests `test_substitute` et `test_quit`.
- Spécification `SPEC-025` consacrée aux commandes `S` et `Q`.

### Changed

- Suppression de la commande spéciale `:quit` et remplacement par `Q`.
- Ajout des aides françaises `?s` et `?q`, sans modification des fichiers de référence.
- Le moteur de modèles expose désormais la position des correspondances pour permettre les substitutions.
- La suite passe de 23 à 25 tests.

### Known limitations

- `Q!<commande TSS>` n'est pas encore disponible et sera traité avec l'exécution externe historique.
- La vérification des buffers associés à des fichiers sera complétée avec les futures commandes de lecture et d'écriture.

## v0.0.6

### Added

- Alias historique `*` pour représenter la plage complète `1,$`.
- Forme seule `*`, équivalente à `1,$P`.
- Formes composées telles que `*P`, `*D` et `*G/<modèle>/P`.
- Aide embarquée `?*` stockée hors des documentations de référence.
- Test d'intégration `test_star_alias`.
- Spécification `SPEC-024` consacrée à l'alias étoile.

### Changed

- Le parser transforme l'alias initial `*` en plage AST normale `1,$`.
- La suite de tests passe de 22 à 23 tests.

## v0.0.5

### Added

- Métadonnées de compilation contenant le commit Git et l'état des sources.
- Aide `?:` consacrée aux commandes spéciales de FREDPP.
- Documentation française `docs/fr/fredpp/commandes-speciales.md`.
- Spécification `SPEC-023` sur l'identité de compilation.

### Changed

- La version est désormais définie uniquement par `project(FREDPP VERSION ...)` dans CMake.
- `?version` et `--version` affichent la version, le commit, l'état des sources et la section de roadmap associée.
- `:help` affiche la même rubrique que `?:`.
- `ROADMAP.md` est entièrement actualisé en français avec le contenu de chaque version.
- Les tests de version et d'aide couvrent les nouvelles informations.
- Les index `?` et `?:` utilisent désormais une présentation alignée et lisible dans le terminal.

## v0.0.4

### Added

- Commande historique `G` avec sélection normale ou inversée (`G~`).
- Commande historique `Z` pour changer la ligne courante sans affichage.
- Moteur d'évaluation des modèles FRED.
- Exécution initiale de `P`, `D` et `Z` à l'intérieur de `G`.
- Trois nouveaux tests : global, zap et correspondance de modèles.

### Changed

- L'aide embarquée lit désormais uniquement les pages sélectionnées sous
  `docs/fr/reference/commandes/`, sans les modifier.
- `?index` est généré à partir de la liste des commandes intégrées.
- Le README indique `?version` au lieu d'un numéro de version écrit en dur.
- Le `switch` de diagnostic des adresses est désormais exhaustif pour Clang.

## v0.0.3

### Added

- Aide Markdown française embarquée et disponible hors connexion.
- `HelpManager` et test unitaire associé.
- Alias `?help` et `?h` pour l'index de l'aide.

## v0.0.2

- Commandes historiques initiales et gestion des buffers nommés.

- amélioration de la lisibilité terminal des aides de commandes sans modification des Markdown de référence ;
