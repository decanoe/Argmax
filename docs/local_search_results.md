> [!TIP]
> Une liste complete des algorithmes est présente [ici](algorithmes.md)

> [!TIP]
> Les tableaux sont disponibles en latex [ici](tables)
> Et les graphiques sont disponibles en pdf [ici](tables)

# Comparaisons des scores pour chaque algorithmes
## Evolution du score en fonction du budget
> [NOTE]
> Pour chaque N et K, une moyenne est faite sur 10 instances différentes avec un budget de 100 000 pour chaque exécution.
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
> La moyenne est calculée sur 10 instances différentes par paire de N, K et autant d'execution nécessaire pour couvrir un budget de 100 000 tests
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | GJ_full_best     | GJ_full_first    | GJ_full_least    | GJ_improve_best  | GJ_improve_first | GJ_improve_least | HC_best          | HC_least         | HC_first         |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         | 32.86717         | <ins>32.878343</ins> | 32.863577        | 32.8673          | 32.86717         | 32.853276        | 32.864414        | 32.86576         | 32.859231        |
| **50_1**         | 34.505982        | 34.865144        | <ins>34.882736</ins> | 34.433857        | 34.539119        | 34.377127        | 34.344199        | 33.712474        | 34.046067        |
| **50_2**         | 35.740111        | 36.643723        | <ins>36.706148</ins> | 35.636274        | 36.283573        | 36.001905        | 35.49554         | 34.97499         | 35.288552        |
| **50_4**         | 35.934489        | 37.021581        | <ins>37.154439</ins> | 35.737288        | 36.835022        | 36.894757        | 35.61568         | 35.472421        | 35.536624        |
| **50_8**         | 35.356509        | 36.063725        | <ins>36.144621</ins> | 35.207436        | 36.037906        | 36.134354        | 35.006457        | 35.016586        | 35.111085        |
| **100_0**        | 66.42511         | <ins>66.448638</ins> | 66.42511         | 66.424769        | 66.42511         | 66.447823        | 66.43283         | 66.43283         | 66.400337        |
| **100_1**        | 68.88609         | <ins>69.669221</ins> | 69.631035        | 68.824216        | 69.188494        | 68.680218        | 68.610697        | 67.696882        | 68.131376        |
| **100_2**        | 72.275839        | 74.01429         | <ins>74.199211</ins> | 72.09345         | 73.458098        | 72.406294        | 71.812728        | 70.68252         | 71.234019        |
| **100_4**        | 72.790482        | 76.123169        | <ins>76.236187</ins> | 72.537993        | 75.438421        | 75.313229        | 72.143794        | 72.516582        | 72.072232        |
| **100_8**        | 71.345418        | 73.63652         | <ins>73.836824</ins> | 71.022922        | 73.550162        | 73.673249        | 70.728405        | 71.137039        | 70.922516        |
</details>

## Meilleur score obtenu pour des budgets fixes
> [NOTE]
> Les meilleurs scores sont calculés en faisant la moyenne du score maximum atteint sur 10 instances différentes

### Meilleur score obtenu pour un budget de 1 000 tests
Sur un budget très limité, les algorithmes greedy_improve_best et greedy_improve_first atteignent des scores plus élevés sur les instances la plus part des instances. Mais sur les instances les plus dures (K = 8), le Hill Climb Random a de meilleurs (car plus d'itérations).
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | GJ_full_best     | GJ_full_first    | GJ_full_least    | GJ_improve_best  | GJ_improve_first | GJ_improve_least | HC_best          | HC_least         | HC_first         |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         | 27.54098         | 28.09824         | <b>32.86717</b>  | 29.89275         | 32.03607         | <b>32.86717</b>  | 30.73164         | <b>32.86717</b>  | <b>32.86717</b>  |
| **50_1**         | 28.55199         | 28.24408         | 34.64719         | 30.22962         | 33.97159         | <b>34.71314</b>  | 33.12759         | 34.50354         | 34.57514         |
| **50_2**         | 29.69471         | 29.01804         | 36.05676         | 32.65756         | 35.91503         | 36.05194         | 35.43983         | 36.09961         | <b>36.30271</b>  |
| **50_4**         | 27.6205          | 26.20335         | 36.49837         | 31.94295         | 35.84898         | 36.63906         | 35.3351          | 36.63424         | <b>36.90377</b>  |
| **50_8**         | 26.3489          | 26.02113         | 35.96935         | 32.53197         | 35.10636         | 35.88613         | 32.02863         | <b>36.48866</b>  | 35.76613         |
| **100_0**        | 51.37087         | 49.60919         | 66.42511         | 54.47805         | 55.88623         | 66.42511         | 53.99935         | 66.42511         | <b>66.42511</b>  |
| **100_1**        | 51.2954          | 50.25853         | 68.64867         | 52.82438         | 58.86414         | <b>68.81285</b>  | 57.12866         | 68.44706         | 66.37941         |
| **100_2**        | 52.43478         | 50.50332         | 72.0822          | 54.33979         | 60.91994         | <b>72.76593</b>  | 58.49114         | 71.69474         | 69.13696         |
| **100_4**        | 51.78077         | 50.84667         | 70.30575         | 56.57087         | 63.12278         | <b>72.58494</b>  | 59.70643         | 72.47889         | 64.57741         |
| **100_8**        | 50.88909         | 50.12205         | 68.45522         | 55.85677         | 65.15502         | 70.18352         | 53.62663         | <b>70.82872</b>  | 59.41112         |
</details>

### Meilleur score obtenu pour un budget de 10 000 tests
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | GJ_full_best     | GJ_full_first    | GJ_full_least    | GJ_improve_best  | GJ_improve_first | GJ_improve_least | HC_best          | HC_least         | HC_first         |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  |
| **50_1**         | <b>34.97442</b>  | 34.33283         | 34.94103         | 34.90085         | 34.85863         | 34.96754         | 34.77175         | 34.89312         | 34.94613         |
| **50_2**         | 37.12244         | 36.05053         | 37.11131         | <b>37.20949</b>  | 36.79049         | 37.01226         | 36.74583         | 36.95076         | 37.2034          |
| **50_4**         | 37.97851         | 36.05817         | 37.79061         | <b>38.10711</b>  | 37.28213         | 37.69093         | 38.05633         | 37.68232         | 38.08972         |
| **50_8**         | 36.73593         | 35.12944         | 37.66188         | 37.31819         | 37.24692         | 37.61294         | 37.38157         | <b>37.80115</b>  | 37.50139         |
| **100_0**        | 62.68496         | <b>66.42511</b>  | 66.42511         | 65.58168         | 66.42511         | 66.42511         | <b>66.42511</b>  | 66.42511         | <b>66.42511</b>  |
| **100_1**        | 65.14573         | 67.63292         | 69.70592         | 68.66122         | 69.08229         | 69.7076          | 68.77447         | 69.29129         | <b>69.72118</b>  |
| **100_2**        | 69.46507         | 71.08727         | 73.79038         | 72.71978         | 72.9688          | <b>73.98254</b>  | 73.07679         | 73.70689         | 73.94096         |
| **100_4**        | 70.14368         | 58.98907         | 74.58161         | 76.42242         | 73.67658         | 74.92031         | 75.74231         | 75.27732         | <b>76.55997</b>  |
| **100_8**        | 66.77445         | 54.43737         | 73.4802          | 73.45938         | 72.06762         | 73.27804         | <b>74.40796</b>  | 74.12656         | 74.05281         |
</details>

### Meilleur score obtenu pour un budget de 100 000 tests
Avec beaucoups de budget, les algorithmes Greedy sont toujours plus performants si la taille des instances n'est pas trop petite.
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | GJ_full_best     | GJ_full_first    | GJ_full_least    | GJ_improve_best  | GJ_improve_first | GJ_improve_least | HC_best          | HC_least         | HC_first         |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  | <b>32.86717</b>  |
| **50_1**         | 34.97942         | 34.81687         | <b>34.9848</b>   | 34.9848          | 34.96754         | 34.9848          | <b>34.9848</b>   | 34.9848          | <b>34.9848</b>   |
| **50_2**         | 37.28097         | 36.79225         | <b>37.329</b>    | 37.2915          | 37.2546          | 37.31815         | 37.30879         | 37.30897         | 37.32801         |
| **50_4**         | <b>38.69557</b>  | 37.4375          | 38.34996         | 38.59722         | 38.22188         | 38.32615         | 38.54548         | 38.44399         | 38.65786         |
| **50_8**         | 38.3728          | 37.16006         | 38.3506          | 38.41285         | 38.17756         | 38.40835         | 38.41868         | <b>38.91033</b>  | 38.88409         |
| **100_0**        | <b>66.42511</b>  | <b>66.42511</b>  | 66.42511         | 66.42511         | 66.42511         | 66.42511         | <b>66.42511</b>  | 66.42511         | <b>66.42511</b>  |
| **100_1**        | 69.90407         | 68.95904         | 69.92123         | 69.93081         | 69.73996         | <b>69.96401</b>  | 69.73218         | 69.70663         | 69.93986         |
| **100_2**        | <b>74.92245</b>  | 73.03069         | 74.47192         | 74.84078         | 73.94138         | 74.39487         | 74.45436         | 74.24453         | 74.7697          |
| **100_4**        | 77.73532         | 74.31157         | 76.13663         | 77.8408          | 75.38315         | 76.62696         | 77.4047          | 76.51002         | <b>77.93245</b>  |
| **100_8**        | 75.90545         | 72.07755         | 74.85355         | 76.11952         | 74.58828         | 74.90298         | 75.9813          | 75.17865         | <b>76.6459</b>   |
</details>

Globalement, sur des instances de petite taille (N = 50), les Hill Climber sont assez performants, mais ils sont vite surpassé par les algorithmes Greedy lorsque N augmente.

<br/><br/><br/><br/><br/>

# Comparaisons des budgets moyens pour chaque algorithmes
> [NOTE]
> La moyenne est calculée sur 2 runs par instances et par paire de N, K.
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | GJ_full_best     | GJ_full_first    | GJ_full_least    | GJ_improve_best  | GJ_improve_first | GJ_improve_least | HC_best          | HC_least         | HC_first         |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         | 201.0            | 3564.8           | 5351.0           | 128.1            | <ins>102.0</ins> | 1793.6           | 1406.0           | 1406.0           | 225.2            |
| **50_1**         | 476.0            | 2739.3           | 4101.0           | 286.0            | 541.2            | 1428.4           | 1158.5           | 1548.5           | <ins>238.8</ins> |
| **50_2**         | 546.0            | 2237.4           | 3611.0           | 313.8            | 737.2            | 1072.4           | 891.0            | 1901.0           | <ins>235.7</ins> |
| **50_4**         | 696.0            | 2096.8           | 3626.0           | 388.0            | 858.2            | 1291.2           | 798.5            | 3898.5           | <ins>253.6</ins> |
| **50_8**         | 636.0            | 1483.2           | 4036.0           | 349.6            | 777.0            | 1476.2           | 496.0            | 5286.0           | <ins>197.4</ins> |
| **100_0**        | 401.0            | 14478.4          | 20501.0          | 250.4            | <ins>202.0</ins> | 6296.2           | 5041.0           | 5041.0           | 581.9            |
| **100_1**        | 1181.0           | 11224.5          | 16351.0          | 702.2            | 1796.0           | 5136.4           | 4251.0           | 6111.0           | <ins>651.6</ins> |
| **100_2**        | 1501.0           | 10126.8          | 13961.0          | 810.3            | 2283.8           | 4411.2           | 3851.0           | 8631.0           | <ins>631.3</ins> |
| **100_4**        | 1651.0           | 8174.2           | 13441.0          | 934.6            | 2604.9           | 4120.3           | 3046.0           | 18596.0          | <ins>695.4</ins> |
| **100_8**        | 1641.0           | 6069.9           | 12681.0          | 929.8            | 3021.3           | 5520.0           | 2036.0           | 29381.0          | <ins>638.2</ins> |
</details>

Seulement 2 runs sont utilisés par instances car certains algorithmes Least n'ont pu finir que 2 runs. Prendre toute les runs effectuées sur chaque instance "cacherait" les instances compliquées.

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