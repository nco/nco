#!/bin/bash
# Usage:
# chmod 755 ~/stdin_tst.sh
# qsub -A CLI115 -V -l nodes=1 -l walltime=00:30:00 -N stdin -j oe -m e -o ~/stdin.out ~/stdin_tst.sh
# qsub -A arpae -l walltime=00:30:00 -l nodes=1 -N stdin -q batch -j oe -m e -o ~/stdin.out ~/stdin_tst.sh
# qsub -A ACME -q acme -l nodes=1 -l walltime=00:30:00 -N stdin -j oe -m e -o ~/stdin.out ~/stdin_tst.sh
# sbatch -A acme --nodes=1 --time=00:30:00 --partition=debug --job-name=stdin --mail-type=END --output=${HOME}/stdin.out ~/stdin_tst.sh
# salloc -A acme --nodes=1 --time=00:30:00 --partition=debug --job-name=stdin
echo foo > ~/foo
printf "No pipe, no re-direction: ~/stdin.sh\n"
~/stdin.sh
printf "Pipe to stdin:            echo hello | ~/stdin.sh\n"
echo hello | ~/stdin.sh
printf "Redirection to stdin:     ~/stdin.sh < foo\n"
~/stdin.sh < ~/foo
