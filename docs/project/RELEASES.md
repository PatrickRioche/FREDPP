# Livraisons de FREDPP

## Objectif

Une version de FREDPP est publiée dans l'espace **Releases** du dépôt GitHub. Elle est construite automatiquement à partir d'un tag Git `vX.Y.Z` correspondant exactement au numéro déclaré dans `CMakeLists.txt`.

Un tag placé au **sommet courant de `dev`** produit une **préversion** (*pre-release*). Un tag placé au **sommet courant de `main`** produit une version stable. Le workflow refuse les tags placés ailleurs ; aucune fusion automatique de `dev` vers `main` n'a lieu.

## Fichiers publiés

Pour chaque version :

```text
FREDPP-vX.Y.Z-windows-x64.zip
FREDPP-vX.Y.Z-debian13-amd64.tar.gz
FREDPP-vX.Y.Z-debian13-arm64.tar.gz
FREDPP-vX.Y.Z-macos-x64.tar.gz
FREDPP-vX.Y.Z-macos-arm64.tar.gz
fredpp_X.Y.Z_amd64.deb
fredpp_X.Y.Z_arm64.deb
FREDPP-Language-vX.Y.Z.vsix
SHA256SUMS.txt
```

Les archives ZIP et TAR.GZ sont portables. Les paquets `.deb` installent la commande `fredpp` dans `/usr/bin`. Les huit fichiers de distribution sont couverts par `SHA256SUMS.txt`. GitHub ajoute automatiquement les deux archives source : **11 assets affichés au total**.

L'APK Android n'est pas compris dans cette matrice de neuf fichiers : les vérifications Android restent dans leur workflow CI distinct.

## Publication

Le développement courant se fait sur `dev`. Des tags intermédiaires facultatifs
(par exemple `dev-0.0.103`) peuvent marquer des points de reprise sans déclencher
de release publique.

Après les tests de développement, créer une **préversion depuis `dev`**, sans
modifier `main` :

```bash
git switch dev
git pull --ff-only origin dev
git tag -a vX.Y.Z -m "FREDPP vX.Y.Z pre-release"
git push origin vX.Y.Z
```

Cette procédure suppose que la version `X.Y.Z` et le workflow de livraison
ont déjà été commités et poussés sur `dev`, que le tag `vX.Y.Z` n'existe pas,
et que les tests locaux ont réussi. Le push du tag déclenche GitHub Actions.

Pour une **release stable ultérieure**, après validation complète et décision
explicite, avancer `main` jusqu'au commit approuvé de `dev` par une mise à jour
*fast-forward*, puis créer le tag stable sur `main` :

```bash
git switch main
git merge --ff-only dev
git push origin main
git tag -a vX.Y.Z -m "FREDPP vX.Y.Z"
git push origin vX.Y.Z
```

Le workflow `.github/workflows/release.yml` :

1. vérifie le tag, la version CMake et la branche de publication (`dev` ou `main`) ;
2. construit et teste les binaires Windows, Debian 13 amd64/arm64 et macOS x64/arm64 sur runners standard ;
3. construit et contrôle l'extension VS Code ;
4. exige les **neuf fichiers** de la matrice et vérifie les empreintes SHA-256 ;
5. crée une préversion pour `dev` ou une release stable pour `main`, avec les neuf fichiers joints et deux archives source générées par GitHub.

## Statistiques de téléchargement

Sous Windows :

```powershell
.\scripts\release-stats.ps1
```

Sous Debian :

```bash
./scripts/release-stats.sh
```

Les compteurs sont fournis par l'API des Releases GitHub, fichier par fichier. Ils représentent des téléchargements et non des utilisateurs uniques.

## Vérification d'un téléchargement

Sous Debian :

```bash
sha256sum -c SHA256SUMS.txt
```

Sous PowerShell :

```powershell
Get-FileHash .\FREDPP-vX.Y.Z-windows-x64.zip -Algorithm SHA256
Get-Content .\SHA256SUMS.txt
```
