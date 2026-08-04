# OIR - Définition prompt pour return ( Option Input Return )

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
OIR - Définition prompt pour return ( Option Input Return )
Syntaxe :
O[+|-]IR/<chaîne>/
Exemples :
o+ir/<RETURN>/
<RETURN>b(bufo)<RETURN>a<RETURN>>Ajout ligne dans le buffer<RETURN>>\F<RETURN>p<RETURN>Ajout ligne dans le buffer
Descriptions :
L'option OIR permet de définir la chaîne de caractères envoyée à chaque return.
Options :
Valeur implicite O-IR.
OI\F - \F nécessaire ( Option Input \F )
Syntaxe :
O[+|-]I\F
Exemples :
b(buf)
a Ajout  d'une ligne  1 dans un buffer>\fo-i\fa Ajout  d'une ligne 2 dans un buffer*Ajout  d'une ligne  1 dans un bufferAjout  d'une ligne  2 dans un buffer
Descriptions :
L'option OI\F permet de rendre obligatoire ou non le \F pour délimiter la fin du mode "entrée" pour les commandes du type A<bl><texte>, I<bl><texte>, et C<bl><texte>.
Options :
Valeur implicite O+I\F.
```
