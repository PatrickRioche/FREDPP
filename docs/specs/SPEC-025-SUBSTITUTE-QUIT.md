# SPEC-025 — Commandes S et Q

## Objet

Cette spécification définit le périmètre du Sprint 2.12 : substitution de texte avec `S` et remplacement de la sortie spéciale `:quit` par la commande historique `Q`.

## Commande S

### Syntaxe prise en charge

```fred
(.,.)S<sep><modèle><sep><chaîne><sep>[P]
```

`<sep>` est un caractère symbolique, par exemple `/`, `?`, `!` ou `;`.

### Règles

- sans adresse, S agit sur la ligne courante ;
- une adresse simple ou une plage peut être fournie ;
- toutes les occurrences non chevauchantes sont remplacées ;
- `&` dans la chaîne représente le texte trouvé ;
- une barre oblique inverse protège le caractère suivant dans la chaîne ;
- `P` après le séparateur final affiche la dernière ligne modifiée ;
- `.` pointe sur la dernière ligne modifiée ;
- le registre de condition est vrai si au moins une substitution a eu lieu ;
- en l'absence de correspondance, l'exécution échoue avec `no text changed` ;
- S est admise comme commande imbriquée dans G.

### Limites

Les options `OS&` et `OS/`, le modèle courant `//` et les suites générales de commandes seront traités ultérieurement.

## Commande Q

### Syntaxe prise en charge

```fred
Q
QQ
```

### Règles

- `Q` demande un arrêt normal ;
- `QQ` demande un arrêt immédiat ;
- la casse est indifférente ;
- Q n'accepte pas d'adresse ;
- la commande spéciale `:quit` est supprimée.

### Limites

`Q!<commande TSS>` est explicitement refusée tant que le mécanisme d'exécution externe portable n'est pas disponible. La protection fondée sur les buffers associés à des fichiers sera complétée lorsque ces associations seront implémentées.

## Documentation

Les pages `s.md` et `q.md` de `docs/fr/reference/commandes/` sont embarquées sans modification.
