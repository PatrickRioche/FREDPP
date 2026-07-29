# SPEC-003 — Infrastructure lexicale

## 1. Objet

Cette spécification définit le premier lexer de FRED++.

Le lexer transforme une suite de caractères en une suite de tokens. Il ne
valide pas encore la grammaire complète d'une commande FRED et n'exécute rien.

## 2. Localisation

Chaque token possède une position de début :

- `offset` : position absolue, à partir de 0 ;
- `line` : ligne, à partir de 1 ;
- `column` : colonne, à partir de 1 ;
- `flow_level` : niveau de la source d'entrée.

Dans cette étape, le lexer travaille sur une chaîne à niveau constant. Le champ
`flow_level` prépare l'intégration future avec le moteur de flux.

## 3. Types de tokens

- `End`
- `Identifier`
- `Number`
- `Command`
- `Comma`
- `LeftParenthesis`
- `RightParenthesis`
- `Backslash`
- `NewLine`
- `Symbol`
- `Unknown`

## 4. Règles lexicales

### Espaces

Les espaces, tabulations et retours chariot sont ignorés.

Le saut de ligne `\n` produit un token `NewLine`.

### Nombres

Une suite non vide de chiffres ASCII produit un token `Number`.

Exemple :

```text
123
```

produit :

```text
Number("123")
```

### Identifiants

Un identifiant commence par une lettre ASCII minuscule ou un soulignement.

Il peut ensuite contenir :

- lettres ASCII ;
- chiffres ;
- soulignements.

Exemple :

```text
buffer_12
```

produit :

```text
Identifier("buffer_12")
```

### Commandes

Une lettre ASCII majuscule isolée ou suivie d'un caractère non alphanumérique
produit un token `Command`.

Cette règle est provisoire et sera affinée par le parser lorsque la syntaxe
complète des commandes sera intégrée.

Exemple :

```text
D
```

produit :

```text
Command("D")
```

### Ponctuation

Les caractères suivants possèdent leur propre token :

- `,`
- `(`
- `)`
- `\`

Les autres caractères ASCII imprimables produisent un token `Symbol`.

Les caractères non reconnus produisent `Unknown`.

## 5. Diagnostics

Un diagnostic contient :

- une gravité ;
- un message ;
- une localisation ;
- éventuellement la ligne source.

Il peut être formaté sous une forme lisible par un humain.

## 6. Hors périmètre

- reconnaissance complète des adresses ;
- reconnaissance des patterns ;
- délimiteurs dépendants du niveau ;
- syntaxe des stream directives ;
- texte brut des commandes ;
- exécution.
