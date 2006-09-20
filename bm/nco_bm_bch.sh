#!/bin/sh 

# Purpose: Automate benchmarks on batch systems
# 1. Create batch-submittable script to run NCO benchmarks
# 2. Submit script to batch system
# 3. Cleanup

# Source: Daniel Wang and Charlie Zender

# Usage: 
# NB: Run this command from NCO's benchmark directory (nco/bm)
# chmod a+x ~/nco/bm/nco_bm_bch.sh
# cd ~/nco/bm;./nco_bm_bch.sh

if [ -z "${DATA}" ]; then
# ${DATA} must be writable directory on cluster-shared filesystem
# Benchmarks will create a sub-directory in ${DATA}
    DATA="/data/${USER}"
fi # endif
if [ -z "${MY_BIN_DIR}" ]; then
    MY_BIN_DIR="/home/${USER}/ncotree/nco/bin"
fi # endif

MPIRUN=`which mpirun` # Steal current MPI instance path
MPI_PRC='3' # [nbr] Number of MPI processes to spawn
PWD=`pwd`
SCRIPT=`uuidgen`
FL_PL='./nco_bm.pl' # [sng] Perl batch script name
CASEID='nco_bm03' # [sng] Case ID
XPT_DSC='NCO_benchmark_script' # [sng] Experiment description
CMD_LN="${FL_PL} --bch --dbg=0 --caseid='${CASEID}' --xpt_dsc='${XPT_DSC}' --regress --udpreport"
#CMD_LN="${FL_PL} --dbg=0 --caseid='${CASEID}' --xpt_dsc='${XPT_DSC}' --regress --udpreport ncbo"

cat > ${SCRIPT} <<EOF1
# Torque (e.g., for ipcc.ess.uci.edu)
## Job Name
#PBS -N ${CASEID}
## Export all environment variables to job
#PBS -V
## Combine stdout with stderr
#PBS -j oe
## Number of nodes:queue_name:mpi_processes_per_node
#PBS -l nodes=2:regular:ppn=1
##PBS -l nodes=compute-0-10+compute-0-11
## Notify user via email at end or if aborted
#PBS -m ea
## PBS output file
#PBS -o /home/zender/cam/esmfipcc02.txt
## Queue name
#PBS -q regular

#$ -S /bin/bash
## /bin/sh may have broken .profile (???) that affects batch logins
## Parallel environment
#$ -pe mpich ${MPI_PRC}
## Job Name
#$ -N ${CASEID}
## Set P4_GLOBMEMSIZE (p4 recv errors otherwise)
#$ -v P4_GLOBMEMSIZE=10000000
## Run in current directory
#$ -cwd
## Combine stdout with stderr
#$ -j yes
## Notify user via email at end or if aborted
##$ -m ea
export DATA=${DATA}
export MY_BIN_DIR=${MY_BIN_DIR}

$CMD_LN --mpi_prc=\$NSLOTS --mpi_upx "$MPIRUN -np \$NSLOTS -machinefile \${PE_HOSTFILE}"
EOF1

qsub ${SCRIPT} # fxm: Make depend on batch system
rm ${SCRIPT} # cleanup
