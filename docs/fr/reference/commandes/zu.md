# ZU -  Conversion en majuscules ( Zap Upper ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
ZU -  Conversion en majuscules ( Zap Upper ).
Syntaxe :
(.,.)ZU[/<modèle>/]
Exemples :
b(buf)
1zu2zu/^./? no text changed1,2pAJOUT  LIGNE DE TEXTE NUMERO 1Ajout  ligne de texte numero 2
Descriptions :
La commande ZU convertit toutes les lettres en majuscules; le nombre de lignes modifiées est mis dans le registre compteur ( # ).
La commande ZU/<modèle>/ met toutes les chaînes de caractères correspondant au modèle en majuscules; le nombre de chaînes converties est mis dans le registre compteur ( # ).
L'adresse de la ligne courante "." pointe sur la dernière ligne où des caractères ont été convertis.
Options :
```
