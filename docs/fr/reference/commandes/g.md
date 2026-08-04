# G - Global commande ( Global ).

> Conversion initiale depuis DNB14A.docx. À restructurer.

```text
G - Global commande ( Global ).
Syntaxe :
(1,$)G[~]/<modèle>/<commandes><nl>
Exemples :
b(buf)
g/ligne/pAjout  ligne de texte numero 1Ajout  ligne de texte numero 2Ajout  ligne de texte numero 3Ajout  ligne de texte numero 4g~/1/pAjout  ligne de texte numero 2Ajout  ligne de texte numero 3Ajout  ligne de texte numero 4
Descriptions :
La commande G copie les commandes à l'intérieur d'un buffer secret et les exécute pour chaque ligne correspondant au modèle.
Si "~" précède le modèle, on exécute les commandes pour chaque ligne ne correspondant pas au modèle. 
A chaque itération le modèle courant (//) est chargé avec le modèle de la global. Le nombre de lignes répondant au modèle est mis dans le registre compteur (#). 
L'adresse de la ligne courante (".") est chargée à la valeur qu'elle avait après l'exécution de la dernière commande.
Options :
```
