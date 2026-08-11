# FREDPP v0.0.3 — Integrated Markdown Help

## Intégration

Copier le contenu du ZIP à la racine du référentiel FREDPP en remplaçant les fichiers existants.

Puis exécuter sous PowerShell :

```powershell
cd .\scripts\
.\rebuild.bat
```

Résultat attendu :

```text
100% tests passed out of 19
```

## Homologation interactive

```text
?
?b
?help
?h
?version
?rubrique-inconnue
:quit
```

La documentation Markdown de `docs/fr/*.md` est générée comme ressource C++ au moment de la configuration CMake puis intégrée à `fredpp.exe`. Elle reste donc modifiable dans Git tout en étant disponible hors ligne dans l'exécutable.

## Commit proposé

```powershell
git add .
git commit -m "v0.0.3 - Add integrated Markdown help"
git push
```
