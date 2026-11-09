# FREDPP 0.0.60 — Guide de recette du lot 51–60

Les commandes 1–50 restent documentées dans le [guide 0.0.50](guide-commandes-0.0.50.md).
Ce lot met en œuvre dix commandes historiques supplémentaires de la famille `Z`.

| No | Commande | Syntaxe d'exemple | Effet |
|---:|---|---|---|
| 51 | `ZA` | `1ZA(source)` | Copie tout le buffer nommé après la ligne donnée (ou courante) ; `0ZA` insère au début. |
| 52 | `ZB` | `1,$ZB` | Tri décroissant ; la condition vaut vrai si les lignes triées sont identiques. |
| 53 | `ZC` | `ZC` | Efface le drapeau de modification ; condition = valeur précédente. |
| 54 | `ZD` | `ZD(temp)` | Supprime le buffer nommé, même s'il comporte des modifications non enregistrées. |
| 55 | `ZL` | `1,$ZL` ou `1,$ZL/ABC/` | Conversion en minuscules ASCII (toutes les lignes ou portions correspondant au modèle). |
| 56 | `ZM` | `1,2ZM(dest)` | Déplace les lignes vers le buffer nommé après son curseur, sans modifier les autres lignes. |
| 57 | `ZP` | `ZP(source)` | Affiche le buffer nommé sans le sélectionner. |
| 58 | `ZS` | `1,$ZS` | Tri croissant ; condition vraie si toutes les lignes triées sont identiques. |
| 59 | `ZT` | `1,$ZT/ABC/xyz/` | Traduit chaque caractère de gauche par le caractère correspondant de droite. |
| 60 | `ZU` | `1,$ZU` ou `1,$ZU/abc/` | Conversion en majuscules ASCII. |

## Compteurs, curseurs et limites

- `ZL` et `ZU` sans modèle placent dans `#` le nombre de lignes modifiées ;
  avec modèle, le nombre de correspondances effectivement converties.
- `ZT` compte dans `#` les caractères réellement remplacés. Les jeux de
  caractères doivent avoir la même longueur et le jeu source doit être sans
  doublon. L'implémentation refuse la traduction d'octets non ASCII.
- `ZL`, `ZU` et `ZT` déplacent `.` vers la dernière ligne effectivement
  modifiée. `ZL`/`ZU` signalent `? no text changed` si rien ne change.
- Les conversions ASCII ne modifient pas les octets non ASCII d'un texte UTF-8.
- Les opérations `ZA` et `ZM` acceptent un buffer de destination égal au buffer
  courant. `ZM` refuse d'inclure sa ligne courante dans une telle opération.
- `ZD` et `ZC` ne sauvegardent pas : leur effet peut masquer ou supprimer
  des modifications non enregistrées. Utiliser un buffer temporaire aux tests.

## Contrôle à effectuer avant le tag

Sur Windows/MSVC, recompiler Debug et Release, exécuter CTest complet, puis
vérifier les commandes dans une session interactive sur des buffers temporaires.
Ne pas créer `dev-0.0.60` avant validation expresse par l'utilisateur.
