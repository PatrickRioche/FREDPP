# M - Déplacement de lignes vers un buffer (Move).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
M - Déplacement de lignes vers un buffer (Move).
Syntaxe :
(.,.)M<nom de buffer>
Exemples :
b(buf)
1,2m(bufm)*Ajout  ligne de texte numero 3Ajout  ligne de texte numero 4b(bufm)*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2
Descriptions :
La commande M copie les lignes sélectionnées dans le buffer désigné, et les supprime du buffer courant. Le contenu antérieur du buffer désigné  est perdu. 
L'adresse de la ligne courante "." du buffer désigné pointe sur la dernière ligne. Le "." du buffer courant pointe sur la ligne qui suit la dernière ligne déplacée.
Options :
```
