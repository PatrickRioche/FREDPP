# FREDPP v0.0.16 — Bootstrap historique des procédures

Cette release étend le moteur de procédures FREDPP afin de reproduire plus
fidèlement le bootstrap historique UWTOOLS.

## Procédures et ligne de commande

Une procédure physique conserve l'extension `.fredpp`, notamment pour
l'association avec le support Visual Studio Code, mais l'extension peut être
omise lors de l'appel.

Exemples :

```text
fredpp hello
fredpp aide hello
```

Les paramètres placés après le nom de la procédure sont transmis dans `B(0)`,
un argument par ligne.

Pour un nom simple, FREDPP recherche la procédure dans le répertoire courant,
puis dans la bibliothèque FREDPP configurée par la plateforme. Les chemins
explicites restent utilisables.

## Buffers système du bootstrap

Au démarrage d'une procédure, FREDPP initialise désormais :

- `B(d)` avec la date locale au format historique `MM/DD/YY` ;
- `B(t)` avec l'heure locale au format historique `HH:MM` ;
- `B(u)` avec l'identifiant de l'utilisateur.

Ces valeurs proviennent de l'environnement et de l'horloge locale du système.

## Initialisation utilisateur

FREDPP prend en charge un fichier `.init.fredpp` propre à l'utilisateur,
exécuté après l'initialisation de `B(d)`, `B(t)` et `B(u)`, mais avant la
construction de `B(0)` et avant le programme principal.

Chemins par défaut :

```text
Windows : %USERPROFILE%\fredpp\.init.fredpp
Unix    : $HOME/fredpp/.init.fredpp
```

L'absence du fichier par défaut n'est pas une erreur. La variable
d'environnement `FREDPP_INIT` permet de fournir un autre fichier ou, avec une
valeur vide, de désactiver explicitement l'init. Une erreur réelle de lecture
ou d'exécution de l'init interrompt le bootstrap.

## Ordre du bootstrap

```text
B(d), B(t), B(u)
        ↓
.init.fredpp éventuel
        ↓
B(0) = paramètres CLI
        ↓
résolution de la procédure
        ↓
B(.) = programme
        ↓
exécution
```

## Maintenance du dépôt

- le logo FREDPP est maintenant suivi dans le dépôt ;
- les anciens scripts temporaires `apply_*.py` ont été retirés et sont
  désormais ignorés ;
- les anciens livrables et fichiers de pilotage intermédiaires ont été
  archivés sous `project/` ;
- les fichiers historiques sous `docs/fr/reference/commandes` restent
  inchangés.

## Validation

Avant préparation de cette release :

```text
37/37 tests réussis
```

La release doit être recréée et validée après le changement de version avant
la création du tag.

## Windows 11 — exécutable non signé

L'exécutable Windows FREDPP n'est pas encore signé numériquement. Selon la
configuration de Windows 11, Smart App Control ou Microsoft Defender
SmartScreen peut donc avertir l'utilisateur ou bloquer l'exécution.

Avant toute exception de sécurité, vérifier que l'archive provient bien des
**Releases GitHub officielles du projet FREDPP**. Les informations de
protection et l'historique de détection peuvent être consultés dans
**Sécurité Windows**.

## Documentation

Pour l'historique et la feuille de route, consulter `CHANGELOG.md`,
`ROADMAP.md` et `docs/project/RELEASES.md`.

La version effectivement exécutée peut être consultée directement dans FREDPP
avec la commande `?version`.
