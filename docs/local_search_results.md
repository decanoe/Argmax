> [!TIP]
> Les tableaux sont disponibles en latex [ici](tables)
> Et les graphiques sont disponibles en pdf [ici](tables)

# Comparaisons des scores pour chaque algorithmes
## Evolution du score en fonction du budget
> [NOTE]
> Pour chaque problème, une moyenne est faite sur 10 instances différentes avec un budget de 1 000 000 pour chaque exécution.
Les graphiques montrent l'évolution du meilleur score trouvé en fonction du budget utilisé.
<details>
<summary>Graphiques pour N = 50</summary>

### K = 1
![score anytime pour les instances 50_1](graphs/50_1_anytime.pdf)
### K = 2
![score anytime pour les instances 50_2](graphs/50_2_anytime.pdf)
### K = 4
![score anytime pour les instances 50_4](graphs/50_4_anytime.pdf)
### K = 8
![score anytime pour les instances 50_8](graphs/50_8_anytime.pdf)
</details>

<details>
<summary>Graphiques pour N = 100</summary>

### K = 1
![score anytime pour les instances 100_1](graphs/100_1_anytime.pdf)
### K = 2
![score anytime pour les instances 100_2](graphs/100_2_anytime.pdf)
### K = 4
![score anytime pour les instances 100_4](graphs/100_4_anytime.pdf)
### K = 8
![score anytime pour les instances 100_8](graphs/100_8_anytime.pdf)
</details>

## Moyenne du score obtenu en une execution de l'algorithme
> [NOTE]
> La moyenne est calculée sur 10 instances différentes par problème
![Moyenne des scores non itérés](tables/avg.md)

## Meilleur score obtenu pour des budgets fixes
> [NOTE]
> Les meilleurs scores sont calculés en faisant la moyenne du score maximum atteint sur 10 instances différentes

### Meilleur score obtenu pour un budget de 1 000 tests
![Moyenne des scores itérés avec un budget de 10<sup>3</sup>](tables/best_1_000.md)

### Meilleur score obtenu pour un budget de 10 000 tests
![Moyenne des scores itérés avec un budget de 10<sup>4</sup>](tables/best_10_000.md)

### Meilleur score obtenu pour un budget de 100 000 tests
![Moyenne des scores itérés avec un budget de 10<sup>5</sup>](tables/best_100_000.md)

### Meilleur score obtenu pour un budget de 1 000 000 tests
![Moyenne des scores itérés avec un budget de 10<sup>6</sup>](tables/best_1_000_000.md)

<br/><br/><br/><br/><br/>

# Comparaisons des budgets moyens pour chaque algorithmes
> [NOTE]
> La moyenne est calculée sur 2 runs par instances et par problème.
![Moyenne des budgets non itérés](tables/budgets.md)

<br/><br/><br/><br/><br/>

# Corrélations entre la taille des sauts et d'autres données
> [NOTE]
> Ces résultats ne sont intéréssants que sur les algorithmes Greedy puisque les algorithmes Hill Climb ne font que des saut d'un seul bit.
## Taille des sauts en fonction du score
> [NOTE]
> À chaque saut, on regarde la taille du saut en fonction du score avant le saut.

<details>
<summary>Graphiques pour N = 100</summary>

### K = 1
![Taille des sauts en fonction du score pour les instances 100_1](graphs/100_1_greedy_jump_size_per_fitness.pdf)
### K = 2
![Taille des sauts en fonction du score pour les instances 100_4](graphs/100_2_greedy_jump_size_per_fitness.pdf)
### K = 4
![Taille des sauts en fonction du score pour les instances 100_4](graphs/100_4_greedy_jump_size_per_fitness.pdf)
### K = 8
![Taille des sauts en fonction du score pour les instances 100_8](graphs/100_8_greedy_jump_size_per_fitness.pdf)
</details>

## Score obtenu après chaque saut en foncion de sa taille
> [NOTE]
> À chaque saut, on regarde le score obtenu après le saut en fonction de la taille du saut.

<details>
<summary>Graphiques pour N = 100</summary>

### K = 1
![Score en fonction de la taille des sauts pour les instances 100_1](graphs/100_1_greedy_fitness_after_jump_size.pdf)
### K = 2
![Score en fonction de la taille des sauts pour les instances 100_1](graphs/100_2_greedy_fitness_after_jump_size.pdf)
### K = 4
![Score en fonction de la taille des sauts pour les instances 100_4](graphs/100_4_greedy_fitness_after_jump_size.pdf)
### K = 8
![Score en fonction de la taille des sauts pour les instances 100_8](graphs/100_8_greedy_fitness_after_jump_size.pdf)
</details>

## Taille des sauts en fonction du nombre de voisins améliorants
> [NOTE]
> À chaque saut, on regarde la taille du saut en fonction du nombre de voisins améliorants avant le saut.

<details>
<summary>Graphiques pour N = 100</summary>

### K = 1
![Taille des sauts en fonction du nombre de voisins améliorants pour les instances 100_1](graphs/100_1_greedy_jump_size_per_neighbor.pdf)
### K = 2
![Taille des sauts en fonction du nombre de voisins améliorants pour les instances 100_1](graphs/100_2_greedy_jump_size_per_neighbor.pdf)
### K = 4
![Taille des sauts en fonction du nombre de voisins améliorants pour les instances 100_4](graphs/100_4_greedy_jump_size_per_neighbor.pdf)
### K = 8
![Taille des sauts en fonction du nombre de voisins améliorants pour les instances 100_8](graphs/100_8_greedy_jump_size_per_neighbor.pdf)
</details>