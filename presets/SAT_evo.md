# ====================================== problem to run and how to run it ======================================
str     problem = SAT
str     algorithm = evolution
bool    debug_screen = true
int     debug_generation_spacing = 16

# start of the debug file name (will be followed by the date)
str     label = SAT_evo

# ====================================== files containing data to run ======================================
str     instance = ./sat_specific/instances/BMS_k3_n100_m429/BMS_k3_n100_m429_0.cnf

# ====================================== evolution parameters ======================================
int     generation_count = 8096

int     population_start_size = 256
int     population_spawn_size = 1
int     population_despawn_size = 1


# nb of childs sawned per 2 parent
int     spawn_per_parent = 1
# nb of individual from wich to pick each parent
int     competition_goup_size = 2
# probability of mutating each args
float   mutation_probability = 0.02
# run a local search algorithm on childs when spawned
# none / hill_climb / tabu_search / lambda_mutation
str     run_algo_on_child = hill_climb
# budget for running the local search algorithm on childs (1 mutation checked = 1 budget)
int     child_algo_budget = 200
# lambda for running lambda_mutation or ban_list_length for running tabu_search
# int     child_algo_parameter = 10
# makes it impossible for newly spawned instances to be despawned
bool    protect_child_from_despawn = true


# removes age * multiplier from the score when searching individuals to despawn
float   despawn_criteria_age_multiplier = 0
# removes diversity * multiplier from the score when searching individuals to despawn
float   despawn_criteria_diversity_multiplier = 0

# islands not implemented yet
int     nb_islands = 1
int     generation_between_migrations = -1
float   percent_of_population_per_migrations = 0.05

# show the best of each generation in debug while running the algorithm
bool    debug_show_best = true
