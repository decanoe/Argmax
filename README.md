# Etude d'algorithmes de recherches locales
## Sommaire
- [Introduction](#introduction)
- [Utilisation](#utilisation)
- [Résultats et interprétations](#résultats-et-interprétations)
- [Derniers ajouts](#derniers-ajouts)

Autres pages :
- [Guide d'utilisation complet](docs/how_to_use.md)
- [Résultats d'évolution](docs/evolution_results.md)
- [Résultats de recherche locale](docs/local_search_results.md)

## Introduction
L'objectif de ce dépot est de tester différents algorithmes de recherche locales sur différents problèmes (principalement des problèmes d'optimisation sur des chaines de variables booléenes).

Les différents problèmes implémentés sont :
- SAT
- NK
- *Far Away (peu représentatif)*

Parmis les différents algorithmes il y a :
- Algorithme évolutif
- Algorithme mimétique
- Hill Climb
- Recherche tabou
- Recherche glouton
- Greedy Jumper

Afin d'étudier les résultats, deux interfaces sont disponibles. Une première pour étudier les executions des algorithmes évolutifs et mimétiques. Une autre pour comparer les perfomances des algorithmes moins complexes.

## Utilisation
> [!NOTE]
> Un guide complet est disponible [ici](docs/how_to_use.md)

Dû au nombre élévé de paramètres pour chaque éxecution, une grande partie des algorithmes sont éxécutés à l'aide d'un fichier contenant toute les informations.

> [!WARNING]
> Le projet complet a été conçu sous Windows 10 et peut ne pas marcher correctement sous d'autres systèmes.

## Résultats et interprétations
L'ensemble des résultats sont répertoriés [ici](docs/local_search_results.md).
> [!NOTE]
> Ces résultats concernent principalement les algorithmes de recherche locales simples. Pour des résultats sur les algorithmes évolutifs et mimétiques, voir [ici](docs/evolution_results.md).

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