# K - Copie de lignes vers un buffer désigné ( Kopie ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
K - Copie de lignes vers un buffer désigné ( Kopie ).
Syntaxe :
(.,.)K<nom de buffer>
Exemples :
b(buf)
1,2k(bufk)*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3Ajout  ligne de texte numero 4b(bufk)*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2
Descriptions :
La commande K copie les lignes sélectionnées dans le buffer désigné. Le contenu antérieur du buffer désigné  est perdu. Les lignes successives, sans <cr> en fin de ligne, sont concaténées.
L'adresse de la ligne courante "." du buffer récepteur est positionnée sur la dernière ligne de ce buffer. Dans le buffer courant, "." pointe sur la dernière ligne  copiée.
Options :
```
