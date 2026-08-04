# A - Insertion  après la ligne courante ( Append).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
A - Insertion  après la ligne courante ( Append).
Syntaxe :
(.)A<bl><texte>
Exemples :
b(buf)
a>Ajout  ligne de texte numero 1>\F$a>Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3>\Fo-i\F$a Ajout  ligne de texte numero 4*Ajout  ligne de texte numero 1Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3Ajout  ligne de texte numero 4
Descriptions :
La commande A permet d'entrer du texte dans un buffer. On peut insérer ce texte après un numéro de ligne précis ou si aucun numéro de ligne n'est précisé l'insertion s'effectuera après la ligne courante.
Options :
Si l'option O-I\F est validée, le texte pourra être entré directement derrière la commande A et il ne sera pas nécessaire de terminer l'entrée par un \F.
Remarques :
Le buffer "buf" avec son contenu nous servira de base pour les exemples des autres commandes.
```
