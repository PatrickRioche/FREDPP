# FREDPP v0.1.2 — Préversion multi-plateforme (dev)

Cette version est une **prérelease** provenant uniquement de la branche `dev`. Elle ne modifie pas la branche `main` et ne constitue pas une version stable.

## Évolutions depuis v0.1.1

- **ELIZA** : `fredpp eliza` lance une conversation pédagogique en commandes FREDPP.
- **TIC-TAC-TOE** : `fredpp tictactoe` conserve le mode deux joueurs ; `fredpp tictactoe solo` permet de jouer X contre FREDPP (O). La stratégie Minimax est calculée à l'avance et intégrée à la procédure : aucun Python n'est nécessaire pour jouer.
- **Temps d'exécution interactif** : l'attente au clavier via `\R` est exclue du délai de sécurité de 60 secondes. Les boucles infinies restent limitées par les gardes de temps de traitement et de nombre d'instructions.
- **Alias Windows** : `fp.exe` est une copie binaire exacte de `fredpp.exe`. Elle est produite lors des builds Windows Debug et Release et incluse avec `fredpp.exe` dans le ZIP Windows. Le packaging compare leurs SHA-256 et leurs versions.
- **Tests** : scénarios ELIZA et TIC-TAC-TOE, adversaire solo, alias Windows et contrôle des bibliothèques distribuées.

## Fichiers distribués

- `FREDPP-v0.1.2-windows-x64.zip`
- `FREDPP-v0.1.2-debian13-amd64.tar.gz`
- `FREDPP-v0.1.2-debian13-arm64.tar.gz`
- `FREDPP-v0.1.2-macos-x64.tar.gz`
- `FREDPP-v0.1.2-macos-arm64.tar.gz`
- `fredpp_0.1.2_amd64.deb`
- `fredpp_0.1.2_arm64.deb`
- `FREDPP-Language-v0.1.2.vsix`
- `SHA256SUMS.txt`

GitHub fournit également deux archives source (ZIP et TAR.GZ). Le script Python de génération de la politique Minimax est destiné aux développeurs, pas aux utilisateurs du jeu.

## Utilisation de l'alias dans Windows PowerShell

Dans le dossier du kit Windows décompressé :

```powershell
.\fp.exe --version
.\fp.exe eliza
.\fp.exe tictactoe solo
.\fredpp.exe tictactoe
```

Pour lancer simplement `fp` à partir d'un autre répertoire, le dossier des exécutables doit figurer dans la variable d'environnement `PATH`.

## Limites connues

- Les 103 entrées historiques reconnues ne correspondent pas encore à une émulation GCOS exhaustive.
- Certaines formes avancées des directives de flot doivent encore être homologuées.
- Les délais d'exécution sont contrôlés entre les instructions, pas au milieu d'une commande externe bloquante.
- Les binaires Windows ne sont pas signés ; SmartScreen peut signaler une application inconnue. Ne désactivez pas les protections pour lancer le programme.
- Android dispose d'un workflow indépendant et n'est pas inclus dans cette matrice de release.
