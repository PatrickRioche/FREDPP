# FREDPP v0.0.21 — Lecture historique `R` et bibliothèque d'exemples

FREDPP v0.0.21 restaure la sémantique historique essentielle de la commande
`R` et ajoute au kit Windows une petite bibliothèque de procédures FREDPP
utilisables comme exemples réels.

## Commande historique `R`

Sans adresse, `R` effectue une lecture complète dans le buffer courant.

Le buffer doit être vide, mais il n'a plus besoin d'être sans association ni
dans un état non modifié. Ceci permet notamment le cas historique :

    *D
    R

Après `*D`, le buffer est vide tout en conservant son association. `R` sans nom
de fichier relit alors le fichier associé.

Un nouveau nom reste possible :

    *D
    R nouveau.fredpp

La lecture complète remplace alors l'association du buffer par le nouveau
fichier et laisse le buffer propre.

## Lecture adressée

Lorsqu'une adresse est fournie, `R` insère le fichier après la position
adressée sans remplacer l'association du buffer courant.

Exemple :

    $R autre.fredpp

Sur un buffer non vide, le fichier est ajouté après la dernière ligne.

Sur un buffer vide, `$` représente la position d'insertion de fin `0`. Le
fichier est donc inséré au début du buffer. Cette règle est propre à
l'insertion par `R` : une adresse de ligne inexistante comme `1R fichier` reste
invalide sur un buffer vide.

Ce comportement est notamment nécessaire à des procédures qui construisent un
buffer par concaténations successives avec `$R`.

## Bibliothèque minimale d'exemples — Windows

Le ZIP Windows contient désormais un répertoire :

    library/

avec quatre procédures fournies comme exemples :

    aide.fredpp
    hello.fredpp
    index.fredpp
    ouya.fredpp

`index.fredpp` parcourt la bibliothèque et affiche la description des
procédures disponibles.

`ouya.fredpp` est volontairement adapté à Windows. Les exemples contenant des
chemins ou commandes Windows ne sont donc pas ajoutés tels quels aux archives
Debian/macOS.

## Validation

La suite automatisée complète a été exécutée après la correction :

    100% tests passed out of 50

Le fonctionnement a également été validé directement avec :

    fredpp index

Résultat attendu et obtenu :

    aide  - Affiche l'aide de la procedure fredpp
    hello - Classique Hello Word
    index - Liste les usages des procedures sous /fred/library
    ouya  - Recherche dans le chemin la liste de fichier correspondant au model

Les fichiers historiques sous `docs/fr/reference/commandes` restent inchangés.

## Paquets de release

Le workflow de release continue de produire les artefacts supportés du projet :

- Windows x64 ;
- Debian/Linux amd64 ;
- Debian/Linux ARM64 ;
- macOS Intel x64 ;
- macOS Apple Silicon ARM64 ;
- extension Visual Studio Code FREDPP 0.1.0 ;
- sommes de contrôle SHA-256.

### Windows 11

`fredpp.exe` n'est pas encore signé numériquement. Selon la configuration de
Windows 11, Smart App Control ou Microsoft Defender SmartScreen peut donc
bloquer son exécution.

Télécharger FREDPP uniquement depuis les Releases GitHub officielles.

### macOS

Les exécutables macOS restent non signés et non notarisés dans cette version.
