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
> La moyenne est calculée sur 10 instances différentes par paire de N, K avec 2 executions par instances
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | GJ_full_best     | GJ_full_first    | GJ_full_least    | GJ_improve_best  | GJ_improve_first | GJ_improve_least | HC_best          | HC_least         | HC_first         |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> |
| **50_1**         | 0.6896           | 0.6969           | <ins>0.6981</ins> | 0.6871           | 0.6897           | 0.6877           | 0.6866           | 0.6704           | 0.6777           |
| **50_2**         | 0.7131           | 0.7345           | <ins>0.7374</ins> | 0.7114           | 0.7271           | 0.7175           | 0.7153           | 0.6982           | 0.7092           |
| **50_4**         | 0.7251           | <ins>0.7439</ins> | <ins>0.7439</ins> | 0.7188           | 0.7348           | 0.7391           | 0.7172           | 0.7103           | 0.7059           |
| **50_8**         | 0.7114           | 0.7099           | <ins>0.7207</ins> | 0.7066           | 0.7205           | 0.7168           | 0.6898           | 0.7008           | 0.7024           |
| **100_0**        | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> |
| **100_1**        | 0.689            | 0.6978           | <ins>0.6982</ins> | 0.6885           | 0.6906           | 0.686            | 0.6857           | 0.6767           | 0.6818           |
| **100_2**        | 0.7216           | 0.7403           | <ins>0.7446</ins> | 0.7195           | 0.7325           | 0.7261           | 0.7199           | 0.7129           | 0.7144           |
| **100_4**        | 0.7293           | 0.7617           | <ins>0.7644</ins> | 0.7264           | 0.7536           | 0.749            | 0.7209           | 0.7212           | 0.7236           |
| **100_8**        | 0.7116           | <ins>0.7407</ins> | 0.7377           | 0.7075           | 0.7321           | 0.7385           | 0.7071           | 0.7121           | 0.7075           |
</details>

## Meilleur score obtenu pour des budgets fixes
> [NOTE]
> Les meilleurs scores sont calculés en faisant la moyenne du score maximum atteint sur 10 instances différentes

### Meilleur score obtenu pour un budget de 1 000 tests
Sur un budget très limité, les algorithmes greedy_improve_best et greedy_improve_first atteignent des scores plus élevés sur la plus part des instances. Mais sur les instances les plus dures (K = 8), le Hill Climb Random a de meilleurs (car plus d'itérations).
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | GJ_full_best     | GJ_full_first    | GJ_full_least    | GJ_improve_best  | GJ_improve_first | GJ_improve_least | HC_best          | HC_least         | HC_first         |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         | <ins>0.6573</ins> | 0.5979           | 0.5508           | <ins>0.6573</ins> | <ins>0.6573</ins> | 0.6146           | 0.6407           | 0.5620           | <ins>0.6573</ins> |
| **50_1**         | 0.6929           | 0.6046           | 0.5710           | <ins>0.6943</ins> | 0.6915           | 0.6626           | 0.6794           | 0.5649           | 0.6901           |
| **50_2**         | 0.7211           | 0.6532           | 0.5939           | 0.7210           | <ins>0.7261</ins> | 0.7088           | 0.7183           | 0.5804           | 0.7220           |
| **50_4**         | 0.7300           | 0.6389           | 0.5524           | 0.7328           | <ins>0.7381</ins> | 0.7067           | 0.7170           | 0.5241           | 0.7327           |
| **50_8**         | 0.7194           | 0.6506           | 0.5270           | 0.7177           | 0.7153           | 0.6406           | 0.7021           | 0.5204           | <ins>0.7298</ins> |
| **100_0**        | <ins>0.6643</ins> | 0.5448           | 0.5137           | <ins>0.6643</ins> | <ins>0.6643</ins> | 0.5400           | 0.5589           | 0.4961           | <ins>0.6643</ins> |
| **100_1**        | 0.6865           | 0.5282           | 0.5130           | <ins>0.6881</ins> | 0.6638           | 0.5713           | 0.5886           | 0.5026           | 0.6845           |
| **100_2**        | 0.7208           | 0.5434           | 0.5243           | <ins>0.7277</ins> | 0.6914           | 0.5849           | 0.6092           | 0.5050           | 0.7169           |
| **100_4**        | 0.7031           | 0.5657           | 0.5178           | <ins>0.7258</ins> | 0.6458           | 0.5971           | 0.6312           | 0.5085           | 0.7248           |
| **100_8**        | 0.6846           | 0.5586           | 0.5089           | 0.7018           | 0.5941           | 0.5363           | 0.6516           | 0.5012           | <ins>0.7083</ins> |
</details>

### Meilleur score obtenu pour un budget de 10 000 tests
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | GJ_full_best     | GJ_full_first    | GJ_full_least    | GJ_improve_best  | GJ_improve_first | GJ_improve_least | HC_best          | HC_least         | HC_first         |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> |
| **50_1**         | 0.6988           | 0.6980           | <ins>0.6995</ins> | 0.6994           | 0.6989           | 0.6954           | 0.6972           | 0.6867           | 0.6979           |
| **50_2**         | 0.7422           | <ins>0.7442</ins> | 0.7424           | 0.7402           | 0.7441           | 0.7349           | 0.7358           | 0.7210           | 0.7390           |
| **50_4**         | 0.7558           | <ins>0.7621</ins> | 0.7596           | 0.7538           | 0.7618           | 0.7611           | 0.7456           | 0.7212           | 0.7536           |
| **50_8**         | 0.7532           | 0.7464           | 0.7347           | 0.7523           | 0.7500           | 0.7476           | 0.7449           | 0.7026           | <ins>0.7560</ins> |
| **100_0**        | <ins>0.6643</ins> | 0.6558           | 0.6268           | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> |
| **100_1**        | 0.6971           | 0.6866           | 0.6515           | 0.6971           | <ins>0.6972</ins> | 0.6877           | 0.6908           | 0.6763           | 0.6929           |
| **100_2**        | 0.7379           | 0.7272           | 0.6947           | <ins>0.7398</ins> | 0.7394           | 0.7308           | 0.7297           | 0.7109           | 0.7371           |
| **100_4**        | 0.7458           | 0.7642           | 0.7014           | 0.7492           | <ins>0.7656</ins> | 0.7574           | 0.7368           | 0.5899           | 0.7528           |
| **100_8**        | 0.7348           | 0.7346           | 0.6677           | 0.7328           | 0.7405           | <ins>0.7441</ins> | 0.7207           | 0.5444           | 0.7413           |
</details>

### Meilleur score obtenu pour un budget de 100 000 tests
Avec beaucoups de budget, les algorithmes Greedy sont toujours plus performants si la taille des instances n'est pas trop petite.
<details>
<summary>Voir les valeurs</summary>

| instance (N_K)   | GJ_full_best     | GJ_full_first    | GJ_full_least    | GJ_improve_best  | GJ_improve_first | GJ_improve_least | HC_best          | HC_least         | HC_first         |
| ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- | ---------------- |
| **50_0**         | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> | <ins>0.6573</ins> |
| **50_1**         | <ins>0.6997</ins> | <ins>0.6997</ins> | 0.6996           | <ins>0.6997</ins> | <ins>0.6997</ins> | <ins>0.6997</ins> | 0.6994           | 0.6963           | <ins>0.6997</ins> |
| **50_2**         | <ins>0.7466</ins> | 0.7458           | 0.7456           | 0.7464           | <ins>0.7466</ins> | 0.7462           | 0.7451           | 0.7358           | 0.7462           |
| **50_4**         | 0.7670           | 0.7719           | <ins>0.7739</ins> | 0.7665           | 0.7732           | 0.7709           | 0.7644           | 0.7488           | 0.7689           |
| **50_8**         | 0.7670           | 0.7683           | 0.7675           | 0.7682           | 0.7777           | 0.7684           | 0.7636           | 0.7432           | <ins>0.7782</ins> |
| **100_0**        | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> | <ins>0.6643</ins> |
| **100_1**        | 0.6992           | 0.6993           | 0.6990           | <ins>0.6996</ins> | 0.6994           | 0.6973           | 0.6974           | 0.6896           | 0.6971           |
| **100_2**        | 0.7447           | 0.7484           | <ins>0.7492</ins> | 0.7439           | 0.7477           | 0.7445           | 0.7394           | 0.7303           | 0.7424           |
| **100_4**        | 0.7614           | 0.7784           | 0.7774           | 0.7663           | <ins>0.7793</ins> | 0.7740           | 0.7538           | 0.7431           | 0.7651           |
| **100_8**        | 0.7485           | 0.7612           | 0.7591           | 0.7490           | <ins>0.7665</ins> | 0.7598           | 0.7459           | 0.7208           | 0.7518           |
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
| **50_1**         | 476.0            | 2739.3           | 4101.0           | 286.1            | 541.1            | 1428.3           | 1158.5           | 1548.5           | <ins>238.8</ins> |
| **50_2**         | 546.0            | 2237.3           | 3611.0           | 313.9            | 737.2            | 1072.5           | 891.0            | 1901.0           | <ins>235.7</ins> |
| **50_4**         | 696.0            | 2096.8           | 3626.0           | 388.0            | 858.2            | 1291.2           | 798.5            | 3898.5           | <ins>253.6</ins> |
| **50_8**         | 636.0            | 1483.2           | 4036.0           | 349.6            | 777.0            | 1476.2           | 496.0            | 5286.0           | <ins>197.3</ins> |
| **100_0**        | 401.0            | 14478.4          | 20501.0          | 250.4            | <ins>202.0</ins> | 6296.2           | 5041.0           | 5041.0           | 581.9            |
| **100_1**        | 1181.0           | 11224.5          | 16351.0          | 702.2            | 1796.0           | 5136.4           | 4251.0           | 6111.0           | <ins>651.6</ins> |
| **100_2**        | 1501.0           | 10126.8          | 13961.0          | 810.3            | 2283.8           | 4411.2           | 3851.0           | 8631.0           | <ins>631.3</ins> |
| **100_4**        | 1651.0           | 8174.1           | 13441.0          | 934.6            | 2604.9           | 4120.3           | 3046.0           | 18596.0          | <ins>695.4</ins> |
| **100_8**        | 1641.0           | 6069.9           | 12681.0          | 929.8            | 3021.3           | 5519.9           | 2036.0           | 29381.0          | <ins>638.2</ins> |
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