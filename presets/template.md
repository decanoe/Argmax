# <!-- #region ==================== problem to run and how to run it =========================== -->
# (optional) Nb of variables for the NK problem
int     N = 100
# (optional) Nb variables linked to the first variable of every function of the NK problem
int     K = 1
# hill_climb / greedy_jumper / tabu_search / greedy_tabu_search / one_lambda_search
str     algorithm = hill_climb

# <!-- #endregion -->
# <!-- #region ==================== files containing data to run =============================== -->
# NK (optional if parameter were provided)
str     instance = ./NK/instances/100_1.nk
# <!-- #endregion -->
# <!-- #region ==================== debug file ================================================= -->
# whether or not to launch the Python viewer
bool    debug_screen = true
# start of the debug file name (will be followed by the date)
str     label = template_<timestamp>.rundata
# run algorithm even if output file already exists (default false)
bool    override = true

# <!-- #endregion -->


# <!-- #region ==================== hill_climb parameters ====================================== -->
int     budget = 1024
# random / least / best / first / cycle
str     selection_criterion = first

# <!-- #endregion -->
# <!-- #region ==================== greedy parameters ====================================== -->
int     budget = 1024
# random / first / best / least
str     selection_criterion = first
# all / improve / half / fixed / tabu
str     neighborhood_scope = all
# maximum number of flips to consider in case of fixed neighborhood_scope relative to the number of bits (default .5 is the same as half)
float   max_flip_factor = .5

# <!-- #endregion -->
# <!-- #region ==================== tabu_search & greedy_tabu_search parameters ===================================== -->
# number of flips rememberd in the tabu list in proportion with the dimension
float     tabu_size = .25
# maximum number added to the base tabu_size each iteration in proportion with the dimension
float     tabu_max_random_size_added = .05
# for greedy_tabu_search only, the order by which to push the flips in the tabu list (clamped means we stop adding flips if we already repaced the whole list in this iteration)
# BestToWorst / BestToWorstClamped / WorstToBest
str     tabu_push_order = BestToWorstClamped

# <!-- #endregion -->
# <!-- #region ==================== one_lambda_search parameters =============================== -->
int     nb_mutation_to_test = 16
int     nb_iteration_max = 512

# <!-- #endregion -->