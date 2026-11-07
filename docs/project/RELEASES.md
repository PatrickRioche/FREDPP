# Livraisons officielles de FREDPP

## Objectif

Une version officielle de FREDPP est publiée dans l'espace **Releases** du dépôt GitHub. Elle est construite automatiquement à partir d'un tag Git correspondant exactement au numéro déclaré dans `CMakeLists.txt`.

## Fichiers publiés

Pour chaque version :

```text
FREDPP-vX.Y.Z-windows-x64.zip
FREDPP-vX.Y.Z-debian13-amd64.tar.gz
fredpp_X.Y.Z_amd64.deb
SHA256SUMS.txt
```

Le ZIP Windows et l'archive Debian sont portables. Le paquet `.deb` installe la commande `fredpp` dans `/usr/bin`.

## Publication

Le développement courant se fait exclusivement sur `dev`. Des tags
intermédiaires facultatifs (par exemple `dev/2026-11-07-flow`) peuvent marquer
des points de reprise sans déclencher de publication officielle.

Après la validation de la campagne de compatibilité, des tests de régression
et du jalon sur Windows 11, avancer `main` jusqu'au commit approuvé de `dev`
par une mise à jour *fast-forward*, puis créer le tag stable sur `main` :

```bash
git switch main
git merge --ff-only dev
git push origin main
git tag -a vX.Y.Z -m "FREDPP vX.Y.Z"
git push origin vX.Y.Z
```

Le workflow `.github/workflows/release.yml` :

1. vérifie la correspondance entre le tag et la version CMake, ainsi que
   l'égalité du commit tagué avec le commit courant de `main` ;
2. construit et teste le binaire Release Windows ;
3. construit et teste les paquets sous Debian 13 ;
4. calcule les empreintes SHA-256 ;
5. crée la Release GitHub et joint les quatre fichiers.

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
