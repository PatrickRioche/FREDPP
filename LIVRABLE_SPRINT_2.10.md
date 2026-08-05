# Livrable Sprint 2.10 — Traçabilité et aide FREDPP

## Version cible

La version est définie exclusivement dans le fichier `CMakeLists.txt` par `project(FREDPP VERSION ...)`.

Pour consulter l'identité du binaire :

```text
?version
```

## Fonctionnalités intégrées

- récupération automatique du commit Git lors de la configuration CMake ;
- détection de l'état propre ou modifié des sources ;
- valeurs de repli `inconnu` lorsque Git ou le dépôt n'est pas disponible ;
- sortie enrichie de `?version` et `--version` ;
- feuille de route entièrement réécrite en français ;
- ajout de l'aide des commandes spéciales avec `?:` ;
- `:help` affiche la même rubrique ;
- présentation terminal alignée et aérée des aides `?` et `?:` ;
- tests de version et d'aide étendus.

## Documentation

Aucun fichier situé sous `docs/fr/reference/commandes/` n'est modifié.

La documentation des commandes spéciales est séparée dans :

```text
docs/fr/fredpp/commandes-speciales.md
```

## Comportement attendu

```text
?version
FREDPP v<version>
Commit Git : <hash court ou inconnu>
État des sources : propre, modifié ou inconnu
Feuille de route : ROADMAP.md — section v<version>
```

```text
?:
============================================================
COMMANDES SPÉCIALES DE FREDPP
============================================================
...
```

## Validation

Le sprint conserve la suite de 22 tests et étend les contrôles existants pour vérifier :

- la version centralisée ;
- le format du commit Git ;
- la cohérence de l'état des sources ;
- la présence de l'aide `?:` ;
- la liste complète des commandes spéciales actuelles.

- rendu terminal structuré des titres, paramètres, exemples et tableaux de compatibilité ;
