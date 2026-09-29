#!/bin/bash

# Minimal standalone reproducer for ncremap:
# /home/ac.zender/bin_chrysalis/ncremap: line 3831: ppp_pid[${fl_idx}]: unbound variable

# Usage:
# sbatch ~/nco/data/e3smu_dbg.sh

#SBATCH --job-name=e3smu_dbg.sh
#SBATCH --account=e3sm
#SBATCH --nodes=1
#SBATCH --output=e3smu_dbg.o%j
#SBATCH --exclusive
#SBATCH --time=00:10:00
#SBATCH --partition=debug

source /home/ac.forsyth2/miniforge3/etc/profile.d/conda.sh
conda activate test-e3sm-to-cmip-master-20260928_run2

NCREMAP=/home/ac.zender/bin_chrysalis/ncremap
VRT_MAP=/lcrc/group/e3sm/diagnostics/e3sm_to_cmip_data/maps/vrt_remap_plev19.nc
SRC_FILE=/lcrc/group/e3sm/ac.forsyth2/zppy_weekly_comprehensive_v3_output/zppy_main_branch_test_20260928_run2/v3.LR.historical_0051/post/atm/180x360_aave/ts/monthly/2yr/U_198501_198612.nc

WORKDIR=$(mktemp -d)
cd "${WORKDIR}" || exit 1
cp -s "${SRC_FILE}" ./test.nc

echo "Running: ${NCREMAP} --npo -p mpi --vrt_ntp=log --vrt_xtr=mss_val --vrt_out=${VRT_MAP} test.nc test.nc.plev"
"${NCREMAP}" --npo -p mpi --vrt_ntp=log --vrt_xtr=mss_val --vrt_out="${VRT_MAP}" test.nc test.nc.plev
echo "Exit code: $?"
