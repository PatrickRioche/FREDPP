# ZM - Déplacement lignes dans un buffer à la suite ( Zap Move ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
ZM - Déplacement lignes dans un buffer à la suite ( Zap Move ).
Syntaxe :
(.,.)ZM<nom de buffer>
Exemples :
b(buf)
1zm(bufm)3zk(bufm)*Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3Ajout  ligne de texte numero 4b(bufm)*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 4
Descriptions :
La commande ZM supprime les lignes sélectionnées du buffer courant et les place dans le buffer désigné, à la suite de la ligne courante de ce buffer.
Le <nom de buffer> peut désigner le buffer courant, mais alors les lignes spécifiées ne peuvent pas contenir la ligne courante ".". La ligne courante "." du buffer émetteur est positionnée sur la ligne qui suit la dernière ligne déplacée; la ligne courante "." du buffer récepteur est mise sur la dernière ligne rajoutée.
Options :
```
