# FREDPP 0.0.27 — Guide pratique des 27 commandes disponibles

> **Portée :** comportement effectivement implémenté dans FREDPP **v0.0.27**.
> Ce guide complète la [référence historique](../reference/refindex.md), sans
> la remplacer ni la modifier. Certaines formes décrites dans les manuels
> d'origine ne fonctionnent pas encore dans FREDPP.

## Lire les exemples

- Les **mnémoniques des commandes** ne dépendent pas de la casse : `p` et
  `P` sont équivalents. Les noms de fichiers restent soumis aux règles du
  système d'exploitation.
- `.` désigne la ligne courante, `$` la dernière, `n` une ligne numérotée et
  `n,m` une plage inclusive. Une adresse absente désigne en général la ligne
  courante, sauf indication contraire dans la fiche.
- `1,$` représente tout le buffer. L'alias `*` vaut `1,$` ; `*` seul
  affiche les lignes du buffer. Le buffer vide n'accepte généralement pas
  d'adresse existante.
- `<fichier>`, `<buffer>`, `<texte>` et `<motif>` sont des paramètres
  explicatifs : ne pas recopier les chevrons.
- Les exemples qui contiennent `exemple.txt` supposent que ce fichier existe,
  sauf lorsqu'ils illustrent une écriture. Les numéros de lignes supposent
  que le buffer contient suffisamment de lignes.
- Les commandes `A`, `I` et `C` demandent ensuite des **lignes de texte** ;
  une ligne autonome `\F` termine cette saisie, dans le terminal comme
  dans les procédures. Cette ligne de fin ne fait pas partie du texte inséré.
- `?r`, `?w`, etc. affichent les **fiches historiques embarquées**.
  Le présent guide précise leur périmètre réellement disponible en 0.0.27.

## Inventaire rapide

| Famille | Commandes prises en charge | Nombre |
|---|---|---:|
| Principales | [A](#a--ajouter), [B](#b--choisir-un-buffer), [C](#c--remplacer), [D](#d--supprimer), [G](#g--sélection-globale), [I](#i--insérer), [K](#k--copier-avec-remplacement), [L](#l--lister-un-fichier), [M](#m--déplacer), [P](#p--afficher), [Q](#q--quitter), [R](#r--lire-un-fichier), [S](#s--substituer), [T](#t--tester-un-motif), [W](#w--écrire-un-fichier), [Z](#z--positionner-la-ligne-courante) | 16 |
| Spéciales | [`"`](#--commentaire), [`=`](#--afficher-le-numéro-de-ligne) | 2 |
| Informations F | [FB](#fb--buffers), [FF](#ff--fichiers-associés), [F?](#f--buffers-modifiés), [FO](#fo--options-actives) | 4 |
| Messages J | [JM](#jm--message-avec-fin-de-ligne), [JP](#jp--message-sans-fin-de-ligne) | 2 |
| Options O | [`OI(`](#oi--parenthèses-des-noms-de-buffer), [OM](#om--trace-des-procédures) | 2 |
| Famille Z | [ZK](#zk--copier-en-insérant) | 1 |
| **Total** | | **27** |

## Édition des lignes

### A — Ajouter

- **Syntaxe :** `A`, `nA`, `0A` ; puis les lignes de texte et `\F`.
- **Effet :** ajoute après la dernière ligne par défaut, après `n` si une
  adresse est fournie, ou au début avec `0A`. La dernière ligne ajoutée
  devient courante.
- **Exemple :**

  ```fred
  A
  Première ligne
  Deuxième ligne
  \F
  ```

- **Limite :** une plage `n,mA` n'est pas admise ; la commande attend un bloc
  de texte, et non une chaîne sur la même ligne.

### B — Choisir un buffer

- **Syntaxe :** `B(nom)`. Avec l'option `O-I(`, formes courtes `Bx`,
  `B0` et `B.` (un seul caractère) également autorisées.
- **Effet :** sélectionne un buffer existant ou le crée sans modifier son texte.
- **Exemple :** `B(travail)`.
- **Limite :** l'état initial est `O+I(` : les formes courtes sont alors
  rejetées à l'exécution. `B` ne prend pas d'adresse de ligne.

### C — Remplacer

- **Syntaxe :** `C`, `nC` ou `n,mC` ; puis texte et `\F`.
- **Effet :** remplace la ligne courante ou la plage indiquée par les lignes
  saisies. Un bloc de remplacement vide supprime les lignes concernées.
- **Exemple :**

  ```fred
  2,3C
  Nouvelle ligne
  \F
  ```

- **Limite :** les adresses doivent correspondre à des lignes existantes.

### D — Supprimer

- **Syntaxe :** `D`, `nD` ou `n,mD`.
- **Effet :** supprime la ligne courante ou une plage ; le buffer devient
  modifié. `*D` supprime toutes les lignes.
- **Exemple :** `2,4D`.
- **Limite :** aucune plage hors limites n'est admise ; l'opération est
  destructive tant que son contenu n'a pas été sauvegardé.

### G — Sélection globale

- **Syntaxe :** `G/motif/commande`, `G~/motif/commande` ou
  `n,mG/motif/commande`.
- **Effet :** exécute la commande imbriquée sur les lignes correspondant au
  motif ; `G~` inverse la sélection. Sans adresse, parcourt tout le buffer.
  Le compteur mémorise le nombre de lignes sélectionnées.
- **Exemples :** `G/erreur/P` (afficher), `1,$G~/utile$/D` (supprimer les
  lignes ne correspondant pas au motif).
- **Limite :** seules les commandes imbriquées **P, D, Z et S** sont exécutables ;
  elles ne doivent pas posséder leur propre adresse. Les enchaînements
  généraux et le rappel du dernier motif via `//` restent à compléter.

### I — Insérer

- **Syntaxe :** `I`, `nI`, `0I` ; puis texte et `\F`.
- **Effet :** insère avant la ligne courante par défaut ou avant la ligne
  indiquée ; `0I` insère au début.
- **Exemple :**

  ```fred
  1I
  En-tête
  \F
  ```

- **Limite :** aucune plage d'adresses ; sur buffer vide, `I` ajoute au début.

### K — Copier avec remplacement

- **Syntaxe :** `K(destination)` ou `n,mK(destination)`.
- **Effet :** copie la ligne courante ou la plage source vers le buffer
  destinataire **en remplaçant intégralement son ancien contenu**. Le texte
  du buffer source est préservé ; le buffer source reste sélectionné.
- **Exemple :** `1,3K(archive)`.
- **Limite :** contrairement à `ZK`, **K n'ajoute pas** aux lignes déjà
  présentes. La copie vers le même buffer remplace son contenu par la sélection.

### M — Déplacer

- **Syntaxe :** `M(destination)` ou `n,mM(destination)`.
- **Effet :** remplace tout le contenu de la destination par les lignes choisies
  puis les supprime de la source ; le buffer source reste sélectionné.
- **Exemple :** `2,3M(extraits)`.
- **Limite :** comportement de remplacement encore provisoire au regard du
  manuel historique ; une copie non destructive utilise `K` ou `ZK`.

### P — Afficher

- **Syntaxe :** `P`, `nP`, `n,mP` ou `*`.
- **Effet :** affiche les lignes demandées sans modifier leur texte ; la
  dernière ligne affichée devient courante.
- **Exemple :** `1,$P`.
- **Limite :** requiert au moins une ligne existante ; `P` ne numérote pas
  les lignes affichées (utiliser `=` pour connaître un numéro).

### S — Substituer

- **Syntaxe :** `S/motif/remplacement/`, `n,mS/motif/remplacement/` et
  suffixe facultatif `P` : `S/motif/remplacement/P`.
- **Effet :** remplace toutes les occurrences non chevauchantes sur les lignes
  ciblées. `&` dans le remplacement reprend le texte trouvé ; le suffixe
  `P` affiche la dernière ligne modifiée.
- **Exemple :** `1,$S/ancien/nouveau/P`.
- **Limite :** si rien n'a été remplacé, une erreur `no text changed` est
  signalée. Les options historiques `OS&` et `OS/` ne sont pas configurables.

### T — Tester un motif

- **Syntaxe :** `T/motif/`, `T~/motif/`, `n,mT/motif/`.
- **Effet :** met le registre de condition à vrai si au moins une ligne de la
  plage correspond au motif. `T~` inverse ce résultat ; aucun texte,
  compteur ou curseur n'est modifié.
- **Exemples :** `T/erreur/`, `1,$T~/erreur/`.
- **Limite :** ce n'est **pas** une commande de transfert de lignes ; les
  autres fonctionnalités historiques restent à homologuer.

### Z — Positionner la ligne courante

- **Syntaxe :** `Z` ou `nZ`.
- **Effet :** déplace uniquement le curseur de ligne vers l'adresse indiquée ;
  sans adresse, conserve la ligne courante.
- **Exemple :** `3Z`.
- **Limite :** n'efface ni ne modifie le texte ; une plage `n,mZ` est refusée.

## Fichiers, sortie et informations

### L — Lister un fichier

- **Syntaxe :** `L fichier.txt` ou `L "fichier avec espaces.txt"`.
- **Effet :** affiche le contenu d'un fichier existant, sans le charger dans
  le buffer courant.
- **Exemple :** `L exemple.txt`.
- **Limite :** `L` sans fichier est reconnu par le parseur mais **pas pris en
  charge à l'exécution** ; aucune adresse de ligne n'est acceptée.

### R — Lire un fichier

- **Syntaxe :** `R fichier.txt`, `R "fichier avec espaces.txt"`,
  `R absent.txt secours.txt`, `nR fichier.txt` ou `$R fichier.txt`.
- **Effet :** sans adresse, charge un fichier dans un buffer **vide**, associe
  le fichier et mémorise encodage et fins de lignes. Avec une adresse, insère
  les lignes **après** celle-ci sans remplacer l'association existante. Le
  compteur reçoit le nombre de lignes lues.
- **Exemple :** `R "rapport source.txt" "rapport secours.txt"` essaie les
  chemins entre guillemets dans l'ordre.
- **Limite :** un buffer non vide refuse `R` sans adresse. Une insertion
  n'accepte qu'une adresse unique ; `$R` fonctionne aussi sur un buffer vide.
  ASCII et UTF-8 sont traités ; les fichiers binaires ne le sont pas.
  Sans nom explicite, l'association du buffer doit déjà exister.

### W — Écrire un fichier

- **Syntaxe :** `W fichier.txt`, `n,mW fichier.txt`,
  `WA fichier.txt`, `WU fichier.txt` ou `W` si un fichier est associé.
- **Effet :** écrit le buffer entier ou une plage. `W` conserve autant que
  possible l'encodage et les fins de lignes ; `WA` force ASCII, `WU`
  force UTF-8 sans BOM. Une écriture complète associe le fichier et marque
  le buffer comme non modifié ; une écriture partielle ne le fait pas.
- **Exemple :** `WU resultat.txt`.
- **Limite :** le fichier cible est remplacé à l'écriture. `WA` refuse les
  caractères non ASCII. `WB` est reconnu mais le format historique GCOS/BCD
  est indisponible ; `WX` n'est pas pris en charge.

### FB — Buffers

- **Syntaxe :** `FB`.
- **Effet :** affiche les buffers dans l'ordre de sélection la plus récente,
  avec le numéro courant, le nombre de lignes, le fichier associé et `?`
  lorsque le buffer est modifié.
- **Exemple :** `FB`.
- **Limite :** un buffer créé sans avoir jamais été sélectionné peut ne pas
  apparaître. Aucun buffer n'est modifié par cette consultation.

### FF — Fichiers associés

- **Syntaxe :** `FF`.
- **Effet :** énumère les **seuls buffers associés à un fichier**, triés par
  nom de buffer ; `?` signale un buffer modifié.
- **Exemple :** `FF`.
- **Limite :** ne liste ni les fichiers du disque ni les buffers sans
  association. Cette commande ne modifie pas le contexte.

### F? — Buffers modifiés

- **Syntaxe :** `F?` ou `f?` (sans espace entre `F` et `?`).
- **Effet :** affiche toujours le **buffer courant en premier**, qu'il soit
  modifié ou non ; affiche ensuite les **autres buffers modifiés** dans l'ordre
  alphabétique des noms. Chaque ligne contient `b(nom)`, le chemin du fichier
  associé lorsqu'il existe, et un `?` final si le buffer est modifié.
  Les buffers modifiés sans fichier associé figurent aussi dans la liste.
- **Exemple :** avec le buffer `travail` courant et modifié, et un autre
  buffer `brouillon` modifié sans fichier :

  ```fred
  F?
  b(travail) travail.txt ?
  b(brouillon) ?
  ```

- **Limites :** aucun buffer n'est enregistré, sélectionné ni modifié. Les
  buffers autres que le courant qui sont propres n'apparaissent pas ; le
  buffer courant n'est jamais répété. Une adresse de ligne ou des paramètres
  sont refusés. La fiche historique est consultable avec `?f?` (ou `?fq`).

### FO — Options actives

- **Syntaxe :** `FO`.
- **Effet :** affiche les états réellement actifs de `OI(` et `OM`, par
  exemple `o+i(` et `o-m` avec les valeurs initiales.
- **Exemple :** `FO`.
- **Limite :** le reste des options historiques n'est pas implémenté ;
  cette commande est informative.

## Messages, commentaires et sortie

### `"` — Commentaire

- **Syntaxe :** une ligne commençant par `"`, suivie d'un texte libre.
- **Effet :** ignore le reste de la ligne, sans afficher ni exécuter son
  contenu.
- **Exemple :** `" suppression reportée : 1,$D`.
- **Limite :** le caractère marque ici une **commande de commentaire** ;
  il n'a pas le même rôle que les guillemets d'un chemin dans `R`.

### `=` — Afficher le numéro de ligne

- **Syntaxe :** `=`, `n=` ou `$=`.
- **Effet :** affiche le numéro courant, ou celui de la ligne indiquée,
  sans déplacer le curseur ni modifier les registres. Sur buffer vide,
  `=` sans adresse affiche `0`.
- **Exemple :** `3=`.
- **Limite :** une adresse explicite doit désigner une ligne existante ;
  les plages ne sont pas acceptées.

### JM — Message avec fin de ligne

- **Syntaxe :** `JM texte` ou `JM/texte/` (délimiteurs adaptés possibles).
- **Effet :** affiche un message suivi d'un retour à la ligne.
- **Exemple :** `JM/Chargement terminé/`.
- **Limite :** longueur maximale de **2 000 octets** après expansion dans
  l'implémentation actuelle (les caractères UTF-8 accentués peuvent occuper
  plusieurs octets). Le REPL n'exécute qu'une commande JM/JP par ligne ; les
  procédures peuvent enchaîner les formes délimitées.

### JP — Message sans fin de ligne

- **Syntaxe :** `JP texte` ou `JP/texte/`.
- **Effet :** affiche un message **sans** retour à la ligne, par exemple
  avant une sortie complémentaire.
- **Exemple :** `JP/Chargement... /`.
- **Limite :** même limite de 2 000 octets et mêmes règles
  d'enchaînement que `JM`.

## Options et fin de session

### `OI(` — Parenthèses des noms de buffer

- **Syntaxe :** `O+I(` ou `O-I(`.
- **Effet :** `O+I(` impose les noms de buffer entre parenthèses ;
  `O-I(` autorise également les noms courts d'un caractère (`Bx`).
- **Exemple :** `O-I(` puis `Bx`.
- **Limite :** `O+I(` est actif au démarrage. `OI(` sans signe est refusé.

### OM — Trace des procédures

- **Syntaxe :** `O+M` ou `O-M`.
- **Effet :** active ou désactive l'affichage de suivi des commandes exécutées
  par le moteur de procédures.
- **Exemple :** `O+M` avant l'exécution d'une procédure, puis `O-M`.
- **Limite :** la trace est désactivée au démarrage ; ce n'est ni un débogueur
  interactif complet ni une modification du contenu des buffers.

### Q — Quitter

- **Syntaxe :** `Q` ou `QQ`.
- **Effet :** `Q` demande la sortie et refuse de quitter si un buffer est
  modifié. `QQ` quitte immédiatement, sans cette protection.
- **Exemple :** `W sauvegarde.txt` puis `Q`.
- **Limite :** `QQ` peut faire perdre des modifications non sauvegardées ;
  `Q!` reste indisponible.

## Copie avec insertion

### ZK — Copier en insérant

- **Syntaxe :** `ZK(destination)` ou `n,mZK(destination)`.
- **Effet :** insère une copie des lignes sélectionnées **après la ligne
  courante du buffer de destination**, sans effacer les lignes déjà présentes.
  Le buffer source reste sélectionné et son texte est préservé.
- **Exemple :** `1,2ZK(extraits)`.
- **Limite :** contrairement à `K`, `ZK` insère. Une copie dans le même
  buffer est interdite si la plage copiée comprend sa ligne courante.

## Choisir entre les commandes proches

| Besoin | Utiliser | Éviter la confusion avec |
|---|---|---|
| Copier en remplaçant la destination | `K` | `ZK`, qui insère |
| Déplacer et supprimer de la source | `M` | `K`, qui conserve la source |
| Copier en conservant la destination | `ZK` | `K`, qui la remplace |
| Afficher un fichier sans charger le buffer | `L fichier` | `R`, qui charge ou insère |
| Afficher une ligne et la rendre courante | `P` | `Z`, qui n'affiche rien |
| Tester un motif sans modifier le texte | `T` | `S`, qui substitue |
| Écrire dans l'encodage UTF-8 | `WU` | `WB`, non pris en charge |
| Quitter en vérifiant les modifications | `Q` | `QQ`, sortie immédiate |
| Identifier les buffers modifiés | `F?` | `FF`, qui liste les buffers associés à un fichier même propres |

## Vérification et périmètre

Les descriptions ci-dessus ont été rapprochées des branches de
`CommandParser`, `CommandExecutor`, `ExecutionContext` et des tests des
commandes. La suite Windows de **57 tests** a été annoncée réussie pour la version
précédente 0.0.26. La validation Windows complète de 0.0.27 reste à réaliser ;
l'ajout de `F?` est couvert par des tests ciblés du parseur, du moteur et de l'aide.
Il n'atteste **pas** une homologation historique complète : celle-ci reste
suivie dans [COMMAND_STATUS.md](../../project/COMMAND_STATUS.md).

Les fonctions supplémentaires `!`, `ZG`, `*`, les directives de flot
(`\B`, `\S`) et les commandes internes de procédures existent dans
FREDPP, mais ne sont **pas** comptées comme des commandes supplémentaires
dans l'inventaire historique des 27. Elles sont détaillées dans la
[documentation du projet](../../project/COMMAND_STATUS.md) et les
[pages complémentaires](commandes-speciales.md).
