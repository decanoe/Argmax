# Comment utiliser le projet
## Execution
Toute les exécutions se font depuis la racine du projet avec différentes options.
- Pour une éxecution normale, mettre simplement le chemin du fichier de paramètres.
- Pour lancer la visualisation des données, utiliser -v (pour la recherche locale) ou -v_evo (pour les algorithmes évolutifs) puis sélectioner les fichiers dans le selecteur python.
- Pour créer une instance NK, utiliser -nk suivi du N, du K et de l'emplacement du fichier de sorti.
## Fichiers d'éxecution
De nombreux paramètres peuvent être indiqué dans un fichier afin de décrire l'execution du programme. Un fichier template est disponible dans le dossier `presets`. Les lignes commençant par un `#` seront ignorées par le code.
## Fichiers de sortie
Si le fichier de paramètre indique de lancer l'écran de visualisation, les données seront sauvegardées dans le dossier `python/data`.
Un label peut être mit dans les paramètres afin d'indiquer un nom pour le fichier ou un chemin dans `python/data`.