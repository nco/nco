#!/bin/sh

# $Header$ -*-shell-script-*-

# Purpose: Run NCO benchmark script in batch environment

# Usage:
# AIX: llsubmit ~/nco/bm/nco_bm.sh (monitor with llq, kill with llcancel)
# NCAR Cray: qsub ~/nco/bm/nco_bm.sh 
# NCAR SGI: npri -h 250 ~/nco/bm/nco_bm.sh >nco_bm.txt 2>&1 &
# UCI Linux: ~/nco/bm/nco_bm.sh > ~/nco_bm.txt 2>&1 &
# UCI SGI: qsub -q q4 -o ~/nco/bm/nco_bm.txt ~/nco/bm/nco_bm.sh
# scp ~/nco/bm/nco_bm.sh ~/nco/bm/nco_bm.pl esmf.ess.uci.edu:nco/bm

# AIX LoadLeveler batch system
# class: Queue name. "llclass" lists available queues.
# node: Number of nodes. 
# tasks_per_node: MPI processes per node, set to 1 for Hybrid OpenMP/MPI codes 
# output: Script output for STDOUT
# error: Script output for STDERR
# job_type: parallel declares that multiple nodes will be used
# network.MPI: Network connection type between nodes. Leave this be.
# node_usage: not_shared acquires dedicated access to nodes
# queue: Tells Loadleveler to submit job

#@ job_name       = nco_bm03
##@ class          = com_rg1
##@ class          = com_rg4
#@ class          = com_rg8
##@ class          = com_rg32
##@ class          = com_node03
#@ node           = 1
#@ tasks_per_node = 3
#@ output         = $(job_name).txt
#@ error          = $(job_name).txt
#@ job_type       = parallel
#@ network.MPI    = csss,shared,us
#@ node_usage     = not_shared
##@ account_no     = 36271012
##@ wall_clock_limit = 3800
#@ queue

## Job Name
#PBS -N esmfipcc00
## Exports all environment variables to the job
#PBS -V
## Combine stdout with stderr
#PBS -j oe
## Number of nodes:queue_name:mpi_processes_per_node
#PBS -l nodes=2:regular:ppn=1
##PBS -l nodes=compute-0-6+compute-0-7
## Maximum wallclock time in HHH:MM:SS
##PBS -l walltime=150:00:00
## notify user via email at beginning and end
#PBS -m ea
## PBS output file
#PBS -o /home/zender/cam/esmfipcc00.txt
## Queue name
#PBS -q regular

## Shell
#$ -S /bin/bash
#$ -pe mpi 2

# Main code
set timestamp
set echo
date
unalias -a # Turn off aliases

# OS-generic
export NTASKS=2 # MPI
PVM_ARCH=`~zender/sh/pvmgetarch`
HOST=`hostname`
# OS-specific
case "${PVM_ARCH}" in 
    AIX* ) 
	export DATA=/ptmp/${USER}
	export MY_BIN_DIR=/u/${USER}/bin/AIX
	case "${HOST}" in 
	    esmf* ) # UCI
		case "${HOST}" in 
		    esmf0[1-7]* ) # UCI
		    export NTHREADS=8 # OpenMP
		    ;; # endif UCI
		    esmf0[8]* ) # UCI
		    export NTHREADS=32 # OpenMP
		    ;; # endif UCI
		esac # endcase ${HOST}
		;; # endif UCI
	    b[bfs]*en ) # babyblue, blackforest, bluesky
	    export NTHREADS=8 # OpenMP
	    ;; # endif NCAR
	esac # endcase ${HOST}
	;; # endif AIX*
    LINUX* ) 
	export DATA=/data/${USER}
	export MY_BIN_DIR=/home/${USER}/bin/${PVM_ARCH}
	export NTHREADS=2
# Attempt to get sufficient stack memory
	ulimit -s unlimited
    ;; # endif LIN*
    SGI* )
	export DATA=/data/${USER}
	export MY_BIN_DIR=/home/${USER}/bin/SGIMP64
	export NTHREADS=4
    ;; # endif SGI
    * )
	echo "ERROR: $PVM_ARCH is unsupported operating system"
	exit 1
    ;; # endif default
esac # endcase ${PVM_ARCH}

# Job-specific
PRG_NM='nco_bm' # [sng] Program name, semantic identifier
FL_NM_SH='nco_bm.sh' # [sng] Shell batch script name
FL_NM_PL='nco_bm.pl' # [sng] Perl batch script name
CASEID='nco_bm03' # [sng] Case ID
# NB: fxm: Whitespace in this string breaks nco_bm.pl because Getopt does not ingest it properly
XPT_DSC='NCO_benchmark_script' # [sng] Experiment description
# Derive data paths
export DATA_NCO=${DATA}/nco_test
export DATA_OUT=${DATA}/${PRG_NM}/${CASEID}
# Run program
if [ ! -d ${DATA_OUT} ]; then
    mkdir -p ${DATA_OUT}
fi # endif
#cd ${DATA_OUT}
export LID="`date +%Y%m%d-%H%M%S`"
#EXE=${DATA_OUT}/${PRG_NM}
FL_PL=${DATA_OUT}/${FL_NM_PL}
#/bin/cp -f -p ~/bin/${PVM_ARCH}/${PRG_NM} ${DATA_OUT} || exit 1
/bin/rm -f ${DATA_OUT}/${FL_NM_PL} || exit 1
/bin/rm -f ${DATA_OUT}/${FL_NM_SH} || exit 1
/bin/cp -f -p ~/nco/bm/${FL_NM_PL} ${DATA_OUT} || exit 1
/bin/cp -f -p ~/nco/bm/${FL_NM_SH} ${DATA_OUT} || exit 1
# Copy from Production lines in nco_bm.pl:
#CMD_LN="${FL_PL} --bch --dbg=0 --thr_nbr=${NTHREADS} --caseid='${CASEID}' --xpt_dsc='${XPT_DSC}' --regress --udpreport"
CMD_LN="${FL_PL} --bch --dbg=0 --mpi_prc=${NTASKS} --caseid='${CASEID}' --xpt_dsc='${XPT_DSC}' --regress --udpreport"
#CMD_LN="${FL_PL} --bch --dbg=0 --thr_nbr=${NTHREADS} --caseid='${CASEID}' --xpt_dsc='${XPT_DSC}' --benchmark --udpreport"
#CMD_LN="${FL_PL} --bch --dbg=0 --mpi_prc=${NTASKS} --caseid='${CASEID}' --xpt_dsc='${XPT_DSC}' --benchmark --udpreport"
FL_STDOUT="${PRG_NM}.log.${LID}"

echo "Timestamp ${LID}"
echo "Batch shell script $0 running CASEID = ${CASEID} on machine ${HOST}"
echo "Invoking executable with ${CMD_LN}"
echo "STDOUT/STDERR re-directed to file:"
echo "/bin/more \${HOME}/nco/bm/${FL_STDOUT}"
case "${PVM_ARCH}" in 
    AIX* ) 
# Set POE environment for interactive jobs  
# LoadLeveler batch jobs ignore these settings
# MP_NODES is node number
# XLSMPOPTS thread stack size
	export MP_EUILIB='us'
	export MP_NODES='1'
	export MP_TASKS_PER_NODE=${NTASKS}
	export MP_RMPOOL='1'
	export XLSMPOPTS='stack=86000000'
	echo ${HOST} > ./host.list
	if [ ${NTASKS} -gt 1 ]; then
	    poe -hostfile=NULL -node=1 -tasks_per_node=${NTASKS} ${CMD_LN} > ${FL_STDOUT} 2>&1
	else
	    env MY_BIN_DIR="${MY_BIN_DIR}" OMP_NUM_THREADS="${NTHREADS}" PATH="/usr/local/bin\:${DATA_OUT}\:${PATH}" ${CMD_LN} > ${FL_STDOUT} 2>&1
	fi # end else OpenMP
	;; # endif AIX*
    LINUX* ) 
	if [ ${NTASKS} -gt 1 ]; then
	    mpirun -np ${NTASKS} ${CMD_LN} > ${FL_STDOUT} 2>&1
	else     
	    env OMP_NUM_THREADS="${NTHREADS}" MPSTKZ="128M" ${CMD_LN} > ${FL_STDOUT} 2>&1
	fi # end else OpenMP
	;; # endif LIN*
    SGI* )
	export TRAP_FPE='UNDERFL=FLUSH_ZERO; OVERFL=ABORT,TRACE; DIVZERO=ABORT,TRACE; INVALID=ABORT,TRACE'
	export OMP_DYNAMIC='FALSE'
	export _DSM_PLACEMENT='ROUND_ROBIN'
	export _DSM_WAIT='SPIN'
	export MPC_GANG='OFF'
# MP_SLAVE_STACKSIZE sets size of thread stack
	export MP_SLAVE_STACKSIZE='40000000'
# Run pure SPMD or pure OpenMP 
	if [ ${NTASKS} -gt 1 ]; then
	    mpirun -np ${NTASKS} ${CMD_LN} > ${FL_STDOUT} 2>&1
	else
	    env MP_SET_NUMTHREADS="${NTHREADS}" ${CMD_LN} > ${FL_STDOUT} 2>&1
	fi # end else OpenMP
	;; # endif SGI
    * )
	echo "ERROR: ${PVM_ARCH} is unsupported operating system"
	exit 1
	;; # endif default
esac # endcase ${PVM_ARCH}

exit_status=$?
if [ "${exit_status}" ]; then
# Successful exit branch
    echo "SUCCESS execution of ${FL_NM_PL} completed successfully"
    date;pwd;ls -la
    echo "$0 batch script completed successfully at `date +%Y%m%d-%H%M%S`"
    exit 0
else
# Error exit branch
    echo "ERROR execution of ${FL_NM_PL} failed"
    date;pwd;ls -la
    echo "$0 batch script error exit at `date +%Y%m%d-%H%M%S`"
    exit 1
fi # endif err

