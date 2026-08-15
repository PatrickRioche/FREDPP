# FREDPP v0.0.18 — Aide interactive FRED/FREDPP

Cette release améliore l'interface interactive de FREDPP et clarifie la
séparation entre le langage historique FRED et les outils propres à FREDPP.

## Aide FRED et FREDPP

L'aide est désormais organisée selon une séparation simple :

- `?` affiche l'index des commandes et options FRED disponibles ;
- `?commande` affiche la documentation détaillée de la commande FRED ;
- `?:` affiche uniquement les commandes, outils et informations propres à FREDPP.

Les rubriques spécifiques `?version`, `?wu` et `?procedure` restent accessibles
directement et sont référencées dans l'aide FREDPP.

Les anciennes commandes spéciales FREDPP devenues redondantes ont été retirées :
`:help`, `:new`, `:use`, `:append`, `:insert` et `:delete`. L'ancienne commande
`:buffers` reste remplacée par la commande FRED `FB`.

## Pager interactif

Les rubriques d'aide utilisent maintenant un pager interactif dans un terminal :

- `Page Up` affiche la page précédente ;
- `Page Down` affiche la page suivante ;
- `Q` quitte l'aide et revient au prompt FREDPP.

La hauteur des pages est adaptée à la fenêtre du terminal. L'écran est effacé
entre les pages et à la sortie du pager.

Lorsque l'entrée standard n'est pas interactive, notamment dans les tests,
scripts ou redirections, l'aide reste affichée intégralement sans attendre de
touche.

## Commande `:cls`

La commande FREDPP `:cls` efface l'écran et replace le curseur en haut à gauche.

Elle s'appuie sur une primitive terminal réutilisable également employée par le
pager d'aide :

- API console Win32 sous Windows ;
- séquences terminal compatibles sous Unix/macOS.

## Démarrage

Le bandeau interactif a été simplifié :

```text
FREDPP v0.0.18 - Type ? for FRED help; type ?: for FREDPP commands; type Q or QQ to exit.
```

La version reste fournie dynamiquement par FREDPP.

## Compatibilité et documentation

La documentation française historique située sous
`docs/fr/reference/commandes` reste inchangée.

FREDPP continue à n'embarquer que la documentation des commandes effectivement
disponibles.

## Paquets de release

Le workflow de release existant reste utilisé pour produire les artefacts
supportés du projet, notamment :

- Windows x64 ;
- Debian/Linux amd64 ;
- Debian/Linux ARM64 ;
- macOS Intel x64 ;
- macOS Apple Silicon ARM64 ;
- extension Visual Studio Code FREDPP ;
- sommes de contrôle SHA-256.

### Windows 11

`fredpp.exe` n'est pas encore signé numériquement. Selon la configuration de
Windows 11, Smart App Control ou Microsoft Defender SmartScreen peut donc
bloquer son exécution.

Télécharger FREDPP uniquement depuis les Releases GitHub officielles. En cas de
blocage, vérifier l'alerte dans **Sécurité Windows** avant d'autoriser
l'exécution.
