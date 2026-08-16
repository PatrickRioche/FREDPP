# FREDPP v0.0.20 — Sauts arrière dans les procédures

FREDPP v0.0.20 complète le contrôle de flot des procédures FRED avec la prise en charge des sauts vers une étiquette située avant l'instruction courante.

## Commande `J` et boucles historiques

La commande `J(label)` peut désormais cibler une étiquette `@(label)` située plus haut dans la procédure.

FREDPP conserve la recherche vers l'avant en priorité. Si aucune étiquette correspondante n'est trouvée après l'instruction `J`, la recherche reprend depuis le début de la procédure afin de permettre un saut arrière.

Ce comportement est indispensable aux boucles utilisées dans les procédures FRED historiques.

Exemple :

    @(loop)
    ...
    J(loop)

Les formes conditionnelles `J(label)T` et `J(label)F` bénéficient du même mécanisme de résolution des étiquettes.

Une étiquette réellement absente continue de provoquer l'erreur :

    ? label not found

## Validation avec une procédure historique

Le comportement a notamment été validé avec la procédure `index.fredpp`.

Cette procédure utilise une boucle de la forme :

    @(ttqlistproc)
    B(listproc)
    N(listproc):$=0 J(fttqlistproc)T
    1M(1fic)
    JM/ le fichier est : .../
    J(ttqlistproc)

    @(fttqlistproc)

Le saut `J(ttqlistproc)` revient désormais correctement vers l'étiquette située en amont.

La procédure peut ainsi parcourir successivement les fichiers présents dans la bibliothèque FREDPP avant de sortir de la boucle lorsque la condition de fin est satisfaite.

## Résolution des étiquettes

La résolution d'un `J(label)` suit désormais l'ordre suivant :

1. recherche d'une étiquette correspondante après l'instruction courante ;
2. si aucune n'est trouvée, recherche depuis le début de la procédure jusqu'à l'instruction courante ;
3. si aucune étiquette correspondante n'existe, émission de `? label not found`.

Cette stratégie conserve le comportement existant pour les sauts vers l'avant tout en ajoutant la possibilité d'effectuer des boucles arrière.

## Tests

La suite automatisée complète a été exécutée après l'implémentation :

    100% tests passed out of 50

Le fonctionnement a également été vérifié directement avec `index.fredpp` sur une bibliothèque réelle.

## Compatibilité

Cette version n'introduit aucun changement de syntaxe.

Elle complète l'implémentation de la commande historique `J` afin de permettre les structures de contrôle utilisant des sauts arrière et améliore ainsi la compatibilité de FREDPP avec les procédures FRED historiques.

## Paquets de release

Le workflow de release existant produit les artefacts supportés du projet :

- Windows x64 ;
- Debian/Linux amd64 ;
- Debian/Linux ARM64 ;
- macOS Intel x64 ;
- macOS Apple Silicon ARM64 ;
- extension Visual Studio Code FREDPP ;
- sommes de contrôle SHA-256.

### Windows 11

`fredpp.exe` n'est pas encore signé numériquement. Selon la configuration de Windows 11, Smart App Control ou Microsoft Defender SmartScreen peut donc bloquer son exécution.

Télécharger FREDPP uniquement depuis les Releases GitHub officielles.

### macOS

Les exécutables macOS restent non signés et non notarisés dans cette version.
