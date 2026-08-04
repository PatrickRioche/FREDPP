# ZT - Traduction ( Traduct ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
ZT - Traduction ( Traduct ).
Syntaxe :
(.,.)ZT/<chaîne1>/<chaîne2>/
Exemples :
b(buf)
*zt/ABCDEFGH/BCDEFGHI/*Bjout  ligne de texte numero 1Bjout  ligne de texte numero 2Bjout  ligne de texte numero 3Bjout  ligne de texte numero 4
Descriptions :
 La commande ZT convertit respectivement les caractères de <chaîne1> en caractères de <chaîne2>.
Il faut que <chaîne2> soit de la même longueur que <chaîne1>, il faut également qu'aucun caractère ne puisse apparaître plus d'une fois dans <chaîne1>.
Le nombre de caractères convertis est mis dans le registre compteur. L'adresse de la ligne courante "." pointe sur la  dernière ligne où des conversions ont eu lieu.
Options :
```
