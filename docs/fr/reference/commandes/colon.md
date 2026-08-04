# : - Imprime une fenêtre autour d'une ligne.

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
: - Imprime une fenêtre autour d'une ligne.
Syntaxe :
:<n>
Exemples :
b(buf)
:1Ajout  ligne de texte numero 1 Ajout  ligne de texte numero 2 :1Ajout  ligne de texte numero 1 Ajout  ligne de texte numero 2 Ajout  ligne de texte numero 3 :1Ajout  ligne de texte numero 2 Ajout  ligne de texte numero 3 Ajout  ligne de texte numero 4 :1Ajout  ligne de texte numero 3 Ajout  ligne de texte numero 4 
Descriptions :
La commande : imprime une fenêtre de <n> ligne autour de la ligne courante. L'adresse de la ligne courante prend la valeur de la dernière ligne affichée. La commande : est équivalent à ".-5,.+5p" en implicite.
Options :
Valeur implicite de <n> est de 5 lignes.
```
