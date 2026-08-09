# Feuille de route de FREDPP

Cette feuille de route présente les fonctionnalités intégrées dans chaque version et les prochains jalons du projet.

L'identité exacte d'un exécutable se consulte directement dans FREDPP avec :

```text
?version
```

Cette commande affiche le numéro de version, le commit Git utilisé lors de la configuration et l'état des sources.

## Versions réalisées

### v0.0.1 — Fondation du dépôt

- création du dépôt structuré ;
- mise en place de CMake et du socle C++20 ;
- premières règles de gouvernance et de contribution.

### v0.0.2 — Premières commandes historiques

- intégration des buffers et du moteur Flow ;
- lexer, flux de jetons, analyse des adresses et des modèles ;
- premières commandes exécutables de FRED ;
- mise en place de la suite de tests CTest.

### v0.0.3 — Aide embarquée et portabilité

- ajout de l'aide française Markdown embarquée ;
- consultation avec `?`, `?index`, `?h` et `?help` ;
- validation de 19 tests sous Windows/MSVC, Debian/GCC et Debian/Clang ;
- ajout du script Linux `scripts/rebuild.sh`.

### v0.0.4 — Sprint 2.9 : commandes G et Z

Modifications intégrées :

- commande historique `G` avec sélection normale et inversée `G~` ;
- commande historique `Z` pour positionner silencieusement la ligne courante ;
- moteur de correspondance des modèles FRED ;
- exécution initiale de `P`, `D` et `Z` à l'intérieur de `G` ;
- intégration sélective des aides françaises de `G` et `Z` ;
- passage de 19 à 22 tests ;
- validation de 22/22 tests sous MSVC, GCC et Clang.

### v0.0.5 — Sprint 2.10 : traçabilité et aide FREDPP

Modifications intégrées :

- numéro de version centralisé dans `project(FREDPP VERSION ...)` de CMake ;
- liaison automatique entre l'exécutable et le commit Git au moment de la configuration ;
- indication `propre`, `modifié` ou `inconnu` pour l'état des sources ;
- enrichissement de `?version` et de `--version` avec ces informations ;
- référence automatique à la section correspondante de cette feuille de route ;
- ajout de `?:` pour lister les commandes spéciales de FREDPP ;
- alignement de `:help` sur la même aide ;
- documentation française séparée des commandes spéciales, sans modification de `docs/fr/reference/commandes/` ;
- suppression de tout numéro de version écrit en dur dans le README ;
- tests étendus pour l'identité de compilation et l'aide `?:` ;
- présentation alignée des aides `?` et `?:` pour améliorer leur lisibilité dans le terminal.

## Version v0.0.6 — Sprint 2.11

### Modifications intégrées

- implémentation de `*` comme alias de la plage complète `1,$` ;
- forme seule `*` équivalente à `1,$P` ;
- formes composées `*P`, `*D` et `*G/<modèle>/P` ;
- transformation de l'alias en `RangeAddressNode` sans ajout d'une commande runtime ;
- aide embarquée `?*`, conservée hors de `docs/fr/reference/commandes/` ;
- ajout d'un test d'intégration et passage à 23 tests ;
- validation attendue sous MSVC, GCC et Clang.

## Version v0.0.7 — Sprint 2.12

### Modifications intégrées

- commande historique `S` sur la ligne courante, une adresse ou une plage ;
- remplacement de toutes les occurrences non chevauchantes ;
- prise en charge de `&` dans la chaîne de remplacement ;
- séparateurs symboliques tels que `/`, `?`, `!` et `;` ;
- affichage optionnel avec la forme `S/<modèle>/<chaîne>/P` ;
- exécution de `S` à l'intérieur de `G` ;
- commande historique `Q` pour l'arrêt normal ;
- commande historique `QQ` pour l'arrêt immédiat ;
- suppression de la commande spéciale `:quit` ;
- intégration sélective des aides françaises `?s` et `?q` ;
- ajout de deux tests et passage à 25 tests ;
- validation attendue sous MSVC, GCC et Clang.

### Limites reportées

- `Q!<commande TSS>` dépendra du futur mécanisme portable d'exécution externe ;
- le contrôle des buffers associés à des fichiers sera complété avec les commandes historiques de lecture et d'écriture ;
- les suites générales de commandes restent à généraliser au-delà du suffixe `P` pris en charge par `S`.

## Version v0.0.8 — Sprint 2.13

### Modifications intégrées

- commande historique `R` pour lire un fichier dans un buffer vide, non associé et non modifié ;
- détection automatique de l'ASCII, de l'UTF-8 et du BOM UTF-8 ;
- commande historique `W` avec nom de fichier optionnel et écriture d'une plage ;
- forme `WA` pour forcer l'ASCII avec refus des caractères non représentables ;
- extension FREDPP `WU` pour écrire en UTF-8 sans BOM ;
- reconnaissance de `WB` avec un diagnostic explicite indiquant que GCOS/BCD n'est pas pris en charge ;
- conservation de LF ou CRLF et de la fin de ligne finale lors d'une réécriture complète ;
- suivi, pour chaque buffer, du fichier associé, de l'encodage et de l'état propre ou modifié ;
- protection de `Q` contre la perte de buffers modifiés, avec `QQ` comme sortie immédiate ;
- intégration sélective des aides `?r`, `?w` et `?wu` ;
- ajout de `test_file_io` et passage à 26 tests ;
- validation attendue sous MSVC, GCC et Clang.

### Limites reportées

- `WB` reste réservé à sa signification historique GCOS/BCD et n'est pas implémenté ;
- `WX` et les listes avancées de fichiers de `R` seront traités ultérieurement ;
- seuls l'ASCII et l'UTF-8 sont pris en charge.

## Version v0.0.9 — Sprint 2.14

### Modifications intégrées

- première chaîne officielle de livraison binaire ;
- ZIP portable Windows x64 construit en configuration Release ;
- archive portable Debian 13 amd64 ;
- paquet Debian natif installant `fredpp` dans le système ;
- publication automatique sur GitHub Releases lors de l'envoi d'un tag `v*` ;
- vérification stricte de la correspondance entre le tag et la version CMake ;
- exécution des tests Release avant toute publication ;
- génération d'un manifeste `SHA256SUMS.txt` ;
- scripts PowerShell et Bash pour produire les paquets localement ;
- scripts PowerShell et Bash pour suivre les téléchargements des ressources GitHub ;
- centralisation de la version attendue par les tests à partir de `PROJECT_VERSION` ;
- ajout du test `test_release_manifest` et passage à 27 tests ;
- suppression du workflow de compilation historique dupliqué.

### Artefacts officiels

```text
FREDPP-vX.Y.Z-windows-x64.zip
FREDPP-vX.Y.Z-debian13-amd64.tar.gz
fredpp_X.Y.Z_amd64.deb
SHA256SUMS.txt
```

## Version v0.0.10 — Sprint 2.15
### Modifications intégrées

- option `O+I(` / `O-I(` et noms courts de buffers ;
- `FO` pour afficher les options actuellement en service ;
- `FB` pour afficher les buffers par ordre décroissant de dernière utilisation, avec ligne courante, nombre de lignes, fichier associé et indicateur de modification ;
- suppression de `:buffers` ;
- aides `?fb`, `?fo` et `?oi(` ;
- tableau d'avancement par rapport à la référence historique ;
- passage à 29 tests.

## Version v0.0.11 — Sprint 2.16
### Modifications intégrées

- commande spéciale historique `"` pour les commentaires de procédures ;
- `JM` pour afficher un message suivi d'un retour à la ligne ;
- `JP` pour afficher un message sans retour à la ligne ;
- formes texte et `/message/` de `JM` et `JP` ;
- aides `?"`, `?jm` et `?jp` ;
- commentaires représentés dans l'AST et exécutés comme no-op historique ;
- couverture de la référence portée à 21/103 ;
- passage à 30 tests.

Cette version est un jalon intermédiaire. La prochaine publication binaire est prévue après le Sprint 2.17.

## État fonctionnel actuel

Commandes FRED disponibles :

```text
P, L, D, A, B, I, C, M, T, G, Z, S, Q, R, W, WA, WU, FB, FO, JM, JP
```

Les limites détaillées sont suivies dans `docs/project/COMMAND_STATUS.md`.

Les options `OI(` et `OM` sont disponibles ; `OM` est implicitement à `O-M`.

La commande spéciale `\"` est disponible pour commenter les procédures.

## Version v0.0.12 — Sprint 2.17

### Modifications intégrées

- option `O+M` / `O-M` de monitorage, avec `O-M` par défaut ;
- `FO` affiche désormais les états de `OI(` et `OM` ;
- moteur `ProcedureRunner` pour exécuter des buffers de commandes ;
- directive autonome `\B(buffer)` avec appels imbriqués et protection de profondeur ;
- lancement direct `fredpp script.fredpp` ;
- chargement initial du script dans `B(.)` et bootstrap minimal ;
- prise en charge de `A`, `I` et `C` avec données jusqu'à `\F` dans les procédures ;
- enchaînement des formes délimitées `JM/.../` et `JP/.../` sur une ligne de procédure ;
- aides `?om` et `?procedure` ;
- couverture de la référence portée à 22/103 ;
- passage attendu de 30 à 33 tests.

Une nouvelle release officielle v0.0.12 est prévue après homologation et tests multiplateformes.

## Version v0.0.13 — Sprint 2.18

### Modifications intégrées

- ajout de la directive de flot `\S(buffer)` selon DNB11A ;
- injection littérale du contenu du buffer désigné ;
- suppression de tous les retours de ligne lors de l'injection par `\S` ;
- absence de réinterprétation des caractères spéciaux provenant du buffer ;
- conservation du comportement existant de `\B(buffer)` ;
- tests de non-régression du moteur de flot ;
- préparation de l'architecture pour les autres directives DNB11A.

## Version v0.0.15 — Sprint 2.20

### Packaging et outillage

- ZIP portable Windows x64 ;
- paquets Debian 13 amd64 et arm64 ;
- compilation et tests natifs ARM64 avant publication ;
- support Raspberry Pi OS 64 bits compatible via le paquet arm64 ;
- support Visual Studio Code versionné sous `editors/vscode` ;
- VSIX universel publié avec les autres artefacts ;
- manifeste SHA-256 couvrant tous les artefacts ;
- avertissement Windows 11 pour le binaire non signé.

## Version v0.0.14 — Sprint 2.19

### Modifications intégrées

- commande historique `!` pour appeler le shell du système hôte ;
- sortie de `!` routée par l'abstraction `Output` de FREDPP ;
- commande historique `ZG(buffer)<commande>` ;
- capture générique de la production destinée à la visualisation ;
- insertion de cette production après la ligne courante du buffer destination ;
- création du buffer destination par `ZG` lorsqu'il n'existe pas ;
- aides historiques `?!` et `?zg` embarquées sans modifier les références.

## Prochains jalons

### Prochaines versions 0.0.x

- réaligner les commandes `M` et `T` sur leur comportement historique documenté ;
- compléter les formes avancées de `R` et `W`, notamment `WX` ;
- compléter les formes courtes de la commande `B` ;
- poursuivre l'implémentation des commandes historiques documentées ;
- étendre `G` aux suites de commandes et aux comportements historiques complémentaires ;
- embarquer uniquement la documentation française des commandes effectivement implémentées.

### v0.1.0 — Premier éditeur historique utilisable

- ensemble cohérent de commandes FRED validées ;
- exécution d'exemples historiques sans adaptation majeure ;
- documentation utilisateur française intégrée ;
- release testée sous MSVC, GCC et Clang.

### v1.0.0 — Réimplémentation moderne de référence

- couverture des commandes principales du langage FRED ;
- procédures, options et environnement documentés ;
- compatibilité multiplateforme automatisée ;
- documentation utilisateur et développeur complète ;
- corpus de tests historiques et de non-régression.

- rendu terminal lisible des documentations Markdown embarquées, sans modifier les sources de référence ;
