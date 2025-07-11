> [!TIP]
> Une liste complete des algorithmes est présente [ici](algorithmes.md)

# Comparaisons des scores pour chaque algorithmes
## Evolution du score en fonction du budget
> [NOTE]
> Pour chaque N et K, une moyenne est faite sur 10 instances différentes avec un budget de 100 000 pour chaque exécution.
Les graphiques montrent l'évolution du meilleur score trouvé en fonction du budget utilisé.
<details>
<summary>Graphiques pour N = 50</summary>

### K = 1
![score anytime pour les instances 50_1](graphs/50_1_anytime.svg)
### K = 2
![score anytime pour les instances 50_2](graphs/50_2_anytime.svg)
### K = 4
![score anytime pour les instances 50_4](graphs/50_4_anytime.svg)
### K = 8
![score anytime pour les instances 50_8](graphs/50_8_anytime.svg)
</details>

<details>
<summary>Graphiques pour N = 100</summary>

### K = 1
![score anytime pour les instances 100_1](graphs/100_1_anytime.svg)
### K = 2
![score anytime pour les instances 100_2](graphs/100_2_anytime.svg)
### K = 4
![score anytime pour les instances 100_4](graphs/100_4_anytime.svg)
### K = 8
![score anytime pour les instances 100_8](graphs/100_8_anytime.svg)
</details>

## Moyenne du score obtenu en une execution de l'algorithme
> [NOTE]
> La moyenne est calculée sur 10 instances différentes par paire de N, K et autant d'execution nécessaire pour couvrir un budget de 100 000 tests
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | greedy_all_best  | greedy_all_first | greedy_all_least | greedy_improve_best | greedy_improve_first | greedy_improve_least | hc_best          | hc_cycle         | hc_first         | hc_least         | hc_random        |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         | 32.86717         |<ins>32.878343</ins>| 32.863577        | 32.8673          | 32.86717         | 32.853276        | 32.864414        | 32.864557        | 32.849807        | 32.86576         | 32.859231        |
| **50_1**         | 34.505982        | 34.865144        |<ins>34.882736</ins>| 34.433857        | 34.539119        | 34.377127        | 34.344199        | 34.011655        | 34.024705        | 33.712474        | 34.046067        |
| **50_2**         | 35.740111        | 36.643723        |<ins>36.706148</ins>| 35.636274        | 36.283573        | 36.001905        | 35.49554         | 35.352443        | 35.315575        | 34.97499         | 35.288552        |
| **50_4**         | 35.934489        | 37.021581        |<ins>37.154439</ins>| 35.737288        | 36.835022        | 36.894757        | 35.61568         | 35.580539        | 35.549809        | 35.472421        | 35.536624        |
| **50_8**         | 35.356509        | 36.063725        |<ins>36.144621</ins>| 35.207436        | 36.037906        | 36.134354        | 35.006457        | 35.090134        | 35.099744        | 35.016586        | 35.111085        |
| **100_0**        | 66.42511         |<ins>66.448638</ins>| 66.42511         | 66.424769        | 66.42511         | 66.447823        | 66.43283         | 66.351848        | 66.25454         | 66.43283         | 66.400337        |
| **100_1**        | 68.88609         |<ins>69.669221</ins>| 69.631035        | 68.824216        | 69.188494        | 68.680218        | 68.610697        | 67.988629        | 68.048581        | 67.696882        | 68.131376        |
| **100_2**        | 72.275839        | 74.01429         |<ins>74.199211</ins>| 72.09345         | 73.458098        | 72.406294        | 71.812728        | 71.227985        | 71.159504        | 70.68252         | 71.234019        |
| **100_4**        | 72.790482        | 76.123169        |<ins>76.236187</ins>| 72.537993        | 75.438421        | 75.313229        | 72.143794        | 71.957188        | 71.936709        | 72.516582        | 72.072232        |
| **100_8**        | 71.345418        | 73.63652         |<ins>73.836824</ins>| 71.022922        | 73.550162        | 73.673249        | 70.728405        | 70.996425        | 71.052282        | 71.137039        | 70.922516        |
</details>

## Meilleur score obtenu pour des budgets fixes
> [NOTE]
> Les meilleurs scores sont calculés en faisant la moyenne du score maximum atteint sur 10 instances différentes

### Meilleur score obtenu pour un budget de 1 000 tests
Sur un budget très limité, les algorithmes greedy_improve_best et greedy_improve_first atteignent des scores plus élevés sur les instances la plus part des instances. Mais sur les instances les plus dures (K = 8), le Hill Climb Random a de meilleurs (car plus d'itérations).
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | greedy_all_best  | greedy_all_first | greedy_all_least | greedy_improve_best | greedy_improve_first | greedy_improve_least | hc_best          | hc_cycle         | hc_first         | hc_least         | hc_random        |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         |<ins>32.86717</ins> | 29.89275         | 27.54098         |<ins>32.86717</ins> |<ins>32.86717</ins> | 30.73164         | 32.03607         |<ins>32.86717</ins> |<ins>32.86717</ins> | 28.09824         |<ins>32.86717</ins> |
| **50_1**         | 34.64719         | 30.22962         | 28.55199         |<ins>34.71314</ins> | 34.57514         | 33.12759         | 33.97159         | 34.42196         | 34.15507         | 28.24408         | 34.50354         |
| **50_2**         | 36.05676         | 32.65756         | 29.69471         | 36.05194         |<ins>36.30271</ins> | 35.43983         | 35.91503         | 36.10439         | 35.77722         | 29.01804         | 36.09961         |
| **50_4**         | 36.49837         | 31.94295         | 27.6205          | 36.63906         |<ins>36.90377</ins> | 35.3351          | 35.84898         | 36.2526          | 35.92693         | 26.20335         | 36.63424         |
| **50_8**         | 35.96935         | 32.53197         | 26.3489          | 35.88613         | 35.76613         | 32.02863         | 35.10636         | 36.28679         | 36.40375         | 26.02113         |<ins>36.48866</ins> |
| **100_0**        |<ins>66.42511</ins> | 54.47805         | 51.37087         |<ins>66.42511</ins> |<ins>66.42511</ins> | 53.99935         | 55.88623         | 64.43622         | 59.51405         | 49.60919         |<ins>66.42511</ins> |
| **100_1**        | 68.64867         | 52.82438         | 51.2954          |<ins>68.81285</ins> | 66.37941         | 57.12866         | 58.86414         | 65.73561         | 61.64481         | 50.25853         | 68.44706         |
| **100_2**        | 72.0822          | 54.33979         | 52.43478         |<ins>72.76593</ins> | 69.13696         | 58.49114         | 60.91994         | 69.62528         | 65.85027         | 50.50332         | 71.69474         |
| **100_4**        | 70.30575         | 56.57087         | 51.78077         |<ins>72.58494</ins> | 64.57741         | 59.70643         | 63.12278         | 70.16371         | 67.84039         | 50.84667         | 72.47889         |
| **100_8**        | 68.45522         | 55.85677         | 50.88909         | 70.18352         | 59.41112         | 53.62663         | 65.15502         | 70.38446         | 69.47161         | 50.12205         |<ins>70.82872</ins> |
</details>

### Meilleur score obtenu pour un budget de 10 000 tests
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | greedy_all_best  | greedy_all_first | greedy_all_least | greedy_improve_best | greedy_improve_first | greedy_improve_least | hc_best          | hc_cycle         | hc_first         | hc_least         | hc_random        |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |
| **50_1**         | 34.94103         | 34.90085         |<ins>34.97442</ins> | 34.96754         | 34.94613         | 34.77175         | 34.85863         | 34.89397         | 34.80274         | 34.33283         | 34.89312         |
| **50_2**         | 37.11131         |<ins>37.20949</ins> | 37.12244         | 37.01226         | 37.2034          | 36.74583         | 36.79049         | 36.86216         | 36.86174         | 36.05053         | 36.95076         |
| **50_4**         | 37.79061         |<ins>38.10711</ins> | 37.97851         | 37.69093         | 38.08972         | 38.05633         | 37.28213         | 37.67835         | 37.60261         | 36.05817         | 37.68232         |
| **50_8**         | 37.66188         | 37.31819         | 36.73593         | 37.61294         | 37.50139         | 37.38157         | 37.24692         | 37.43335         | 37.5617          | 35.12944         |<ins>37.80115</ins> |
| **100_0**        |<ins>66.42511</ins> | 65.58168         | 62.68496         |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |
| **100_1**        | 69.70592         | 68.66122         | 65.14573         | 69.7076          |<ins>69.72118</ins> | 68.77447         | 69.08229         | 68.92645         | 68.94549         | 67.63292         | 69.29129         |
| **100_2**        | 73.79038         | 72.71978         | 69.46507         |<ins>73.98254</ins> | 73.94096         | 73.07679         | 72.9688          | 72.69555         | 72.32775         | 71.08727         | 73.70689         |
| **100_4**        | 74.58161         | 76.42242         | 70.14368         | 74.92031         |<ins>76.55997</ins> | 75.74231         | 73.67658         | 74.64312         | 74.49308         | 58.98907         | 75.27732         |
| **100_8**        | 73.4802          | 73.45938         | 66.77445         | 73.27804         | 74.05281         |<ins>74.40796</ins> | 72.06762         | 73.13398         | 73.35448         | 54.43737         | 74.12656         |
</details>

### Meilleur score obtenu pour un budget de 100 000 tests
Avec beaucoups de budget, les algorithmes Greedy sont toujours plus performants si la taille des instances n'est pas trop petite.
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | greedy_all_best  | greedy_all_first | greedy_all_least | greedy_improve_best | greedy_improve_first | greedy_improve_least | hc_best          | hc_cycle         | hc_first         | hc_least         | hc_random        |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |<ins>32.86717</ins> |
| **50_1**         |<ins>34.9848</ins>  |<ins>34.9848</ins>  | 34.97942         |<ins>34.9848</ins>  |<ins>34.9848</ins>  |<ins>34.9848</ins>  | 34.96754         | 34.97922         | 34.95975         | 34.81687         |<ins>34.9848</ins>  |
| **50_2**         |<ins>37.329</ins>   | 37.2915          | 37.28097         | 37.31815         | 37.32801         | 37.30879         | 37.2546          | 37.29758         | 37.25478         | 36.79225         | 37.30897         |
| **50_4**         | 38.34996         | 38.59722         |<ins>38.69557</ins> | 38.32615         | 38.65786         | 38.54548         | 38.22188         | 38.53358         | 38.40819         | 37.4375          | 38.44399         |
| **50_8**         | 38.3506          | 38.41285         | 38.3728          | 38.40835         | 38.88409         | 38.41868         | 38.17756         | 38.65516         | 38.24358         | 37.16006         |<ins>38.91033</ins> |
| **100_0**        |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |<ins>66.42511</ins> |
| **100_1**        | 69.92123         | 69.93081         | 69.90407         |<ins>69.96401</ins> | 69.93986         | 69.73218         | 69.73996         | 69.61948         | 69.56062         | 68.95904         | 69.70663         |
| **100_2**        | 74.47192         | 74.84078         |<ins>74.92245</ins> | 74.39487         | 74.7697          | 74.45436         | 73.94138         | 74.18519         | 73.79735         | 73.03069         | 74.24453         |
| **100_4**        | 76.13663         | 77.8408          | 77.73532         | 76.62696         |<ins>77.93245</ins> | 77.4047          | 75.38315         | 76.1391          | 75.78376         | 74.31157         | 76.51002         |
| **100_8**        | 74.85355         | 76.11952         | 75.90545         | 74.90298         |<ins>76.6459</ins>  | 75.9813          | 74.58828         | 75.38416         | 75.19425         | 72.07755         | 75.17865         |
</details>

Globalement, sur des instances de petite taille (N = 50), les Hill Climber sont assez performants, mais ils sont vite surpassé par les algorithmes Greedy lorsque N augmente.

<br/><br/><br/><br/><br/>

# Comparaisons des budgets moyens pour chaque algorithmes
> [NOTE]
> La moyenne est calculée sur 10 instances différentes par paire de N, K et autant d'execution nécessaire pour couvrir un budget de 100 000 tests
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | greedy_all_best  | greedy_all_first | greedy_all_least | greedy_improve_best | greedy_improve_first | greedy_improve_least | hc_best          | hc_cycle         | hc_first         | hc_least         | hc_random        |
| ---------------: | ---------------: | ---------------: | ---------------: | ---------------: | ---------------: | ---------------: | ---------------: | ---------------: | ---------------: | ---------------: | ---------------: |
| **50_0**         | 201.0            | 3588.7           | 5194.1           | 126.0            | 102.0            | 1641.9           | 1308.2           | 380.5            | 691.5            | 1308.2           | 232.5            |
| **50_1**         | 493.5            | 2509.6           | 3996.1           | 286.3            | 500.2            | 1394.1           | 1120.3           | 390.6            | 614.1            | 1505.8           | 240.5            |
| **50_2**         | 572.8            | 2390.8           | 3518.2           | 329.2            | 741.6            | 1220.1           | 941.0            | 398.9            | 557.0            | 2058.6           | 247.6            |
| **50_4**         | 632.8            | 2059.8           | 3577.7           | 361.7            | 791.4            | 1356.8           | 761.7            | 365.1            | 486.0            | 3931.6           | 253.1            |
| **50_8**         | 604.8            | 1465.9           | 3512.8           | 346.4            | 751.7            | 1431.6           | 529.2            | 277.8            | 338.0            | 4710.8           | 229.9            |
| **100_0**        | 401.0            | 14383.5          | 20526.0          | 250.9            | 202.0            | 6386.2           | 5106.8           | 1376.7           | 2614.1           | 5106.8           | 562.8            |
| **100_1**        | 1212.0           | 10575.6          | 15624.3          | 699.1            | 1699.4           | 5264.6           | 4358.6           | 1481.7           | 2393.4           | 6144.6           | 612.7            |
| **100_2**        | 1453.1           | 9929.7           | 14152.5          | 828.1            | 2512.0           | 4331.4           | 3752.0           | 1465.3           | 2122.2           | 8531.6           | 645.0            |
| **100_4**        | 1651.7           | 8212.2           | 13357.5          | 926.3            | 2706.8           | 4255.7           | 2938.5           | 1318.0           | 1818.3           | 18243.9          | 662.6            |
| **100_8**        | 1672.8           | 5756.9           | 13265.8          | 930.5            | 2907.1           | 5212.9           | 2045.4           | 1003.0           | 1286.8           | 29479.6          | 631.7            |
</details>

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
![Taille des sauts en fonction du score pour les instances 100_1](graphs/100_1_greedy_jump_size_per_fitness.svg)
### K = 4
![Taille des sauts en fonction du score pour les instances 100_4](graphs/100_4_greedy_jump_size_per_fitness.svg)
### K = 8
![Taille des sauts en fonction du score pour les instances 100_8](graphs/100_8_greedy_jump_size_per_fitness.svg)
</details>

## Score obtenu après chaque saut en foncion de sa taille
> [NOTE]
> À chaque saut, on regarde le score obtenu après le saut en fonction de la taille du saut.

<details>
<summary>Graphiques pour N = 100</summary>

### K = 1
![Score en fonction de la taille des sauts pour les instances 100_1](graphs/100_1_greedy_fitness_after_jump_size.svg)
### K = 4
![Score en fonction de la taille des sauts pour les instances 100_4](graphs/100_4_greedy_fitness_after_jump_size.svg)
### K = 8
![Score en fonction de la taille des sauts pour les instances 100_8](graphs/100_8_greedy_fitness_after_jump_size.svg)
</details>

## Taille des sauts en fonction du nombre de voisins améliorants
> [NOTE]
> À chaque saut, on regarde la taille du saut en fonction du nombre de voisins améliorants avant le saut.

<details>
<summary>Graphiques pour N = 100</summary>

### K = 1
![Taille des sauts en fonction du nombre de voisins améliorants pour les instances 100_1](graphs/100_1_greedy_jump_size_per_neighbor.svg)
### K = 4
![Taille des sauts en fonction du nombre de voisins améliorants pour les instances 100_4](graphs/100_4_greedy_jump_size_per_neighbor.svg)
### K = 8
![Taille des sauts en fonction du nombre de voisins améliorants pour les instances 100_8](graphs/100_8_greedy_jump_size_per_neighbor.svg)
</details>