#!/bin/bash
# Usage:
# chmod 755 ~/stdin.sh
# Invoke stdin.sh with stdin_tst.sh as described in that file
if [ -p /dev/stdin ]; then
    printf "[ -p /dev/stdin ] is true\n"
else
    printf "[ -p /dev/stdin ] is false \n"
fi
if [ ! -t 0 ]; then
    printf "[ ! -t 0 ] is true\n"
else
    printf "[ ! -t 0 ] is false\n"
fi
if [ -z "${TERM}" ]; then
    printf "[ -z \"\${TERM}\" ] is true\n"
else
    printf "[ -z \"\${TERM}\" ] is false\n"
fi
if [ -z "${PS1}" ]; then
    printf "[ -z \"\${PS1}\" ] is true\n"
else
    printf "[ -z \"\${PS1}\" ] is false\n"
fi
if [ -n "${PBS_ENVIRONMENT}" ]; then
    printf "[ -n \"\${PBS_ENVIRONMENT}\" ] is true and value is \"${PBS_ENVIRONMENT}\"\n"
else
    printf "[ -n \"\${PBS_ENVIRONMENT}\" ] is false\n"
fi
if [ -n "${SLURM_PTY_PORT}" ]; then
    printf "[ -n \"\${SLURM_PTY_PORT}\" ] is true and value is \"${SLURM_PTY_PORT}\"\n"
else
    printf "[ -n \"\${SLURM_PTY_PORT}\" ] is false\n"
fi
if [ -n "${SLURM_JOBID}" ]; then
    printf "[ -n \"\${SLURM_JOBID}\" ] is true and value is \"${SLURM_JOBID}\"\n"
else
    printf "[ -n \"\${SLURM_JOBID}\" ] is false\n"
fi
read -t 0 line
if [ -n ${line} ] ; then
    printf "\"read -t 0 line\" is true and contains \"${line}\"\n"
else
    printf "\"read line\" is false\n"
fi
