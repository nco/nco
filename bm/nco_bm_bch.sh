#!/bin/sh 

# Usage: 
# NB: This command should be run from the NCO benchmark directory
# chmod a+x ~/nco/bm/nco_bm_bch.sh
# cd ~/nco/bm;./nco_bm_bch.sh

if [ -n "${DATA}" ]; then
# DATA should be a writable directory on the cluster shared filesystem (makes a subdir from here)
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
# /bin/sh seems to have a broken .profile (???) that affects batch logins
#$ -pe mpich ${MPI_PRC}
#$ -N ${CASEID}
# set the P4_GLOBMEMSIZE (p4 recv errors otherwise)
#$ -v P4_GLOBMEMSIZE=10000000
# run in the current directory
#$ -cwd
# cd $PWD # not needed since we have -cwd.
export DATA=$DATA
export MY_BIN_DIR=$MY_BIN_DIR

$CMD_LN --mpi_prc=\$NSLOTS --mpi_upx "$MPIRUN -np \$NSLOTS -machinefile \$TMP/machines"
EOF1

qsub $SCRIPT
rm $SCRIPT
