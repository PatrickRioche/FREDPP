# I - Insertion de texte avant la ligne courante ( Insert ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
I - Insertion de texte avant la ligne courante ( Insert ).
Syntaxe :
(.)I<bl><texte>
Exemples :
b(buf)
1i>Insert ligne de texte numero 0>\Fo-i($i Insert ligne de texte numero $-1*Insert ligne de texte numero 0Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3Insert ligne de texte numero $-1Ajout  ligne de texte numero 4
Descriptions :
La commande I permet d'entrer du texte dans un buffer. On peut insérer ce texte avant un numéro de ligne précis ou si aucun numéro de ligne n'est précisé l'insertion s'effectuera avant la ligne courante.
Options :
Si l'option o-i\F est validée, le texte pourra être entré directement derrière la commande I et il ne sera pas nécessaire de terminer l'entrée par un \F.
```
