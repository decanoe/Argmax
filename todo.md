Argmax :
    - implement ilands

In order :
    - binary fitness function & solution
    - binary SAT fitness function (F_sat)
    - compare HC first and HC best on F_sat
    - implement greedy local search
        check neighbors and sort them
        apply all/part of the mutations
        remove them one by one
        pick the best score found (jump)
    - do comparisons & graphs
        fitness over budget
        nb better neighbors over budget
        size of the jump over budget
    - implement NK problem
    - compare HC first and HC best on F_NK for different K
        100,1 / 100,2 / 100,4 / 100,8
->