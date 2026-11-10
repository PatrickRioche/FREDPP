# FREDPP 0.0.90 — Guide de recette du lot 81–90

Le lot comprend dix commandes historiques supplémentaires : `JO`, `OC`,
`OI\F`, `OIE`, `OIF`, `OOL`, `OOO`, `OOU`, `OP` et `OS/`.

Cette implémentation est une adaptation *portable* des capacités d'un terminal
GCOS. Les documents de référence historiques ne sont pas modifiés.

| Commande | Exemple | Effet observable sous FREDPP |
|---|---|---|
| `JO` | dans une procédure : `JO`, `JOT`, `JOF` | Abandon de la procédure en cours ; variantes conditionnelles selon le registre de condition. Ne quitte pas FREDPP. |
| `OC` | `O+C/Bonjour/Salut/` puis `O-C` | Remplacement littéral non chevauchant à la saisie A/I/C ; limitation à 120 caractères par élément. |
| `OI\F` | `O-I\F` / `O+I\F` | Autorise ou désactive le besoin de saisir un bloc A/I/C terminé par `\F`. En mode désactivé, `A texte`, `I texte` et `C texte` traitent une seule ligne. |
| `OIE` | `O+IE!`, `O-IE` | Remplace `\F` par `!F` comme terminateur de bloc texte ; retour au caractère `\` avec `O-IE`. |
| `OIF` | `O-IF`, `O+IF` | Contrôle le saut de ligne ajouté aux messages de retour clavier `OIR`. Le retour propre au terminal Windows reste sous le contrôle de Windows. |
| `OOL` | `O+OL`, `O-OL` | Dans P/L, encode les minuscules avec un préfixe `%` (ou celui défini par OIL) suivi de la majuscule. |
| `OOO` | `O+OO`, `O-OO` | Dans P/L, écrit les octets de contrôle sous la forme `\014`, `\000`, etc. Option active par défaut, comme dans la documentation historique. |
| `OOU` | `O+OU`, `O-OU` | Dans P/L, encode les majuscules avec un préfixe `^` (ou celui défini par OIU) suivi de la minuscule. |
| `OP` | `O+P2`, `O-P` | Insère une ligne de séparation entre les groupes de deux lignes imprimées par P ou L ; pas de pause bloquante dans une procédure. |
| `OS/` | `O-S/`, `O+S/` | Permet ou interdit l'omission du séparateur de fermeture dans une substitution S. |

Les options sorties `OOL` et `OOU` sont mutuellement exclusives : elles servent
à simuler des terminaux **ne sachant pas imprimer les deux casses**. L'affichage
normal d'un terminal moderne reste inchangé tant que ces options sont désactivées.

## Recette interactive

Dans `fredpp.exe`, saisir :

```fred
FV
B(lot90)
O-I\F
A Première ligne
1P
O+IE!
A
Seconde ligne
!F
1,$P
O+C/ligne/LIGNE/
A
Troisième ligne
!F
O-C
1,$P
O+P2
1,$P
O-P
O+OL
1P
O-OL
O+OU
1P
O-OU
O+OO
FO
O-OO
O+IF
O+I\F
O-IE
QQ
```

Points attendus :

- `A Première ligne` insère une ligne sans demander `\F` grâce à `O-I\F`.
- Le mode bloc suivant annonce `!F` ; il est terminé par `!F`.
- `O+C` remplace « ligne » par « LIGNE » sur le texte **nouvellement entré**,
  sans réécrire rétroactivement les lignes déjà présentes.
- `O+P2` produit un séparateur `-- page suivante --` entre les lignes 2 et 3.
- `O+OL` / `O+OU` rendent visibles les encodages de secours des deux casses.
- `FO` liste les options non implicites actives, tandis que `QQ` termine la session.

## Vérifier JO en procédure

Créer un buffer `B(proc90)` et y saisir les lignes suivantes avec `A` :

```fred
JM/Début/
JO
JM/Ne doit pas apparaître/
```

Terminer la saisie par `!F` si `O+IE!` est encore actif, ou `\F` après
`O-IE`. Exécuter ensuite `\\B(proc90)`. Seul `Début` doit apparaître.

## Limites connues

- `JO` est pris en charge dans les buffers de procédures. L'interruption
  d'une itération `G` ou `U` n'est pas encore implémentée.
- `O-I\F` offre les formes *sans adresse* `A texte`, `I texte`, `C texte`.
  Pour des adresses explicites, conserver la saisie par blocs.
- `OIE` personnalise le marqueur d'entrée A/I/C ; d'autres échappements de
  flot continuent d'utiliser l'antislash historique.
- `OIF` ne peut pas désactiver l'écho physique du clavier Windows.
- La pagination `OP` est non bloquante pour la sécurité des scripts.
- `OOL`, `OOO` et `OOU` concernent actuellement les sorties de lignes des
  commandes P/L, pas la totalité des sorties de diagnostic.
- `O-S/` accepte l'omission du **dernier** délimiteur de la substitution ;
  l'interprétation des motifs et des remplacements reste inchangée.

## Publication

Le jalon `dev-0.0.90` sera créé **après** compilation, tests CTest et recette
interactive sur Windows. Aucune modification de `main` n'est prévue.
