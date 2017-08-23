# Purpose: Sample Pythong script to run ncclimo

# Source: Xylar Asay-Davis, 20170815

import subprocess

def ncclimo_tst():

    args = ['/global/homes/z/zender/bin_edison/ncclimo', '--clm_md=mth', '-a', 'sdd', '-m', 'mpaso',
            '-p', 'serial', '-v', 'timeMonthly_avg_meridionalHeatTransportLat,'
            'timeMonthly_avg_meridionalHeatTransportLatZ', '--seasons=ANN',
            '-s', '0022', '-e', '0022', '-i',
            '/global/cscratch1/sd/sprice/acme_scratch/edison/g60to30_SSSrestore/run',
            '-o', '/tmp/zender/MHT_oEC60to30v3wLI']

    subprocess.check_call(args)

if __name__ == "__main__":
    ncclimo_tst()
