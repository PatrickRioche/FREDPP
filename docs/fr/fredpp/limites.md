# LIMITES D'EXÉCUTION ET DE SÉCURITÉ FREDPP

Cette rubrique décrit les **protections de FREDPP**, et non des commandes ou
restrictions garanties par l'éditeur FRED historique.

## Afficher les valeurs réellement actives

Dans PowerShell ou dans un terminal :

```powershell
fredpp --limits
```

L'affichage précise `(default)` lorsque la limite est celle de FREDPP et
`(environment)` lorsqu'une variable d'environnement l'a remplacée.
La commande ne lance aucune procédure ni fichier d'initialisation.

## Variables et valeurs par défaut

| Variable d'environnement | Défaut | Portée |
| --- | ---: | --- |
| `FREDPP_PROCEDURE_MAX_STEPS` | 1 000 000 | Lignes exécutées dans une procédure et ses appels `\\B` |
| `FREDPP_PROCEDURE_MAX_SECONDS` | 60 | Durée écoulée de cette exécution |
| `FREDPP_REPEAT_MAX_STEPS` | 1 000 000 | Itérations cumulées de commandes `U` imbriquées |
| `FREDPP_REPEAT_MAX_SECONDS` | 60 | Durée écoulée des répétitions `U` imbriquées |

Autres plafonds actuellement appliqués : **256 niveaux de procédures**
(la profondeur de la procédure active est comptée), **16 niveaux de `U`**,
**262 143 répétitions** pour un `U` avec compteur explicite, et
**10 000 répétitions** pour un `U` sans compteur explicite.

Les quatre variables acceptent uniquement un entier strictement positif.
Les deux durées sont plafonnées à **86 400 secondes**. Une valeur incorrecte
est signalée par une erreur : elle n'est pas ignorée silencieusement.

## Personnaliser sous PowerShell

```powershell
$env:FREDPP_PROCEDURE_MAX_STEPS = '5000000'
$env:FREDPP_PROCEDURE_MAX_SECONDS = '300'
$env:FREDPP_REPEAT_MAX_STEPS = '5000000'
$env:FREDPP_REPEAT_MAX_SECONDS = '300'
fredpp --limits
```

Pour rétablir une valeur par défaut :

```powershell
Remove-Item Env:FREDPP_REPEAT_MAX_STEPS
```

Ces variables ne modifient que la session PowerShell courante et les
processus qu'elle lance. Les budgets de procédure se réinitialisent à
chaque exécution principale. Les budgets de répétition se réinitialisent
entre deux `U` indépendants, y compris en mode interactif.

## Comportement en cas de dépassement

Une procédure est arrêtée avec un diagnostic explicite et, lorsque cela
est possible, un retour au mode interactif pour inspection.
Les sorties conditionnelles historiques restent autorisées.
Les dépassements de sécurité des répétitions `U` ne sont pas assimilés
à une erreur ordinaire récupérable par `UE`.

**Limitation importante :** les délais sont contrôlés entre les lignes
de procédure ou les itérations `U`. Une commande bloquée pendant son
propre traitement (notamment un programme externe) n'est pas interrompue
par ces quatre variables. Elles ne plafonnent pas non plus l'espace
mémoire des buffers ni le volume de sortie.

Pour les autres détails de l'exécution, consulter `?procedure`.
