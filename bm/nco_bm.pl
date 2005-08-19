#!/usr/bin/env perl

# $Header: /data/zender/nco_20150216/nco/bm/nco_bm.pl,v 1.65 2005-08-19 21:58:23 zender Exp $

# Usage:  usage(), below, has more information
# ~/nco/bld/nco_bm.pl # Tests all operators
# ~/nco/bld/nco_bm.pl ncra # Test one operator
# ~/nco/bld/nco_bm.pl --thr_nbr=2 --regress --udpreport
# scp ~/nco/bm/nco_bm.pl esmf.ess.uci.edu:nco/bm

# NB: When adding tests, _be sure to use -O to overwrite files_
# Otherwise, script hangs waiting for interactive response to overwrite queries
require 5.6.1 or die "This script requires Perl version >= 5.6.1, stopped";
use Cwd 'abs_path';
use English; # WCS96 p. 403
use Getopt::Long; # GNU-style getopt #qw(:config no_ignore_case bundling);
use strict; # Protect all namespaces

# Declare vars for strict

use vars qw(
$arg_nbr $bch_flg $bm @bm_cmd_ary $bm_dir $cmd_ln $dbg_lvl $dodap $fl_cnt
$dot_fmt $dot_nbr $dot_nbr_min $dot_sng $dsc_fmt $dsc_lng_max $dsc_sng $dta_dir
$pth_rmt_scp_tst
%failure $fl_pth $foo1_fl $foo2_fl $foo_fl $foo_avg_fl $foo_T42_fl $foo_tst $foo_x_fl
$foo_xy_fl $foo_xymyx_fl $foo_y_fl $foo_yx_fl @ifls $itmp $localhostname
$md5 $MY_BIN_DIR $notbodi $nsr_xpc $omp_flg $opr_fmt $opr_lng_max @opr_lst
@opr_lst_all $opr_nm $orig_outfile $outfile $prefix $prfxd $prg_nm $que $rcd
$result $rgr $server_ip $server_name $server_port $sock $spc_fmt $spc_nbr
$spc_nbr_min $spc_sng %subbenchmarks %success %sym_link $thr_nbr $tmr_app
%totbenchmarks @tst_cmd $tst_fl_cr8 $tst_fmt $tst_id_sng $tst_idx %tst_nbr
$udp_reprt $udp_rpt $usg $wnt_log $xpt_dsc $timed $sys_time @sys_tim_arr
%real_tme %usr_tme %sys_tme $mpi_prc @opr_lst_mpi $mpi_prfx  $timestamp
$opr_sng_mpi $nco_D_flg $opr_rgr_mpi
);
 
my @fl_cr8_dat;  # holds the strings for the fl_cr8() routine 
my @fl_tmg; # holds the timing data for the fl_cr8() routines.
 
# Initializations
# Re-constitute commandline
$prg_nm=$0; # $0 is program name Camel p. 136
$cmd_ln = "$0 "; $arg_nbr = @ARGV;
for (my $i=0; $i<$arg_nbr; $i++){ $cmd_ln .= "$ARGV[$i] ";}
$bm_dir = `pwd`; chomp $bm_dir;

# Set defaults for command line arguments
$bch_flg=0; # [flg] Batch behavior
$dbg_lvl = 0; # [enm] Print tests during execution for debugging
$nco_D_flg = "";
my $nvr_data=$ENV{'DATA'} ? $ENV{'DATA'} : '';
my $nvr_home=$ENV{'HOME'} ? $ENV{'HOME'} : '';
my $USER = $ENV{'USER'};
my $nvr_my_bin_dir=$ENV{'MY_BIN_DIR'} ? $ENV{'MY_BIN_DIR'} : '';
$MY_BIN_DIR = $nvr_my_bin_dir;
$fl_pth = '';
$que = 0;
$thr_nbr = 0; # If not zero, pass explicit threading argument 
$tst_fl_cr8 = "0";
$udp_rpt = 0;
$usg = 0;
$wnt_log = 0;
$md5 = 0;
$mpi_prc = 0; # by default, don't want no steekin MPI 
$mpi_prfx = "";
$timestamp = `date -u "+%x %R"`; chomp $timestamp;
$dodap = "FALSE"; # Unless redefined bythe cmdline, it doesn't get set
$fl_cnt = 32; # nbr of files to process (reduced to 4 if using remote/dods files
$pth_rmt_scp_tst='dust.ess.uci.edu:/var/www/html/dodsdata';

# other inits
$localhostname = `hostname`;
$notbodi = 0; # specific for hjm's puny laptop
my $prfxd = 0;
my $prefix = '';
if ($localhostname !~ "bodi") {$notbodi = 1} # spare the poor laptop
$ARGV = @ARGV;

if ($ARGV == 0) {usage();}

$rcd=Getopt::Long::Configure('no_ignore_case'); # Turn on case-sensitivity
&GetOptions(
	'bch_flg!'     => \$bch_flg,    # [flg] Batch behavior
	'benchmark'    => \$bm,         # Run benchmarks 
	'bm'           => \$bm,         # Run benchmarks 
	'dbg_lvl=i'    => \$dbg_lvl,    # debug level
	'debug=i'      => \$dbg_lvl,    # debug level
	'dods:s'       => \$dodap,      # string is the URL to the DODs data
	'dap:s'        => \$dodap,      #  "
	'opendap:s'    => \$dodap,      #  "
	'h'            => \$usg,        # explains how to use this thang
	'help'         => \$usg,        #            ditto
	'log'          => \$wnt_log,    # set if want output logged
	'mpi_prc=i'    => \$mpi_prc,    # set number of mpi processes
	'queue'        => \$que,        # if set, bypasses all interactive stuff
        'pth_rmt_scp_tst' => \$pth_rmt_scp_tst, # [drc] Path to scp regression test file
	'regress'      => \$rgr,        # test regression 
	'rgr'          => \$rgr,        # test regression 
	'test_files=s' => \$tst_fl_cr8, # Create test files "134" does 1,3,4
	'tst_fl=s'     => \$tst_fl_cr8, # Create test files "134" does 1,3,4
	'thr_nbr=i'    => \$thr_nbr,    # Number of OMP threads to use
	'udpreport'    => \$udp_rpt,    # punt the timing data back to udpserver on sand
	'usage'        => \$usg,        # explains how to use this thang
	'caseid=s'     => \$xpt_dsc,    # short string to tag test dir and batch queue
	'xpt_dsc=s'    => \$xpt_dsc,    #                   ditto
	'md5'          => \$md5,        # requests md5 checksumming results (longer but more exacting)
);

my $NUM_FLS = 4; # max number of files in the file creation series

#test nonfatally for useful modules
my $hiresfound;
if($dbg_lvl > 0){printf ("$prg_nm: \$cmd_ln = $cmd_ln\n");} # endif dbg
if($dbg_lvl > 0){printf ("$prg_nm: \$xpt_dsc = $xpt_dsc\n");} # endif dbg
if($dbg_lvl > 0){printf ("$prg_nm: \$rgr = $rgr\n");} # endif dbg
if($dbg_lvl > 0){printf ("$prg_nm: \$bm = $bm\n");} # endif dbg
if($dbg_lvl > 0){printf ("$prg_nm: \$bch_flg = $bch_flg\n");} # endif dbg
if($dbg_lvl > 0){printf ("$prg_nm: \$nvr_data = $nvr_data\n");} # endif dbg
if($dbg_lvl > 0){printf ("$prg_nm: \$nvr_home = $nvr_home\n");} # endif dbg
if($dbg_lvl > 0){printf ("$prg_nm: \$nvr_my_bin_dir = $nvr_my_bin_dir\n");} # endif dbg
if($dbg_lvl > 0){printf ("$prg_nm: \@ENV = @ENV\n");} # endif dbg

# resolve conflicts early
if ($mpi_prc > 0 && $thr_nbr > 0) {die "\nThe  '--mpi' option and '--thr_nbr' (OMP) option are mutaully exclusive.\nPlease decide which you want to run and try again.\n";}

# any irrationally exuberant values?
if ($mpi_prc > 16) {die "\nThe '--mpi' value was set to an irrationally exuberant [$mpi_prc].  Try a lower value\n ";}
if ($thr_nbr > 16) {die "\nThe '--thr_nbr' value was set to an irrationally exuberant [$thr_nbr].  Try a lower value\n ";}
if (length($xpt_dsc) > 80) {die "\nThe caseid string is > 80 characters - please reduce it to less than 80 chars.\nIt's used to create file and directory names, so it has to be relatively short\n";}

print "\n===== Testing for required modules\n";
BEGIN {eval "use Time::HiRes qw(usleep ualarm gettimeofday tv_interval)"; $hiresfound = $@ ? 0 : 1}
#$hiresfound = 0;  # uncomment to simulate not found
if ($hiresfound == 0) {
    print "\nOoops! Time::HiRes (needed for accurate timing) not found\nContinuing without timing.";
} else {
    print "\tTime::HiRes ... found.\n";
}
my $iosockfound;
BEGIN {eval "use IO::Socket"; $iosockfound = $@ ? 0 : 1}
#$iosockfound = 0;  # uncomment to simulate not found
if ($iosockfound == 0) {
    print "\nOoops! IO::Socket module not found - continuing with no udp logging.\n";
} else {
    print "\tIO::Socket  ... found.\n";
}

my $md5found;
BEGIN {eval "use Digest::MD5"; $md5found = $@ ? 0 : 1} 
# $md5found = 0;  # uncomment to simulate no MD5 
if ($md5 == 1) {
	if ($md5found == 0) { 
		print "\nOoops! Digest::MD5 module not found - continuing with simpler error checking\n\n" ;
	} else {
		print "\tDigest::MD5 ... found.\n\n";
	}
} else {
	print "\tMD5 NOT requested; continuing with ncks checking of single values.\n\n";
}
# the md5 table needs to be populated with the md5 checksums for the output files
# generated with the no-history option.
# as in these examples - each key is the name of the test.  The md5 checksum is 
# the hash of the entire output file generated with the -h flag (prevents a history
# from being written to the end of the file and therefore changing the md5.
# also have to change the benchmarks to add this flag.
my %MD5_table = ( # holds md5 hashes for each test 
# Benchmark entries
"ncap long algebraic operation_0" => "7037002c30744e71565bc3f07bfe718a", #MD5
"ncap long algebraic operation_1" => "fceea540e58e657e0c4f16bb8928f447", #MD5
"ncbo differencing two files_0" => "9fb75945653ce8fafb74eca0997fcc3d", #MD5
"ncea averaging 2^5 files_0" => "fe74d3a77118b7d5a66ab9fa0fea03ae", #MD5
"ncea averaging 2^5 files_1" => "f58c721ff4d2d5f13949ab3f3ecf8453", #MD5
"ncecat joining 2^5 files_0" => "e311755e9f01e427418ef8020155d8b1", #MD5
"ncecat joining 2^5 files_1" => "fbb1b621cdfe1daa1841727423a24138", #MD5
"ncflint weight-averaging 2 files_0" => "7037002c30744e71565bc3f07bfe718a", #MD5
"ncflint weight-averaging 2 files_1" => "38ee6518c570a9aa59f4aaa9d93d801d", #MD5
"ncpdq dimension-order reversal_0" => "1999df64e07a9346eb12da4219156b1d", #MD5
"ncpdq dimension-order re-ordering_0" => "1999df64e07a9346eb12da4219156b1d", #MD5
"ncpdq dimension-order re-ordering & reversing_0" => "da972b99c8cb2b059d4d54b346f59cc3", #MD5
"ncpdq packing a file_0" => "da41b3b24dfe369097c68dea9521119c", #MD5
"ncra time-averaging 2^5 (i.e. one month) ipcc files_0" => "e2853478c04242b29c74bf983b38e5e4", #MD5
"ncrcat joining 2^5 files_0" => "4f3582ac41db57a506be62efae964d4d", #MD5
"ncrcat joining 2^5 files_1" => "b1656d04ce999855f09a58b3fedef4f3", #MD5
"ncwa averaging all variables to scalars - stl_5km.nc & sqrt_0" => "b6cb85aceb699670c2eca197ebbe10b4", #MD5
"ncwa averaging all variables to scalars - stl_5km.nc & rms_0" => "250352270a64b65194000165061b1df1",#MD5
"ncwa averaging all variables to scalars - ipcc_dly_T85.nc & sqt_0" => "38ee6518c570a9aa59f4aaa9d93d801d", #MD5

# Regression entries
"Testing float modulo float_0" => "8887503481d53aa13ea8bab73bca280e", #MD5
"Testing foo=log(e_flt)^1 (fails on AIX TODO ncap57)_0" => "551d25425d4c9bd69932ccbbe6b3e63c", #MD5
"Testing foo=log(e_dbl)^1_0" => "3bca0c79dec8371bf39f3059a4d2c078", #MD5
"Testing foo=4*atan(1)_0" => "2d579cd7cce9c45299abbdf9810933bf", #MD5
"Testing foo=erf(1)_0" => "e72aa31bdba32c0cd496bb2617228b5e", #MD5
"Testing foo=gamma(0.5)_0" => "e2fba9c54d9019c2e4354c71303186a3", #MD5_table
"Testing foo=sin(pi/2)_0" => "3436044dca29ccab26c25396a882d1f4", #MD5
"Testing foo=cos(pi)_0" => "44fb1ba7a59b55d825c15983dc4fbc31", #MD5
"Modify all existing units attributes to meter second-1_0" => "ee887abdddef87cefb6ed35e8ea23497", #MD5
"difference scalar missing value_0" => "df9d318631a4e2b914c0fe487dd5eda0", #MD5
"difference with missing value attribute_0" => "1677d1db5e5ebef58bbd6f67cda192bf", #MD5
"difference without missing value attribute_0" => "30a8a849af6540e1e92ea794d917c30c", #MD5
"missing_values differ between files_0" => "31f5a7b995a103e8cf70010f8aed1185", #MD5
"missing_values differ between files_1" => "9042d513e4aebe9b476665a13d0ac1c6", #MD5
"missing_values differ between files_2" => "88fff81701125f219866deaa4de5f704", #MD5
"ncdiff symbolically linked to ncbo_0" => "1677d1db5e5ebef58bbd6f67cda192bf", #MD5
"difference with missing value attribute_0" => "1677d1db5e5ebef58bbd6f67cda192bf", #MD5
"difference without missing value attribute_0" => "30a8a849af6540e1e92ea794d917c30c", #MD5
"ensemble mean of int across two files_0" => "d2e3e8911d679325904f4a31ed5cc807", #MD5
"ensemble mean with missing values across two files_0" => "5d0fbbd95cca6c1fd34432129e10c3ff", #MD5
"ensemble min of float across two files_0" => "f323ac17d08a3d94683986ddb92007de", #MD5
"scale factor + add_offset packing/unpacking_0" => "673cc9518a67aa8898dd8c40e1eb54c4", #MD5
"concatenate two files containing only scalar variables_2" => "5b8dd29c4f22b57737e3ac1f00ffc864", #MD5
"identity weighting_0" => "4cc63c64f5bf6147a979ce5bccf00337", #MD5
"identity interpolation_1" => "abc71f105ac3a450086b6c86731c77f5", #MD5
"identity interpolation_2" => "daf6017dc89568c34573569bf077636c", #MD5
"Create T42 variable named one, uniformly 1.0 over globe_0" => "1318ba0f045ebb96283f1d9984b97c97", #MD5
"Create T42 variable named one, uniformly 1.0 over globe_1" => "16b03dc221613d911e32215203207e58", #MD5
"Create T42 variable named one, uniformly 1.0 over globe_2" => "84ea0b4116e01c03be5250d7023ec717", #MD5
"extract a dimension_0" => "f8634d6bef601cda97cff5f487114baf", #MD5
"extract a variable with limits_0" => "fe01ea80027e2988f345eb831edc7477", #MD5
"extract variable of type NC_INT_0" => "b861ca6eddfc8199cd18c0fe1b935eaa", #MD5
"Multi-slab lat and lon with srd_0" => "5740ff522eaf09023c844fcadc4a03b5", #MD5
"Multi-slab with redundant hyperslabs_0" => "e5c0cfa23955d8396d84224a8a3cc790", #MD5
"Multi-slab with coordinates_0" => "e5c0cfa23955d8396d84224a8a3cc790", #MD5
"Double-wrapped hyperslab_0" => "9cb9a60c5a357a1380716ec9e5dfcaa7", #MD5
"dimension slice using UDUnits library (fails without UDUnits library support)_0" => "9275a118e47d8d08aa7ce1986bfb0de6", #MD5
"variable wildcards A (fails without regex library)_0" => "7710233e83d78c8cc6b6447637d1e672", #MD5
"variable wildcards B (fails without regex library)_0" => "5bbf844271063c19a415e10fd3a95ad9", #MD5
"Offset past end of file_0" => "6492bf368d38ba8135ec82d71f733be7", #MD5
"reverse coordinate_0" => "80d0710f2275c530458556fd9eea6684", #MD5
"reverse three dimensional variable_0" => "468aad3c93e9137b8d360dbbf2e83475", #MD5
"re-order three dimensional variable_0" => "6e87bf2c6a8c78e2b6b0f5dbd911aed8", #MD5
"Pack and then unpack scalar (uses only add_offset)_0" => "3be2035918afec462b682a73016e3942", #MD5
"Pack and then unpack scalar (uses only add_offset)_1" => "17129104117daf91894e78bac99ed20b", #MD5
"record mean of int across two files_0" => "eb87ddae5a4f746c1370895573d80b1c", #MD5
"record mean of float with double missing values_0" => "c3f217fa194091474d9bceda8ec7e5c4", #MD5
"record mean of float with integer missing values_0" => "664e939df5248124fa70fd18445e18ac", #MD5
"record mean of integer with integer missing values_0" => "b982a5194ab6dc6b2d39a84ee18f34f9", #MD5
"record mean of integer with float missing values_0" => "2bea917d948229efe191e6aa5575d789", #MD5
"record mean of packed double with double missing values_0" => "d5c51a50a5a1f815a5aa181ccd7038ea", #MD5
"record mean of packed double to test precision_0" => "5681a63c98c2a26e85d658e8daed11f3", #MD5
"record mean of packed float to test precision_0" => "b91c2683c438476c3cfa347eb17b9d58", #MD5
"record mean of float with double missing values across two files_0" => "c3f217fa194091474d9bceda8ec7e5c4", #MD5
"record min of float with double missing values across two files_0" => "e122f17b034a0b0aec63c9b711e9ae8a", #MD5
"record max of float with double missing values across two files_0" => "d248e388229b4702029cfab576066936", #MD5
"record ttl of float with double missing values across two files_0" => "2b4a42a1bf202909da034ac2a44b755c", #MD5
"record rms of float with double missing values across two files_0" => "da5d943a97fedf07d506a36911855371", #MD5
"record sdn of float with double missing values across two files_1" => "c3f217fa194091474d9bceda8ec7e5c4", #MD5
"record sdn of float with double missing values across two files_2" => "24b1cb6803df6a6e00b087e1e46aac42", #MD5
"record sdn of float with double missing values across two files_3" => "18978d9c6da90b833b23338573bba3a0", #MD5
"record sdn of float with double missing values across two files_4" => "25051141b7b891e167d0b7be6eed1e82", #MD5
"normalize by denominator upper hemisphere_0" => "149b93703aaab1033e912968ed7df075", #MD5
"do not normalize by denominator_0" => "fdbf846378041855856e589cef610999", #MD5
"average with missing value attribute_0" => "b2225e092b4c572675c7075a53be775c", #MD5
"average without missing value attribute_0" => "13c674e187bc6dd62c3064cda4e77fd9", #MD5
"average masked coordinate_0" => "cb384ac035d17d47fa24c276c165a686", #MD5
"average masked variable_0" => "f00e2747fa2be0852454fef5d6cfd301", #MD5
"average masked, weighted coordinate_0" => "3484a9001e715142132440fac325d1b9", #MD5
"average masked, weighted variable_0" => "8fbd06664f1bc309b95ad3bf2269fdc0", #MD5
"weight conforms to var first time_0" => "0f39db04b2c10623c77b79830e14fef4", #MD5
"average all missing values with weights_0" => "fdf6393eba06d9d233425c7e90a37371", #MD5
"average some missing values with unity weights_0" => "8f8b34bec9190555a726b721f9d0c8c1", #MD5
"average masked variable with some missing values_0" => "552eded30274728bbb84044f510be3f9", #MD5
"min switch on type double, some missing values_0" => "81a242bf078958264585b5909f7b046f", #MD5
"Dimension reduction with min switch and missing values_0" => "e11acafc1bc5b1bcc2156010be01452d", #MD5
"Dimension reduction with min switch and missing values_1" => "e11acafc1bc5b1bcc2156010be01452d", #MD5
"Dimension reduction on type int with min switch and missing values_0" => "514ed9168c45eabf4011d2a2e8c6a8ef", #MD5
"Dimension reduction on type int with min switch and missing values_1" => "514ed9168c45eabf4011d2a2e8c6a8ef", #MD5
"Dimension reduction on type short variable with min switch and missing values_0" => "d68a97fec1c4c774116458c74c53b31f", #MD5
"Dimension reduction on type short variable with min switch and missing values_1" => "d68a97fec1c4c774116458c74c53b31f", #MD5
"Dimension reduction with min flag on type float variable_0" => "4cf3f52ee7529ce8276484262ce3f2d7", #MD5
"Max flag on type float variable_0" => "f714106cc99bb9b1ef2430c9b164b849", #MD5
"Dimension reduction on type double variable with max switch and missing values_0" => "c9fa752d585240d8881cde3fab5a36f6", #MD5
"Dimension reduction on type double variable with max switch and missing values_1" => "c9fa752d585240d8881cde3fab5a36f6", #MD5
"Dimension reduction on type int variable with min switch and missing values_0" => "53ac188efcf21756274e0299adbcb96f", #MD5
"Dimension reduction on type int variable with min switch and missing values_1" => "53ac188efcf21756274e0299adbcb96f", #MD5
"Dimension reduction on type short variable with max switch and missing values_0" => "2b5a5972ffce0cef0a9a6ae6f31a97c0", #MD5
"Dimension reduction on type short variable with max switch and missing values_1" => "2b5a5972ffce0cef0a9a6ae6f31a97c0", #MD5
"rms with weights_0" => "4efa5f65ddfa4dd571c1073d8e877bf9", #MD5
"weights would cause SIGFPE without dbl_prc patch_0" => "6a3e8ecb9768cb2a5132637e6a28a17b", #MD5
"avg would overflow without dbl_prc patch_0" => "e4d70a9fd9b8b823cf43301555434e2c", #MD5
"ttl would overflow without dbl_prc patch, wraps anyway so exact value not important (failure expected/OK on Xeon chips because of different wrap behavior)_0" => "d12212aa41e3fef4ea7c2cdb718ee647", #MD5
"min with weights_0" => "0fcc1eb5239a4658cf8af2a0692fb5bf", #MD5
"max with weights_0" => "7855d7111ad8d937fa9ab4df171f2902", #MD5
"running ncap.in script into nco_tst.pl_0" => "", #MD5
);

my %wcTable = ( # word count table for $dta_dir/wc_out
# Benchmark entries
"ncap long algebraic operation_0" => "111111 338754 5872990", #wc
"ncap long algebraic operation_1" => "9480 30734 410027", #wc
"ncbo differencing two files_0" => "111111 338754 5542049", #wc
"ncea averaging 2^5 files_0" => "111111 333695 5528455", #wc
"ncea averaging 2^5 files_1" => "47 323 1862", #wc
"ncecat joining 2^5 files_0" => "111111 333665 4667532", #wc
"ncecat joining 2^5 files_1" => "42 302 2000", #wc
"ncflint weight-averaging 2 files_0" => "111111 338754 5872990", #wc
"ncflint weight-averaging 2 files_1" => "9479 30721 409885", #wc
"ncpdq dimension-order reversal_0" => "111111 338754 5872990", #wc
"ncpdq dimension-order re-ordering_0" => "111111 338754 5872990", #wc
"ncpdq dimension-order re-ordering & reversing_0" => "111111 338754 5873056", #wc
"ncpdq packing a file_0" => "111111 339842 4964039", #wc
"ncrcat joining 2^5 files_0" => "111111 222534 3556822", #wc
"ncrcat joining 2^5 files_1" => "42 302 2007", #wc
"ncra time-averaging 2^5 (i.e. one month) ipcc files_0" => "111111 338810 5874572", #wc
"ncwa averaging all variables to scalars - stl_5km.nc & sqrt_0" => "47 323 1862", #wc
"ncwa averaging all variables to scalars - stl_5km.nc & rms_0" => "47 323 1869", #wc
"ncwa averaging all variables to scalars - ipcc_dly_T85.nc & sqt_0" => "9479 30721 409885", #wc

# Regression entries
"Testing float modulo float_0" => "24 131 947", #wc
"Testing foo=log(e_flt)^1 (fails on AIX TODO ncap57)_0" => "11 95 569", #wc
"Testing foo=log(e_dbl)^1_0" => "11 95 571", #wc
"Testing foo=4*atan(1)_0" => "11 95 575", #wc
"Testing foo=erf(1)_0" => "11 95 576", #wc
"Testing foo=gamma(0.5)_0" => "11 95 583", #wc
"Testing foo=sin(pi/2)_0" => "15 121 687", #wc
"Testing foo=cos(pi)_0" => "15 121 688", #wc
"Modify all existing units attributes to meter second-1_0" => "2188 13222 118042", #wc
"difference scalar missing value_0" => "13 119 754", #wc
"difference with missing value attribute_0" => "21 169 995", #wc
"difference without missing value attribute_0" => "20 157 922", #wc
"missing_values differ between files_0" => "27 179 1150", #wc
"missing_values differ between files_1" => "27 179 1114", #wc
"missing_values differ between files_2" => "27 179 1118", #wc
"ncdiff symbolically linked to ncbo_0" => "21 169 995", #wc
"difference with missing value attribute_0" => "21 169 995", #wc
"difference without missing value attribute_0" => "20 157 922", #wc
"ensemble mean of int across two files_0" => "23 178 1095", #wc
"ensemble mean with missing values across two files_0" => "23 180 1171", #wc
"ensemble min of float across two files_0" => "23 180 1169", #wc
"scale factor + add_offset packing/unpacking_0" => "13 154 910", #wc
"concatenate two files containing only scalar variables_2" => "17 126 754", #wc
"identity weighting_0" => "12 106 627", #wc
"identity interpolation_1" => "2188 12862 114282", #wc
"identity interpolation_2" => "12 106 627", #wc
"Create T42 variable named one, uniformly 1.0 over globe_0" => "280 586 8245", #wc
"Create T42 variable named one, uniformly 1.0 over globe_1" => "280 586 6909", #wc
"Create T42 variable named one, uniformly 1.0 over globe_2" => "16676 49836 750137", #wc
"extract a dimension_0" => "23 202 1224", #wc
"extract a variable with limits_0" => "72 492 3666", #wc
"extract variable of type NC_INT_0" => "12 106 640", #wc
"Multi-slab lat and lon with srd_0" => "24 195 1379", #wc
"Multi-slab with redundant hyperslabs_0" => "20 175 1179", #wc
"Multi-slab with coordinates_0" => "20 175 1179", #wc
"Double-wrapped hyperslab_0" => "20 175 1179", #wc
"dimension slice using UDUnits library (fails without UDUnits library support)_0" => "2188 12862 114254", #wc
"variable wildcards A (fails without regex library)_0" => "439 2813 25704", #wc
"variable wildcards B (fails without regex library)_0" => "105 643 4327", #wc
"Offset past end of file_0" => "16 115 696", #wc
"reverse coordinate_0" => "14 107 643", #wc
"reverse three dimensional variable_0" => "72 492 3666", #wc
"re-order three dimensional variable_0" => "72 492 3666", #wc
"Pack and then unpack scalar (uses only add_offset)_0" => "14 163 992", #wc
"Pack and then unpack scalar (uses only add_offset)_1" => "13 152 935", #wc
"record mean of int across two files_0" => "23 178 1099", #wc
"record mean of float with double missing values_0" => "23 180 1173", #wc
"record mean of float with integer missing values_0" => "23 180 1170", #wc
"record mean of integer with integer missing values_0" => "23 180 1168", #wc
"record mean of integer with float missing values_0" => "23 180 1168", #wc
"record mean of packed double with double missing values_0" => "24 195 1330", #wc
"record mean of packed double to test precision_0" => "23 187 1172", #wc
"record mean of packed float to test precision_0" => "23 187 1169", #wc
"record mean of float with double missing values across two files_0" => "23 180 1173", #wc
"record min of float with double missing values across two files_0" => "23 180 1169", #wc
"record max of float with double missing values across two files_0" => "23 180 1171", #wc
"record ttl of float with double missing values across two files_0" => "23 180 1174", #wc
"record rms of float with double missing values across two files_0" => "23 180 1195", #wc
"record sdn of float with double missing values across two files_1" => "23 180 1173", #wc
"record sdn of float with double missing values across two files_2" => "17 149 963", #wc
"record sdn of float with double missing values across two files_3" => "61 237 2212", #wc
"record sdn of float with double missing values across two files_4" => "23 180 1189", #wc
"normalize by denominator upper hemisphere_0" => "29 225 1195", #wc
"do not normalize by denominator_0" => "1342 7637 58740", #wc
"average with missing value attribute_0" => "17 146 857", #wc
"average without missing value attribute_0" => "16 134 790", #wc
"average masked coordinate_0" => "12 106 641", #wc
"average masked variable_0" => "17 143 843", #wc
"average masked, weighted coordinate_0" => "20 198 1166", #wc
"average masked, weighted variable_0" => "26 246 1444", #wc
"weight conforms to var first time_0" => "11 95 571", #wc
"average all missing values with weights_0" => "17 146 877", #wc
"average some missing values with unity weights_0" => "17 148 881", #wc
"average masked variable with some missing values_0" => "18 159 981", #wc
"min switch on type double, some missing values_0" => "17 149 961", #wc
"Dimension reduction with min switch and missing values_0" => "64 376 2825", #wc
"Dimension reduction with min switch and missing values_1" => "64 376 2825", #wc
"Dimension reduction on type int with min switch and missing values_0" => "64 376 2812", #wc
"Dimension reduction on type int with min switch and missing values_1" => "64 376 2812", #wc
"Dimension reduction on type short variable with min switch and missing values_0" => "64 373 2807", #wc
"Dimension reduction on type short variable with min switch and missing values_1" => "64 373 2807", #wc
"Dimension reduction with min flag on type float variable_0" => "25 201 1156", #wc
"Max flag on type float variable_0" => "37 319 1796", #wc
"Dimension reduction on type double variable with max switch and missing values_0" => "50 291 1988", #wc
"Dimension reduction on type double variable with max switch and missing values_1" => "50 291 1988", #wc
"Dimension reduction on type int variable with min switch and missing values_0" => "86 478 4001", #wc
"Dimension reduction on type int variable with min switch and missing values_1" => "86 478 4001", #wc
"Dimension reduction on type short variable with max switch and missing values_0" => "86 475 3993", #wc
"Dimension reduction on type short variable with max switch and missing values_1" => "86 475 3993", #wc
"rms with weights_0" => "11 95 570", #wc
"weights would cause SIGFPE without dbl_prc patch_0" => "17 144 860", #wc
"avg would overflow without dbl_prc patch_0" => "17 144 879", #wc
"ttl would overflow without dbl_prc patch, wraps anyway so exact value not important (failure expected/OK on Xeon chips because of different wrap behavior)_0" => "17 144 880", #wc
"min with weights_0" => "11 95 572", #wc
"max with weights_0" => "11 95 571", #wc
);

if($dbg_lvl > 0){$nco_D_flg = "-D" .  "$dbg_lvl";}

# Determine where $DATA should be, prompt user if necessary
if($dbg_lvl > 1){printf ("$prg_nm: Calling set_dat_dir()...\n");}
set_dat_dir(); # Set $dta_dir

# make sure that the $fl_pth gets set to a reasonable defalt
$fl_pth = "$dta_dir";
# also want to try to grok /usr/bin/time, as in the shell scripts
if (-e "/usr/bin/time" && -x "/usr/bin/time") {
	$tmr_app = "/usr/bin/time";
#	# if use time -p get 'POSIX' style time output; if '-v', get verbose (long!) output
	if (`uname` =~ "inux") { $tmr_app .= " -p ";} 
} else { # just use whatever the shell thinks is the time app
	$tmr_app = "time"; #could be the bash builtin or another 'time'-like app (AIX)
}

# Initialize & set up some variables
if($dbg_lvl > 1){printf ("$prg_nm: Calling initialize()...\n");}
initialize($bch_flg,$dbg_lvl);

# Pass in the MPI prefix if --mpi is set. Should be able to just plug into the commandline
# MPI'ed nco's: mpirun -np 4 mpncbo etc
#                MPI prefix <--++--> regular command line
# note that $prefix contains both MPI and non-MPI commands from here on - $mpi_prfx isn't nec.
$mpi_prfx = "$tmr_app mpirun -np $mpi_prc  $MY_BIN_DIR/mp"; $prfxd = 1; $timed = 1;
$prefix   = "$tmr_app $MY_BIN_DIR/"; $prfxd = 1; $timed = 1;

# Use variables for file names in regressions; some of these could be collapsed into
# fewer ones, no doubt, but keep them separate until whole shebang starts working correctly
$outfile       = "$dta_dir/foo.nc"; # replaces outfile in tests, typically 'foo.nc'
$orig_outfile  = "$dta_dir/foo.nc";
$foo_fl        = "$dta_dir/foo";
$foo_avg_fl    = "$dta_dir/foo_avg.nc";
$foo_tst       = "$dta_dir/foo.tst";
$foo1_fl       = "$dta_dir/foo1.nc";
$foo2_fl       = "$dta_dir/foo2.nc";
$foo_x_fl      = "$dta_dir/foo_x.nc";
$foo_y_fl      = "$dta_dir/foo_y.nc";
$foo_xy_fl     = "$dta_dir/foo_xy.nc";
$foo_yx_fl     = "$dta_dir/foo_yx.nc";
$foo_xymyx_fl  = "$dta_dir/foo_xymyx.nc";
$foo_T42_fl    = "$dta_dir/foo_T42.nc";

# the real udping server
$server_name = "sand.ess.uci.edu";
$server_ip = "128.200.14.132";
$server_port = 29659;

if ($usg) { usage()};   # dump usage blurb
if (0) { tst_hirez(); } # tests the hires timer - needs explict code mod to do this

if ($iosockfound) {
	$sock = IO::Socket::INET->new (
		Proto    => 'udp',
		PeerAddr => $server_ip,
		PeerPort => $server_port
	) or print "\nCan't get the socket - continuing anyway.\n"; # if off network..
} else {$udp_reprt = 0;}

if ($wnt_log) { 
    open(LOG, ">$bm_dir/nctest.log") or die "\nUnable to open log file 'nctest.log' - check permissions on it\nor the directory you are in.\n stopped";
}

# Pass explicit threading argument
if ($thr_nbr > 0){$omp_flg="--thr_nbr=$thr_nbr ";} else {$omp_flg='';}

# does dodap require that we ignore both MPI and OpenMP?  Let's leave it in for now.
# If dodap is not set then test with local files
# If dodap is set and string is NULL, then test with OPeNDAP files on sand.ess.uci.edu
# If string is NOT NULL, use URL to grab files

if ($dbg_lvl > 1 ) {print "before dodap, fl_pth = $fl_pth\n";}
# $dodap asks for and if defined, carries, the URL that's inserted in the '-p' place in nco cmdlines
if ($dodap ne "FALSE") { 
	if ($dodap eq "") { 
		$fl_pth = "http://sand.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata";
		$fl_cnt = 4;
	} elsif ($dodap =~ /http/) { 
		$fl_pth = $dodap; 
		$fl_cnt = 4;
	} else {
		die "\nThe URL specified with the --dods option:\n $dodap \ndoesn't look like a valid URL.\nTry again\n\n";
	}
}
if ($dbg_lvl > 1 ) {print "after dodap, fl_pth = $fl_pth\n";}


# Initialize & set up some variables
#if($dbg_lvl > 0){printf ("$prg_nm: Calling initialize()...\n");}
#initialize($bch_flg,$dbg_lvl);

# Grok /usr/bin/time, as in shell scripts
if (-e "/usr/bin/time" && -x "/usr/bin/time") {
	$tmr_app = "/usr/bin/time";
	if (`uname` =~ "inux" && $dbg_lvl > 1){$tmr_app.=" -v ";}
} else { # just use whatever the shell thinks is the time app
	$tmr_app = "time"; # bash builtin or other 'time'-like application (AIX)
} # endif time

# Regression tests 
if ($rgr){
	perform_tests();
	smrz_rgr_rslt();
} # endif rgr

# Start real benchmark tests
# Test if necessary files are available - if so, may skip creation tests
if($bm && $dodap eq "FALSE"){
	if($dbg_lvl > 0){printf ("$prg_nm: Calling fl_cr8_dat_init()...\n");}
	fl_cr8_dat_init(); # Initialize data strings & timing array for files
}
	
# Check if files have already been created
# If so, skip file creation if not requested
if ($bm && $tst_fl_cr8 == 0 && $dodap eq "FALSE") { 
	for (my $i = 0; $i < $NUM_FLS; $i++) {
		my $fl = $fl_cr8_dat[$i][2] . ".nc"; # file root name stored in $fl_cr8_dat[$i][2] 
		if (-e "$dta_dir/$fl" && -r "$dta_dir/$fl") {
			printf ("%16s exists - can skip creation\n", $fl);
		} else {
			my $e = $i+1;
			$tst_fl_cr8 .= "$e";
		}
	}
}

# file creation tests
if ($tst_fl_cr8 ne "0"){
    my $fc = 0;
    if ($tst_fl_cr8 =~ "[Aa]") { $tst_fl_cr8 = "1234";}
    if ($tst_fl_cr8 =~ /1/){ fl_cr8(0); $fc++; }
    if ($tst_fl_cr8 =~ /2/){ fl_cr8(1); $fc++; }
    if ($tst_fl_cr8 =~ /3/){ fl_cr8(2); $fc++; }
    if ($notbodi && $tst_fl_cr8 =~ /4/) { fl_cr8(3); $fc++; }	
    if ($fc >0) {smrz_fl_cr8_rslt(); } # prints and udpreports file creation timing
} 

my $doit=1; # for skipping various tests

# and now, the REAL benchmarks, set up as the regression tests below to use go() and smrz_rgr_rslt()
if ($bm) {
	print "\nStarting Benchmarks now\n";
	if($dbg_lvl > 1){print "bm: prefix = $prefix\n";}
	
my $in_pth = " -p ../data ";
print "";
if ($dodap ne "" && $fl_pth =~ /http/ ) { $in_pth = " -p $fl_pth "; }
if ($dodap eq "") { $in_pth = " -p  http://sand.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata "; } 
	

	################### Set up the symlinks ###################
	
	if ($bm && $dodap eq "FALSE") {
		if ($dbg_lvl > 0) {print "\n\nSetting up symlinks for test nc files\n";}
		for (my $f=0; $f<$NUM_FLS; $f++) {
			my $rel_fle = "$dta_dir/$fl_cr8_dat[$f][2]" . ".nc" ;
			my $ldz = "0"; # leading zero for #s < 10
			if ($dbg_lvl > 0) {print "\tsymlinking $rel_fle\n";}
			for (my $n=0; $n<32; $n ++) {
				if ($n>9) {$ldz ="";}
				my $lnk_fl_nme = "$dta_dir/$fl_cr8_dat[$f][2]" . "_" . "$ldz" . "$n" . ".nc";
				if (-r $rel_fle && -d $dta_dir && -w $dta_dir){
					symlink $rel_fle, $lnk_fl_nme;
				}
			}
		}
	}


	#################### begin ncap benchmark hjm - needs to be verified.
	$opr_nm='ncap';
	$dsc_sng = 'ncap long algebraic operation';
	###################
#if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}

	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
		$tst_cmd[0] = "ncap -h -O $nco_D_flg -s  \"nu_var1[time,lat,lon,lev]=d4_01*d4_02*(d4_03**2)-(d4_05/d4_06)\" 	-s \"nu_var2[lat,time,lev,lon]=(d4_13/d4_02)*((d4_03**2)-(d4_05/d4_06))\" -s \"nu_var3[time,lat,lon]=(d3_08*3d_01)-(3d_05**3)-(3d_11*3d_16)\" -s \"nu_var4[lon,lat,time]=(d3_08+3d_01)-(3d_05*3)-3d_11-17.33)\"  -p $fl_pth ipcc_dly_T85.nc  $outfile";
		$tst_cmd[1] = "ncwa -O $omp_flg -y sqrt -a lat,lon $outfile  $outfile";
		$tst_cmd[2] = "ncks -C -H -s '%f' -v d2_00  $outfile"; 
		$nsr_xpc = "4.024271";
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng]\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### begin ncbo benchmark
	$opr_nm='ncbo';
	$dsc_sng = 'ncbo differencing two files';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncbo -h -O $nco_D_flg $omp_flg --op_typ='-' -p $fl_pth ipcc_dly_T85.nc ipcc_dly_T85_00.nc $outfile";
		if($dbg_lvl > 2){print "entire cmd: $tst_cmd[0]\n";}
		$tst_cmd[1] = "ncks -C -H -s '%f' -v sleepy $outfile";
		$nsr_xpc = "0.000000";	
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng]\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### begin ncea benchmark 
	$opr_nm='ncea';
	$dsc_sng = 'ncea averaging 2^5 files';
	####################	
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng, files=$fl_cnt\n";}
		$tst_cmd[0] = "ncea -h -O $nco_D_flg $omp_flg -n $fl_cnt,2,1 -p $fl_pth stl_5km_00.nc $outfile";
		if($dbg_lvl > 2){print "entire cmd: $tst_cmd[0]\n";}
		$tst_cmd[1] = "ncwa -h -O $nco_D_flg $omp_flg -y sqrt -a lat,lon $outfile $outfile";
		$tst_cmd[2] = "ncks -C -H -s '%f' -v d2_00  $outfile"; 
		$nsr_xpc = "1.604304";
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}

	
	#################### begin ncecat benchmark 
	$opr_nm='ncecat';
	$dsc_sng = 'ncecat joining 2^5 files'; # skn_lgs.nc * 32 = 1.51GB
	####################	
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng, files=$fl_cnt\n";}
		$tst_cmd[0] = "ncecat -h -O $nco_D_flg $omp_flg -n $fl_cnt,2,1 -p $fl_pth skn_lgs_00.nc $outfile";
		$tst_cmd[1] = "ncwa -h -O $nco_D_flg $omp_flg  $outfile $outfile";
		$tst_cmd[2] = "ncks -C -H -s '%f' -v PO2  $outfile"; 
		# following required due to shortened length of test under dap.
		if ($dodap eq "FALSE") { $nsr_xpc = "12.759310";}
		else { $nsr_xpc = "18.106375";}
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### begin ncflint benchmark  - needs to be verified and md5/wc sums created.
	$opr_nm='ncflint';
	$dsc_sng = 'ncflint weight-averaging 2 files';
	####################	
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncflint -h -O $nco_D_flg   -w '0.5' -p $fl_pth ipcc_dly_T85_00.nc  ipcc_dly_T85_01.nc  $outfile";
		if($dbg_lvl > 2){print "entire cmd: $tst_cmd[0]\n";}
		$tst_cmd[1] = "ncwa -h -O $nco_D_flg $omp_flg -y sqrt -a lat,lon $outfile $outfile";	
		$tst_cmd[2] = "ncks -C -H -s '%f ' -v d1_00  $outfile"; 
		$nsr_xpc = "1.800000 1.800000 1.800000 1.800000 1.800000 1.800000 1.800000 1.800000"; 
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### begin ncpdq benchmark - reversal
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq dimension-order reversal';
	####################	
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		#!!WARN - change back to testing the ipcc file after verify
		$tst_cmd[0] = "ncpdq -h -O $nco_D_flg $omp_flg -a -time -lev -lat -lon -p $fl_pth  ipcc_dly_T85.nc  $outfile";
		# ~2m on sand for ipcc_dly_T85.nc
		$tst_cmd[1] = "ncks -C -H -s \"%f\" -v dopey $outfile";  #ipcc
		$nsr_xpc = "0.800000"; #ipcc
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### next ncpdq benchmark - re-ordering
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq dimension-order re-ordering';
	####################	
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncpdq -h -O $nco_D_flg $omp_flg -a 'lon,time,lev,lat' -p $fl_pth  ipcc_dly_T85.nc  $outfile";
		$tst_cmd[1] = "ncks -C -H -s \"%f\" -v dopey $outfile";  #ipcc
		$nsr_xpc = "0.800000"; #ipcc
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### next ncpdq benchmark - re-ordering & reversing
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq dimension-order re-ordering & reversing';
	####################	
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncpdq -h -O $nco_D_flg $omp_flg -a '-lon,-time,-lev,-lat' -p $fl_pth  ipcc_dly_T85.nc  $outfile";
		$tst_cmd[1] = "ncks -C -H -s \"%f\" -v dopey $outfile";  #ipcc
		$nsr_xpc = "0.800000"; #ipcc
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### next ncpdq benchmark 
	$opr_nm='ncpdq';
	$dsc_sng = 'ncpdq packing a file';
	####################	
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncpdq -h -O $nco_D_flg $omp_flg -P all_new  -p $fl_pth  ipcc_dly_T85.nc  $outfile";
		$tst_cmd[1] = "ncks -C -H -s \"%f\" -v dopey $outfile"; 
		$nsr_xpc = "0.000000";
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	################### Set up the symlinks ###################
	if ($dbg_lvl > 0 && $dodap eq "FALSE") {print "\n\nSetting up local symlinks for test nc files\n";}
	if ($dodap eq "FALSE") {
		for (my $f=0; $f<$NUM_FLS; $f++) {
			my $rel_fl = "$dta_dir/$fl_cr8_dat[$f][2]" . ".nc" ;
			my $ldz = "0"; # leading zero for #s < 10
			if ($dbg_lvl > 0) {print "\tsymlinking $rel_fl\n";}
			for (my $n=0; $n<32; $n ++) {
				if ($n>9) {$ldz ="";}
				my $lnk_fl_nme = "$dta_dir/$fl_cr8_dat[$f][2]" . "_" . "$ldz" . "$n" . ".nc";
				if (-r $rel_fl && -d $dta_dir && -w $dta_dir){
					symlink $rel_fl, $lnk_fl_nme;
				}
			}
		}
	} elsif ($dbg_lvl > 0) {print "\n\nSkipping local symlinks for test nc files (using remote files).\n";}


	#################### begin cz benchmark list #2
	$opr_nm='ncra';
	$dsc_sng = 'ncra time-averaging 2^5 (i.e. one month) ipcc files';
	####################	
	if ($notbodi) { # too big for bodi
		if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#			if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
			$tst_cmd[0] = "ncra -h -O $nco_D_flg $omp_flg -n $fl_cnt,2,1 -p $fl_pth ipcc_dly_T85_00.nc $outfile";
			# ~4m on sand.
			$tst_cmd[1] =  "ncks -C -H -s '%f' -v d1_03    $outfile ";
			$nsr_xpc = "1.800001";
			go();
			if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
		} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}
	}


	if ($dodap eq "FALSE") { # only if not being done by remote
	#################### begin ncrcat benchmark 
	$opr_nm='ncrcat';
	$dsc_sng = 'ncrcat joining 2^5 files'; # skn_lgs.nc * 32 = 1.51GB
	####################	
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncrcat -h -O $nco_D_flg $omp_flg -n $fl_cnt,2,1 -p $fl_pth skn_lgs_00.nc $outfile";
		$tst_cmd[1] = "ncwa -h -O $nco_D_flg $omp_flg  $outfile $outfile";
		$tst_cmd[2] = "ncks -C -H -s '%f' -v PO2  $outfile"; 
		$nsr_xpc = "12.759310";
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
		} else {
			print "\nNB: ncrcat benchmark skipped for OpenDAP test - takes too long.\n\n";
		}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	#################### begin ncwa benchmark list #1a
	$opr_nm='ncwa';
	$dsc_sng = 'ncwa averaging all variables to scalars - stl_5km.nc & sqrt';
	####################	
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncwa -h -O $nco_D_flg $omp_flg -y sqrt -a lat,lon -p $fl_pth stl_5km.nc $outfile";
		$tst_cmd[1] = "ncks -C -H -s '%f' -v d2_02  $outfile";
		$nsr_xpc = "1.604304";  # was 1.974842
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	# following fails on numeric cmp but why should the result the same?
	#################### begin ncwa benchmark list #1b
	$opr_nm='ncwa';
	$dsc_sng = 'ncwa averaging all variables to scalars - stl_5km.nc & rms';
	####################
	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
#		if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
		$tst_cmd[0] = "ncwa -h -O $nco_D_flg $omp_flg -y rms -a lat,lon -p $fl_pth stl_5km.nc $outfile";
		$tst_cmd[1] = "ncks -C -H -s '%f' -v d2_02  $outfile";
		$nsr_xpc = "2.826392"; # past result = 3.939694
		go();
		if($dbg_lvl > 0){print "\n[past benchmark stanza - $dsc_sng\n";}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}


	if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_sng_mpi =~ /$opr_nm/)) {
		if ($notbodi) { #  ipcc too big for bodi
			#################### begin ncwa benchmark list #1c
			$opr_nm='ncwa';
			$dsc_sng = 'ncwa averaging all variables to scalars - ipcc_dly_T85.nc & sqt';
			####################
#			if ($dbg_lvl > 0) {print "\nBenchmark:  $dsc_sng\n";}
			$tst_cmd[0] = "ncwa -h -O $nco_D_flg $omp_flg -y sqrt   -a lat,lon -p $fl_pth ipcc_dly_T85.nc $outfile";
			$tst_cmd[1] = "ncks -C -H -s '%f' -v skanky  $outfile"; 
			$nsr_xpc = "0.800000";
			go();
		}
	} else {print "Skipping Benchmark [$opr_nm] - not MPI-ready\n";}
	
	# and summarize the benchmarks results
	smrz_rgr_rslt(); 
}


sub go {
	
# Perform tests of requested operator; default is all
	if (!defined $tst_nbr{$opr_nm}) { 
		@tst_cmd=();  # Clear test array
		# and init the timing hashes
		$real_tme{$opr_nm} = 0;
		$usr_tme{$opr_nm}  = 0;
		$sys_tme{$opr_nm}  = 0;
		return; 
	}
	
	$subbenchmarks{$opr_nm} = 0;
	$tst_nbr{$opr_nm}++;
	my $tst_cmdcnt = 0;
	my $t = 0;
	my $lst_cmd = @tst_cmd;
	$lst_cmd--;
	
	
	&verbosity("\n\n====================================================================\nResult Stanza for [$opr_nm] ($dsc_sng)\nSubtest [$tst_nbr{$opr_nm}] :\n===========\n");
	
	# csz++
	$dot_nbr_min=3; # Minimum number of dots between description and "ok" result
	$dot_sng='.....................................................................';
	$dsc_lng_max=50; # Maximum length of description printed
	$opr_lng_max=7; # Maximum length of operator name
	$spc_nbr_min=1; # Minimum number of spaces between test ID and description
	$spc_sng='       ';
	
	$dot_nbr=$dot_nbr_min; # Number of dots printed
	$spc_nbr=$spc_nbr_min; # Number of spaces printed
	$opr_fmt=sprintf("%%.%ds",$opr_lng_max);
	if($opr_lng_max-length($opr_nm)>0){$spc_nbr+=$opr_lng_max-length($opr_nm);}
	$spc_fmt=sprintf("%%.%ds",$spc_nbr);
	$tst_id_sng=sprintf("$opr_fmt$spc_fmt",$opr_nm,$spc_sng).sprintf("test %02d: ",$tst_nbr{$opr_nm}); 
	if($dsc_lng_max-length($dsc_sng)>0){$dot_nbr+=$dsc_lng_max-length($dsc_sng);}
	$dsc_fmt=sprintf('%%.%ds',$dsc_lng_max);
	$dot_fmt=sprintf("%%.%ds",$dot_nbr);
	$tst_fmt="$tst_id_sng$dsc_fmt$dot_fmt";
	printf STDERR ($tst_fmt,$dsc_sng,$dot_sng);
	# csz--	
	
	foreach (@tst_cmd){
		my $md5_chk = 1;
		&verbosity("\n\n## Part $tst_cmdcnt ## \n");
		if ($_ !~ /foo.nc/) {$md5_chk = 0;}
		my $opcnt = 0;
		my $md5_dsc_sng = $dsc_sng . "_$tst_cmdcnt";
		# Add $prefix only to NCO operator commands, not things like 'cut'.  
		foreach my $op (@opr_lst_all) { 
			if ($_ =~ m/^$op/ ) { # if the op is in the main list
				if ($mpi_prc > 0 && $opr_sng_mpi =~ /$op/) { $_ = $mpi_prfx . $_; } # and in the mpi list
				else { $_ = $prefix . $_; } # the std prefix
			}
		}
		
		# Perform an individual test
		&verbosity("  Full commandline:\n   $_\n");
		# timing code using Time::HiRes
		my $t0;
		if ($hiresfound) {$t0 = [gettimeofday];}
		else {$t0 = time;}
		#####################################################################################
		# and execute the command, splitting off stderr to file 'nco-stderror'
		$result=`($_) 2> nco-stderror`; # stderr should contain timing info if it exists.
		#####################################################################################
		chomp $result;
		if ($timed) {
			$sys_time = `cat nco-stderror`;
			$sys_time =~ s/\n/ /g;
			@sys_tim_arr = split(" ", $sys_time); # [0]real [1]0.00 [2]user [3]0.00 [4]sys [5]0.00
			$real_tme{$opr_nm} += $sys_tim_arr[1];
			$usr_tme{$opr_nm}  += $sys_tim_arr[3];
			$sys_tme{$opr_nm}  += $sys_tim_arr[5];
		}
#my $wait = <STDIN>;
		my $elapsed;
		if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday]);}
		else {$elapsed = time - $t0;}
		
		#print "inter benchmark for $opr_nm = $subbenchmarks{$opr_nm} \n";
		$subbenchmarks{$opr_nm} += $elapsed;
#		$tst_idx = $tst_nbr{$opr_nm}-1;
		if($dbg_lvl > 3){print "\t$opr_nm subtest [$t] took $elapsed seconds\n";}
		&verbosity("  Result = $result");
		
		#and here, check results by md5 checksum for each step - insert guts of check_nco_results()
		# have to mod the input string -  suffix with the cycle#
		# follow check only if the MD5 module is present, there's a foo.nc to check ($outfile = 'foo.nc')
		# & non-terminal cmd (the terminal command is ncks which is expected to return a single value or string)
		if ($md5 && $md5_chk && $tst_cmdcnt < $lst_cmd) {check_nco_results($outfile, $md5_dsc_sng);} 
		if ($md5_chk == 0 && $dbg_lvl > 0) {print "warn: No MD5/wc check on intermediate file.\n";}
		
		# else the oldstyle check has already been done and the results are in $result, so process normally
		$tst_cmdcnt++;
		if ($dbg_lvl > 2) {
			print "\ngo: test cycle held - hit <Enter> to continue\n"; 
			my $wait = <STDIN>;
		}

	} # end loop over sub-tests
	 
	if($dbg_lvl > 2){print STDERR "\tTotal time for $opr_nm [$tst_nbr{$opr_nm}] = $subbenchmarks{$opr_nm} s\n\n";}
	$totbenchmarks{$opr_nm}+=$subbenchmarks{$opr_nm};
    
	 # this comparing of the results shouldn't even be necessary as we're validating the whole file,
	 # not just a single value.
	chomp $result;  # Remove trailing newline for easier regex comparison
    
	# Compare numeric results
	if ($nsr_xpc =~/\d/) { # && it equals the expected value
		if ($dbg_lvl > 1){print STDERR "\$nsr_xpc assumed to be numeric: $nsr_xpc\n\$result = $result\n";}
		if ($nsr_xpc == $result) {
			$success{$opr_nm}++;
			printf STDERR (" SVn ok\n");
	 		&verbosity("\n   PASSED - Numeric output: [$opr_nm]:\n");
		} elsif (abs($result - $nsr_xpc) < 0.02) {
			$success{$opr_nm}++;
			printf STDERR (" SVn prov. ok\n");
	 		&verbosity("\n   PASSED PROVISIONALLY ($nsr_xpc vs $result) - Numeric output: [$opr_nm]:\n");
		} else {
      	&failed($nsr_xpc);
			&verbosity("\n   FAILED (expected: $nsr_xpc vs result: $result)- Numeric output: [$opr_nm]:\n");
		}
	} elsif ($nsr_xpc =~/\D/)  {# Compare non-numeric tests
		if ($dbg_lvl > 1){print STDERR "\$nsr_xpc assumed to be alphabetic: $nsr_xpc\n\$result = $result\n";}
		# Compare $result with $nsr_xpc
		if (substr($result,0,length($nsr_xpc)) eq $nsr_xpc) {
			$success{$opr_nm}++;
			printf STDERR (" SVa ok\n");
			&verbosity("\n   PASSED Alphabetic output: [$opr_nm]:\n");
		} else {
			&failed($nsr_xpc);
			&verbosity("\n   FAILED (expected: $nsr_xpc vs result: $result) Alphabetic output: [$opr_nm]\n");
		}
	}  else {  # No result at all?
		&failed();
		&verbosity("\n   FAILED - No result from [$opr_nm]\n");
	}
	@tst_cmd=(); # Clear test
	print Total $totbenchmarks{$opr_nm}
} # end go()

sub perform_tests
{
# Tests are in alphabetical order by operator name
    
# The following tests are organized and laid out as follows:
# - $tst_cmd[] holds command lines for each operator being tested
# - $dsc_sng holds test description line
# - $nsr_xpc is string or expression that is correct result of command
# - go() is function which executes each test
my $in_pth = " -p ../data ";

if ($dodap ne "" && $fl_pth =~ /http/ ) { $in_pth = " -p $fl_pth "; }
if ($dodap eq "") { $in_pth = " -p  http://sand.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata "; } 

#if (0) {
####################
#### ncap tests ####
####################
    $opr_nm='ncap';
#################### 
	$tst_cmd[0]="ncap -h -O $nco_D_flg -v -S ncap.in $in_pth in.nc  $outfile";
	$dsc_sng="running ncap.in script into nco_tst.pl";
	$nsr_xpc ="ncap: WARNING Replacing missing value data in variable val_half_half";
	&go();
	
	$tst_cmd[0]="ncap -h -O $nco_D_flg -C -v -s 'tpt_mod=tpt%273.0f' $in_pth in.nc  $outfile";
	$tst_cmd[1]="ncks -C -H -s '%.1f'  $outfile";
	$dsc_sng="Testing float modulo float";
	$nsr_xpc ="0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0";
	&go();
	
	$tst_cmd[0]="ncap -h -O $nco_D_flg -C -v -s 'foo=log(e_flt)^1' $in_pth in.nc  $outfile";
	$tst_cmd[1]="ncks -C -H -s '%.6f\n'  $outfile";
	$dsc_sng="Testing foo=log(e_flt)^1 (fails on AIX TODO ncap57)";
	$nsr_xpc ="1.000000";
	&go();
	
	$tst_cmd[0]="ncap -h -O $nco_D_flg -C -v -s 'foo=log(e_dbl)^1' $in_pth in.nc  $outfile";
	$tst_cmd[1]="ncks -C -H -s '%.12f\n'  $outfile";
	$dsc_sng="Testing foo=log(e_dbl)^1";
	$nsr_xpc ="1.000000000000";
	&go();
	
	$tst_cmd[0]="ncap -h -O $nco_D_flg -C -v -s 'foo=4*atan(1)' $in_pth in.nc  $outfile";
	$tst_cmd[1]="ncks -C -H -s '%.12f\n'  $outfile";
	$dsc_sng="Testing foo=4*atan(1)";
	$nsr_xpc ="3.141592741013";
	&go();
	
	$tst_cmd[0]="ncap -h -O $nco_D_flg -C -v -s 'foo=erf(1)' $in_pth in.nc  $outfile";
	$tst_cmd[1]="ncks -C -H -s '%.12f\n'  $outfile";
	$dsc_sng="Testing foo=erf(1)";
	$nsr_xpc ="0.842701";
	&go();
	
	#fails - wrong result
	$tst_cmd[0]="ncap -h -O $nco_D_flg -C -v -s 'foo=gamma(0.5)' $in_pth in.nc  $outfile";
	$tst_cmd[1]="ncks -C -H -s '%.12f\n'  $outfile";
	$dsc_sng="Testing foo=gamma(0.5)";
	$nsr_xpc ="1.772453851";
	&go();
	
	$tst_cmd[0]="ncap -h -O $nco_D_flg -C -v -s 'pi=4*atan(1);foo=sin(pi/2)' $in_pth in.nc  $outfile";
	$tst_cmd[1]="ncks -C -H -v foo -s '%.12f\n'  $outfile";
	$dsc_sng="Testing foo=sin(pi/2)";
	$nsr_xpc ="1.000000000000";
	&go();
	
	$tst_cmd[0]="ncap -h -O $nco_D_flg -C -v -s 'pi=4*atan(1);foo=cos(pi)' $in_pth in.nc  $outfile";
	$tst_cmd[1]="ncks -C -H -v foo -s '%.12f\n'  $outfile";
	$dsc_sng="Testing foo=cos(pi)";
	$nsr_xpc ="-1.000000000000";
	&go();

####################
#### ncatted tests #
####################
    $opr_nm="ncatted";
####################
	$tst_cmd[0]="ncatted -h -O $nco_D_flg -a units,,m,c,'meter second-1' $in_pth in.nc  $outfile";
	$tst_cmd[1]="ncks -C -H -s '%s' -v lev  $outfile | grep units | cut -d' ' -f 11-12";
	$dsc_sng="Modify all existing units attributes to meter second-1";
	$nsr_xpc="meter second-1";
	&go();
	 
####################
#### ncbo tests ####
####################
    $opr_nm="ncbo";
####################
if ($mpi_prc == 0 || ($mpi_prc > 0 && $opr_rgr_mpi =~ /$opr_nm/)) {
	$tst_cmd[0]="ncbo $omp_flg -h -O $nco_D_flg --op_typ='-' -v mss_val_scl $in_pth in.nc in.nc $outfile";;
	$tst_cmd[1]="ncks -C -H -s '%g' -v mss_val_scl $outfile";
	$dsc_sng="difference scalar missing value";
	$nsr_xpc= 1.0e36 ; 
	&go();
	
	$tst_cmd[0]="ncbo $omp_flg -h -O $nco_D_flg --op_typ='-' -d lon,1 -v mss_val  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v mss_val $outfile";
	$dsc_sng="difference with missing value attribute";
	$nsr_xpc= 1.0e36 ; 
	&go();
	
	$tst_cmd[0]="ncbo $omp_flg -h -O $nco_D_flg --op_typ='-' -d lon,0 -v no_mss_val  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v no_mss_val $outfile";
	$dsc_sng="difference without missing value attribute";
	$nsr_xpc= 0 ; 
	&go();
	
	$tst_cmd[0]="ncks -h -O $nco_D_flg -v mss_val_fst  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncrename -h -O $nco_D_flg -v mss_val_fst,mss_val $outfile";
	$tst_cmd[2]="ncbo $omp_flg  -h -O $nco_D_flg -y '-' -v mss_val  $outfile  ../data/in.nc $outfile 2> $foo_tst";
	$tst_cmd[3]="ncks -C -H -s '%f,' -v mss_val $outfile";
	$dsc_sng="missing_values differ between files";
	$nsr_xpc= "-999,-999,-999,-999" ; 
	&go();	 
	 
	$tst_cmd[0]="ncdiff $omp_flg -h -O $nco_D_flg -d lon,1 -v mss_val  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v mss_val $outfile";
	$dsc_sng="ncdiff symbolically linked to ncbo";
	$nsr_xpc= 1.0e36 ; 
	&go();
	
	$tst_cmd[0]="ncdiff $omp_flg -h -O $nco_D_flg -d lon,1 -v mss_val  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v mss_val $outfile";
	$dsc_sng="difference with missing value attribute";
	$nsr_xpc= 1.0e36 ; 
	&go();
	
	$tst_cmd[0]="ncdiff $omp_flg -h -O $nco_D_flg -d lon,0 -v no_mss_val  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v no_mss_val $outfile";
	$dsc_sng="difference without missing value attribute";
	$nsr_xpc= 0 ; 
	&go();

	
	if ($mpi_prc == 0) {  #FXM - commented 8.19.05 - mpncwa causes this to hang on following ncks
	$tst_cmd[0]="ncwa $omp_flg -C -h -O $nco_D_flg -v rec_var_flt_mss_val_dbl  $in_pth in.nc foo_avg.nc";
	$tst_cmd[1]="ncbo $omp_flg -C -h -O $nco_D_flg -v rec_var_flt_mss_val_dbl  $in_pth in.nc foo_avg.nc foo.nc";
	$tst_cmd[1]="ncks -C -H -d time,3 -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="Difference which tests broadcasting (TODO nco550,551,552)";
	$nsr_xpc= -1.0 ; 
	&go();   
	} else { print "NB: for MPI, last ncra test skipped due to ncbo failure.\n";}

}

####################
#### ncea tests ####
####################
    $opr_nm='ncea';
####################
	$tst_cmd[0]="ncra -Y ncea $omp_flg -h -O $nco_D_flg -v one_dmn_rec_var -d time,4  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v one_dmn_rec_var $outfile";
	$dsc_sng="ensemble mean of int across two files";
	$nsr_xpc= 5 ; 
	&go();
	
	$tst_cmd[0]="ncra -Y ncea $omp_flg -h -O $nco_D_flg -v rec_var_flt_mss_val_flt -d time,0  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v rec_var_flt_mss_val_flt $outfile";
	$dsc_sng="ensemble mean with missing values across two files";
	$nsr_xpc= 1.0e36 ; 
	&go();
	
	$tst_cmd[0]="ncra -Y ncea $omp_flg -h -O $nco_D_flg -y min -v rec_var_flt_mss_val_dbl -d time,1  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="ensemble min of float across two files";
	$nsr_xpc= 2 ; 
	&go();
	
	$tst_cmd[0]="ncra -Y ncea $omp_flg -h -O $nco_D_flg -C -v pck  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v pck $outfile";
	$dsc_sng="scale factor + add_offset packing/unpacking";
	$nsr_xpc= 3 ; 
	&go();
	
	$tst_cmd[0]="ncra -Y ncea $omp_flg -h -O $nco_D_flg -v rec_var_int_mss_val_int  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d ' -v rec_var_int_mss_val_int $outfile";
	$dsc_sng="ensemble mean of integer with integer missing values across two files";
	$nsr_xpc= '-999 2 3 4 5 6 7 8 -999 -999' ; 
	&go();

####################
## ncecat tests ####
####################
    $opr_nm='ncecat';
####################
	$tst_cmd[0]="ncks -h -O $nco_D_flg -v one  $in_pth in.nc $foo1_fl";
	$tst_cmd[1]="ncks -h -O $nco_D_flg -v one  $in_pth in.nc $foo2_fl";
	$tst_cmd[2]="ncecat $omp_flg -h -O $nco_D_flg $foo1_fl $foo2_fl $outfile";
	$tst_cmd[3]="ncks -C -H -s '%f, ' -v one $outfile";
	$dsc_sng="concatenate two files containing only scalar variables";
	$nsr_xpc= "1, 1" ; 
	&go();
    
####################
## ncflint tests ###
####################
    $opr_nm='ncflint';
####################
	$tst_cmd[0]="ncflint $omp_flg -h -O $nco_D_flg -w 3,-2 -v one  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v one $outfile";
	$dsc_sng="identity weighting";
	$nsr_xpc= 1.0 ; 
	&go();
	
	$tst_cmd[0]="ncrename -h -O $nco_D_flg -v zero,foo  $in_pth in.nc $foo1_fl";
	$tst_cmd[1]="ncrename -h -O $nco_D_flg -v one,foo  $in_pth in.nc $outfile";
	$tst_cmd[2]="ncflint $omp_flg -h -O $nco_D_flg -i foo,0.5 -v two $foo1_fl $outfile $outfile";
	$tst_cmd[3]="ncks -C -H -s '%e' -v two $outfile";
	$dsc_sng="identity interpolation";
	$nsr_xpc= 2.0 ; 
	&go();
	
	$tst_cmd[0]="ncks -h -O $nco_D_flg -C -d lon,1 -v mss_val  $in_pth in.nc $foo_x_fl";
	$tst_cmd[1]="ncks -h -O $nco_D_flg -C -d lon,0 -v mss_val  $in_pth in.nc $foo_y_fl";
	$tst_cmd[2]="ncflint $omp_flg -h -O $nco_D_flg -w 0.5,0.5 $foo_x_fl $foo_y_fl $foo_xy_fl";
	$tst_cmd[3]="ncflint $omp_flg -h -O $nco_D_flg -w 0.5,0.5 $foo_y_fl $foo_x_fl $foo_yx_fl";
	$tst_cmd[4]="ncdiff $omp_flg -h -O $nco_D_flg $foo_xy_fl $foo_yx_fl $foo_xymyx_fl";
	$tst_cmd[5]="ncks -C -H -s '%g' -v mss_val $foo_xymyx_fl";
	$dsc_sng="switch order of occurrence to test for commutivity";
	$nsr_xpc= 1e+36 ; 
	&go();

####################
#### ncks tests ####
####################
    $opr_nm='ncks';
####################
	$tst_cmd[0]="ncks -h -O $nco_D_flg -v lat_T42,lon_T42,gw_T42  $in_pth in.nc $foo_T42_fl";
	$tst_cmd[1]="ncrename -h -O $nco_D_flg -d lat_T42,lat -d lon_T42,lon -v lat_T42,lat -v gw_T42,gw -v lon_T42,lon $foo_T42_fl";
	$tst_cmd[2]="ncap -h -O $nco_D_flg -s 'one[lat,lon]=lat*lon*0.0+1.0' -s 'zero[lat,lon]=lat*lon*0.0' $foo_T42_fl $foo_T42_fl";
	$tst_cmd[3]="ncks -C -H -s '%g' -v one -F -d lon,128 -d lat,64 $foo_T42_fl";
	$nsr_xpc="1";
	$dsc_sng="Create T42 variable named one, uniformly 1.0 over globe in $foo_T42_fl ";
	&go();
		
	#passes, but returned string includes tailing NULLS (<nul> in nedit)
	$tst_cmd[0]="ncks -C -H -s '%c' -v fl_nm  $in_pth in.nc";
	$dsc_sng="extract filename string";
	$nsr_xpc= "/home/zender/nco/data/in.cdl" ;
	&go();
	
	$tst_cmd[0]="ncks -h -O $nco_D_flg -v lev  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f,' -v lev $outfile";
	$dsc_sng="extract a dimension";
	$nsr_xpc= "100.000000,500.000000,1000.000000" ; 
	&go();
	
	$tst_cmd[0]="ncks -h -O $nco_D_flg -v three_dmn_var  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,3,3 $outfile";
	$dsc_sng="extract a variable with limits";
	$nsr_xpc= 23;
	&go();
	
	$tst_cmd[0]="ncks -h -O $nco_D_flg -v int_var  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v int_var $outfile";
	$dsc_sng="extract variable of type NC_INT";
	$nsr_xpc= "10" ;
	&go();
	
	$tst_cmd[0]="ncks -h -O $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,0,0 -d lev,2,2 -d lon,0,,2 -d lon,1,,2  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var $outfile";
	$dsc_sng="Multi-slab lat and lon with srd";
	$nsr_xpc= "12.0,13.0,14.0,15.0,20.0,21.0,22.0,23.0";
	&go();
	
	$tst_cmd[0]="ncks -h -O $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0,3 -d lon,1,3  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var $outfile";
	$dsc_sng="Multi-slab with redundant hyperslabs";
	$nsr_xpc= "20.0,21.0,22.0,23.0";
	&go();
	
	$tst_cmd[0]="ncks -h -O $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0.,,2 -d lon,90.,,2  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var $outfile";
	$dsc_sng="Multi-slab with coordinates";
	$nsr_xpc= "20.0,21.0,22.0,23.0";
	&go();
	
	$tst_cmd[0]="ncks -h -O $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,800.,200. -d lon,270.,0.  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var $outfile";
	$dsc_sng="Double-wrapped hyperslab";
	$nsr_xpc= "23.0,20.0,15.0,12.0";
	&go();
    
# $tst_cmd[0]="ncks -h -O $nco_D_flg -C -v three_double_dmn -d lon,2,2 -d time,8,8  in.nc $outfile";
# $tst_cmd[1]="ncks -C -H -s '%f,' -v three_double_dmn $outfile";
# $dsc_sng="Hyperslab of a variable that has two identical dims";
# $nsr_xpc= 59.5;
# &go();
    
	$tst_cmd[0]="ncks -h -O $nco_D_flg -C -d time_udunits,'1999-12-08 12:00:0.0','1999-12-09 00:00:0.0'  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%6.0f' -d time_udunits,'1999-12-08 18:00:0.0','1999-12-09 12:00:0.0',2 -v time_udunits  $in_pth in.nc";
	$dsc_sng="dimension slice using UDUnits library (fails without UDUnits library support)";
	$nsr_xpc= 876018;
	&go();
	
	$tst_cmd[0]="ncks -h -O $nco_D_flg -C -H -v wvl -d wvl,'0.4 micron','0.7 micron' -s '%3.1e'  $in_pth in.nc";
	$dsc_sng="dimension slice using UDUnit conversion (fails without UDUnits library support)";
	$nsr_xpc= 1.0e-06;
	&go();
	
	#fails
	$tst_cmd[0]="ncks -h -O $nco_D_flg -C -v '^three_*'  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f'-C -v three $outfile";
	$dsc_sng="variable wildcards A (fails without regex library)";
	$nsr_xpc= 3 ;
	&go();
	
	$tst_cmd[0]="ncks -h -O $nco_D_flg -C -v '^[a-z]{3}_[a-z]{3}_[a-z]{3,}\$'  $in_pth in.nc $outfile";
	# for this test, the regex is mod'ed                       ^
	$tst_cmd[1]="ncks -C -H -s '%d' -C -v val_one_int $outfile";
	$dsc_sng="variable wildcards B (fails without regex library)";
	$nsr_xpc= 1;
	&go();
	
	$tst_cmd[0]="ncks -h -O $nco_D_flg -C -d time,0,1 -v time  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -C -d time,2, $outfile";
	$dsc_sng="Offset past end of file";
	$nsr_xpc="ncks: ERROR User-specified dimension index range 2 <= time <=  does not fall within valid dimension index range 0 <= time <= 1";
	&go();
	
	$tst_cmd[0]="ncks -C -H -s '%d' -v byte_var  $in_pth in.nc";
	$dsc_sng="Print byte value";
	$nsr_xpc= 122 ;
	&go();
	
####################
#### ncpdq tests ####
####################
    $opr_nm='ncpdq';
####################

	$tst_cmd[0]="ncpdq $omp_flg -h -O $nco_D_flg -a -lat -v lat  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v lat -d lat,0 $outfile";
	$dsc_sng="reverse coordinate";
	$nsr_xpc= 90 ; 
	&go();
	
	$tst_cmd[0]="ncpdq $omp_flg -h -O $nco_D_flg -a -lat,-lev,-lon -v three_dmn_var  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,0 -d lev,0 -d lon,0 $outfile";
	$dsc_sng="reverse three dimensional variable";
	$nsr_xpc= 23 ; 
	&go();
	
	$tst_cmd[0]="ncpdq $omp_flg -h -O $nco_D_flg -a lon,lat -v three_dmn_var  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,0 -d lev,2 -d lon,3 $outfile";
	$dsc_sng="re-order three dimensional variable";
	$nsr_xpc= 11 ; 
	&go();
	
	$tst_cmd[0]="ncpdq $omp_flg -h -O $nco_D_flg -P all_new -v upk  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncpdq $omp_flg -h -O $nco_D_flg -P upk -v upk $outfile $outfile";
	$tst_cmd[2]="ncks -C -H -s '%g' -v upk $outfile";
	$dsc_sng="Pack and then unpack scalar (uses only add_offset)";
	$nsr_xpc= 3 ; 
	&go();
    
####################
#### ncra tests ####
####################
    $opr_nm='ncra';
####################
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -v one_dmn_rec_var  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v one_dmn_rec_var $outfile";
	$dsc_sng="record mean of int across two files";
	$nsr_xpc= 5 ; 
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -v rec_var_flt_mss_val_dbl  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record mean of float with double missing values";
	$nsr_xpc= 5 ; 
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -v rec_var_flt_mss_val_int  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_int $outfile";
	$dsc_sng="record mean of float with integer missing values";
	$nsr_xpc= 5 ; 
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -v rec_var_int_mss_val_int  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_int $outfile";
	$dsc_sng="record mean of integer with integer missing values";
	$nsr_xpc= 5 ; 
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -v rec_var_int_mss_val_int  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_int $outfile";
	$dsc_sng="record mean of integer with integer missing values across two files (TODO nco543)";
	$nsr_xpc= 5 ; 
	&go();

	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -v rec_var_int_mss_val_flt  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_flt $outfile";
	$dsc_sng="record mean of integer with float missing values";
	$nsr_xpc= 5 ; 
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -v rec_var_int_mss_val_flt  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_flt $outfile";
	$dsc_sng="record mean of integer with float missing values across two files (TODO nco543)";
	$nsr_xpc= 5 ; 
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -v rec_var_dbl_mss_val_dbl_pck  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_dbl_mss_val_dbl_pck $outfile";
	$dsc_sng="record mean of packed double with double missing values";
	$nsr_xpc= 5 ;
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -v rec_var_dbl_pck  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_dbl_pck $outfile";
	$dsc_sng="record mean of packed double to test precision";
	$nsr_xpc= 100.55 ;
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -v rec_var_flt_pck  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%3.2f' -v rec_var_flt_pck $outfile";
	$dsc_sng="record mean of packed float to test precision";
	$nsr_xpc= 100.55 ;
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -y avg -v rec_var_flt_mss_val_dbl  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record mean of float with double missing values across two files";
	$nsr_xpc= 5 ; 
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -y min -v rec_var_flt_mss_val_dbl  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record min of float with double missing values across two files";
	$nsr_xpc= 2 ; 
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -y max -v rec_var_flt_mss_val_dbl  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record max of float with double missing values across two files";
	$nsr_xpc= 8 ; 
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -y ttl -v rec_var_flt_mss_val_dbl  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record ttl of float with double missing values across two files";
	$nsr_xpc= 70 ;
	&go();
	
	$tst_cmd[0]="ncra $omp_flg -h -O $nco_D_flg -y rms -v rec_var_flt_mss_val_dbl  $in_pth in.nc in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%1.5f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record rms of float with double missing values across two files";
	$nsr_xpc= 5.38516 ;
	&go();
		
	if ($mpi_prc == 0) {  #FXM - commented 8.19.05 - related to ncbo failure with diff sized files
	$outfile =  $foo1_fl;
	$tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $nco_D_flg -v rec_var_flt_mss_val_dbl  $in_pth in.nc in.nc $outfile 2>$foo_tst";
	$outfile =  $orig_outfile;
	$tst_cmd[1]="ncra $omp_flg -h -O $nco_D_flg -y avg -v rec_var_flt_mss_val_dbl  $in_pth in.nc in.nc $outfile";
	$tst_cmd[2]="ncwa $omp_flg -h -O $nco_D_flg -a time $outfile $outfile";
	$tst_cmd[3]="ncdiff $omp_flg -h -O $nco_D_flg -v rec_var_flt_mss_val_dbl $foo1_fl $outfile $outfile";
	$tst_cmd[4]="ncra $omp_flg -h -O $nco_D_flg -y rms -v rec_var_flt_mss_val_dbl $outfile $outfile";
	$tst_cmd[5]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="record sdn of float with double missing values across two files";
	$nsr_xpc= 2 ;
	&go();
   } else { print "NB: for MPI, last ncra test skipped due to ncbo failure.\n";}
    
####################
#### ncwa tests ####
####################
    $opr_nm='ncwa';
####################
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -a lat,lon -w gw -d lat,0.0,90.0 $foo_T42_fl $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v one $outfile";
	$dsc_sng="normalize by denominator upper hemisphere";
	$nsr_xpc= 1;
	&go();
    
#${MY_BIN_DIR}/ncwa -n $omp_flg -h -O $nco_D_flg -a lat,lon -w gw $foo_T42_fl$outfile";
#$tst_cmd[1]="ncks -C -H -s '%f' -v one $outfile";
#$dsc_sng="normalize by tally but not weight";
#$nsr_xpc= 0.0312495 ; 
#&go();
#${MY_BIN_DIR}/ncwa -W $omp_flg -h -O $nco_D_flg -a lat,lon -w gw $foo_T42_fl$outfile";
#$tst_cmd[1]="ncks -C -H -s '%f' -v one $outfile";
#$dsc_sng="normalize by weight but not tally";
#$nsr_xpc= 8192 ; 
#&go();
    
	$tst_cmd[0]="ncwa -N $omp_flg -h -O $nco_D_flg -a lat,lon -w gw  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v mask $outfile";
	$dsc_sng="do not normalize by denominator";
	$nsr_xpc= 50 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -a lon -v mss_val  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f' -v mss_val $outfile";
	$dsc_sng="average with missing value attribute";
	$nsr_xpc= 73 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -a lon -v no_mss_val  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%g' -v no_mss_val $outfile";
	$dsc_sng="average without missing value attribute";
	$nsr_xpc= 5.0e35 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -v lat -m lat -M 90.0 -T eq -a lat  $in_pth in.nc $outfile"; 
	$tst_cmd[1]="ncks -C -H -s '%e' -v lat $outfile";
	$dsc_sng="average masked coordinate";
	$nsr_xpc= 90.0 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -v lat_var -m lat -M 90.0 -T eq -a lat  $in_pth in.nc $outfile"; 
	$tst_cmd[1]="ncks -C -H -s '%e' -v lat_var $outfile";
	$dsc_sng="average masked variable";
	$nsr_xpc= 2.0 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -v lev -m lev -M 100.0 -T eq -a lev -w lev_wgt  $in_pth in.nc $outfile"; 
	$tst_cmd[1]="ncks -C -H -s '%e' -v lev $outfile";
	$dsc_sng="average masked, weighted coordinate";
	$nsr_xpc= 100.0 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -v lev_var -m lev -M 100.0 -T gt -a lev -w lev_wgt  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%e' -v lev_var $outfile";
	$dsc_sng="average masked, weighted variable";
	$nsr_xpc= 666.6667 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -v lat -a lat -w gw -d lat,0  $in_pth in.nc $outfile"; 
	$tst_cmd[1]="ncks -C -H -s '%e' -v lat $outfile";
	$dsc_sng="weight conforms to var first time";
	$nsr_xpc= -90.0 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -v mss_val_all -a lon -w lon  $in_pth in.nc $outfile"; 
	$tst_cmd[1]="ncks -C -H -s '%g' -v mss_val_all $outfile";
	$dsc_sng="average all missing values with weights";
	$nsr_xpc= 1.0e36 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -v val_one_mss -a lat -w wgt_one  $in_pth in.nc $outfile"; 
	$tst_cmd[1]="ncks -C -H -s '%e' -v val_one_mss $outfile";
	$dsc_sng="average some missing values with unity weights";
	$nsr_xpc= 1.0 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -v msk_prt_mss_prt -m msk_prt_mss_prt -M 1.0 -T lt -a lon  $in_pth in.nc $outfile"; 
	$tst_cmd[1]="ncks -C -H -s '%e' -v msk_prt_mss_prt $outfile";
	$dsc_sng="average masked variable with some missing values";
	$nsr_xpc= 0.5 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y min -v rec_var_flt_mss_val_dbl  $in_pth in.nc $outfile 2>$foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%e' -v rec_var_flt_mss_val_dbl $outfile";
	$dsc_sng="min switch on type double, some missing values";
	$nsr_xpc= 2 ; 
	&go();	 
		
	$tst_cmd[0]="ncwa $omp_flg  -h -O $nco_D_flg -y min -v three_dmn_var_dbl -a lon  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f,' -v three_dmn_var_dbl $outfile >$foo_fl";
	$tst_cmd[2]="cut -d, -f 7 $foo_fl";
	$dsc_sng="Dimension reduction with min switch and missing values";
	$nsr_xpc= -99 ;
	&go();
	$tst_cmd[0]="cut -d, -f 20 $foo_fl";
	$dsc_sng="Dimension reduction with min switch";
	$nsr_xpc= 77 ;
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y min -v three_dmn_var_int -a lon  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d,' -v three_dmn_var_int $outfile >$foo_fl";
	$tst_cmd[2]="cut -d, -f 5 $foo_fl";
	$dsc_sng="Dimension reduction on type int with min switch and missing values";
	$nsr_xpc= -99 ;
	&go();
	$tst_cmd[0]="cut -d, -f 7 $foo_fl";
	$dsc_sng="Dimension reduction on type int variable";
	$nsr_xpc= 25 ;
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y min -v three_dmn_var_sht -a lon  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%d,' -v three_dmn_var_sht $outfile >$foo_fl";
	$tst_cmd[2]="cut -d, -f 20 $foo_fl";
	$dsc_sng="Dimension reduction on type short variable with min switch and missing values";
	$nsr_xpc= -99 ;
	&go();
	$tst_cmd[0]="cut -d, -f 8 $foo_fl";
	$dsc_sng="Dimension reduction on type short variable";
	$nsr_xpc= 29 ;
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y min -v three_dmn_rec_var  $in_pth in.nc $outfile 2>$foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_rec_var $outfile";
	$dsc_sng="Dimension reduction with min flag on type float variable";
	$nsr_xpc= 1 ;
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y max -v four_dmn_rec_var  $in_pth in.nc $outfile 2> $foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%f' -v four_dmn_rec_var $outfile";
	$dsc_sng="Max flag on type float variable";
	$nsr_xpc= 240 ;
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y max -v three_dmn_var_dbl -a lat,lon  $in_pth in.nc $outfile";
	$tst_cmd[1]="ncks -C -H -s '%f,' -v three_dmn_var_dbl $outfile >$foo_fl";
	$tst_cmd[2]="cut -d, -f 4 $foo_fl";
	$dsc_sng="Dimension reduction on type double variable with max switch and missing values";
	$nsr_xpc= -99 ;
	&go();
	$tst_cmd[0]="cut -d, -f 5 $foo_fl";
	$dsc_sng="Dimension reduction on type double variable";
	$nsr_xpc= 40 ;
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y max -v three_dmn_var_int -a lat  $in_pth in.nc $outfile";;
	$tst_cmd[1]="ncks -C -H -s '%d,' -v three_dmn_var_int $outfile >$foo_fl";
	$tst_cmd[2]="cut -d, -f 9 $foo_fl";
	$dsc_sng="Dimension reduction on type int variable with min switch and missing values";
	$nsr_xpc= -99 ;
	&go();
	$tst_cmd[0]="cut -d, -f 13 $foo_fl";
	$dsc_sng="Dimension reduction on type int variable";
	$nsr_xpc= 29 ;
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y max -v three_dmn_var_sht -a lat  $in_pth in.nc $outfile";;
	$tst_cmd[1]="ncks -C -H -s '%d,' -v three_dmn_var_sht $outfile >$foo_fl";
	$tst_cmd[2]="cut -d, -f 37 $foo_fl";
	$dsc_sng="Dimension reduction on type short variable with max switch and missing values";
	$nsr_xpc= -99 ;
	&go();
	$tst_cmd[0]="cut -d, -f 33 $foo_fl";
	$dsc_sng="Dimension reduction on type short, max switch variable";
	$nsr_xpc= 69 ;
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y rms -w lat_wgt -v lat  $in_pth in.nc $outfile 2>$foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%f' -v lat $outfile";;
	$dsc_sng="rms with weights";
	$nsr_xpc= 90 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -w val_half_half -v val_one_one_int  $in_pth in.nc $outfile 2> $foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%ld' -v val_one_one_int $outfile";;
	$dsc_sng="weights would cause SIGFPE without dbl_prc patch";
	$nsr_xpc= 1 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y avg -v val_max_max_sht  $in_pth in.nc $outfile 2> $foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%d' -v val_max_max_sht $outfile";;
	$dsc_sng="avg would overflow without dbl_prc patch";
	$nsr_xpc= 17000 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y ttl -v val_max_max_sht  $in_pth in.nc $outfile 2> $foo_tst";
	$tst_cmd[1]="ncks -C -H -s '%d' -v val_max_max_sht $outfile";
	$dsc_sng="ttl would overflow without dbl_prc patch, wraps anyway so exact value not important (failure expected/OK on Xeon chips because of different wrap behavior)";
	$nsr_xpc= -31536 ; # Expected on Pentium IV GCC Debian 3.4.3-13, PowerPC xlc
#    $nsr_xpc= -32768 ; # Expected on Xeon GCC Fedora 3.4.2-6.fc3
#    $nsr_xpc= -32768 ; # Expected on PentiumIII (Coppermine) gcc 3.4 MEPIS 
	&go();

	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y min -a lat -v lat -w gw  $in_pth in.nc $outfile";;
	$tst_cmd[1]="ncks -C -H -s '%g' -v lat $outfile";;
	$dsc_sng="min with weights";
	$nsr_xpc= -900 ; 
	&go();
	
	$tst_cmd[0]="ncwa $omp_flg -h -O $nco_D_flg -y max -a lat -v lat -w gw  $in_pth in.nc $outfile";;
	$tst_cmd[1]="ncks -C -H -s '%g' -v lat $outfile";;
	$dsc_sng="max with weights";
	$nsr_xpc= 900 ; 
	&go();
	 
    
####################
##### net tests ####
####################
    $opr_nm='net';
####################
	$tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	$tst_cmd[1]="ncks -H -O $nco_D_flg -s '%e' -v one -p ftp://dust.ess.uci.edu/pub/zender/nco -l /tmp in.nc";
	$dsc_sng="Anonymous FTP protocol (requires anonymous FTP access to dust.ess.uci.edu)";
	$nsr_xpc= 1.000000e+00 ;
	&go();

	$tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	$tst_cmd[1]="ncks -H -O $nco_D_flg -s '%e' -v one -p sftp://dust.ess.uci.edu:/home/ftp/pub/zender/nco -l /tmp in.nc";
	$dsc_sng="Secure FTP (SFTP) protocol (requires SFTP access to dust.ess.uci.edu)";
	$nsr_xpc= 1.000000e+00 ;
	&go();

	$tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	$tst_cmd[1]="ncks -H -O $nco_D_flg  -s '%e' -v one -p $pth_rmt_scp_tst -l /tmp in.nc";
	$dsc_sng="SSH protocol (requires authorized SSH/scp access to dust.ess.uci.edu)";
	$nsr_xpc= 1 ;
	&go();

	$tst_cmd[0]="ncks -C -O -d lon,0 -s '%e' -v lon -p http://www.cdc.noaa.gov/cgi-bin/nph-nc/Datasets/ncep.reanalysis.dailyavgs/surface air.sig995.1975.nc";
	$dsc_sng="OPeNDAP protocol (requires OPeNDAP/DODS-enabled NCO)";
	$nsr_xpc= 0 ;
	&go();	
	
	if($USER eq 'zender'){	
	    $tst_cmd[0]="/bin/rm -f /tmp/etr_A4.SRESA1B_9.CCSM.atmd.2000_cat_2099.nc";
	    $tst_cmd[1]="ncks -H -O $nco_D_flg -s '%e' -d time,0 -v time -p ftp://climate.llnl.gov//sresa1b/atm/yr/etr/ncar_ccsm3_0/run9 -l /tmp etr_A4.SRESA1B_9.CCSM.atmd.2000_cat_2099.nc";
	    $dsc_sng="Password-protected FTP protocol (requires .netrc-based FTP access to climate.llnl.gov)";
	    $nsr_xpc= 182.5;
	    &go();

	    $tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	    $tst_cmd[1]="ncks -H -O $nco_D_flg -v one -p mss:/ZENDER/nc -l /tmp in.nc";
	    $tst_cmd[2]="ncks -C -H -s '%e' -v one $outfile";
	    $dsc_sng="msrcp protocol (requires msrcp and authorized access to NCAR MSS)";
	    $nsr_xpc= 1; 
	    &go();
	} else { print "Skipping net tests of mss: and password protected FTP protocol retrieval---user not zender\n";}

	if($USER eq 'zender' || $USER eq 'hjm'){	
	    $tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	    $tst_cmd[1]="ncks -H -O $nco_D_flg -s '%e' -v one -p wget://dust.ess.uci.edu/nco -l /tmp in.nc";
	    $dsc_sng="HTTP protocol (requires developers to implement wget in NCO nudge nudge wink wink)";
	    $nsr_xpc= 1; 
	    &go();
	} else { print "Skipping net test wget: protocol retrieval---user not zender or hjm\n";}
	
} # end of perform_test()

# usage - informational blurb for script
sub usage {
    print << 'USAGE';
    
Usage:
nco_bm.pl (options) [list of operators to test from the following list]

ncap ncatted ncbo ncflint ncea ncecat
ncks ncpdq ncra ncrcat ncrename ncwa net      (default tests all)

where (options) are:
    --usage || -h ...dumps this help
    --debug {1-3) ...puts the script into debug mode; emits more and (hopefully)
                     more useful info.  Also inserts the comparable -D flag into
                     the nco commandline.
    --caseid {short id string} 
                     this string can be used to identity and separate results 
                     from different runs.
    --dap {OPeNDAP url} ...retrieve test files from OPeNDAP server URL
    --opendap "               "
    --log ..........requests that the debug info is logged to 'nctest.log'
                     as well as spat to STDOUT.
    --mpi {#>0}.....number of MPI processes to spawn (incompatible with --thr_nbr)
    --udpreport.....requests that the test results are communicated back to
                     NCO Central to add your test, timing, and build results.
                            NB: This option uses udp port 29659 and may set off
                            firewall alarms if used unless that port is open.
    --test_files....requests the testing & excercise of the file creation script
                     'ncgen' and the Left Hand Casting ability of ncap.  
                            Currently gives the option to test 4 files of increasing
                            size and complexity:
                            0 - none; skip this section
                            1 - example Gene Expression  ~ 50MB  ~few sec
                            2 - small Satellite data set ~100MB  ~several sec
                            3 - 5km Satellite data set   ~300MB  ~min
                            4 - IPCC Daily T85 data set  ~  4GB  ~several min
                            A - All 
    --thr_nbr {#>0}....Number of OMP threads to use (incompatible with --mpi)
    --regress.......do the regression tests
    --benchmark.....do the benchmarks

nco_bm.pl is a semi-automated script for testing the accuracy and
robustness of the nco (netCDF Operators), typically after they are
built, using the 'make benchmark' command.  In this mode, a user should
never have to see this message, so this is all I'll say about it.

In nco debug/testing  mode, it tries to validate the NCO's for both
accuracy and robustness.  It also can collect benchmark statistics via
sending test results to a UDP server.

NB: When adding tests, be sure to use -O to overwrite files.
Otherwise, script hangs waiting for interactive response to
overwrite queries. Also, unless history is required, use '-h' to inhibit
appending history to file.

This script is part of the netCDF Operators package:
  http://nco.sourceforge.net

Copyright © 1994-2005 Charlie 'my surname is' Zender (surname@uci.edu)

USAGE
exit(0);
} # end usage()
    
####################
sub initialize($$){
	my $bch_flg; # [flg] Batch behavior
	my $dbg_lvl; # [flg] Debugging level
	($bch_flg,$dbg_lvl)=@_;
	# Enumerate operators to test
	@opr_lst_all = qw( ncap ncdiff ncatted ncbo ncflint ncea ncecat ncks ncpdq ncra ncrcat ncrename ncwa net );
	@opr_lst_mpi = qw( ncbo ncecat ncflint ncpdq ncra  ncwa ncpdq ncra);
	$opr_sng_mpi = "ncbo ncdiff ncecat ncflint ncwa ncpdq ncra"; # ncpdq ncra MPI, but fail bench
	$opr_rgr_mpi = "ncbo ncdiff ncecat ncflint ncpdq ncea ncrcat ncra ncwa"; # need all of them for regression 
	
	if (scalar @ARGV > 0){@opr_lst=@ARGV;}else{@opr_lst=@opr_lst_all;}
	if (defined $ENV{'MY_BIN_DIR'} &&  $ENV{'MY_BIN_DIR'} ne ""){$MY_BIN_DIR=$ENV{'MY_BIN_DIR'};}
	else{
		# Set and verify MY_BIN_DIR
		$MY_BIN_DIR=abs_path("../src/nco");
		printf "ENV var 'MY_BIN_DIR' not specified, so using default\n"; 
#		if($bch_flg){
#			die "unable to continue in batch mode without MY_BIN_DIR\n stopped\n";
#		}else{ # !bch_flg
#			$MY_BIN_DIR=abs_path("../src/nco");
#			printf "use $MY_BIN_DIR?\n['y' or specify] ";
#			my $ans = <STDIN>;
#			chomp $ans;
#			$MY_BIN_DIR = $ans unless (lc($ans) eq "y" || lc($ans) eq '');
#		} # !bch_flg
	} # !$MY_BIN_DIR
	if($dbg_lvl > 0){printf ("$prg_nm: initialize() reports \$MY_BIN_DIR = $MY_BIN_DIR, \$opr_lst[0] = $opr_lst[0], \@opr_lst=@opr_lst\n");}
	# Die if this path still does not work
	die "$MY_BIN_DIR/$opr_lst[0] does not exist\n stopped" unless (-e "$MY_BIN_DIR/$opr_lst[0]" || $opr_lst[0] eq "net");
	
	# Create symbolic links for testing
	# If libtool created shared libraries, then point to real executables 
	# in ../src/nco/.libs 
	my $dotlib = '';
	$dotlib = ".libs/lt-" if `head -1 $MY_BIN_DIR/ncatted` =~ m/sh/;
	$sym_link{ncdiff}=$dotlib . "ncbo";
	$sym_link{ncea}=$dotlib . "ncra";
	$sym_link{ncrcat}=$dotlib . "ncra";
	foreach(keys %sym_link) {
		system("cd $MY_BIN_DIR && ln -s -f $sym_link{$_} $_ || (/bin/rm -f $_ && ln -s -f $sym_link{$_} $_)");
	}

# Go to data directory where tests are actually run
	my $data_dir = "../data";
	chdir $data_dir or die "$OS_ERROR\n stopped";
	
	# Make sure in.nc exists, make it if possible, or die
	# ncgen is not part of 
	unless (-e "in.nc"){
		system("ncgen -o in.nc in.cdl") if (`which ncgen` and -e "in.cdl");
	} die "The netCDF file \"in.nc\" is necessary for testing NCO, however, it could not be found in \"$data_dir\".  Also, it could not be generated because \"ncgen\" could not be found in your path and/or the file \"$data_dir/in.cdl\" does not exist.\n stopped" unless (-e "in.nc");
	
	# Initialize hashes for each operator to test
	foreach(@opr_lst) {
		$tst_nbr{$_}=0;
		$success{$_}=0;
		$failure{$_}=0;
	}
} # end of initialize()

####################
sub verbosity {
	my $ts = shift;
	if($dbg_lvl > 0){printf ("$ts");}
	if($wnt_log){printf (LOG "$ts");}
} # end of verbosity()

####################

####################
sub failed {
	my $nsr_xpc = shift;  
	$failure{$opr_nm}++;
	print "FAILED\n$opr_nm failure: $dsc_sng\n";
	foreach(@tst_cmd) {print "\t$_\n";}
	if ($nsr_xpc) { print "\tResult: [$result] != Expected: [$nsr_xpc]\n" ; } 
	else { print "\tproduced no results\n"; }
	print "\t-----------\n";
	return;
}

####################
sub smrz_fl_cr8_rslt {
	if ($dbg_lvl > 0){print "Summarizing results of file creation\n";}
	my $CC = `../src/nco/ncks --compiler`;
	my $CCinfo = '';
	if ($CC =~ /gcc/) {$CCinfo = `gcc --version |grep -i gcc`;}
	elsif ($CC =~ /xlc/) {$CCinfo = "xlc version ??";}
	elsif ($CC =~ /icc/) {$CCinfo = "Intel C Compiler version ??";}
	my $reportstr = '';
	my $idstring = `uname -a` . "using: " . $CCinfo; chomp $idstring;
	my $udp_dat = "File Creation | $timestamp | $idstring | ";
	$reportstr .= "\n\nNCO File Creation Test Result Summary: [$timestamp]\n$idstring\n";
	$reportstr .=  "      Test                       Total Wallclock Time (s) \n";
	$reportstr .=  "=====================================================\n";

	for (my $i=0; $i<$NUM_FLS; $i++) {
		$reportstr .= sprintf "Creating   %15s:           %6.4f \n", $fl_tmg[$i][0], $fl_tmg[$i][1];
		$reportstr .= sprintf "Populating %15s:           %6.4f \n", $fl_tmg[$i][0], $fl_tmg[$i][2];
		$udp_dat   .= sprintf "%s : %6.4f : %6.4f",$fl_tmg[$i][0], $fl_tmg[$i][1], $fl_tmg[$i][2];
	}
	$reportstr .= sprintf "\n\n";
#	$udp_dat   .= sprintf "@"; # not needed anymore - ends up being a single line
	print $reportstr;
	if ($udp_rpt) { 
		$sock->send($udp_dat); 
		if ($dbg_lvl > 0) { print "File Creation: udp stream sent to $server_ip:\n$udp_dat\n";}
	} # and send it back separately
} # end of smrz_fl_cr8_rslt 


sub smrz_rgr_rslt {
	my $CC = `$MY_BIN_DIR/ncks --compiler`;
	my $CCinfo = '';
	if ($CC =~ /gcc/) {$CCinfo = `gcc --version |grep -i gcc`;}
	elsif ($CC =~ /xlc/) {$CCinfo = "xlc version ??";}
	elsif ($CC =~ /icc/) {
		my $icc_ver = `icc --version`; chomp $icc_ver;
		$CCinfo = "Intel C Compiler version $icc_ver";
	}
	chomp $CCinfo;
	my $idstring = `uname -a`; chomp($idstring);
	my $reportstr = "\n\n" . $idstring . "; " . $CCinfo . "; " . $timestamp . "\n";
	$reportstr .= "\n                    Test Results                Seconds to complete\n";    
	$reportstr .=      "             --------------------------   ----------------------------------------\n";    
	$reportstr .=      "      Test   Success    Failure   Total   WallClock    Real   User  System    Diff";
	if ($thr_nbr > 0) {$reportstr .= "   (OMP threads = $thr_nbr)\n";}
	else {$reportstr .= "\n";}
	my $udp_dat = $idstring . " using: " . $CCinfo . "|" . $cmd_ln . "|";
	foreach(@opr_lst) {
		my $total = $success{$_}+$failure{$_};
		my $fal_cnt = '';
		if ($failure{$_} == 0){	$fal_cnt = "   "; }
		else {$fal_cnt = sprintf "%3d", $failure{$_};}
		#printf "$_:\tsuccess: $success{$_} of $total\n";
		if ($total > 0) {
			my $io_tm = $real_tme{$_} - $usr_tme{$_} - $sys_tme{$_};
			$reportstr .= sprintf "%10s:      %3d        %3s     %3d     %6.2f   %6.2f %6.2f  %6.2f  %6.2f\n", $_, $success{$_},  $fal_cnt, $total, $totbenchmarks{$_}, $real_tme{$_}, $usr_tme{$_}, $sys_tme{$_}, $io_tm;
			
			$udp_dat   .= sprintf "%s %3d %3d %6.2f %6.2f %6.2f %6.2f %6.2f:",$_, $success{$_}, $total, $totbenchmarks{$_},$real_tme{$_}, $usr_tme{$_}, $sys_tme{$_}, $io_tm ;
			# above line uses whitespace sep, with ':' separating test names.
		}
	}
	$reportstr .= sprintf "\nNB:MD5: test passes MD5 checksum on file(s) May be more than one intermediate file.\nSVx: test passes single terminal value check SVn=numeric, SVa=alphabetic\n";
	chdir "../bld";
	if ($dbg_lvl == 0) {print $reportstr;}
	else { &verbosity($reportstr); }
#	$udp_dat .= "@";  # use an infrequent char as separator token
print "Regression: udp stream sent:\n$udp_dat\n";
	if ($udp_rpt) { 
		$sock->send($udp_dat); 
		if ($dbg_lvl > 0) { print "Regression: udp stream sent to $server_ip:\n$udp_dat\n";}
	}
} # end of sub smrz_rgr_rslt

sub set_dat_dir {
    my $tmp;
    my $datadir;
#	umask 0000;
    my $umask = umask;
    # does user have a DATA dir defined in his env?  It has to be readable and 
    # writable to be usable for these tests, so if it isn't just bail, with a nasty msg
	if ($xpt_dsc ne "") {
		$xpt_dsc =~ s/[^\w]/_/g;
#		print "converted idstring = $xpt_dsc\n";
	}

	if (defined $ENV{'DATA'} && $ENV{'DATA'} ne "") { # then is it readwritable?
		if (-w $ENV{'DATA'} && -r $ENV{'DATA'}) {
			if ($que == 0) {print "Using your environment variable DATA ($ENV{'DATA'}) as the root DATA directory for this series of tests\n\n";}
			if ($xpt_dsc ne "") {
				$dta_dir = "$ENV{'DATA'}/nco_bm/$xpt_dsc";
				my $err = `mkdir -p -m0777 $dta_dir`;
				if ($err ne "") {die "mkdir err: $dta_dir\n";}
				# mkdir "$dta_dir",0777 or die "Can't make $dta_dir\n";
			} else { # just dump it into nco_bm
				$dta_dir = "$ENV{'DATA'}/nco_bm";
				my $err = `mkdir -p -m0777 $dta_dir`;
				if ($err ne "") {die "mkdir err: $dta_dir\n";}
				#mkdir "$dta_dir",0777 or die "Can't make $dta_dir\n";
			}
		} else {
			die "You have defined a DATA dir ($ENV{'DATA'}) that cannot be written to or read\nfrom or both - please try again.\n stopped";
		}
	} elsif ($que == 0) {
		$tmp = 'notset';
		print "You do not have a DATA dir defined and some of the test files can be several GB. \nWhere would you like to write the test data?  It will be placed in the indicated directory,\nunder nco_bm, using the '--caseid' option to set the name of the subdirectory. \n[$ENV{'HOME'}] or specify: ";
		$tmp = <STDIN>;
		chomp $tmp;
		print "You entered [$tmp] \n";
		if ($tmp eq '') {
			$dta_dir = "$ENV{'HOME'}/nco_bm/$xpt_dsc";  # if $xpt_dsc not set, then it decays to $ENV{'HOME'}/nco_bm/
			if (-e "$dta_dir") {
				print "$dta_dir already exists - OK to re-use?\n[N/y] ";
				$tmp = <STDIN>;
				chomp $tmp;
				if ($tmp =~ "[nN]" || $tmp eq '') {
					die "\nFine - decide what to use and start over again - bye! stopped";
				} else { print "\n";	}
			} else { # have to make it
				print "Making $dta_dir & continuing\n";
				my $err = `mkdir -p -m0777 $dta_dir`;
				if ($err ne "") {die "mkdir err: $dta_dir\n";}
				#mkdir "$dta_dir",0777;
			}
		} else {
			$dta_dir = "$tmp/nco_bm/$xpt_dsc"; 
			# and now test it
			if (-w $dta_dir && -r $dta_dir) {
				print "OK - we will use [$dta_dir] to write to.\n\n";
			} else { # we'll have to make it
				print "[$dta_dir] doesn't exist - will try to make it.\n";
				my $err = `mkdir -p -m0777 $dta_dir`;
				if ($err ne "") {die "mkdir err: $dta_dir\n";}
				#mkdir("$dta_dir",0777);
				if (-w $dta_dir && -r $dta_dir) {
					print "OK - [$dta_dir] is available to write to\n";
				} else {
					die "ERROR - [$dta_dir] could not be made - check this and try again.\n stopped";
				}
			}
		}
	} else { # que != 0
		die "You MUST define a DATA environment variable to run this in a queue\n stopped";
	} # !defined $ENV{'DATA'})
} # end set_dat_dir()

sub fl_cr8_dat_init {
	for (my $i = 0; $i < $NUM_FLS; $i++) { $fl_tmg[$i][1] = $fl_tmg[$i][2] = " omitted "; }

#	$fl_cr8_dat[0][0] = "example gene expression"; # option descriptor
#	$fl_cr8_dat[0][1] = "~50MB";                   # file size
#	$fl_cr8_dat[0][2] = $fl_tmg[0][0] = "gne_exp";                 # file name root
#	$fl_cr8_dat[0][3] = "\'base[ge_atoms,rep,treat,cell,params]=5.67f\'";
	
	$fl_cr8_dat[0][0] = "long skinny file"; # option descriptor
	$fl_cr8_dat[0][1] = "~52MB";                   # file size
	$fl_cr8_dat[0][2] = $fl_tmg[0][0] = "skn_lgs";                 # file name root
	$fl_cr8_dat[0][3] = "\'time[time]=1.0f;hmdty[time]=98.3f;PO2[time]=18.7f;PCO2[time]=1.92f;PN2[time]=77.4f;w_vel[time]=14.8f;w_dir[time]=321.3f;temp[time]=23.5f;lmbda_260[time]=684.2f\'";
	
	$fl_cr8_dat[1][0] = "small satellite";         # option descriptor
	$fl_cr8_dat[1][1] = "~100MB";                  # file size
	$fl_cr8_dat[1][2] = $fl_tmg[1][0] = "sml_stl";                 # file name root
	$fl_cr8_dat[1][3] = "\'d2_00[lat,lon]=16.37f;d2_01[lat,lon]=2.8f;d2_02[lat,lon]=3.8f;\'";
	
	$fl_cr8_dat[2][0] = "5km satellite";           # option descriptor
	$fl_cr8_dat[2][1] = "~300MB";                  # file size
	$fl_cr8_dat[2][2] = $fl_tmg[2][0] = "stl_5km";                 # file name root
	$fl_cr8_dat[2][3] = "\'weepy=1.23456f;d2_00[lat,lon]=2.8f;d2_01[lat,lon]=2.8f;d2_02[lat,lon]=2.8f;d2_03[lat,lon]=2.8f;d2_04[lat,lon]=2.8f;d2_05[lat,lon]=2.8f;d2_06[lat,lon]=2.8f;d2_07[lat,lon]=2.8f;\'";
	
	$fl_cr8_dat[3][0] = "IPCC Daily";              # option descriptor
	$fl_cr8_dat[3][1] = "~4GB";                    # file size
	$fl_cr8_dat[3][2] = $fl_tmg[3][0] = "ipcc_dly_T85";            # file name root
	$fl_cr8_dat[3][3] = "\'weepy=0.8f;dopey=0.8f;sleepy=0.8f;grouchy=0.8f;sneezy=0.8f;doc=0.8f;wanky=0.8f;skanky=0.8f;d1_00[time]=1.8f;d1_01[time]=1.8f;d1_02[time]=1.8f;d1_03[time]=1.8f;d1_04[time]=1.8f;d1_05[time]=1.8f;d1_06[time]=1.8f;d1_07[time]=1.8f;d2_00[lat,lon]=16.2f;d2_01[lat,lon]=16.2f;d2_02[lat,lon]=16.2f;d2_03[lat,lon]=16.2f;d2_04[lat,lon]=16.2f;d2_05[lat,lon]=16.2f;d2_06[lat,lon]=16.2f;d2_07[lat,lon]=16.2f;d2_08[lat,lon]=16.2f;d2_09[lat,lon]=16.2f;d2_10[lat,lon]=16.2f;d2_11[lat,lon]=16.2f;d2_12[lat,lon]=16.2f;d2_13[lat,lon]=16.2f;d2_14[lat,lon]=16.2f;d2_15[lat,lon]=16.2f;d3_00[time,lat,lon]=64.0f;d3_01[time,lat,lon]=64.0f;d3_02[time,lat,lon]=64.0f;d3_03[time,lat,lon]=64.0f;d3_04[time,lat,lon]=64.0f;d3_05[time,lat,lon]=64.0f;d3_06[time,lat,lon]=64.0f;d3_07[time,lat,lon]=64.0f;d3_08[time,lat,lon]=64.0f;d3_09[time,lat,lon]=64.0f;d3_10[time,lat,lon]=64.0f;d3_11[time,lat,lon]=64.0f;d3_12[time,lat,lon]=64.0f;d3_13[time,lat,lon]=64.0f;d3_14[time,lat,lon]=64.0f;d3_15[time,lat,lon]=64.0f;d3_16[time,lat,lon]=64.0f;d3_17[time,lat,lon]=64.0f;d3_18[time,lat,lon]=64.0f;d3_19[time,lat,lon]=64.0f;d3_20[time,lat,lon]=64.0f;d3_21[time,lat,lon]=64.0f;d3_22[time,lat,lon]=64.0f;d3_23[time,lat,lon]=64.0f;d3_24[time,lat,lon]=64.0f;d3_25[time,lat,lon]=64.0f;d3_26[time,lat,lon]=64.0f;d3_27[time,lat,lon]=64.0f;d3_28[time,lat,lon]=64.0f;d3_29[time,lat,lon]=64.0f;d3_30[time,lat,lon]=64.0f;d3_31[time,lat,lon]=64.0f;d3_32[time,lat,lon]=64.0f;d3_33[time,lat,lon]=64.0f;d3_34[time,lat,lon]=64.0f;d3_35[time,lat,lon]=64.0f;d3_36[time,lat,lon]=64.0f;d3_37[time,lat,lon]=64.0f;d3_38[time,lat,lon]=64.0f;d3_39[time,lat,lon]=64.0f;d3_40[time,lat,lon]=64.0f;d3_41[time,lat,lon]=64.0f;d3_42[time,lat,lon]=64.0f;d3_43[time,lat,lon]=64.0f;d3_44[time,lat,lon]=64.0f;d3_45[time,lat,lon]=64.0f;d3_46[time,lat,lon]=64.0f;d3_47[time,lat,lon]=64.0f;d3_48[time,lat,lon]=64.0f;d3_49[time,lat,lon]=64.0f;d3_50[time,lat,lon]=64.0f;d3_51[time,lat,lon]=64.0f;d3_52[time,lat,lon]=64.0f;d3_53[time,lat,lon]=64.0f;d3_54[time,lat,lon]=64.0f;d3_55[time,lat,lon]=64.0f;d3_56[time,lat,lon]=64.0f;d3_57[time,lat,lon]=64.0f;d3_58[time,lat,lon]=64.0f;d3_59[time,lat,lon]=64.0f;d3_60[time,lat,lon]=64.0f;d3_61[time,lat,lon]=64.0f;d3_62[time,lat,lon]=64.0f;d3_63[time,lat,lon]=64.0f;d4_00[time,lev,lat,lon]=1.1f;d4_01[time,lev,lat,lon]=1.2f;d4_02[time,lev,lat,lon]=1.3f;d4_03[time,lev,lat,lon]=1.4f;d4_04[time,lev,lat,lon]=1.5f;d4_05[time,lev,lat,lon]=1.6f;d4_06[time,lev,lat,lon]=1.7f;d4_07[time,lev,lat,lon]=1.8f;d4_08[time,lev,lat,lon]=1.9f;d4_09[time,lev,lat,lon]=1.11f;d4_10[time,lev,lat,lon]=1.12f;d4_11[time,lev,lat,lon]=1.13f;d4_12[time,lev,lat,lon]=1.14f;d4_13[time,lev,lat,lon]=1.15f;d4_14[time,lev,lat,lon]=1.16f;d4_15[time,lev,lat,lon]=1.17f;d4_16[time,lev,lat,lon]=1.18f;d4_17[time,lev,lat,lon]=1.19f;d4_18[time,lev,lat,lon]=1.21f;d4_19[time,lev,lat,lon]=1.22f;d4_20[time,lev,lat,lon]=1.23f;d4_21[time,lev,lat,lon]=1.24f;d4_22[time,lev,lat,lon]=1.25f;d4_23[time,lev,lat,lon]=1.26f;d4_24[time,lev,lat,lon]=1.27f;d4_25[time,lev,lat,lon]=1.28f;d4_26[time,lev,lat,lon]=1.29f;d4_27[time,lev,lat,lon]=1.312f;d4_28[time,lev,lat,lon]=1.322f;d4_29[time,lev,lat,lon]=1.332f;d4_30[time,lev,lat,lon]=1.342f;d4_31[time,lev,lat,lon]=1.352f;\'";
}; # end of fl_cr8_dat_init()

# fl_cr8 creates populated files for the benchmarks
sub fl_cr8 {
	print "fl_cr8: prefix=$prefix\n";
	my $idx = shift;
	my $t0;
	my $elapsed;
	my $fl_in = my $fl_out = "$dta_dir/$fl_cr8_dat[$idx][2].nc" ;
	print "==== Creating $fl_cr8_dat[$idx][0] data file from template in [$dta_dir]\n";
	print "Executing: $tmr_app ncgen -b -o $fl_out $bm_dir/$fl_cr8_dat[$idx][2].cdl\n";
	if ($hiresfound) {$t0 = [gettimeofday];}
	else {$t0 = time;}
	# File creation now timed
	system  "$tmr_app ncgen -b -o $fl_out   $bm_dir/$fl_cr8_dat[$idx][2].cdl";
	if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday]);}
	else {$elapsed = time - $t0;}
	# log it to common timing array
	$fl_tmg[$idx][0] = "$fl_cr8_dat[$idx][2]"; # name root
	$fl_tmg[$idx][1] = $elapsed; # creation time	
	if ($idx == 0) { # skn_lgs needs some extra massaging
		if ($dbg_lvl > 0) {print "\nextra steps for skn_lgs - ncecat...\n";}
		system "$prefix/ncecat -O -h                         $fl_in    $fl_out";  # inserts a record dimension
		if ($dbg_lvl > 0) {print "\nncpdq...\n";}
		system "$prefix/ncpdq -O -h -a time,record             $fl_in    $fl_out"; # swaps time and 'record'
		if ($dbg_lvl > 0) {print "\nncwa...\n";}
		system "$prefix/ncwa -O -h -a record   $fl_in    $fl_out"; # renames 'record' out of the way
		# now skn_lgs ready for ncap'ing
	}
	print "\n==== Populating $fl_out file.\nTiming results:\n";
	#print "fl_cr8: prefix = $prefix\n";
	print "Executing: $tmr_app $prefix/ncap -h -O $nco_D_flg -s $fl_cr8_dat[$idx][3] $fl_in $fl_out\n";
	if ($hiresfound) {$t0 = [gettimeofday];}
	else {$t0 = time;}
	system "$tmr_app $prefix/ncap -O -h -s $fl_cr8_dat[$idx][3] $fl_in $fl_out";
	if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday]);}
	else {$elapsed = time - $t0;}
	$fl_tmg[$idx][2] = $elapsed; # population time
	print "==========================\nEnd of $fl_cr8_dat[$idx][2] section\n==========================\n\n\n";
} # end sub fl_cr8

sub check_nco_results {
	# taken substantially from process_tacg_results (in testtacg.pl), hjm
	my $file = shift;  # 1st arg
	my $testtype = shift; # 2nd arg
	my $prefix = "$MY_BIN_DIR"; $prfxd = 1; #embed the timer command and local bin in cmd
	my $cmdline = $_;
	my $return_value = $result; # this should be the return value of executing the non-terminal cmds
	my $hash = "";
	my @wc_lst;
	my $wc = "";
   if ($return_value != 0) { # nco.h:131:#define EXIT_SUCCESS 0
		print LOG "NonZero return value = $cmdline\n";
	} else {
		# 1st do an ncks dump on the 1st 111111 lines (will cause a sig13 due to the head cmd)
		system("$prefix/ncks -P  $file |head -111111  > $fl_pth/wc_out");
		@wc_lst = split(/\s+/, `wc $fl_pth/wc_out`);
		$wc = $wc_lst[1] . " " . $wc_lst[2] . " " . $wc_lst[3];

		# however we can do an md5 fo the entire output file 
		if ($md5found == 1) { 
			open(FILE, "$file") or die "Can't open $fl_pth/wc_out\n"; 
			binmode(FILE); 
			$hash = Digest::MD5->new->addfile(*FILE)->hexdigest ; 
		} else { $hash = "UNDEFINED"; }
		print LOG "\n\"$testtype\" => ", "\"$hash\", #MD5\n\"$testtype\" => ", "\"$wc\", #wc\n$cmdline\n\n";
	}
	if ($md5found == 1) { 
		if ( $MD5_table{$testtype} eq $hash ) { print " MD5"; verbosity " MD5"; } 
		else {
			print " MD5 fail,"; verbosity " MD5 fail,";  # test: $testtype\n";
			if ($dbg_lvl > 1) {verbosity "MD5 sig: $hash should be: $MD5_table{$testtype}\n";}
			if ($wc eq $wcTable{$testtype}) { print "WC PASS "; verbosity "WC PASS "; } 
			else { print " WC fail,"; verbosity " WC fail,"; }
			my $errfile = "$file" . ".MD5.err"; # will get overwritten; halt test if want to keep it.
			system("cp $file $errfile");
		}
	} else {
		if ($wcTable{$testtype} eq $wc) { print "passed wc \n"; verbosity "passed wc \n"; } 
		else {
			print " WC fail,";verbosity " WC fail,";
			my $errfile = "$testtype" . ".wc.err";
			system("cp $fl_pth/out $fl_pth/$errfile");
		}
	}
	return $hash;
} # end check_nco_results()

# tst_hirez just excercises the hirez perl timer to make sure that there's no funny biz going on.
sub tst_hirez{
	for (my $W=0; $W<50; $W++) {
		my $t0;
		if ($hiresfound) {$t0 = [gettimeofday];}
		else {$t0 = time;}
		my $E = 0.0;
		for (my $R=0; $R<999999; $R++) { my $E = $E * 1.788830478347; }
		my $elapsed;
		if ($hiresfound) {$elapsed = tv_interval($t0, [gettimeofday]);}
		else {$elapsed = time - $t0;}
		print " Run $W - Elapsed time =  $elapsed \n" ; 
	}
	if ($dbg_lvl == 1) {die "that's all folks!!\n stopped";}
}

