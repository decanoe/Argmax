# ====================================== problem to run and how to run it ======================================
# FA / SAT
str     problem = FA
# hill_climb / tabu_search / evolution
str     algorithm = evolution
# whether or not to launch the Python viewer
bool    debug_screen = true
# if set, data will only be saved every <debug_generation_spacing> generation
int     debug_generation_spacing = 64

# start of the debug file name (will be followed by the date)
str     label = template

# ====================================== files containing data to run ======================================
str     cards = ./FA/data/cards.txt
str     sanctuaries = ./FA/data/sanctuary.txt
str     subset = ./FA/subsets/8_7_1.txt
str     instance = ./sat_specific/instances/BMS_k3_n100_m429/BMS_k3_n100_m429_0.cnf

# ====================================== hill_climb parameters ======================================
int     nb_iteration_max = 512

# ====================================== tabu_search parameters ======================================
int     nb_iteration_max = 2048
int     ban_list_size = 25
# not implemented
int     ban_list_random_added_size = 5

# ====================================== evolution parameters ======================================
int     generation_count = 1024

int     population_start_size = 256
int     population_spawn_size = 64
int     population_despawn_size = 64


# nb of childs sawned per 2 parent
int     spawn_per_parent = 1
# nb of individual from wich to pick each parent
int     competition_goup_size = 2
# probability of mutating each args
float   mutation_probability = 0.1
# run a local search algorithm on childs when spawned
# none / hill_climb / tabu_search / lambda_mutation
str     run_algo_on_child = none
# budget for running the local search algorithm on childs (1 mutation checked = 1 budget)
int     child_algo_budget = 128
# lambda for running lambda_mutation or ban_list_length for running tabu_search
int     child_algo_parameter = 8
# makes it impossible for newly spawned instances to be despawned
bool    protect_child_from_despawn = true


# removes age * multiplier from the score when searching individuals to despawn
float   despawn_criteria_age_multiplier = 0
# removes diversity * multiplier from the score when searching individuals to despawn
float   despawn_criteria_diversity_multiplier = 0

# islands not implemented yet
int     nb_islands = 1
int     generation_between_migrations = 32
float   percent_of_population_per_migrations = 0.05

# show the best of each generation in debug while running the algorithm
bool    debug_show_best = true
