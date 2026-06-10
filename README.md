# Recherches locales gloutonnes pour la résolution de problèmes d’optimisation combinatoire
## Sommaire
- [Introduction](#introduction)
- [Greedy Jumper](#greedy-jumper)
- [Utilisation](#utilisation)
- [Résultats et interprétations](#résultats-et-interprétations)
- [Derniers ajouts](#derniers-ajouts)

## Introduction
Ce dépôt corresponds à deux stages de recherche effectués à l'Université d'Angers en 3ème année de Double Licence Mathématiques-Informatique et en 1ère année de Master Informatique.
Ces stages portent sur l'étude de méthodes heuristiques d'optimisation combinatoire appliquées à des problèmes pseudo-booléens,

Leur objectif est d'explorer une alternative aux approches classiques en étudiant une méthode de recherche locale basée sur des mouvements agrégés de plusieurs variables, appelée *Greedy Jumper*. Cette méthode a été conçue dans le cadre du stage avec l'encadrant et constitue le cœur du travail réalisé.

## Greedy Jumper
L'idée est d'étendre le voisinage classique (basé sur des flips unitaires) vers des mouvements agrégés, tout en conservant la logique de sélection gloutonne propre à la recherche locale.

Chaque itération du Greedy Jumper se déroule comme suit :
Dans un premier temps, les variables sont triées en fonction de la variation de fitness induite par un flip individuel. On considère ensuite N sauts s<sub>i</sub> formés chacun des i variables ayant le meilleur impact local. C'est parmi ces N sauts appliqués à la solution actuelle que le Greedy Jumper choisira un voisin améliorant pour continuer sa recherche.

## Utilisation
> [!NOTE]
> Un guide complet est disponible [ici](docs/how_to_use.md)

> [!WARNING]
> Le projet complet a été conçu sous Linux et peut ne pas marcher correctement sous d'autres systèmes.

Pour compiler le projet, se placer à la racine du répertoire, puis exécuter les commandes suivantes :
```
mkdir build
cd build
cmake ..
make
cd ..
```

Pour exécuter, utiliser la commande :
```
./build/Argmax <file> <options>
```
ou
```
./build/Argmax <options>
```

Dû au nombre élevé de paramètres, chaque algorithme est exécuté à l'aide d'un fichier contenant l'ensemble des informations.

## Résultats et interprétations
L'ensemble des résultats sont répertoriés [ici](docs/local_search_results.md).

## Derniers ajouts
- réécriture des tableaux de budgets moyens ([ici](docs/local_search_results.md))
- ajouts des tableaux en latex ([ici](docs/tables))
- réécriture des tableaux de scores moyens et maximal par budget ([ici](docs/local_search_results.md))
- normalisation des graphes (et augmentation de la taille de la légende et des axes)
- ajout d'un nouveau type de graphe pour avoir une visualisation plus claire de la corrélation entre certaines variables (pas encore satisfaisant mais mieux qu'avant)
- ajout de graphe *fitness_delta* per *jump_size* ([ici](docs/graphs))
- ajout de graphe *neighbor_count_delta* per *neighbor_count* ([ici](docs/graphs))
- ajout d'un fichier avec tout les graphs ([ici](docs/graphs/all_graphs.pdf))
- implémentation des algorithmes Tabu, GreedyTabu, GreedyFixed (Top-k), GreedyRandom, one-lambda et greedy one-lambda
- réécriture des tableaux ([ici](docs/tables))