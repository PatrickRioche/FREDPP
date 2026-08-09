# FREDPP Language Support for Visual Studio Code

Support du langage FREDPP pour les fichiers de procédures `.fredpp`.

## Fonctionnalités

- association automatique de l'extension `.fredpp` au langage FREDPP ;
- coloration des commandes FREDPP actuellement prises en charge ;
- coloration des adresses de lignes (`1`, `.`, `$`, `1,$`) ;
- coloration des motifs `/.../` ;
- coloration des directives de flux actuellement prises en charge (`\B`,
  `\L`, `\S`, `\F`) ;
- prise en charge de la commande système `!` et de `ZG` ;
- snippets de base pour les procédures FREDPP ;
- commentaire de ligne FRED avec `"`.

## Installation d'un VSIX

Dans Visual Studio Code :

1. ouvrez la vue **Extensions** ;
2. ouvrez le menu `...` ;
3. choisissez **Install from VSIX...** ;
4. sélectionnez le fichier `FREDPP-Language-vX.Y.Z.vsix`.

Le VSIX est déclaratif et ne contient aucun binaire natif FREDPP.

## Développement

Ouvrez le dossier `editors/vscode` dans Visual Studio Code puis utilisez le
mode de développement des extensions de VS Code.

## Exemple

```fredpp
" Exemple FREDPP
JM/Bonjour depuis FREDPP/
!echo FREDPP
ZG(capture)!echo sortie capturée
B(capture)
*
```

La liste des règles de coloration doit être mise à jour au fur et à mesure des
commandes effectivement implémentées dans FREDPP.
