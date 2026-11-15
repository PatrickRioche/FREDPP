# ELIZA — dialogue interactif en FREDPP

La procédure fournie dans [library/eliza.fredpp](../../../library/eliza.fredpp)
est une démonstration pédagogique de chatbot **à règles**, inspirée d'ELIZA
et de son scénario DOCTOR (Joseph Weizenbaum, MIT, 1964–1966).

Le programme historique était écrit en **MAD-SLIP**, sur IBM 7094.
Des adaptations en Lisp, notamment par Bernie Cosell, ont été réalisées ensuite.
Cette procédure FREDPP est une **adaptation simplifiée et originale** :
elle n'est ni la transcription du code historique, ni une intelligence
artificielle générative, ni un outil de consultation psychologique.

## Démarrage

Depuis un kit FREDPP comportant son répertoire library :

~~~powershell
fredpp eliza
~~~

Sous Windows depuis une compilation locale, on peut aussi lancer :

~~~powershell
& "$env:LOCALAPPDATA\FREDPP\build\x64-Debug\Debug\fredpp.exe" .\library\eliza.fredpp
~~~

Saisissez une phrase après chaque invite VOUS >. Tapez **FIN**, **STOP**,
**QUITTER** ou **AU REVOIR** (sans ponctuation ou avec un signe final)
pour arrêter la conversation. Aucun argument ou fichier externe n'est requis.

## Exemple de conversation

~~~text
ELIZA - demonstration de conversation par regles.
Ecrivez une phrase. Tapez FIN pour terminer.
VOUS > Bonjour
ELIZA > Bonjour. De quoi aimeriez-vous parler ?
VOUS > Je suis fatigue
ELIZA > Depuis quand etes-vous fatigue ?
VOUS > Je veux partir
ELIZA > Pourquoi voulez-vous partir ?
VOUS > fin
ELIZA > Au revoir. Merci pour cet echange.
~~~

## Fonctionnement

- La directive \R lit une ligne littérale depuis le terminal.
- La commande T recherche des mots-clés en ignorant la casse ASCII au moyen
  de classes de caractères explicites.
- La commande S reformule les phrases commençant par « je suis »,
  « je veux », « je pense que » ou « j'ai besoin de ».
- J et les étiquettes déterminent la réponse, tandis que N fait alterner
  trois réponses génériques lorsqu'aucun mot-clé n'est reconnu.
- Les réponses sont préprogrammées : aucune compréhension linguistique ou
  inférence d'intention n'est effectuée.

Les modèles sont volontairement simples. Les accents, pluriels, variantes
de ponctuation et tournures non prévues ne sont pas tous interprétés.
La procédure ne fait aucun accès réseau et n'écrit aucun fichier de
l'utilisateur. Un flux non interactif doit se terminer par FIN ; une
fin d'entrée standard prématurée provoque une erreur de lecture \R.

## Références historiques

- Joseph Weizenbaum (1966), « ELIZA—A Computer Program For the Study of
  Natural Language Communication Between Man and Machine », Communications
  of the ACM, 9(1), 36–45. DOI : https://doi.org/10.1145/365153.365168
- Original ELIZA, historique et source MAD-SLIP :
  https://elizagen.org/original-eliza.html
