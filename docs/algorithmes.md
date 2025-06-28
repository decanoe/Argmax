# Algorithmes
## Algorithmes évolutifs & mimétiques
> [!NOTE]
> Cette documentation n'est pas encore faite

## Algorithmes de recherche locale
### Hill Climb Best (hc_best)
Après évaluation de tout les voisins de la solution actuelle, le meilleur voisin améliorant est choisi. On recommence ainsi jusqu'à avoir atteint le budget maximal ou une solution sans voisin améliorant.
### Hill Climb Least (hc_least)
Similaire au Hill Climb Best mais le pire voisin améliorant est choisi.
### Hill Climb First (hc_first)
Les voisins sont évalués un par un (en commançant par le premier bit et jusqu'au dernier). Le premier voisin améliorant est choisi pour remplacer la solution actuelle (coupant ainsi une partie des recherches). On recommence ainsi jusqu'à avoir atteint le budget maximal ou une solution sans voisin améliorant.
### Hill Climb Random (hc_random)
Similaire au Hill Climb First mais les voisins sont évalués un par un dans un ordre aléatoire.
### Hill Climb Cycle (hc_cycle)
Similaire au Hill Climb First mais les voisins sont évalués un par un en commançant par un bit différent à chaque itération. (ex: itération 1 on commence par le bit 1, puis 2, ..., puis n. itération 2 on commence par le bit 2, puis 3, ..., puis n, puis 1. etc...)

### Greedy All Best
On trie toute les mutations dans l'ordre de la plus bénéfique à la moins bénéfique. On applique ensuite toute ces mutations en même temps avant de les enlever au fur et à mesure en commençant par la dernière. On choisi ensuite la meilleur solution ainsi construite pour l'itération suivante.
### Greedy All First
Similaire au Greedy All Best mais la première solution améliorante est choisi pour l'itération suivante. C'est donc celle qui change le plus de bit en une seule fois (parmis les solutions testées).
### Greedy All Least
Similaire au Greedy All Best mais la pire solution améliorante est choisie.
### Greedy Improve Best/First/Least
Similaire au Greedy All Best/First/Least mais seules les mutations améliorantes sont conservées lors de la première étape et utilisées lors de la seconde.