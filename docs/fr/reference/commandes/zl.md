# ZL - Conversion en minuscules ( Zap Lower ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
ZL - Conversion en minuscules ( Zap Lower ).
Syntaxe :
(.,.)ZL[/<modèle>/]
Exemples :
b(buf)
1zl2zl/^./1,2pajout  ligne de texte numero 1ajout  ligne de texte numero 2
Descriptions :
La commande ZL convertit toutes les lettres en minuscules; le nombre de lignes modifiées est mis dans le registre compteur ( # ).
La commande ZL/<modèle>/ met toutes les chaînes de caractères conformes au modèle en minuscules; le nombre de chaînes converties est mis dans le registre compteur ( # ).
L'adresse de la ligne courante "." pointe sur la dernière ligne où des caractères ont été modifiés par une conversion.
Options :
```
