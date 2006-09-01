#!/bin/sh 

# Purpose: Automate benchmarks on batch systems
# 1. Create batch-submittable script to run NCO benchmarks
# 2. Submit script to batch system
# 3. Cleanup

# Usage: 
# NB: Run this command from NCO's benchmark directory (nco/bm)
# chmod a+x ~/nco/bm/nco_bm_bch.sh
# cd ~/nco/bm;./nco_bm_bch.sh

if [ -n "${DATA}" ]; then
# ${DATA} must be writable directory on cluster-shared filesystem
# Benchmarks will create a sub-directory in ${DATA}
    DATA="/data/${USER}"
fi # endif
if [ -n "${MY_BIN_DIR}" ]; then
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
#$ -S /bin/bash
# /bin/sh may have broken .profile (???) that affects batch logins
#$ -pe mpich ${MPI_PRC}
#$ -N ${CASEID}
# Set P4_GLOBMEMSIZE (p4 recv errors otherwise)
#$ -v P4_GLOBMEMSIZE=10000000
# Run in current directory
#$ -cwd
# cd $PWD # not needed since we have -cwd.
export DATA=${DATA}
export MY_BIN_DIR=${MY_BIN_DIR}

$CMD_LN --mpi_prc=\$NSLOTS --mpi_upx "$MPIRUN -np \$NSLOTS -machinefile \$TMP/machines"
EOF1

qsub ${SCRIPT} # fxm: Make depend on batch system
rm ${SCRIPT} # cleanup
