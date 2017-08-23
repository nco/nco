#!/bin/bash -l

# Purpose: Sample SLURM script to run ncclimo via Python

# Source: Xylar Asay-Davis, 20170815

#SBATCH --partition=debug
#SBATCH --nodes=1
#SBATCH --time=0:05:00
#SBATCH --account=acme
#SBATCH --job-name=ncclimo_tst
#SBATCH --output=ncclimo_tst%j
#SBATCH --error=ncclimo_tst%j
#SBATCH -L cscratch1,SCRATCH,project

cd $SLURM_SUBMIT_DIR   # optional, since this is the default behavior

export OMP_NUM_THREADS=1

module use /global/project/projectdirs/acme/software/modulefiles/all
module load python/anaconda-2.7-acme
export PATH=/global/homes/z/zender/bin_${NERSC_HOST}:${PATH}

srun -n 1 -N 1 python ncclimo_tst.py
