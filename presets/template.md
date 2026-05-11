# General infos for all runs of this file
<infos>

# budget of all the runs
int     budget = 1000000

# the seed to use for the start of each run (default is drivent by time)
int     seed = 0

# whether to save the runs or not
bool    save_run = true

# whether to override any runs already saved or not
bool    override = false

</infos>




# all informations for 1 instance of a problem (multiple problems can be provided in different <instance> brackets, all will be run with each algorithm)
<instance>

# the type of problem (NK / Sat / Qubo)
str     problem = NK

# the path to the instance
str     instance_path = ./instances/NK/100_4_0.nk

# the parameters of the problem (if no instance path is provided)
int     N = 100
int     4 = 8

# the path to save the run (if save_run is set to true)
# any <label> token will be replaced by the label of the algorithm
str     save_path = local_search/NK/N100/K4/<label>_0.rundata

</instance>



# all informations for 1 algorithm (multiple algorithms can be provided in different <algo> brackets, all will be run on each instance)
<algo>

# the type of algorithm (hill_climb / greedy_jumper / tabu_search / greedy_tabu_search / one_lambda_search / greedy_one_lambda_search)
str     algorithm = greedy_jumper

# whether to iterate the algorithm or not
bool    iterate = true

# the label of the algorithm (used to save runs)
str     label = greedy_<scope>_<criterion>

# <!-- #region ==================== hill_climb parameters ====================================== -->
# random / least / best / first / cycle
str     selection_criterion = first
# <!-- #endregion -->

# <!-- #region ==================== greedy parameters ====================================== -->
# random / first / best / least
str     selection_criterion = first
# all / improve / half / fixed / adaptative
str     neighborhood_scope = all
# maximum number of flips to consider in case of fixed neighborhood_scope relative to the number of bits (default .5 is the same as half)
float   max_flip_factor = .5
# <!-- #endregion -->

# <!-- #region ==================== tabu_search & greedy_tabu_search parameters ===================================== -->
# number of flips rememberd in the tabu list in proportion with the dimension
float     tabu_size = .25
# maximum number added to the base tabu_size each iteration in proportion with the dimension
float     tabu_max_random_size_added = .05
# is aspiration enabled
bool     enable_aspiration = false
# for greedy_tabu_search only, the order by which to push the flips in the tabu list (clamped means we stop adding flips if we already repaced the whole list in this iteration)
# BestToWorst / BestToWorstClamped / WorstToBest
str     tabu_push_order = BestToWorstClamped
# <!-- #endregion -->

# <!-- #region ==================== one_lambda_search parameters =============================== -->
# number of flips to consider in proportion to the dimension
float     lambda = .25
# <!-- #endregion -->

</algo>