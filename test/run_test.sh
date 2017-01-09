#!/usr/bin/env bash
rm *.hdf5 *.log

NPARTICLES=1000000
NJOBS=10 #assumes you are running on a multi-core machine
PARTICLENAMES="geantino gamma e- proton neutron"

# use the parallel utility to parallelise across available cores
parallel --env NPARTICLES --env NJOBS --env PARTICLENAMES \
--jobs $NJOBS -q bash -c './test {1} {2} {3} {4} >> {1}.log' \
::: $PARTICLENAMES ::: $(seq 0 3) ::: $NPARTICLES ::: $(seq 1 $NJOBS)

#run the analysis in parallel
parallel --env NPARTICLES --env NJOBS --env PARTICLENAMES \
--jobs $NJOBS -q bash -c 'python ../analysis.py {1} {2} {3}' \
::: $PARTICLENAMES ::: $NPARTICLES ::: $NJOBS

#open images in the default viewer
#eog .
