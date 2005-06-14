#!/bin/sh

# $Header: /data/zender/nco_20150216/nco/bm/nco_bm.sh,v 1.5 2005-06-14 00:24:03 zender Exp $ -*-shell-script-*-

# Purpose: Run NCO benchmark script in batch environment

# Usage:
# NCAR AIX: llsubmit nco_bm.sh (monitor with llq, kill with llcancel)
# NCAR Cray: qsub nco_bm.sh 
# NCAR SGI: npri -h 250 nco_bm.sh >nco_bm.txt 2>&1 &
# UCI Linux: nco_bm.sh >nco_bm.txt 2>&1 &
# UCI SGI: qsub -q q4 -o ~/nco/bm/nco_bm.txt nco_bm.sh
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

#@ job_name       = nco_bm01
##@ class          = com_rg1
##@ class          = com_rg4
#@ class          = com_rg8
##@ class          = com_rg32
##@ class          = com_node03
#@ node           = 1
#@ tasks_per_node = 1
#@ output         = $(job_name).txt
#@ error          = $(job_name).txt
#@ job_type       = parallel
#@ network.MPI    = csss,shared,us
#@ node_usage     = not_shared
##@ account_no     = 36271012
##@ wall_clock_limit = 3800
#@ queue

# NQS tokens
# -A nbr        Project number (CMS=03010063, AMWG=93300075, ESMF=36271012)
# -J m          Append NQS output to stdout
# -eo           Glue together stderr and stdout
# -l mpp_p=nbr  Maximum # of CPUs = OMP_NUM_THREADS+4
# -lm sz -lm sz Maximum memory footprint
# -lt tm -lT tm Maximum run time in queue
# -me           Send mail when job ends
# -o fl_out     Output file
# -q queue_name Queue name
# -s shell      Shell to use (default is /bin/sh)

# UCI and NCAR both
#QSUB -me
#QSUB -J m

# UCI only
#QSUB -q q4
##QSUB -q genq1_4
#QSUB -l mpp_p=8
#QSUB -eo -o /home/ess/zender/zender/nco/bm/nco_bm.txt

# NCAR only
##QSUB -q ded_4
##QSUB -eo -o /fs/cgd/home0/zender/nco/bm/nco_bm.txt
##QSUB -lt 420000 -lT 420000
##QSUB -lm 650Mb -lM 650Mb

# Main code
set timestamp
set echo
date
unalias -a # Turn off aliases

# OS-generic
export NTASKS=1 # MPI
PVM_ARCH=`~zender/bin/sh/pvmgetarch`
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
	export MY_BIN_DIR=/home/${USER}/bin/LINUX
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
CASEID='nco_bm01' # [sng] Case ID
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
#CMD_LN="${FL_PL} --bch --dbg=0 --thr_nbr=${NTHREADS} --xpt_dsc='${XPT_DSC}' --regress --udpreport"
#CMD_LN="${FL_PL} --bch --dbg=0 --thr_nbr=${NTHREADS} --xpt_dsc='${XPT_DSC}' --benchmark --udpreport"
CMD_LN="${FL_PL} --bch --dbg=0 --thr_nbr=${NTHREADS} --xpt_dsc='${XPT_DSC}' --benchmark --regress --udpreport"
FL_STDOUT="${PRG_NM}.log.${LID}"

echo "Timestamp ${LID}"
echo "Batch shell script $0 running CASEID = ${CASEID} on machine ${HOST}"
echo "Invoking executable with ${CMD_LN}"
echo "STDOUT/STDERR re-directed to file:"
echo "/bin/more ${DATA_OUT}/${FL_STDOUT}"
case "${PVM_ARCH}" in 
    AIX* ) 
# Set POE environment for interactive jobs  
# LoadLeveler batch jobs ignore these settings
# MP_NODES is node number
# XLSMPOPTS thread stack size
	export MP_EUILIB='us'
	export MP_NODES="${NTASKS}"
	export MP_TASKS_PER_NODE='1'
	export MP_RMPOOL='1'
	export XLSMPOPTS='stack=86000000'
	if [ ${NTASKS} -gt 1 ]; then
	    poe ${CMD_LN} > ${FL_STDOUT} 2>&1
	else
	    env MY_BIN_DIR="${MY_BIN_DIR}" OMP_NUM_THREADS="${NTHREADS}" PATH=/usr/local/bin\:${DATA_OUT}\:${PATH} ${CMD_LN} > ${FL_STDOUT} 2>&1
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

