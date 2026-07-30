# FREDPP — Sprint 2.3 — Commande I (Insert)

## Objectif

Valider et isoler l'implémentation de la commande historique `I`.

## Comportement couvert

- `I` : insère avant la ligne courante ;
- `nI` : insère avant la ligne adressée ;
- `0I` : insère au début du buffer ;
- `I` sur buffer vide : crée les premières lignes ;
- la saisie se termine avec `\\F` ;
- la ligne courante devient la dernière ligne insérée ;
- une saisie vide ne modifie pas le contenu.

## Fichiers du sprint

- `tests/test_insert.cpp` : tests unitaires dédiés à `I` ;
- `tests/test_append.cpp` : retrait des tests de `I`, désormais isolés ;
- `tests/CMakeLists.txt` : enregistrement de `test_insert`.

## Validation attendue sous Windows

```powershell
cd scripts
.\rebuild.bat
```

Résultat attendu :

```text
100% tests passed out of 14
```

## Recette fonctionnelle

Dans `fredpp.exe` :

```text
A
A
B
C
D
\F
3I
X
Y
\F
1,$P
1I
FIRST
\F
$I
BEFORE-LAST
\F
1,$P
```

