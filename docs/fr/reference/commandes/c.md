# C - Remplacement de n lignes ( Change ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
C - Remplacement de n lignes ( Change ).
Syntaxe :
(.,.)C<bl><texte>
Exemples :
b(buf)
1c>Change ligne de texte numero 1>\F1,2c>Change ligne de texte numero 1Change ligne de texte numero 2>\Fo-i\F$c Change ligne de texte numero 4*Change ligne de texte numero 1Change ligne de texte numero 2Ajout  ligne de texte numero 3Change ligne de texte numero 4
Descriptions :
La commande C permet de changer des lignes de texte dans un buffer. On peut changer ce texte après un numéro de ligne précis ou si aucun numéro de ligne n'est précisé le remplacement s'effectuera après la ligne courante. Il est également possible de remplacer un groupe de ligne.
Options :
Si l'option O-I\F est validée, le texte pourra être entré directement derrière la commande C et il ne sera pas nécessaire de terminer l'entrée par un \F.
```
