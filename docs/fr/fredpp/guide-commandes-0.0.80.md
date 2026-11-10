# FREDPP 0.0.80 — Guide de recette du lot 71–80

Ce lot ajoute **10 commandes historiques** aux 70 commandes de `0.0.70` :
`#`, `:`, `@`, `\\`, `Y`, `OIA`, `OIC`, `OIR`, `OIL` et `OIU`.

Les fichiers historiques de `docs/fr/reference/commandes/` ne sont pas modifiés.
Le tableau ci-dessous décrit **le comportement réellement développé** dans
FREDPP, notamment lorsque le terminal GCOS de 1970 n'existe plus.

| Commande | Syntaxe prise en charge | Comportement de FREDPP |
|---|---|---|
| `#` | `#` | Affiche le compteur partagé, alimenté par différentes opérations, sans le modifier. |
| `:` | `:`, `:0`, `:n` | Imprime au maximum `n` lignes avant et après la ligne courante ; valeur implicite 5. Le curseur avance à la dernière ligne imprimée. |
| `@` | `@(NOM)` | Étiquette de procédure sur 1 à 15 caractères ; reconnue par les sauts existants. Saisie seule, elle ne produit pas de sortie. |
| `\\` | `\\0`, `\\n` | Suspend l'exécution pendant `n` secondes entières, de 0 à 86 400. |
| `Y` | `Y` | Rappelle le dernier message d'échec de parsing, d'exécution ou `JE`, sans l'effacer. |
| `OIA` | `O+IA/texte/`, `O-IA` | Personnalise le prompt des modes A/I/C ; le rétablit à `text> `. |
| `OIC` | `O+IC/texte/`, `O-IC` | Personnalise le prompt de commande ; le rétablit au nom du buffer suivi de `> `. |
| `OIR` | `O+IR/texte/`, `O-IR` | Affiche le message après chaque entrée validée au clavier, ou le désactive. |
| `OIL` | `O+IL%`, `O-IL` | Le préfixe `%` force en minuscules ASCII le caractère suivant dans une saisie A/I/C. |
| `OIU` | `O+IU^`, `O-IU` | Le préfixe `^` force en majuscules ASCII le caractère suivant dans une saisie A/I/C. |

Le délimiteur `/` des chaînes OIA/OIC/OIR est un exemple : un autre symbole
identique aux deux extrémités est accepté. L'antislash permet d'échapper un
délimiteur à l'intérieur du texte. La longueur utile est limitée à 120 caractères.
Les caractères d'échappement `OIL` et `OIU` doivent être **différents**.

## Recette interactive Windows

Les commandes suivantes sont à saisir dans `fredpp.exe`. Le texte entre
`A` et `\F` est un contenu de buffer, et non une commande.

```fred
FV
FO
B(test80)
A
Alpha
Bravo
Charlie
Delta
Echo
\F
3P
:1
#
O+IA/SAISIE> /
O+IC/COMMANDE> /
O+IR/RETURN/
O+IL%
O+IU^
FO
A
M%I%N%U%S%C%U%L%E
m^A^J^U^S^C^U^L^E
\F
1,$P
O-IA
O-IC
O-IR
O-IL
O-IU
\\0
Y
FO
```

Résultats importants :

- `:1` imprime la fenêtre voisine de la ligne courante ; l'affichage exact
  dépend de la position avant la commande.
- Les deux lignes nouvellement ajoutées valent `Minuscule` et `mAJUSCULE`.
- `FO` reflète les options actives ; les cinq `O-I*` les désactivent.
- `Y` indique « aucune erreur enregistrée » s'il n'y a pas eu de diagnostic
  depuis le lancement de la session. Après une commande invalide, il affiche
  le dernier diagnostic conservé.
- `\\0` ne provoque aucune attente perceptible ; la durée des autres
  variantes est exprimée en secondes.

## Précautions et limites

- `@` est surtout pertinent **dans une procédure** ; le traitement des
  étiquettes et des sauts par le moteur de procédures existait avant ce lot.
- `Y` mémorise un diagnostic exploitable, mais ne reconstitue pas le
  contexte complet d'un message GCOS comme le faisait parfois FRED original.
- La casse forcée par `OIL` / `OIU` ne concerne que l'ASCII. Dans une saisie
  texte, un préfixe final sans caractère suivant reste littéral.
- Les prompts `OIA` et `OIC` agissent sur le REPL de bureau ; le moteur
  utilisé sans terminal ne crée pas lui-même de demande d'entrée.
- `OIR` affiche une ligne de message ; la fidélité aux terminaux historiques
  (écho clavier, fin de ligne, codes de contrôle) reste à homologuer.
- La limite `\\86400` correspond à **24 heures d'attente réelle** : ne pas
  l'utiliser dans les tests automatiques.
- L'aide `?:` reste celle des commandes additionnelles FREDPP ; pour la
  commande historique `:` utiliser `?window`, et pour `\\`, `?wait`.

## Validation et jalon Git

Construire et exécuter toute la suite CTest sous Windows/MSVC, puis tester
le scénario interactif. Ne publier **`dev-0.0.80` qu'après validation explicite**
des résultats. Aucun changement de `main` ou release stable n'est demandé.
