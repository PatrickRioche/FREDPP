# SPEC-026 — Lecture et écriture de fichiers

## Objet

Cette spécification définit le premier périmètre opérationnel des commandes `R` et `W` dans FREDPP, ainsi que les formes d'encodage `WA` et `WU`.

## Lecture avec R

```fred
R <nom de fichier>
```

`R` exige un buffer courant vide, non associé à un fichier et non modifié. Le fichier est lu en mode binaire, puis interprété comme :

- ASCII si tous les octets sont inférieurs à 128 ;
- UTF-8 si la séquence est valide ;
- UTF-8 avec BOM si les trois octets `EF BB BF` sont présents au début.

Le BOM n'est pas conservé dans le texte du buffer. Les fichiers binaires ou les séquences UTF-8 invalides sont refusés.

## Écriture avec W

```fred
W [<nom de fichier>]
(.,.)W [<nom de fichier>]
```

Sans nom, `W` utilise le fichier associé au buffer. Sans association, un nom est obligatoire. Une écriture complète réussie associe le fichier au buffer et rend celui-ci propre. Une écriture partielle ne modifie ni l'association ni l'état modifié.

## Encodages

```fred
WA <nom de fichier>
WU <nom de fichier>
WB <nom de fichier>
```

- `WA` conserve sa signification historique d'écriture ASCII. Toute ligne contenant un octet non ASCII provoque une erreur.
- `WU` est une extension FREDPP qui écrit en UTF-8 sans BOM.
- `WB` conserve sa signification historique GCOS/BCD. La forme est reconnue mais son exécution est refusée explicitement.
- `W` conserve l'encodage connu du buffer. Pour un nouveau buffer, UTF-8 est utilisé.

## Fins de ligne

`R` détecte LF ou CRLF. Une écriture complète conserve ce choix ainsi que la présence ou l'absence d'une fin de ligne finale. Une écriture partielle termine l'extrait par une fin de ligne.

## Protection de la sortie

`Q` refuse l'arrêt normal lorsqu'au moins un buffer est modifié. `QQ` force l'arrêt immédiat.

## Limites

Le premier périmètre n'inclut pas :

- `WX` ;
- les listes de noms de fichiers de `R` ;
- le format GCOS/BCD ;
- les encodages autres que l'ASCII et l'UTF-8.
