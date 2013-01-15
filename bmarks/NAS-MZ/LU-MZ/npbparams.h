c NPROCS = 1 CLASS = B
c  
c  
c  This file is generated automatically by the setparams utility.
c  It sets the number of processors and the class of the NPB
c  in this directory. Do not modify it by hand.
c  
        character class
        parameter (class='B')
        integer num_procs, num_procs2
        parameter (num_procs=1, num_procs2=1)
        integer x_zones, y_zones
        parameter (x_zones=4, y_zones=4)
        integer gx_size, gy_size, gz_size
        parameter (gx_size=304, gy_size=208, gz_size=17)
        integer problem_size
        parameter (problem_size = 76)
        integer max_xysize, max_xybcsize
        integer proc_max_size, proc_max_size5, proc_max_bcsize
        parameter (max_xysize=64064)
        parameter (max_xybcsize=20000)
        parameter (proc_max_size=max_xysize*gz_size)
        parameter (proc_max_size5=proc_max_size*5)
        parameter (proc_max_bcsize=max_xybcsize*(gz_size-2))
        integer max_numzones
        parameter (max_numzones=16)

c number of iterations and how often to print the norm
        integer itmax_default, inorm_default
        parameter (itmax_default=250, inorm_default=250)
        double precision dt_default, ratio
        parameter (dt_default = 2.0d0, ratio = 1.d0)
        integer start1, start5, qstart_west, qstart_east
        integer qstart_south, qstart_north, qoffset
        integer qcomm_size, qstart2_west, qstart2_east
        integer qstart2_south, qstart2_north
        logical  convertdouble
        parameter (convertdouble = .false.)
        character compiletime*11
        parameter (compiletime='14 Jan 2013')
        character npbversion*5
        parameter (npbversion='3.3.1')
        character cs1*6
        parameter (cs1='mpif77')
        character cs2*6
        parameter (cs2='$(F77)')
        character cs3*46
        parameter (cs3='-Xlinker -rpath -Xlinker $(PATH_LIBMSR) -L$...')
        character cs4*6
        parameter (cs4='(none)')
        character cs5*24
        parameter (cs5='-O3 -openmp -fPIC -DPIC ')
        character cs6*13
        parameter (cs6='-O3  -openmp ')
        character cs7*6
        parameter (cs7='randi8')
