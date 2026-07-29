# SPEC-004 — CharacterStream et TokenStream

## 1. Objet

Cette spécification définit les couches de navigation situées avant et après le
lexer.

Le but est de découpler :

1. la provenance des caractères ;
2. leur analyse lexicale ;
3. la consommation des tokens par le futur parser.

## 2. Architecture

```text
Source de caractères
        ↓
CharacterStream
        ↓
Lexer
        ↓
TokenStream
        ↓
Parser
```

## 3. Character

Un caractère enrichi contient :

- sa valeur ;
- sa localisation de début.

```cpp
struct Character {
    char value;
    SourceLocation location;
};
```

## 4. CharacterStream

L'interface fournit :

```cpp
peek(lookahead)
consume()
eof()
position()
rewind(position)
end_location()
```

### 4.1 `peek`

Retourne le caractère situé à la position courante plus le décalage demandé,
sans modifier la position courante.

Un résultat vide indique que la fin est atteinte.

### 4.2 `consume`

Retourne le caractère courant et avance d'une position.

À la fin, retourne un résultat vide et conserve la position.

### 4.3 `position`

Retourne l'indice du prochain caractère à consommer.

### 4.4 `rewind`

Replace le flux à une position déjà valide, comprise entre 0 et la taille de la
source incluse.

Une position invalide déclenche `std::out_of_range`.

### 4.5 `end_location`

Retourne la localisation correspondant à la fin de la source. Elle permet au
lexer de localiser précisément le token `End`.

## 5. StringCharacterStream

`StringCharacterStream` est la première implémentation concrète.

Elle matérialise les caractères et leurs localisations lors de sa construction.
Les lignes et colonnes commencent à 1 ; l'offset commence à 0.

Un saut de ligne appartient à la ligne et à la colonne où il commence. Le
caractère suivant débute à la ligne suivante, colonne 1.

Le niveau de flux est constant pour toute la chaîne dans cette étape.

## 6. Lexer

Le lexer accepte désormais une référence vers un `CharacterStream`.

Un constructeur de commodité à partir de `std::string_view` reste disponible et
possède alors son propre `StringCharacterStream`.

Les règles lexicales de SPEC-003 restent inchangées.

## 7. TokenStream

Le `TokenStream` reçoit un `Lexer` et fournit :

```cpp
const Token& peek(std::size_t lookahead = 0);
Token consume();
bool eof();
std::size_t position() const;
void rewind(std::size_t position);
```

### 7.1 Production paresseuse

Les tokens sont demandés au lexer uniquement lorsqu'un appel à `peek`,
`consume` ou `eof` en a besoin.

Chaque token produit est mis en cache.

### 7.2 Fin de flux

Le token `End` est unique dans le cache.

Les consultations au-delà de la fin retournent toujours ce même token `End`.

La consommation de `End` ne fait pas progresser la position.

### 7.3 Retour en arrière

`rewind` ne peut revenir qu'à une position déjà atteinte ou à la position
courante. Une tentative de déplacement vers une position qui n'a pas encore été
produite ou située après la position courante déclenche `std::out_of_range`.

## 8. Tests obligatoires

### CharacterStream

- localisation initiale ;
- `peek` sans consommation ;
- `consume` ;
- suivi des lignes et colonnes ;
- lookahead ;
- retour en arrière ;
- fin de flux ;
- rejet d'une position invalide.

### TokenStream

- lookahead sans consommation ;
- consommation ordonnée ;
- position courante ;
- retour en arrière ;
- stabilité du token `End` ;
- rejet d'un retour invalide.

### Non-régression

Tous les tests des étapes 1 à 3 doivent rester conformes.

## 9. Hors périmètre

- parser ;
- AST ;
- adaptation directe d'un flux imbriqué `FlowEngine` caractère par caractère ;
- changement dynamique du niveau au sein d'une même source ;
- grammaire FRED.
