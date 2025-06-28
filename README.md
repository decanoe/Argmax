# Etude d'algorithmes de recherches locales
## Sommaire
- [Introduction](#introduction).
- [Utilisation](#utilisation).
- [Résultats et interprétations](#résultats-et-interprétations).
- [À faire](#à-faire).

Autres pages :
- [Guide d'utilisation complet](docs/how_to_use.md).
- [Résultats d'évolution](docs/evolution_results.md).
- [Résultats de recherche locale](docs/local_search_results.md).

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

## À faire
Algorithmes évolutifs / mimétiques :
- [ ] implementation des îles

Comparaison des algorithmes de recherche locale :
- [x] éxecuter les algorithmes jusqu'à utilisation du budget complet
- [ ] éxecuter les algorithmes sur des instances plus grandes (entre 200 et 1000 variables)
- [ ] créer un fichier faisant la moyenne des éxecutions entre plusieurs instances