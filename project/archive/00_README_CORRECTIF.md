# Correctif Sprint 2.10 — Rendu terminal des aides de commandes

Ce correctif améliore l'affichage de `?a`, `?b`, `?c`, etc. sans modifier les fichiers Markdown sous `docs/fr/reference/commandes/`.

## Modifications

- conversion des titres Markdown en sections terminal visibles ;
- conversion des tableaux de paramètres et de compatibilité en colonnes alignées ;
- encadrement des blocs d'exemples FRED ;
- suppression des marqueurs Markdown affichés dans le terminal ;
- conservation de l'accès au Markdown brut dans `HelpManager::load()` ;
- ajout de `HelpManager::load_for_terminal()` pour le rendu console ;
- tests étendus.

## Application

Depuis la racine du dépôt :

```powershell
Expand-Archive `
  -Path .\FREDPP_Sprint_2.10_Correctif_Rendu_Commandes.zip `
  -DestinationPath . `
  -Force

.\scripts\rebuild.bat
```

Fermer préalablement toute instance de `fredpp.exe`.

## Vérification

Dans FREDPP :

```text
?a
?g
?z
?:
```

Aucun fichier de `docs/fr/reference/commandes/` n'est inclus dans ce correctif.
