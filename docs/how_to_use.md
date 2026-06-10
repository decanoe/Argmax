# Comment utiliser le projet
## Sommaire
- [Compilation](#compilation)
- [Exécution](#exécution)
- [Utilisation](#utilisation)

## Compilation
Pour compiler le projet, se placer à la racine du répertoire, puis exécuter les commandes suivantes :
```
mkdir build
cd build
cmake ..
make
cd ..
```

## Exécution
Pour exécuter, utiliser la commande :
```
./build/Argmax <file> <options>
```
pour exécuter des algorithmes ou la commande
```
./build/Argmax -v
```
pour lancer la visualisation des résultats.

Dû au nombre élevé de paramètres, chaque algorithme est exécuté à l'aide d'un fichier contenant l'ensemble des informations.
Le paramètre *-t* peut être utilisé pour utiliser plusieurs threads lors de l'exécution.

## Fichier de paramètres
Chaque fichier de paramètres est divisé en 3 type de séctions

### Infos
La section **infos** (une par fichier) donne les informations générales sur la façon d'exécuter les algorithmes.
Elle se présente comme suit :
```
<infos>
# budget des runs (en nombre d'exécutions)
int     budget = 1000000

# la seed à utiliser pour la génération des valeurs aléatoires (si la valeur n'est pas renseignée, elle sera donnée par le temps)
int     seed = 0

# booléen indiquant si les résultats doivent être sauvegardés dans un fichier de run ou s'ils doivent être affichés
bool    save_run = true

# booléen indiquant (en cas de sauvegarde de fichier) si tout fichier déjà existant doit être écrasé
bool    override = false

# le dossier dans lequel sauvegarder les données de runs
str     save_dir = local_search/b1_000_000
</infos>
```
### Instance
Chaque section **instance** (au moins une par fichier) donne les informations sur 1 problème sur lequel faire tourner les algorithmes.
Plusieurs instances de problèmes peuvent être renseignés dans chaque fichier.
Elle se présente comme suit :
```
<instance>
# le type de problème (NK / Sat / Qubo)
str     problem = NK

# le chemin vers l'instance
str     instance_path = ./instances/NK/100_4_0.nk

# les paramètres pour la génération de l'instance (si aucun chemin n'a été donné)
int     N = 100
int     4 = 8

# le chemin vers le fichier dans lequel sauvegarder les données de runs (si save_run est mit à true)
# ce chemin est relatif au save_dir indique dans <infos>
# tout token <label> sera remplacé par le label de l'algorithme
str     save_path = local_search/NK/N100/K4/<label>_0.rundata
</instance>
```
### Algo
Chaque section **algo** (au moins une par fichier) donne les informations sur 1 algorithme qui doit tourner sur chaque instance.
Plusieurs algorithmes peuvent être renseignés dans chaque fichier.
Elle se présente comme suit :
```
<algo>
# le type d'algorithme (hill_climb / greedy_jumper / tabu_search / greedy_tabu_search / one_lambda_search / greedy_one_lambda_search)
str     algorithm = greedy_jumper

# booléen indiquant si l'algorithme doit être itéré
bool    iterate = true

# le label de l'algorithme (utilisé pour enregistrer les runs)
str     label = greedy_<scope>_<criterion>
</algo>
```
Pour les paramètres relatifs à chaque type d'algorithme, se référer au [template](../presets/template.md)