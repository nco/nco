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

MPI_PRC='3' # [nbr] Number of MPI processes to spawn
PWD=`pwd`
SCRIPT=`uuidgen`
FL_PL='./nco_bm.pl' # [sng] Perl batch script name
CASEID='nco_bm03' # [sng] Case ID
XPT_DSC='NCO_benchmark_script' # [sng] Experiment description
CMD_LN="${FL_PL} --bch --dbg=0 --caseid='${CASEID}' --xpt_dsc='${XPT_DSC}' --regress --udpreport"
#CMD_LN="${FL_PL} --dbg=0 --caseid='${CASEID}' --xpt_dsc='${XPT_DSC}' --regress --udpreport ncbo"

if [ -z "${HOST}" ]; then
    if fl_is_exe /bin/hostname ; then
	export HOST=`/bin/hostname`
    elif fl_is_exe /usr/bin/hostname ; then
	export HOST=`/usr/bin/hostname`
    else
	echo "$0: ERROR Unable to determine ${HOST}, exiting..."
	exit 1 # Bail out
    fi # endif hostname exists
fi # endif ${HOST}

case "${HOST}" in 
    esmf* ) MPI_CMD=`which mpirun` ; SBM_CMD='llsubmit' ; Q_NM='com_rg8' ; ;; # endif UCI ESMF
    ipcc* ) MPI_CMD=`which mpiexec` ; SBM_CMD='qsub' ; Q_NM='regular' ; ;; # endif UCI IPCC
    mpc* ) MPI_CMD=`which mpirun` ; SBM_CMD='qsub' ; Q_NM='opteron' ; ;; # endif UCI MPC
    * ) # Default 
	echo "$0: ERROR Unable to find Torque options for ${HOST}..."
	exit 0 # Bail out if this is a non-interactive shell
	;; # endif default
esac # endcase ${HOST}

cat > ${SCRIPT} <<EOF1
# Torque (e.g., ipcc.ess.uci.edu, mpc.uci.edu)
## Job Name
#PBS -N ${CASEID}
## Export all environment variables to job
#PBS -V
## Combine stdout with stderr
#PBS -j oe
## Notify user via email at end or if aborted
#PBS -m ea
## PBS output file
#PBS -o ${CASEID}.txt
## Queue name
#PBS -q ${Q_NM}
## Number of nodes:mpi_processes_per_node
#PBS -l nodes=${MPI_PRC}:ppn=1
##PBS -l nodes=compute-0-10+compute-0-11

# Grid Engine (aka SGE) (e.g., pbs.ess.uci.edu)
## Job Name
#$ -N ${CASEID}
#$ -S /bin/bash
## /bin/sh may have broken .profile (???) that affects batch logins
## Run in current directory
#$ -cwd
## Combine stdout with stderr
#$ -j yes
## Notify user via email at end or if aborted
##$ -m ea
## SGE output file
#$ -o ${CASEID}.txt
## Parallel environment (NB: This sets NSLOTS = total # of MPI processes on all nodes)
#$ -pe mpich ${MPI_PRC}
## Set P4_GLOBMEMSIZE (p4 recv errors otherwise)
#$ -v P4_GLOBMEMSIZE=10000000
export DATA=${DATA}
export MY_BIN_DIR=${MY_BIN_DIR}

${CMD_LN} --mpi_prc=\${NSLOTS} --mpi_upx "${MPI_CMD} -np \${NSLOTS} -machinefile \${PE_HOSTFILE}"
EOF1

# Submit script to scheduler
${SBM_CMD} ${SCRIPT}
rm ${SCRIPT} # cleanup
