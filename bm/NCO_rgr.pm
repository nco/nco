package NCO_rgr;

# $Header: /data/zender/nco_20150216/nco/bm/NCO_rgr.pm,v 1.458 2014-02-20 17:08:58 zender Exp $

# Purpose: All REGRESSION tests for NCO operators
# BENCHMARKS are coded in "NCO_benchmarks.pm"

# NB: When adding tests, _be sure to use -O to overwrite files_
# Otherwise, script hangs waiting for interactive response to overwrite queries

require 5.6.1 or die "This script requires Perl version >= 5.6.1, stopped";
use English; # WCS96 p. 403 makes incomprehensible Perl errors sort of comprehensible
use Cwd 'abs_path';
use strict;

use NCO_bm qw(dbg_msg tst_run
	      $pfx_cmd $pfx_cmd_crr $drc_dat @fl_mtd_sct $opr_sng_mpi $opr_nm $dsc_sng $prsrv_fl $nsr_xpc $srv_sd
	      );

require Exporter;
our @ISA = qw(Exporter);
our @EXPORT = qw (
		  tst_rgr
		  $fl_out $dodap $dbg_lvl $pfx_cmd $pfx_cmd_crr $opr_sng_mpi $opr_nm $dsc_sng $prsrv_fl $nsr_xpc
		  $foo1_fl $foo_fl $foo_tst $fl_out_rgn $foo_avg_fl $foo_x_fl $foo_y_fl $foo_yx_fl
		  $foo_xy_fl $foo_xymyx_fl $pth_rmt_scp_tst $omp_flg $nco_D_flg %NCO_RC
		  );
use vars qw(
	    $dodap $dbg_lvl $dsc_sng $dust_usr $fl_fmt $fl_pth $foo1_fl $foo2_fl $foo_avg_fl
	    $foo_fl $foo_tst $foo_x_fl $foo_xy_fl
	    $foo_xymyx_fl $foo_y_fl $foo_yx_fl $mpi_prc $nco_D_flg $localhostname
	    $nsr_xpc $omp_flg $opr_nm $opr_rgr_mpi $fl_out_rgn
	    $fl_out $pth_rmt_scp_tst $prsrv_fl @tst_cmd $USER %NCO_RC
	    );

sub tst_rgr {
# Tests are in alphabetical order by operator name
    
# The following tests are organized and laid out as follows:
# - $tst_cmd[] holds command lines for each operator being tested
#   the last 2 lines are the expected value and the serverside string, either:
#       "NO_SS" - No Serverside allowed or (all regr are NO_SS still)
#       "SS_OK" - OK to send it serverside. (has to be requested with '--serverside'
# - $dsc_sng still holds test description line
# - tst_run() is function which executes each test
    
    my $in_pth="../data";
    my $in_pth_arg="-p $in_pth";
    $prsrv_fl=0;
    
# csz++
# fxm: pass as arguments or use exporter/importer instead?
    *dbg_lvl=*main::dbg_lvl;
    *dodap=*main::dodap;
    *fl_fmt=*main::fl_fmt;
    *fl_out=*main::fl_out;
    *localhostname=*main::localhostname;
    *nco_D_flg=*main::nco_D_flg;
    *omp_flg=*main::omp_flg;
    
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$dbg_lvl = $dbg_lvl");
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$dodap = $dodap");
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$fl_fmt = $fl_fmt");
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$fl_out = $fl_out");
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$localhostname = $localhostname");
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$nco_D_flg = $nco_D_flg");
    NCO_bm::dbg_msg(1,"in package NCO_rgr, \$omp_flg = $omp_flg");
# csz--
    
# in general, $fl_out    -> %tmp_fl_00%
#             $foo_fl     -> %tmp_fl_01%
#             $foo_tst    -> %tmp_fl_02%
#             $foo_avg_fl -> %tmp_fl_03%
#             $foo1_fl    -> %tmp_fl_01%
#             $foo2_fl    -> %tmp_fl_02%
    
    if($dodap ne "FALSE"){
	print "DEBUG: in tst_rgr(), \$dodap = $dodap \n";
	if ($dodap ne "" && $fl_pth =~ /http/ ) { $in_pth_arg = "-p $fl_pth"; }
	if ($dodap eq "") { $in_pth_arg = "-p http://dust.ess.uci.edu/cgi-bin/dods/nph-dods/dodsdata"; }
    }
    NCO_bm::dbg_msg(1,"-------------  REGRESSION TESTS STARTED from tst_rgr()  -------------");
    
    if(0){} #################  SKIP THESE #####################


# NCO 4.2.4
# Output for some tests vary when ENABLE_NETCDF4 is not set
# Add check for ENABLE_NETCDF4 by reading config.h 

# error messages for ENABLE_NETCDF4 not defined
my $ncks_msg_no_netcdf4 = "ncks: HINT: Obtain or build a netCDF4-enabled version of NCO.  Try, e.g., ./configure --enable-netcdf4 ...;make;make install";
my $ncecat_msg_no_netcdf4 = "ncecat: HINT: Obtain or build a netCDF4-enabled version of NCO.  Try, e.g., ./configure --enable-netcdf4 ...;make;make install";
# error messages for HAVE_NETCDF4_H not defined
my $ncks_msg_no_have_netcdf4 = "nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)";
# strings to find
my $find_undef_enable_netcdf4 = "undef ENABLE_NETCDF4";
my $find_define_enable_netcdf4 = "define ENABLE_NETCDF4";
my $find_undef_have_netcdf4 = "undef HAVE_NETCDF4_H";
my $find_define_have_netcdf4 = "define HAVE_NETCDF4_H";
my $ENABLE_NETCDF4=-1;
my $HAVE_NETCDF4_H=-1;
    
print "\n";

# Read config.h
my $use_config_h=0;
if ($use_config_h == 1){
	open FILE, "../config.h" or die $!;
	while (my $line = <FILE>) { 
	if ($line =~ /$find_undef_enable_netcdf4/){
	  print "INFO: ENABLE_NETCDF4 is disabled in config.h\n";
	  $ENABLE_NETCDF4=0;
	 }
	if ($line =~ /$find_define_enable_netcdf4/){
	  print "INFO: ENABLE_NETCDF4 is defined in config.h\n";
	  $ENABLE_NETCDF4=1;
	 }
	if ($line =~ /$find_undef_have_netcdf4/){
	  print "INFO: HAVE_NETCDF4_H is disabled in config.h\n";
	  $HAVE_NETCDF4_H=0;
	 }
	if ($line =~ /$find_define_have_netcdf4/){
	  print "INFO: HAVE_NETCDF4_H is defined in config.h\n";
	  $HAVE_NETCDF4_H=1;
	 } 
	}
} #$use_config_h

system("ncks --get_prg_info");
# system() runs a command and returns exit status information as a 16 bit value: 
# Low 7 bits are signal process died from, if any, and high 8 bits are actual exit value
# fxm: 20130125 deprecate this hack in favor of obtaining tokens directly from ncks
if( $? == -1 ){
    print "failed to execute: ncks --get_prg_info: $!\n";
}else{
  my $exit_value=$? >> 8;
  if ($exit_value==20) {$HAVE_NETCDF4_H=0;}else{$HAVE_NETCDF4_H=1;}
  if ($exit_value==30) {$ENABLE_NETCDF4=1;}else{$ENABLE_NETCDF4=0;}
}
print "\n";

####################
#### ncap2 tests ####
####################
    $opr_nm='ncap2';
####################
    
# This stanza will not map to the way the SS is done - needs a %stdouterr% added but all the rest of them
# have an ncks which triggers this addition from the sub tst_run() -> gnarly_pything.
# this stanza also requires a script on the SS.
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -S ncap2.in $in_pth_arg in.nc %tmp_fl_00% %stdouterr%";
    $dsc_sng="running ncap2.in script in nco_bm.pl (failure expected on netCDF4 ncap81)";
    $tst_cmd[1]="ncks -C -H -v b2 --no_blank -s '%d' %tmp_fl_00%";
    $tst_cmd[2]="999";
#	$tst_cmd[3]="NO_SS";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
# printf("paused @ [%s:%d]  - hit return to continue\n", __FILE__, __LINE__); my $wait = <STDIN>;
    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'tpt_mod=tpt%273.0f' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -v tpt_mod -s '%.1f ' %tmp_fl_00%";
    $dsc_sng="Testing float modulo float";
    $tst_cmd[2]="0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0 ";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#printf("paused @ [%s:%d]  - hit return to continue\n", __FILE__, __LINE__); my $wait = <STDIN>;

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=log(e_flt)^1' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -v foo -s '%.6f\\n' %tmp_fl_00%";
    $dsc_sng="Testing foo=log(e_flt)^1 (fails on AIX TODO ncap57)";
    $tst_cmd[2]="1.000000";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
#print "paused - hit return to continue"; my $wait = <STDIN>;
    
# where did e_dbl tst_run??  it's in in.cdl but gets lost thru the rgrs...?
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=log(e_dbl)^1' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%.12f\\n' %tmp_fl_00%";
    $dsc_sng="Testing foo=log(e_dbl)^1";
    $tst_cmd[2]="1.000000000000";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=4*atan(1)' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%.12f\\n' %tmp_fl_00%";
    $dsc_sng="Testing foo=4*atan(1)";
    $tst_cmd[2]="3.141592741013";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=erf(1)' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%.12f\\n' %tmp_fl_00%";
    $dsc_sng="Testing foo=erf(1) (fails on AIX TODO ncap57)";
    $tst_cmd[2]="0.842701";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
    #fails - wrong result ???
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'foo=gamma(0.5)' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%.12f\\n' %tmp_fl_00%";
    $dsc_sng="Testing foo=gamma(0.5) (fails on AIX TODO ncap57)";
    $tst_cmd[2]="1.772453851";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'pi=4*atan(1);foo=sin(pi/2)' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -v foo -s '%.12f\\n' %tmp_fl_00%";
    $dsc_sng="Testing foo=sin(pi/2)";
    $tst_cmd[2]="1.000000000000";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'pi=4*atan(1);foo=cos(pi)' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -v foo -s '%.12f\\n' %tmp_fl_00%";
    $dsc_sng="Testing foo=cos(pi)";
    $tst_cmd[2]="-1.000000000000";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array\
    
    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'defdim(\"a\",3);defdim(\"b\",4); a[\$a,\$b]=10;c=a(1,1);' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -v c -s '%i' %tmp_fl_00%";
    $dsc_sng="Casting variable with same name as dimension (failure expected on netCDF4 ncap81)";
    ##TODO ncap81
    $tst_cmd[2]="10";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -C -v -s 'defdim(\"a\",1); b[\$a]=10;c=b(0:0);' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncwa -h -O $fl_fmt $nco_D_flg -C -a a %tmp_fl_00% %tmp_fl_01%";
    $tst_cmd[2]="ncks -C -H -v b -s '%i' %tmp_fl_01%";
    $dsc_sng="Casting variable with a single dim of sz=1";
    ##TODO ncap81
    $tst_cmd[3]="10";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

# printf("paused @ %s:%d  - hit return to continue", __FILE__ , __LINE__); my $wait=<STDIN>;
    
    if ($dodap eq "FALSE") {
####################
#### ncatted tests #
####################
	$opr_nm="ncatted";
####################

#ncatted #1

	# FAILS!
	$tst_cmd[0]="ncatted -h -O $nco_D_flg -a units,,m,c,'meter second-1' $in_pth_arg in.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks -C -m -v lev %tmp_fl_00% | grep units | cut -d' ' -f 11-12"; ## daniel:fixme cut/ncks but how to do grep?
	$dsc_sng="Modify all existing units attributes to meter second-1";
	$tst_cmd[2]="meter second-1";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array
	
#printf("paused @ %s:%d  - hit return to continue", __FILE__ , __LINE__); my $wait = <STDIN>;

#ncatted #2
	
	$tst_cmd[0]="ncatted -h -O $nco_D_flg -a _FillValue,val_one_mss,m,f,0.0 $in_pth_arg in.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks -C -H -s '%g' -d lat,1 -v val_one_mss %tmp_fl_00%";
	$dsc_sng="Change _FillValue attribute from 1.0e36 to 0.0";
        ###TODO 665
	$tst_cmd[2]="0";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array

#ncatted #3

	$tst_cmd[0]="ncatted -h -O $nco_D_flg -a _FillValue,wgt_one,c,f,200.0 $in_pth_arg in.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks -C -H -s '%g' -d lat,1 -v wgt_one %tmp_fl_00%";
	$dsc_sng="Create new _FillValue attribute ";
	$tst_cmd[2]="1";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array

#ncatted #4

# Fragile: This test fails when length of command changes
	$tst_cmd[0]="ncatted -O --hdr_pad=1000 $nco_D_flg -a missing_value,val_one_mss,m,f,0.0 $in_pth_arg in.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks -M %tmp_fl_00% | grep hdr_pad | wc > %tmp_fl_01%";
	$tst_cmd[2]="cut -c 14-15 %tmp_fl_01%"; ## Daniel:fxm cut/ncks, but how to do grep and wc???
	$dsc_sng="Pad header with 1000B extra for future metadata (failure OK/expected since test depends on command-line length)";
	$tst_cmd[3]="27";
	$tst_cmd[4]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array

#ncatted #5

	$tst_cmd[0]="ncatted -O $nco_D_flg -a nw1,'^three*',c,i,999 $in_pth_arg in.nc %tmp_fl_00%";
	$tst_cmd[1]="ncap2 -v -C -O -s 'n2=three_dmn_var_int\@nw1;' %tmp_fl_00% %tmp_fl_01%";
	$tst_cmd[2]="ncks -O -C -H -s '%i'  -v n2 %tmp_fl_01%";
	$dsc_sng="Variable wildcarding (requires regex)";
	$tst_cmd[3]="999";
	$tst_cmd[4]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array
	
#4.3.8	
#ncatted #6
#ncatted -O -a purpose,rlev,m,c,new_value in_grp_3.nc out.nc

    if($HAVE_NETCDF4_H == 1){
	$tst_cmd[0]="ncatted -O $nco_D_flg -a purpose,rlev,m,c,new_value $in_pth_arg in_grp_3.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks -m -g g3 -v rlev %tmp_fl_00%";
	$dsc_sng="(Groups) Modify attribute for variable (input relative name)";
	$tst_cmd[2]="rlev attribute 0: purpose, size = 9 NC_CHAR, value = new_value";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array	
	
#ncatted #7	
	
	$tst_cmd[0]="ncatted -O $nco_D_flg -a purpose,/g3/rlev,m,c,new_value $in_pth_arg in_grp_3.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks -m -g g3 -v rlev %tmp_fl_00%";
	$dsc_sng="(Groups) Modify attribute for variable (input absolute name)";
	$tst_cmd[2]="rlev attribute 0: purpose, size = 9 NC_CHAR, value = new_value";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array	
	
#ncatted #8

	$tst_cmd[0]="ncatted -O $nco_D_flg -a g3_group_attribute,group,m,c,new_value $in_pth_arg in_grp_3.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks -M %tmp_fl_00% | grep g3_group_attribute";
	$dsc_sng="(Groups) Modify attribute for group (input relative name)";
	$tst_cmd[2]="Group attribute 0: g3_group_attribute, size = 9 NC_CHAR, value = new_value";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array	

#ncatted #9

	$tst_cmd[0]="ncatted -O $nco_D_flg -a nw1,'^three*',c,i,999 $in_pth_arg in_grp.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks -m -C -g g10 -v three_dmn_rec_var %tmp_fl_00%";
	$dsc_sng="(Groups) Variable wildcarding (requires regex)";
	$tst_cmd[2]="three_dmn_rec_var attribute 1: nw1, size = 1 NC_INT, value = 999";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array	

#ncatted #10

	$tst_cmd[0]="ncatted -O $nco_D_flg -a purpose,,m,c,new_value $in_pth_arg in_grp_3.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks -m -g g3 -v rlev %tmp_fl_00%";
	$dsc_sng="(Groups) Edit all variables";
	$tst_cmd[2]="rlev attribute 0: purpose, size = 9 NC_CHAR, value = new_value";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array

#ncatted #11

	$tst_cmd[0]="ncatted -O $nco_D_flg -a Conventions,group,m,c,new_value $in_pth_arg in_grp_3.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks -M %tmp_fl_00% | grep Conventions";
	$dsc_sng="(Groups) Modify global attribute";
	$tst_cmd[2]="Group attribute 0: Conventions, size = 9 NC_CHAR, value = new_value";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array		


	
    } # $HAVE_NETCDF4_H	
    }
    
# printf("paused @ %s:%d  - hit return to continue", __FILE__ , __LINE__); my $wait = <STDIN>;
    
####################
#### ncbo tests ####
####################
    $opr_nm="ncbo";
####################

# ncbo #1

    $tst_cmd[0]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_dbl_mss_val_dbl_pck $in_pth_arg in.nc in.nc %tmp_fl_00%";;
    $tst_cmd[1]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg %tmp_fl_00% %tmp_fl_03%";;
    $tst_cmd[2]="ncks -C -H -s '%g' -v rec_var_dbl_mss_val_dbl_pck %tmp_fl_03%";
    $dsc_sng="Difference a packed variable and be sure everything is zero or _FillValue by average whole remainder and comparing to zero.) FAILS with netCDF4";
    $tst_cmd[3]="0.0";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array - ok

# ncbo #2

    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a time $in_pth_arg in.nc %tmp_fl_03%";;
    $tst_cmd[1]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg $in_pth/in.nc %tmp_fl_03% %tmp_fl_00%";;
    $tst_cmd[2]="ncks -C -H -d time,9 -s '%g' -v rec_var_flt %tmp_fl_00%";
    $dsc_sng="Whole file difference with broadcasting (OK to fail here fxm TODO nco757. Works with --mmr_drt, triggers segfault on OSs like FC Linux which have C-library armor. Harmless Failure caused by free'ing dangling pointer during memory cleanup.)";
    $tst_cmd[3]="4.5";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array - ok

# ncbo #3    
# ncbo -h -O --op_typ='-' -v mss_val_scl  in.nc in.nc out.nc
    
    $tst_cmd[0]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg --op_typ='-' -v mss_val_scl $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H --no_blank -s '%g' -v mss_val_scl %tmp_fl_00%";
    $dsc_sng="difference scalar missing value";
    $tst_cmd[2]="1.0e36";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array - ok
    
# ncbo #4    
    
    $tst_cmd[0]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg --op_typ='-' -d lon,1 -v mss_val $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H --no_blank -s '%g' -v mss_val %tmp_fl_00%";
    $dsc_sng="difference with missing value attribute";
    $tst_cmd[2]=1.0e36;
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array ok
    
# ncbo #5  
    
    $tst_cmd[0]="ncbo $omp_flg -h -O $fl_fmt $nco_D_flg --op_typ='-' -d lon,0 -v no_mss_val $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v no_mss_val %tmp_fl_00%";
    $dsc_sng="difference without missing value attribute";
    $tst_cmd[2]="0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array ok
    
# ncbo #6    
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v mss_val_fst $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncrename -h -O $nco_D_flg -v mss_val_fst,mss_val %tmp_fl_00%";
    $tst_cmd[2]="ncbo -C $omp_flg -h -O $fl_fmt $nco_D_flg -y '-' -v mss_val %tmp_fl_00% ../data/in.nc %tmp_fl_01% 2> %tmp_fl_02%";
    $tst_cmd[3]="ncks -C -H --no_blank -s '%f,' -v mss_val %tmp_fl_01%";
    $dsc_sng="_FillValues differ between files";
    $tst_cmd[4]="-999.000000,-999.000000,-999.000000,-999.000000";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array ok
    
# ncbo #7   
    
    $tst_cmd[0]="ncrename -h $nco_D_flg -O -v no_mss_val,one_dmn_arr_with_and_without_mss_val $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncrename -h $nco_D_flg -O -v mss_val,one_dmn_arr_with_and_without_mss_val $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncbo $omp_flg  -h -O $fl_fmt $nco_D_flg -y '-' -v one_dmn_arr_with_and_without_mss_val %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% 2> %tmp_fl_03%";
    $tst_cmd[3]="ncks -C -H -d lon,0 -s '%f' -v one_dmn_arr_with_and_without_mss_val %tmp_fl_02%";
    $dsc_sng="_FillValue in one variable (not both variables)";
    $tst_cmd[4]=0.0;
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array ok
    
# ncbo #8    
    
    $tst_cmd[0]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg -d lon,1 -v mss_val $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H --no_blank -s '%g' -v mss_val %tmp_fl_00%";
    $dsc_sng="ncdiff symbolically linked to ncbo";
    $tst_cmd[2]=1.0e36;
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array ok
    
# ncbo #9    
    
    $tst_cmd[0]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg -d lon,1 -v mss_val $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H --no_blank -s '%g' -v mss_val %tmp_fl_00%";
    $dsc_sng="difference with missing value attribute";
    $tst_cmd[2]=1.0e36;
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array ok
    
# ncbo #10   
    
    $tst_cmd[0]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg -d lon,0 -v no_mss_val $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v no_mss_val %tmp_fl_00%";
    $dsc_sng="Difference without missing value attribute";
    $tst_cmd[2]="0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array ok
    
# ncbo #11   
    
    $tst_cmd[0]="ncwa $omp_flg -C -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc %tmp_fl_03%";
    $tst_cmd[1]="ncbo $omp_flg -C -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth/in.nc %tmp_fl_03% %tmp_fl_00%";
    $tst_cmd[2]="ncks -C -H -d time,3 -s '%f' -v rec_var_flt_mss_val_dbl %tmp_fl_00%";
    $dsc_sng="Difference which tests broadcasting and changing variable IDs";
    $tst_cmd[3]="-1.0";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
# ncbo #12    
    
    $tst_cmd[0]="ncks -C -O $fl_fmt $nco_D_flg -v three_dmn_var_dbl $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -C -O $fl_fmt $nco_D_flg -v -s 'three_dmn_var_dbl[lon]={0.0,1,2,3};' $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncbo $omp_flg -C -h -O $fl_fmt $nco_D_flg %tmp_fl_00% %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[3]="ncwa $omp_flg -C -h -O $fl_fmt $nco_D_flg -y ttl -v three_dmn_var_dbl %tmp_fl_02% %tmp_fl_03%";
    $tst_cmd[4]="ncks -C -H -s '%f' -v three_dmn_var_dbl %tmp_fl_03%";
    $dsc_sng="Difference which tests 3D broadcasting";
    $tst_cmd[5]="2697";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
# ncbo #13   
# 	
    $tst_cmd[0]="ncks -C -O $fl_fmt $nco_D_flg -v three_dmn_var_dbl $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -C -O $fl_fmt $nco_D_flg -v -s 'three_dmn_var_dbl[lon]={0.0,1,2,3};' $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncbo $omp_flg -C -h -O $fl_fmt $nco_D_flg -d time,0,4 -d time,9 -d lon,0 -d lon,3 %tmp_fl_00% %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[3]="ncwa $omp_flg -C -h -O $fl_fmt $nco_D_flg -y ttl -v three_dmn_var_dbl %tmp_fl_02% %tmp_fl_03%";
    $tst_cmd[4]="ncks -C -H -s '%f' -v three_dmn_var_dbl %tmp_fl_03%";
    $dsc_sng="Addition + 3D broadcasting+ MSA";
    $tst_cmd[5]="422";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	

# ncbo #14

    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -v three_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncbo $omp_flg -O $fl_fmt $nco_D_flg -v three_dmn_rec_var %tmp_fl_00% %tmp_fl_00% %tmp_fl_01%";
    $tst_cmd[2]="ncks -C -H -v three_dmn_rec_var -d time,9,9,1 -d lat,1,1,1 -d lon,3,3,1 %tmp_fl_01%";
    $dsc_sng="Copy associated coordinates -v three_dmn_rec_var";
    $tst_cmd[3]="time[9]=10 lat[1]=90 lon[3]=270 three_dmn_rec_var[79]=0";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array     
    

#ncbo #15
#Operate files containing same variable in different orders
# ncks -O    -v time,one ~/nco/data/in.nc ~/foo1.nc
# ncks -O -a -v one,time ~/nco/data/in.nc ~/foo2.nc
# ncbo -O -p ~ foo1.nc foo2.nc ~/foo3.nc
# ncks -C -H -v one ~/foo3.nc

    $dsc_sng="Concatenate variables with different ID ordering";
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C    -v time,one $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -C -a -v one,time $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncbo -h -O $fl_fmt $nco_D_flg %tmp_fl_00% %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[3]="ncks -C -H -v one %tmp_fl_02%";
    $tst_cmd[4]="one = 0";
    $tst_cmd[5]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			

#### Group tests	
	if($HAVE_NETCDF4_H == 1){

#    
# NCO 4.3.0: added support for groups; ncbo -g
#   
#ncbo #16
# ncbo -O -y add -g g4 -v one_dmn_rec_var in_grp.nc in_grp.nc out.nc
# ncks -C -H -s '%d'  -d time,0,0,1 -g g4 -v one_dmn_rec_var out.nc
# /g4/one_dmn_rec_var
# time[0]=1 one_dmn_rec_var[0]=2 

    $dsc_sng="(Groups) Addition -y add -g g4 -v one_dmn_rec_var";
    $tst_cmd[0]="ncbo -O $fl_fmt $nco_D_flg -y add -g g4 -v one_dmn_rec_var $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -C -H -s '%d' -d time,0,0,1 -g g4 -v one_dmn_rec_var %tmp_fl_00%";
    $tst_cmd[2]="2";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			
    
#ncbo #17
# ncbo -O -y add -g g4 -v one_dmn_rec_var ~/nco/data/in_grp.nc ~/nco/data/in_grp.nc out.nc
# ncks -C -H -s '%d'  -d time,0,0,1 -g g4 -v one_dmn_rec_var out.nc
# /g4/one_dmn_rec_var

    $dsc_sng="(Groups) Addition with limits -d time,1,1,1 -y add -g g4 -v one_dmn_rec_var";
    $tst_cmd[0]="ncbo -O $fl_fmt $nco_D_flg -d time,1,1,1 -y add -g g4 -v one_dmn_rec_var $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -C -H -s '%d' -d time,0,0,1 -g g4 -v one_dmn_rec_var %tmp_fl_00%";
    $tst_cmd[2]="4";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			    
    
#ncbo #18
# ncbo -O -v one_dmn_rec_var ~/nco/data/in_grp.nc ~/nco/data/in_grp.nc out.nc
# ncks -C -H -s '%d'  -d time,0,0,1 -g g4 -v one_dmn_rec_var out.nc
# /g4/one_dmn_rec_var

    $dsc_sng="(Groups) Subtraction -v one_dmn_rec_var";
    $tst_cmd[0]="ncbo -O $fl_fmt $nco_D_flg -v one_dmn_rec_var $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -C -H -s '%d' -d time,1,1,1 -g g4 -v one_dmn_rec_var %tmp_fl_00%";
    $tst_cmd[2]="0";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			    

#ncbo #19
# ncbo -O -y mlt -g g4 -v one_dmn_rec_var ~/nco/data/in_grp.nc ~/nco/data/in_grp.nc out.nc
# ncks -C -H -s '%d'  -d time,0,0,1 -g g4 -v one_dmn_rec_var out.nc
# /g4/one_dmn_rec_var

    $dsc_sng="(Groups) Multiplication with limits -d time,2,2,1 -y add -g g4 -v one_dmn_rec_var";
    $tst_cmd[0]="ncbo -O $fl_fmt $nco_D_flg -d time,2,2,1 -y mlt -g g4 -v one_dmn_rec_var $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -C -H -s '%d' -d time,0,0,1 -g g4 -v one_dmn_rec_var %tmp_fl_00%";
    $tst_cmd[2]="9";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			  

#ncbo #20
# ncbo -O -y mlt -g g4 -v one_dmn_rec_var in_grp.nc in_grp.nc out.nc
# ncks -C -H -s '%d'  -d time,0,0,1 -g g4 -v one_dmn_rec_var out.nc
# /g4/one_dmn_rec_var


    $dsc_sng="(Groups) Division with limits -d time,2,2,1 -y add -g g4 -v one_dmn_rec_var";
    $tst_cmd[0]="ncbo -O $fl_fmt $nco_D_flg -d time,2,2,1 -y dvd -g g4 -v one_dmn_rec_var $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -C -H -s '%d' -d time,0,0,1 -g g4 -v one_dmn_rec_var %tmp_fl_00%";
    $tst_cmd[2]="1";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 		

# ncbo #21
# ncbo -O -g g1 -v var1  in_grp_1.nc  in_grp_2.nc out.nc

    $dsc_sng="(Groups) Process different types -g g1 -v var1 in_grp_1.nc  in_grp_2.nc";
    $tst_cmd[0]="ncbo -O $fl_fmt $nco_D_flg  -g g1 -v var1 $in_pth_arg in_grp_1.nc  in_grp_2.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks %tmp_fl_00% | grep 'var1: type NC_FLOAT, 1 dimension, 0 attributes, chunked? no, compressed? no, packed? no'";
    $tst_cmd[2]="var1: type NC_FLOAT, 1 dimension, 0 attributes, chunked? no, compressed? no, packed? no";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			  
    
# ncbo #22

    $dsc_sng="(Groups) Process relative match -v var2 in_grp_1.nc in_grp_2.nc";
    $tst_cmd[0]="ncbo -O $fl_fmt $nco_D_flg -v var2 $in_pth_arg in_grp_1.nc  in_grp_2.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -d lon1,2,2,1 %tmp_fl_00%";
    $tst_cmd[2]="lon1[2] var2[2]=-1";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			      
    
# ncbo #23

    $dsc_sng="(Groups) Process relative match from model to observations cmip5.nc -> obs.nc";
    $tst_cmd[0]="ncbo -O $fl_fmt $nco_D_flg $in_pth_arg cmip5.nc obs.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -H -g giss -v tas -d time,3,3,1 %tmp_fl_00%";
    $tst_cmd[2]="time[3] tas[3]=1";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			       

# ncbo #24

    $dsc_sng="(Groups) Process relative match from observations to model obs.nc -> cmip5.nc";
    $tst_cmd[0]="ncbo -O $fl_fmt $nco_D_flg $in_pth_arg obs.nc cmip5.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -H -g giss -v tas -d time,3,3,1 %tmp_fl_00%";
    $tst_cmd[2]="time[3] tas[3]=-1";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 	

   } # end HAVE_NETCDF4_H
   
   
####################
#### nces tests #### - OK !
####################
    $opr_nm='nces';
####################
    
#nces #01	
	
    $tst_cmd[0]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -v one_dmn_rec_var -d time,4 $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="ensemble mean of int across two files";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#nces #02	
    
    $tst_cmd[0]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -v mss_val_scl $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H --no_blank -s '%g' -v mss_val_scl %tmp_fl_00%";
    $dsc_sng="ensemble sum of missing value across two files";
    $tst_cmd[2]="1.0e36";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#nces #03	
    
    $tst_cmd[0]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_flt -d time,0 $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H --no_blank -s '%g' -v rec_var_flt_mss_val_flt %tmp_fl_00%";
    $dsc_sng="ensemble mean with missing values across two files";
    $tst_cmd[2]="1.0e36";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#nces #04	
    
    $tst_cmd[0]="/bin/rm -f %tmp_fl_00%";
    $tst_cmd[1]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v rec_var_flt_mss_val_dbl -d time,1 $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[2]="ncks -C -H -s '%e' -v rec_var_flt_mss_val_dbl %tmp_fl_00%";
    $dsc_sng="ensemble min of float across two files";
    $tst_cmd[3]="2";
    $tst_cmd[4]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#nces #05	
    
    $tst_cmd[0]="/bin/rm -f %tmp_fl_00%";
    $tst_cmd[1]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -C -v pck $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[2]="ncks -C -H -s '%e' -v pck %tmp_fl_00%";
    $dsc_sng="scale factor + add_offset packing/unpacking";
    $tst_cmd[3]="3";
    $tst_cmd[4]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#nces #06	
    
    $tst_cmd[0]="/bin/rm -f %tmp_fl_00%";
    $tst_cmd[1]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_int $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[2]="ncks -C -H --no_blank -s '%d ' -v rec_var_int_mss_val_int %tmp_fl_00%";
    $dsc_sng="ensemble mean of integer with integer missing values across two files";
    $tst_cmd[3]="-999 2 3 4 5 6 7 8 -999 -999";
    $tst_cmd[4]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#nces #07	
	
    $tst_cmd[0]="/bin/rm -f %tmp_fl_00%";
    $tst_cmd[1]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -C -d time,0,2 -d lon,0 -d lon,3 -v three_dmn_var_dbl $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[2]="ncwa $omp_flg -C -h -O $fl_fmt $nco_D_flg -y ttl -v three_dmn_var_dbl %tmp_fl_00% %tmp_fl_01%";
    $tst_cmd[3]="ncks -C -H -s '%3.f' -v three_dmn_var_dbl %tmp_fl_01%";
    $dsc_sng="ensemble mean of 3D variable across two files with MSA";
    $tst_cmd[4]="150";
    $tst_cmd[5]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#nces #08	
	
    $tst_cmd[0]="/bin/rm -f %tmp_fl_00%";
    $tst_cmd[1]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -C $in_pth_arg lrg_bm.nc lrg_bm1.nc %tmp_fl_00%";
    $tst_cmd[2]="ncra -Y ncfe -t 1 -h -O $fl_fmt $nco_D_flg -C $in_pth_arg lrg_bm.nc lrg_bm1.nc %tmp_fl_01%";
    $tst_cmd[3]="ncbo -C -h -O %tmp_fl_00% %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[4]="ncwa -t 1 -O -C %tmp_fl_02% %tmp_fl_03% 2>%tmp_fl_05%";
    $dsc_sng="Check op with OpenMP";
#    $tst_cmd[5]="ncks -C -H -s '%d' -v R %tmp_fl_03%";
    $tst_cmd[5]="ncap2 -t 1 -h -v -O -s 'n2= ((fabs(R)<0.01) && (fabs(S)<0.01) && (fabs(T)<0.01) && (fabs(U)<0.01) && (fabs(V)<0.01) && (fabs(W)<0.01) && (fabs(X)<0.01));print(n2)' %tmp_fl_03% %tmp_fl_04%";
    $tst_cmd[6]="n2 = 1";
    $tst_cmd[7]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#nces #09	
    
    $tst_cmd[0]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -v pck_7 $in_pth_arg in.nc in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v pck_7 %tmp_fl_00%";
    $dsc_sng="Ensemble mean of variable packed with netCDF convention across three files";
    $tst_cmd[2]="7";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#nces #10	
    
    $tst_cmd[0]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg --hdf_upk -v pck_7 $in_pth_arg in.nc in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v pck_7 %tmp_fl_00%";
    $dsc_sng="Ensemble mean of variable packed with HDF convention across three files";
    $tst_cmd[2]="-8";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
	
#### Group File tests	
	if($HAVE_NETCDF4_H == 1){

	
#nces #11
# same as #nces #01 , with group
	
    $tst_cmd[0]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -g g4 -v one_dmn_rec_var -d time,4 $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -g g4 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="(Groups) ensemble mean of int across two files";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
	
#nces #12
# 2 groups, each one with a record (part 1 )
# ncra -Y nces -h -O -g g25g1,g25g2 -v one_dmn_rec_var -d time,4  in_grp_3.nc in_grp_3.nc out.nc
	
    $tst_cmd[0]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -g g25g1,g25g2  -v one_dmn_rec_var -d time,4 $in_pth_arg in_grp_3.nc in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -g g25g1 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="(Groups) 2 groups ensemble mean of int across two files (part 1)";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	

#nces #13
# 2 groups, each one with a record (part 2 )
# ncra -Y nces -h -O -g g25g1,g25g2 -v one_dmn_rec_var -d time,4  in_grp_3.nc in_grp_3.nc out.nc
	
    $tst_cmd[0]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -g g25g1,g25g2  -v one_dmn_rec_var -d time,4 $in_pth_arg in_grp_3.nc in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -g g25g2 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="(Groups) 2 groups ensemble mean of int across two files (part 2)";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	

    } # #### Group File tests	
	
     #### Group Ensemble tests 
    if($HAVE_NETCDF4_H == 1){
	
#nces #14
# ncra -Y ncge -h -O  mdl.nc out.nc
# ncks -g cesm -v tas1 out.nc
	
    $tst_cmd[0]="ncra -Y ncge $omp_flg -h -O $fl_fmt $nco_D_flg $in_pth_arg mdl.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -g cesm -v tas1 %tmp_fl_00%";
    $dsc_sng="(Groups) 1 file mdl.cdl ensemble";
    $tst_cmd[2]="time[3]=4 tas1[3]=272.15";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#nces #15
	
    $tst_cmd[0]="ncra -Y ncge $omp_flg -h -O --nsm_sfx=_avg $fl_fmt $nco_D_flg $in_pth_arg mdl.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -g cesm_avg -v tas1 %tmp_fl_00%";
    $dsc_sng="(Groups) 1 file mdl.cdl --nsm_sfx suffix ensemble";
    $tst_cmd[2]="time[3]=4 tas1[3]=272.15";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#nces #16
# ncra -Y ncge -h -O -G /gpe_grp mdl.nc out.nc
	
    $tst_cmd[0]="ncra -Y ncge $omp_flg -h -O -G /gpe_grp $fl_fmt $nco_D_flg $in_pth_arg mdl.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -g /gpe_grp/ecmwf -v tas1 %tmp_fl_00%";
    $dsc_sng="(Groups) GPE 1 file mdl.cdl ensemble";
    $tst_cmd[2]="time[3] tas1[3]=273.15";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#nces #17
#ncra -Y ncge -O mdl.nc mdl2.nc out.nc
    $tst_cmd[0]="ncra -Y ncge $omp_flg -h -O $fl_fmt $nco_D_flg $in_pth_arg mdl.nc mdl2.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -g ecmwf -v tas1 %tmp_fl_00%";
    $dsc_sng="(Groups) 2 files ensembles";
    $tst_cmd[2]="time[3] tas1[3]=273.25";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

# NEW NCO 4.4.2
#nces #18 (check fixed variables)
# ncra -Y ncge -h -O  mdl.nc out.nc
# ncks -g cesm -v time out.nc
	
    $tst_cmd[0]="ncra -Y ncge $omp_flg -h -O $fl_fmt $nco_D_flg $in_pth_arg mdl.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -g cesm -v time  %tmp_fl_00%";
    $dsc_sng="(Groups) Ensemble fixed variables";
    $tst_cmd[2]="time dimension 0: time, size = 4 NC_DOUBLE, chunksize = 1 (Record coordinate is time)";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

     } # #### Group Ensemble tests	
	 	
#nces #19 # TODO	
    #for i in $(seq -w 0 999) ; do iii=$( printf "%03d" ${i} ) ; ln in.nc foo${iii}.nc ; done
	# TO DO run bash script above by perl
    $tst_cmd[0]="ncra -Y ncfe $omp_flg -h -O $fl_fmt $nco_D_flg -v dgn_var $in_pth_arg in.nc in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks %tmp_fl_00%";
    $dsc_sng="Test file loop";
    $tst_cmd[2]="dgn[0]=73 dgn_var[0]=73";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		 
    
# print "paused - hit return to continue"; my $wait=<STDIN>;
    
####################
## ncecat tests #### OK !
####################
    $opr_nm='ncecat';
####################

#ncecat #1

    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v one $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -v one $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncecat $omp_flg -h -O $fl_fmt $nco_D_flg %tmp_fl_00% %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[3]="ncks -C -H -s '%6.3f, ' -v one %tmp_fl_02%";
    $dsc_sng="concatenate two files containing only scalar variables";
    $tst_cmd[4]=" 1.000, "; # is this effectively equal to the previous " 1.000,  1.000, "
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncecat #2    
    
    $tst_cmd[0]="ncks -C -h -O $fl_fmt $nco_D_flg -v three_dmn_var_int $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -C -v -O $fl_fmt $nco_D_flg -s 'three_dmn_var_int+=100;' $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncecat -C -h -O $omp_flg $fl_fmt $nco_D_flg -d time,0,3 -d time,8,9 -d lon,0,1 -d lon,3,3 -v three_dmn_var_int %tmp_fl_00% %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[3]="ncwa -C -h -O $omp_flg $fl_fmt $nco_D_flg -y avg -v three_dmn_var_int %tmp_fl_02% %tmp_fl_03%";
    $tst_cmd[4]="ncks -C -O -H -s '%d' -v three_dmn_var_int %tmp_fl_03%";
    $dsc_sng="concatenate two 3D vars with multihyperslabbing";
    $tst_cmd[5]="84"; 
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
	
#### Group tests	
	if($HAVE_NETCDF4_H == 1){

#ncecat #3    
    
    if ($ENABLE_NETCDF4 == 1) {
    $tst_cmd[0]="ncks -C -h -O $fl_fmt $nco_D_flg -v area $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncecat -C -h -O $omp_flg $fl_fmt $nco_D_flg -G ensemble -d lat,1,1 -v area %tmp_fl_00% %tmp_fl_00% %tmp_fl_01%";
    $tst_cmd[2]="ncks -C -O -h -m -v area %tmp_fl_01% | grep \"ensemble../area\" | wc | cut -c 7";
    $dsc_sng="group aggregate var with hyperslabbing (requires netCDF4)";
    $tst_cmd[3]="2"; 
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 
    }elsif ($ENABLE_NETCDF4 == 0) {
    $tst_cmd[0]="ncks -C -h -O $fl_fmt $nco_D_flg -v area $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncecat -C -h -O $omp_flg $fl_fmt $nco_D_flg -G ensemble -d lat,1,1 -v area %tmp_fl_00% %tmp_fl_00% %tmp_fl_01%";
    $dsc_sng="group aggregate var with hyperslabbing (requires netCDF4)";
    $tst_cmd[2]=$ncecat_msg_no_netcdf4; 
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 
    } # end $ENABLE_NETCDF4
    
#    
# NCO 4.3.1 - ncecat for groups 
#
#ncecat #4 part1
#ncecat -h -O -g g1g1 -v v1 in_grp.nc in_grp.nc out.nc
#ncks -d record,1,1,1 out.nc

    $dsc_sng="(Groups) Concatenate variables/groups 1: scalars -g g1g1 -v v1";
    $tst_cmd[0]="ncecat $nco_D_flg -h -O -g g1g1 -v v1 $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -H -d record,1,1,1 %tmp_fl_00%";
    $tst_cmd[2]="record[1] v1[1]=11";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			     

#ncecat #5 same as #4 but look metadata
#ncecat -h -O -g g1g1 -v v1 in_grp.nc in_grp.nc out.nc
#ncks -d record,1,1,1 out.nc    
    
    $dsc_sng="(Groups) Concatenate variables/groups 2: scalars -g g1g1 -v v1";
    $tst_cmd[0]="ncecat $nco_D_flg -h -O -g g1g1 -v v1 $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks %tmp_fl_00% | grep 'v1 dimension 0: /record, size = 2, chunksize = 1 (Record non-coordinate dimension)'";
    $tst_cmd[2]="v1 dimension 0: /record, size = 2, chunksize = 1 (Record non-coordinate dimension)";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			    
    

#ncecat #6 part1
#ncecat  -h -O -g g6g1 -v area in_grp.nc in_grp.nc out.nc
#ncks -H -C -d record,1,1,1 -d lat,1,1,1 -g g6g1 -v area  out.nc

    $dsc_sng="(Groups) Concatenate variables/groups 1: 1D -g g6g1 -v area";
    $tst_cmd[0]="ncecat $nco_D_flg -h -O -g g6g1 -v area $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -H -C -d record,1,1,1 -d lat,1,1,1 -g g6g1 -v area %tmp_fl_00%";
    $tst_cmd[2]="record[1] lat[1]=90 area[3]=50";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			     

#ncecat #7 same as #6 but look metadata
#ncecat  -h -O -g g6g1 -v area in_grp.nc in_grp.nc out.nc
#ncks -C -g g6g1 -v area  out.nc
#area dimension 0: record, size = 2 (Record non-coordinate dimension)
    
    $dsc_sng="(Groups) Concatenate variables/groups 2: 1D -g g6g1 -v area";
    $tst_cmd[0]="ncecat $nco_D_flg -h -O -g g6g1 -v area $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -C -g g6g1 -v area %tmp_fl_00% | grep 'area dimension 0: /record, size = 2, chunksize = 1 (Record non-coordinate dimension)'";
    $tst_cmd[2]="area dimension 0: /record, size = 2, chunksize = 1 (Record non-coordinate dimension)";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			    
    
#ncecat #8 part1
#ncecat  -h -O -v two_dmn_rec_var in_grp.nc in_grp.nc out.nc
#ncks -C -d record,1,1,1 -d time,9,9,1 -d lev,2,2,1  -v two_dmn_rec_var  out.nc

    $dsc_sng="(Groups) Concatenate variables/groups 1: 2D -v two_dmn_rec_var";
    $tst_cmd[0]="ncecat $nco_D_flg -h -O -v two_dmn_rec_var $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -C -d record,1,1,1 -d time,9,9,1 -d lev,2,2,1 -v two_dmn_rec_var %tmp_fl_00%";
    $tst_cmd[2]="record[1] time[9]=10 lev[2]=1000 two_dmn_rec_var[59]=3";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 		
	   
#ncecat #9 
#Check that "time" is eliminated as record
#ncecat -O  in_grp_3.nc in_grp_3.nc out.nc
#ncks -m -C -g g25g1  -v one_dmn_rec_var out.nc

    $dsc_sng="(Groups) Concatenate variables/groups";
    $tst_cmd[0]="ncecat $nco_D_flg -h -O $in_pth_arg in_grp_3.nc in_grp_3.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -m -C -g g25g1  -v one_dmn_rec_var %tmp_fl_00%";
    $tst_cmd[2]="one_dmn_rec_var dimension 1: time, size = 10 NC_DOUBLE, chunksize = 10 (Coordinate is time)";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 		

#ncecat #10
#Chunking 
# ncecat -O -4 -D 5 -C --cnk_plc=all -v date_int in.nc in.nc out.nc

    $dsc_sng="Chunking --cnk_plc=all -v date_int";
    $tst_cmd[0]="ncecat -O -4 $nco_D_flg --cnk_plc=all -v date_int $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks %tmp_fl_00% | grep 'date_int dimension 0'";
    $tst_cmd[2]="date_int dimension 0: record, size = 2, chunksize = 1 (Record non-coordinate dimension)";
    $tst_cmd[3]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 

#ncecat #11
#Chunking 
#ncecat -O -C -4  -v four_dmn_rec_var --cnk_dmn lat,2 --cnk_dmn lon,4 in.nc in.nc out.nc

    $dsc_sng="Chunking -v four_dmn_rec_var --cnk_dmn lat,2 --cnk_dmn lon,4";
    $tst_cmd[0]="ncecat -O -C -4 $nco_D_flg  -v four_dmn_rec_var --cnk_dmn lat,2 --cnk_dmn lon,4  $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks %tmp_fl_00% | grep 'four_dmn_rec_var dimension 2'";
    $tst_cmd[2]="four_dmn_rec_var dimension 2: lat, size = 2, chunksize = 2 (Non-coordinate dimension)";
    $tst_cmd[3]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 	

    } #### Group tests	
	
		
#ncecat #12
#Concatenate files containing same variable in different orders
# ncks -O    -v time,one ~/nco/data/in.nc ~/foo1.nc
# ncks -O -a -v one,time ~/nco/data/in.nc ~/foo2.nc
# ncecat -O -p ~ foo1.nc foo2.nc ~/foo3.nc
# ncks -C -H -v one -d record,1 -s '%g' ~/foo3.nc

    $dsc_sng="Concatenate variables with different ID ordering";
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C    -v time,one $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -C -a -v one,time $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncecat -h -O $fl_fmt $nco_D_flg %tmp_fl_00% %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[3]="ncks -C -H -d record,1 -v one -s '%g' %tmp_fl_02%";
    $tst_cmd[4]="1";
    $tst_cmd[5]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 	

	

#print "paused - hit return to continue"; my $wait=<STDIN>;
    
#####################
## ncflint tests #### OK !
#####################
    $opr_nm='ncflint';
####################

#ncflint #1


    $tst_cmd[0]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -w 3,-2 -v one $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v one %tmp_fl_00%";
    $dsc_sng="identity weighting";
    $tst_cmd[2]="1.0";
    $tst_cmd[3]="NO_SS";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncflint #2    
    
    if ($dodap eq "FALSE"){
	$tst_cmd[0]="ncrename -h -O $nco_D_flg -v zero,foo $in_pth_arg in.nc %tmp_fl_01%";
	$tst_cmd[1]="ncrename -h -O $nco_D_flg -v one,foo $in_pth_arg in.nc %tmp_fl_00%";
	$tst_cmd[2]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -i foo,0.5 -v two %tmp_fl_01% %tmp_fl_00% %tmp_fl_02%";
	$tst_cmd[3]="ncks -C -H -s '%e' -v two %tmp_fl_02%";
	$dsc_sng="identity interpolation";
	$tst_cmd[4]="2.0";
	$tst_cmd[5]="NO_SS";
	$tst_cmd[5]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array
    }
    
#ncflint #3
    
    $dsc_sng="switch order of occurrence to test for commutivity";
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -d lon,1 -v mss_val $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -C -d lon,0 -v mss_val $in_pth_arg in.nc %tmp_fl_02%";
    $tst_cmd[2]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -w 0.5,0.5 %tmp_fl_01% %tmp_fl_02% %tmp_fl_03%";
    $tst_cmd[3]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -w 0.5,0.5 %tmp_fl_02% %tmp_fl_01% %tmp_fl_04%  $foo_y_fl $foo_x_fl $foo_yx_fl";
    $tst_cmd[4]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg %tmp_fl_03% %tmp_fl_04% %tmp_fl_05%";
    $tst_cmd[5]="ncks -C -H --no_blank -s '%g' -v mss_val %tmp_fl_05% ";
    $tst_cmd[6]="1e+36";
    $tst_cmd[7]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncflint #4    

    $dsc_sng="output type float when interpolating between two packed floats";
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v pck_3 $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -C -v pck_5 $in_pth_arg in.nc %tmp_fl_02%";
    $tst_cmd[2]="ncrename -h -O $fl_fmt $nco_D_flg -v pck_5,pck_3 %tmp_fl_02%";
    $tst_cmd[3]="ncflint $omp_flg -h -O $fl_fmt $nco_D_flg -v pck_3 %tmp_fl_01% %tmp_fl_02% %tmp_fl_03%";
    $tst_cmd[4]="ncks -C -H -s '%g' -v pck_3 %tmp_fl_03% ";
    $tst_cmd[5]="4";
    $tst_cmd[6]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array



#ncflint #5
#Operate files containing same variable in different orders
# ncks -O    -v time,one ~/nco/data/in.nc ~/foo1.nc
# ncks -O -a -v one,time ~/nco/data/in.nc ~/foo2.nc
# ncra -O -p ~ foo1.nc foo2.nc ~/foo3.nc
# ncks -C -H -v one ~/foo3.nc

    $dsc_sng="Concatenate variables with different ID ordering";
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C    -v time,one $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -C -a -v one,time $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncflint -h -O $fl_fmt $nco_D_flg %tmp_fl_00% %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[3]="ncks -C -H -v one %tmp_fl_02%";
    $tst_cmd[4]="one = 1";
    $tst_cmd[5]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 	

	
#### Group tests	
	if($HAVE_NETCDF4_H == 1){	
		
#ncflint #6 
# ncflint -4 -O -w 0.8,0.0 in.nc in.nc out.nc
# ncks -H -C -v time  -d time,9,9,1 out.nc

    $dsc_sng="-w 0.8,0.0 in.nc in.nc";
    $tst_cmd[0]="ncflint $nco_D_flg -4 -O -w 0.8,0.0 $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks  -H -C -v time  -d time,9,9,1 %tmp_fl_00%";
    $tst_cmd[2]="time[9]=8";
    $tst_cmd[3]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			

#ncflint #7  
# ncflint -4 -O -w 0.8,0.0 in.nc in.nc out.nc
# ncks -H -C -v time  -d time,9,9,1 out.nc
# --fix_rec_crd prevents ncflint from multiplying or interpolating any coordinate variables, including record coordinate variables

    $dsc_sng="--fix_rec_crd -w 0.8,0.0 in.nc in.nc";
    $tst_cmd[0]="ncflint $nco_D_flg -4 -O --fix_rec_crd -w 0.8,0.0 $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks  -H -C -v time  -d time,9,9,1 %tmp_fl_00%";
    $tst_cmd[2]="time[9]=10";
    $tst_cmd[3]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 	
   
# NCO 4.3.2 ncflint -- groups   
 
#ncflint #8
# ncflint -h -O -g g4 -v one_dmn_rec_var -w 1,1 in_grp.nc in_grp.nc out.nc
# ncks  -H -C -O -g g4  -d time,9 -v one_dmn_rec_var  out.nc

    $dsc_sng="(Groups) Weight 1D -g g4 -v one_dmn_rec_var -w 1,1 in_grp.nc in_grp.nc";
    $tst_cmd[0]="ncflint $nco_D_flg -h -O -v one_dmn_rec_var -w 1,1 $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks  -H -C -O -g g4 -d time,9 -v one_dmn_rec_var %tmp_fl_00%";
    $tst_cmd[2]="time[9]=20 one_dmn_rec_var[9]=20";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			   

    }  	#### Group tests
 

    
####################
#### ncks tests #### OK !
####################
    $opr_nm='ncks';
####################

#ncks #01   

    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v lat_T42,lon_T42,gw_T42 $in_pth_arg in.nc %tmp_fl_03%";
    $tst_cmd[1]="ncrename -h -O $nco_D_flg -d lat_T42,lat -d lon_T42,lon -v lat_T42,lat -v gw_T42,gw -v lon_T42,lon %tmp_fl_03%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -s 'one[lat,lon]=lat*lon*0.0+1.0' -s 'zero[lat,lon]=lat*lon*0.0' %tmp_fl_03% %tmp_fl_04%";
    $tst_cmd[3]="ncks -C -H -s '%g' -v one -F -d lon,128 -d lat,64 %tmp_fl_04% ";
    $dsc_sng="Create T42 variable named one, uniformly 1.0 over globe in %tmp_fl_03%. (FAILURE with netCDF4 ncrename nco821)";
    $tst_cmd[4]=1;
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncks #02   
#passes, but returned string includes tailing NULLS (<nul> in nedit)

    $tst_cmd[0]="ncks -C -H -s '%c' -v fl_nm $in_pth_arg in.nc";
    $dsc_sng="extract filename string";
    $tst_cmd[1]="/home/zender/nco/data/in.cdl";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #03
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v lev $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f,' -v lev %tmp_fl_00%";
    $dsc_sng="extract a dimension";
    $tst_cmd[2]="100.000000,500.000000,1000.000000";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #04
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v three_dmn_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,3,3 %tmp_fl_00%";
    $dsc_sng="extract a variable with limits";
    $tst_cmd[2]="23";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #05
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v int_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v int_var %tmp_fl_00%";
    $dsc_sng="extract variable of type NC_INT";
    $tst_cmd[2]="10";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #06
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,0,0 -d lev,2,2 -d lon,0,,2 -d lon,1,,2 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var %tmp_fl_00%";
    $dsc_sng="Multi-slab lat and lon with srd";
    $tst_cmd[2]="12.0,13.0,14.0,15.0,20.0,21.0,22.0,23.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #07
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0,3 -d lon,1,3 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var %tmp_fl_00%";
    $dsc_sng="Multi-slab with redundant hyperslabs";
    $tst_cmd[2]="20.0,21.0,22.0,23.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #08
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,2,2 -d lon,0.,,2 -d lon,90.,,2 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var %tmp_fl_00%";
    $dsc_sng="Multi-slab with coordinates";
    $tst_cmd[2]="20.0,21.0,22.0,23.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
    
    if(0){ # fxm use this test once script allows error exits to be intended result
	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v lat -d lat,20.,20.001 $in_pth_arg in.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks -C -H -s '%4.1f,' -v lat %tmp_fl_00%";
	$dsc_sng="No data in domain (OK. TODO nco1007. ncks behaves perfectly here. Unfortunately, the perl test script does not recognize the expected answer string as being valid)";
	$tst_cmd[2]="ncks: ERROR Domain 20 <= lat <= 20.001 brackets no coordinate values.";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array
    } # endif 0
    
#ncks #09
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v three_dmn_var -d lat,1,1 -d lev,800.,200. -d lon,270.,0. $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%4.1f,' -v three_dmn_var %tmp_fl_00%";
    $dsc_sng="Double-wrapped hyperslab";
    $tst_cmd[2]="23.0,20.0,15.0,12.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #10
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -d time_udunits,'1999-12-08 12:00:0.0','1999-12-09 00:00:0.0' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%6.0f' -d time_udunits,'1999-12-08 18:00:0.0','1999-12-09 12:00:0.0',2 -v time_udunits $in_pth_arg in.nc";
    $dsc_sng="dimension slice using UDUnits library (fails without UDUnits library support)";
    $tst_cmd[2]="876018";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #11
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v lon_cal -d lon_cal,'1964-3-1 00:00:0.0','1964-3-4 00:00:0.0' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -O -v -C -s 'lon_cln_ttl=lon_cal.total();print(lon_cln_ttl)' %tmp_fl_00% %tmp_fl_01%";
    $tst_cmd[2]="lon_cln_ttl = 10";
    $dsc_sng="dim slice using UDUnits library and cal 365_days (fails without UDUnits library support)";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #12

    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v lat_cal -d lat_cal,'1964-3-1 0:00:0.0','1964-3-4 00:00:0.0' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -O -v -C -s 'lat_cln_ttl=lat_cal.total();print(lat_cln_ttl)' %tmp_fl_00% %tmp_fl_01%";
    $tst_cmd[2]="lat_cln_ttl = 18";
    $dsc_sng="dim slice using UDUnits library and cal 360_days (fails without UDUnits library support)";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #13

    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -H -v wvl -d wvl,'0.4 micron','0.7 micron' -s '%3.1e' $in_pth_arg in.nc";
    $dsc_sng="dimension slice using UDUnit conversion (fails without UDUnits library support)";
    $tst_cmd[1]="1.0e-06";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #14

    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v '^three_*' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -C -v three %tmp_fl_00%";
    $dsc_sng="variable wildcards A (fails without regex library)";
    $tst_cmd[2]="3";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncks #15

    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v '^[a-z]{3}_[a-z]{3}_[a-z]{3,}\$' $in_pth_arg in.nc %tmp_fl_00%";
    # for this test, the regex is mod'ed                       ^
    $tst_cmd[1]="ncks -C -H -s '%d' -C -v val_one_int %tmp_fl_00%";
    $dsc_sng="variable wildcards B (fails without regex library)";
    $tst_cmd[2]="1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
    if(0){ # fxm use this test once script allows error exits to be intended result
	$tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -d time,0,1 -v time $in_pth_arg in.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks -C -H -s '%g' -C -d time,2, %tmp_fl_00%";
	$dsc_sng="Offset past end of file (OK. TODO nco693. ncks behaves perfectly here. Unfortunately, the perl test script does not recognize the expected answer string as being valid)";
	$tst_cmd[2]="ncks: ERROR User-specified dimension index range 2 <= time <= 1 does not fall within valid dimension index range 0 <= time <= 1";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array
    } # endif 0

#ncks #16

    $tst_cmd[0]="ncks -C -H -s '%d' -v byte_var $in_pth_arg in.nc";
    $dsc_sng="Print byte value";
    $tst_cmd[1]="122";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncks #17

    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -v cnv_CF_crd $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v lon_gds -d gds_crd,2 %tmp_fl_00%";
    $dsc_sng="CF convention coordinates attribute";
    $tst_cmd[2]="180";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #18 

    $tst_cmd[0]="ncatted -O $fl_fmt $nco_D_flg -a _FillValue,global,c,l,222 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -O %tmp_fl_00% %tmp_fl_01%";
    $tst_cmd[2]="ncap2 -v -O -s 'n2=global\@_FillValue;' %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[3]="ncks -C -H -s '%d' -v n2 %tmp_fl_02%";
    $dsc_sng="Create/copy of global _FillValue";
    $tst_cmd[4]="222";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array  
	
#### Group tests	
	if($HAVE_NETCDF4_H == 1){	
#   
#NCO 4.2.2   
# 

#ncks #19 groups: add associated variable "lat" of "area" to extraction list

    $dsc_sng="(Groups) Extract associated coordinate variable";
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v area $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 0){    
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";
    }elsif($HAVE_NETCDF4_H == 1){
    if($ENABLE_NETCDF4 == 1){
    $tst_cmd[1]="ncks -C -H -s '%g' -v lat %tmp_fl_00%";
    $tst_cmd[2]="-9090";
    $tst_cmd[3]="SS_OK";
    }elsif($ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";
    } # !ENABLE_NETCDF4 
    } # !HAVE_NETCDF4_H
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array    
 
    
#ncks #20 groups: Add to extraction list all coordinates associated with CF convention

    $dsc_sng="(Groups) Add CF convention variables";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -g g7 -v gds_var $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 1){
    $tst_cmd[1]="ncks -C -H -s '%g' -v lat_gds %tmp_fl_00%";  
    $tst_cmd[2]="-90-30-3000303090";
    $tst_cmd[3]="SS_OK";
    }elsif($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #21 groups: Extract variables in groups (test -g with -v )

    $dsc_sng="(Groups) Extract variables in groups";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -v scl -g g1g1,g1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 1){
    $tst_cmd[1]="ncks -C -H -s '%g' %tmp_fl_00%";
    $tst_cmd[2]="1.11";
    $tst_cmd[3]="SS_OK";
    }elsif($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncks #22 groups: Create variables in groups (test -G with -v and -g )

    $dsc_sng="(Groups) Create variables in groups";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -G g8 -g g3 -v scl $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 1){
    $tst_cmd[1]="ncks -C -H -s '%g' %tmp_fl_00%"; 
    $tst_cmd[2]="1.3";
    $tst_cmd[3]="SS_OK";
    }elsif($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #23 groups: Hyperslabs (test -d with -v and -g: Extracts the second value (2) from g4/one_dmn_rec_var  )

    $dsc_sng="(Groups) Hyperslabs in groups";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -g g4 -v one_dmn_rec_var -d time,1,1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 1){
    $tst_cmd[1]="ncks -H -s '%d' %tmp_fl_00%"; 
    $tst_cmd[2]="2";
    $tst_cmd[3]="SS_OK";
    }elsif($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#   
#NCO 4.2.3   
# 

#ncks #24 groups: Extract group attributes

    $dsc_sng="(Groups) Extract group attributes";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -g g3 $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 1){
    $tst_cmd[1]="ncks %tmp_fl_00% | grep g3_group_attribute";
    $tst_cmd[2]="Group attribute 0: g3_group_attribute, size = 18 NC_CHAR, value = g3_group_attribute";
    $tst_cmd[3]="SS_OK";
    }elsif($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #25 groups: Extract global attributes

    $dsc_sng="Extract global attributes";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 1){
    $tst_cmd[1]="ncks %tmp_fl_00% | grep Conventions";
    $tst_cmd[2]="Global attribute 0: Conventions, size = 6 NC_CHAR, value = CF-1.0";
    $tst_cmd[3]="SS_OK";
    }elsif($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #26 groups: Extract "bounds" variables (extract /g8/ilev)

    $dsc_sng="(Groups) Extract 'bounds' variables";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -v lev $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 1){
    $tst_cmd[1]="ncks -a -O -H  -s '%g' %tmp_fl_00% ";    
    $tst_cmd[2]="1005001000";
    $tst_cmd[3]="SS_OK";
    }elsif($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
#ncks #27 groups: Extract group attributes with GPE

    $dsc_sng="(Groups) GPE group attribute extraction";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -G g9 -g g3 -v scl $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 1){
    $tst_cmd[1]="ncks %tmp_fl_00% | grep g3_group_attribute";
    $tst_cmd[2]="Group attribute 0: g3_group_attribute, size = 18 NC_CHAR, value = g3_group_attribute";
    $tst_cmd[3]="SS_OK";
    }elsif($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncks #28 groups: Group dimension hyperslabs

    $dsc_sng="(Groups) Group dimension hyperslabs";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -v gds_var -d gds_crd,1,1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 1){
    $tst_cmd[1]="ncks -C -H -v lat_gds -s '%g' %tmp_fl_00%";
    $tst_cmd[2]="-30";
    $tst_cmd[3]="SS_OK";
    }elsif($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#   
#NCO 4.2.4   
# 
    
#ncks #29 groups: Sort output alphabetically

    $dsc_sng="(Groups) Sort output alphabetically";
    $tst_cmd[0]="ncks -z $in_pth_arg in_grp.nc | tail -1";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="var: /unique";
    $tst_cmd[2]="SS_OK";
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

    
#ncks #30 2D variable definition

    $dsc_sng="2D variable definition";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -v ilev $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks %tmp_fl_00%";
    $tst_cmd[2]="lev[2] vrt_nbr[1] ilev[5]=1013.25";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array   

#ncks #31 Variable/Group extraction test 1 (netCDF4 file)
#extract all variables in g6 = area,area1, refine to area1 only

    $dsc_sng="(Groups) Variable/Group extraction test 1 (netCDF4 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -g g6 $in_pth_arg in_grp.nc %tmp_fl_00%";
	if($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 1){
    $tst_cmd[1]="ncks -v area1 %tmp_fl_00%";
    $tst_cmd[2]="lat[1] area1[1]=31";
    $tst_cmd[3]="SS_OK";
	 }elsif($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 

#ncks #32 Variable/Group extraction test 2 (netCDF4 file)
#extract all variables in g6 = area,area1, refine to area1 only
#use -d 

    $dsc_sng="(Groups) Variable/Group extraction test 2 (netCDF4 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -g g6 $in_pth_arg in_grp.nc %tmp_fl_00%";
	if($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 1){
    $tst_cmd[1]="ncks -v area1 -d lat,0,0 %tmp_fl_00%";
    $tst_cmd[2]="lat[0] area1[0]=21";
    $tst_cmd[3]="SS_OK";
	 }elsif($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 
 
#ncks #33 Variable/Group extraction test 3 (netCDF4 file)
#extract all variables in g6g1 (second level group) = area


    $dsc_sng="(Groups) Variable/Group extraction test 3 (netCDF4 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -g g6g1 $in_pth_arg in_grp.nc";
	if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="lat[1]=90 area[1]=50";
    $tst_cmd[2]="SS_OK";  
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
  
	
#ncks #34 Variable/Group extraction test 4 (netCDF4 file)
#extract all variables "area" = /area /g6/area /g6/g6g1/area
#grep -w = all word
#NOTE: test repeated for 3 strings, for non-netcDF4 cases, output is empty string, wildcard . used  

    $dsc_sng="(Groups) Variable/Group extraction test 4-1 (netCDF4 file)";
   $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -v are. $in_pth_arg in_grp.nc | grep -w /g6/g6g1/area";
	if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="/g6/g6g1/area";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=""; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 
	
#ncks #35 Variable/Group extraction test 4 (netCDF4 file)
#extract all variables "area" = /area /g6/area /g6/g6g1/area
#grep -w = all word
#NOTE: test repeated for 3 strings, for non-netcDF4 cases, output is empty string, wildcard . used  

    $dsc_sng="(Groups) Variable/Group extraction test 4-2 (netCDF4 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -v are. $in_pth_arg in_grp.nc | grep -w /g6/area";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="/g6/area";
    $tst_cmd[2]="SS_OK"; 
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=""; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 		
	
#ncks #36 Variable/Group extraction test 4 (netCDF4 file)
#extract all variables "area" = /area /g6/area /g6/g6g1/area
#grep -w = all word
#NOTE: test repeated for 3 strings, for non-netcDF4 cases, output is empty string, wildcard . used  

    $dsc_sng="(Groups) Variable/Group extraction test 4-3 (netCDF4 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -v are. $in_pth_arg in_grp.nc | grep -o -w area";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="area";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=""; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			
	
#ncks #37 Variable/Group extraction test 5 (netCDF4 file)
# Extract all variables "area" in g6g1

    $dsc_sng="(Groups) Variable/Group extraction test 5 (netCDF4 file)";
    $tst_cmd[0]="ncks -H $fl_fmt $nco_D_flg -C -s '%g' -v area -g g6g1 -d lat,0 $in_pth_arg in_grp.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="40";
    $tst_cmd[2]="SS_OK";    
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array			
	
#ncks #38 Variable/Group extraction test 6 (netCDF4 file)
#extract all variables "area" in g6g1 = g6/g6g1/area 


    $dsc_sng="(Groups) Variable/Group extraction test 6 (netCDF4 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -v area -g g6g1 $in_pth_arg in_grp.nc";
	if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="lat[1]=90 area[1]=50";
    $tst_cmd[2]="SS_OK";  
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

	
#ncks #39 Variable/Group extraction test 7 (netCDF4 file)
#extract all variables "area" in g6 and g6g1 = g6/g6g1/area, g6/area
#NOTE: test repeated for 2 strings, for non-netcDF4 case, output is empty string 

    $dsc_sng="(Groups) Variable/Group extraction test 7-1 (netCDF4 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -v area -g g6,g6g1 $in_pth_arg in_grp.nc | grep -w /g6/g6g1/area";
	if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="/g6/g6g1/area";
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=""; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 							
	
#ncks #40 Variable/Group extraction test 7 (netCDF4 file)
#extract all variables "area" in g6 and g6g1 = g6/g6g1/area, g6/area
#NOTE: test repeated for 2 strings, for non-netcDF4 case, output is empty string 

    $dsc_sng="(Groups) Variable/Group extraction test 7-2 (netCDF4 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -v area -g g6,g6g1 $in_pth_arg in_grp.nc | grep -w /g6/area";
	if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="/g6/area";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=""; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 
	
#ncks #41 Variable/Group extraction test 8 (netCDF3 file)
#extract all variables "area" = /area 

    $dsc_sng="(Groups) Variable/Group extraction test 8 (netCDF3 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -v area $in_pth_arg in.nc";
    $tst_cmd[1]="lat[1]=90 area[1]=10 meter2";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 							
	
#ncks #42 Extract associated coordinates test 1 (netCDF3 file) 

    $dsc_sng="Extract associated coordinates test 1 (netCDF3 file) ";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -v rz $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -v rlev %tmp_fl_00% | grep -o -w rlev";
    $tst_cmd[2]="rlev";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 
	
#ncks #43 Extract associated coordinates test 2 (netCDF3 file) 

    $dsc_sng="Extract associated coordinates test 2 (netCDF3 file) ";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -C -v rz $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -v rlev %tmp_fl_00% | grep -w /rlev";
    $tst_cmd[2]="";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 	
	
#ncks #44 Extract associated coordinates test 3 (netCDF4 file)
# rlev is dimension of rz 
# /g3/rz
# /g3/rlev
# /g5/g1/rz
# /g5/rlev
# output must not contain /g3

    $dsc_sng="(Groups) Extract associated coordinates test 3 (netCDF4 file) ";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg -g g5g1 -v rz $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 1){
	$tst_cmd[1]="ncks -H %tmp_fl_00%";
	$tst_cmd[2]="rlev[2]=1"; # data for /g3/rlev differs from /g5/rlev
    $tst_cmd[3]="SS_OK";
    }elsif($HAVE_NETCDF4_H == 1 && $ENABLE_NETCDF4 == 0){
    $tst_cmd[1]=$ncks_msg_no_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=$ncks_msg_no_have_netcdf4; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 
	
#ncks #45 Extract CF 'coordinates' variables(netCDF4 file)
#gds_crd:coordinates = "lat_gds lon_gds";
#ncks -v gds_crd in_grp.nc 

    $dsc_sng="(Groups) Extract CF 'coordinates' variables(netCDF4 file)";
    $tst_cmd[0]="ncks $nco_D_flg -v gds_crd $in_pth_arg in_grp.nc | grep -w /g7/lat_gds";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="/g7/lat_gds";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=""; 
    $tst_cmd[2]="SS_OK";
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 	
	
#ncks #46 Extract CF 'coordinates' variables (netCDF3 file)
#gds_crd:coordinates = "lat_gds lon_gds";

    $dsc_sng="Extract CF 'coordinates' variables (netCDF3 file)";
    $tst_cmd[0]="ncks $nco_D_flg -v gds_crd $in_pth_arg in.nc | grep -o -w lat_gds";
    $tst_cmd[1]="lat_gds";
    $tst_cmd[2]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 		
	
#ncks #47 Extract CF 'bounds' variables (netCDF4 file)
#lev:bounds = "ilev";

    $dsc_sng="(Groups) Extract CF 'bounds' variables (netCDF4 file)";
    $tst_cmd[0]="ncks $nco_D_flg -g g8 -v lev $in_pth_arg in_grp_3.nc | grep -w /g8/ilev";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="/g8/ilev";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]=""; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 		
	
#ncks #48 Extract CF 'bounds' variables (netCDF3 file)
#lev:bounds = "ilev";

    $dsc_sng="Extract CF 'bounds' variables (netCDF3 file)";
    $tst_cmd[0]="ncks $nco_D_flg -v lev $in_pth_arg in.nc | grep -o -w ilev";
    $tst_cmd[1]="ilev";
    $tst_cmd[2]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			
    
#ncks #49

    $dsc_sng="Check --mk_rec_dmn (netCDF3 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg --mk_rec_dmn lat -v three_dmn_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -m -v lat %tmp_fl_00% | egrep -o -w 'Record coordinate is lat'";
    $tst_cmd[2]="Record coordinate is lat";
    $tst_cmd[3]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			

#ncks #50    
    
    $dsc_sng="(Groups) Check --mk_rec_dmn (netCDF4 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg --mk_rec_dmn lat -v lat_lon $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -C -m -v lat %tmp_fl_00% | egrep -o -w 'Record coordinate is lat'";
    $tst_cmd[2]="Record coordinate is lat";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			

#ncks #51
    
    $dsc_sng="Check --fix_rec_dmn (netCDF3 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg --fix_rec_dmn time -v three_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -m -v time %tmp_fl_00% | egrep -o -w 'Coordinate is time'";
    $tst_cmd[2]="Coordinate is time";
    $tst_cmd[3]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			
    
#ncks #52    

    $dsc_sng="(Groups) Check --fix_rec_dmn (netCDF4 file)";
    $tst_cmd[0]="ncks -O $fl_fmt $nco_D_flg --fix_rec_dmn time -v three_dmn_rec_var $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -C -m -v time %tmp_fl_00% | egrep -o -w 'Coordinate is time'";
    $tst_cmd[2]="Coordinate is time";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			

#ncks #53    
    
    $dsc_sng="--fix_rec_dmn with MM3 workaround (netCDF3->netCDF3 file)";
    $tst_cmd[0]="ncks -O $nco_D_flg --fix_rec_dmn time -v one,two,one_dmn_rec_var,two_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -m -v time %tmp_fl_00% | egrep -o -w 'Coordinate is time'";
    $tst_cmd[2]="Coordinate is time";
    $tst_cmd[3]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			

    if(0){
	# NB: does not actually test3 code for reasons explained in header of nco_use_mm3_workaround()
	$dsc_sng="(Groups) --fix_rec_dmn with MM3 workaround (netCDF4->netCDF3 file)";
	$tst_cmd[0]="ncks -O -3 $nco_D_flg --fix_rec_dmn time -v /g10/two_dmn_rec_var,/g10/three_dmn_rec_var $in_pth_arg in_grp.nc %tmp_fl_00%";
	if($HAVE_NETCDF4_H == 1){
	    $tst_cmd[1]="ncks -C -m -v time %tmp_fl_00% | egrep -o -w 'Coordinate dimension'";
	    $tst_cmd[2]="Record coordinate dimension";
	    $tst_cmd[3]="SS_OK";   
	}elsif($HAVE_NETCDF4_H == 0){
	    $tst_cmd[1]=""; 
	    $tst_cmd[2]="SS_OK";     
	}
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array 			
    } # endif false
    
    
# NCO 4.2.6 tests
# This version has a major change in the way dimensions are handled; the global array lmt_all was eliminated and replaced
# with GTT (Group Traversal Table) structures that contain full dimension paths; it needs extensive testing, in special
# 1) Limits
# 2) MSA
# 3) Chunking
#
# ncks eliminate lmt_all array and replace with GTT (Group Traversal Table)
#structure that stores full path names for dimensions and coordinate variables.
#	nco_xtr_wrt_trv() writes extracted data to output file
#nco_xtr_dfn_trv() defines extracted groups, variables, and attributes in output file
#	nco_prn_xtr_val_trv() prints variable data
#
#	* ncks limits are stored in the GTT dimension structure and MSA is applied.
#	Distribute these to variables when writing with nco_prn_var_val_trv().
#	nco_bld_lmt_trv() assigns user specified dimension limits to traversal table dimensions.   

# 
# Chunking tests
#
 
#ncks #54: This test applies chunking all policy to -v lat_lon(lat,lon); lat(2) and lon(4) are by default chunked with a size == dimension
# The test greps chunksize = 2 for lat
# Policy: Chunk All Variables [default]
# Map:Chunksize Equals Dimension Size [default]
#ncks -O -4 -v lat_lon --cnk_plc=all in_grp.nc out.nc"

    $dsc_sng="(Groups) Chunking --cnk_plc=all --v lat_lon";
    $tst_cmd[0]="ncks $nco_D_flg -O -4 -v lat_lon --cnk_plc=all $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -C -m -v lat_lon %tmp_fl_00% | egrep -o -w 'lat_lon dimension 0: lat, size = 2 NC_FLOAT, chunksize = 2'";
    $tst_cmd[2]="lat_lon dimension 0: lat, size = 2 NC_FLOAT, chunksize = 2";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";      
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			
    
#ncks #55: This test applies: time chunk is explicitely set to 2 instead of the default 1 for record dimension
# Policy: Chunk Variables with at least Three Dimensions Definition: Chunk all variables possible with at least three dimensions
# Alternate invocation: none cnk plc key values: ‘g3d’, ‘cnk_g3d’, ‘plc_g3d’
# Definition: Chunksize defaults to dimension size. Explicitly specify chunksizes for particular dimensions with ‘--cnk_dmn’ option.
# Map:Chunksize Equals Dimension Size [default] Explicitly specify chunksizes for particular dimensions with ‘--cnk_dmn’ option.

    $dsc_sng="(Groups) Chunking --cnk_plc=cnk_g3d --cnk_dmn time,2";
    $tst_cmd[0]="ncks $nco_D_flg -O -4 --cnk_plc=cnk_g3d --cnk_dmn time,2 -v three_dmn_rec_var $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -C -m -v three_dmn_rec_var %tmp_fl_00% | grep 'three_dmn_rec_var dimension 0: /time, size = 10 NC_DOUBLE, chunksize = 2 (Record coordinate is /time)'";
    $tst_cmd[2]="three_dmn_rec_var dimension 0: /time, size = 10 NC_DOUBLE, chunksize = 2 (Record coordinate is /time)";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";      
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			 
    
# 
# Limit/MSA tests
#

#ncks #56:
# ncks -H -C --dmn time,1,3,2 --dmn lev,1,1,1  -v two_dmn_rec_var  ~/nco/data/in_grp.nc
# /g10/two_dmn_rec_var
# time[1]=2 lev[1]=500 two_dmn_rec_var[4]=2.1 
# time[3]=4 lev[1]=500 two_dmn_rec_var[10]=2.3 
#
    $dsc_sng="(Groups) MSA --dmn time,1,3,2 --dmn lev,1,1,1";
    $tst_cmd[0]="ncks $nco_D_flg -H -C --dmn time,1,3,2 --dmn lev,1,1,1 -v two_dmn_rec_var $in_pth_arg in_grp.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="time[3]=4 lev[1]=500 two_dmn_rec_var[10]=2.3";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			    

#ncks #57:
#  ncks -H -C --dmn time,1,1,1 --dmn time,3,3,1 --dmn lev,0,0,1 --dmn lev,2,2,1   -v two_dmn_rec_var  ~/nco/data/in_grp.nc
#/g10/two_dmn_rec_var
#time[1]=2 lev[0]=100 two_dmn_rec_var[3]=1 
#time[1]=2 lev[2]=1000 two_dmn_rec_var[5]=3 
#time[3]=4 lev[0]=100 two_dmn_rec_var[9]=1 
#time[3]=4 lev[2]=1000 two_dmn_rec_var[11]=3 

    $dsc_sng="(Groups) MSA --dmn time,1,1,1 --dmn time,3,3,1 --dmn lev,0,0,1";
    $tst_cmd[0]="ncks $nco_D_flg -H -C --dmn time,1,1,1 --dmn time,3,3,1 --dmn lev,0,0,1 --dmn lev,2,2,1 -v two_dmn_rec_var $in_pth_arg in_grp.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="time[3]=4 lev[2]=1000 two_dmn_rec_var[11]=3";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			  

#ncks #58 This test uses limits to print a dimension "lon2(4)" that does NOT have a coordinate variable
#  ncks -H -d lon2,1,3,2  -v lon2_var  in_grp.nc
#/g16/lon2_var
#lon2[1] lon2_var[1]=1 
#lon2[3] lon2_var[3]=3 

    $dsc_sng="(Groups) MSA -d lon2,1,3,2 -v lon2_var";
    $tst_cmd[0]="ncks $nco_D_flg -H -d lon2,1,3,2 -v lon2_var $in_pth_arg in_grp_3.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="lon2[3] lon2_var[3]=3";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			  
    
#ncks #59 same as #58 but with 2 limits with the same result
# ncks -H -d lon2,1,1,1 -d lon2,3,3,1  -v lon2_var  in_grp.nc 
#/g16/lon2_var
#lon2[1] lon2_var[1]=1 
#lon2[3] lon2_var[3]=3 

    $dsc_sng="(Groups) MSA -d lon2,1,3,2 -v lon2_var";
    $tst_cmd[0]="ncks $nco_D_flg -d lon2,1,1,1 -d lon2,3,3,1 -v lon2_var $in_pth_arg in_grp_3.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="lon2[3] lon2_var[3]=3";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";       
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			  
    
#ncks #60
# This test tests both limits and extraction of associated CF variables
# ncks -H -v gds_var -d gds_crd,1,1,1 in_grp.nc  
#/g7/g7g1/gds_var
#gds_var[1]=273.2 
#/g7/gds_crd
#gds_crd[1]=1 
#/g7/lat_gds
#gds_crd[1]=1 lat_gds[1]=-30 
#/g7/lon_gds
#gds_crd[1]=1 lon_gds[1]=0 

    $dsc_sng="(Groups) CF MSA -v gds_var -d gds_crd,1,1,1";
    $tst_cmd[0]="ncks $nco_D_flg -H -v gds_var -d gds_crd,1,1,1 $in_pth_arg in_grp.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="gds_crd[1]=1 lon_gds[1]=0";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";       
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			  
  
#ncks #61
# This test tests both limits printing of coordinates that are in ancestor groups
# ncks -H  -v gds_var -d gds_crd,1,1,1  in_grp.nc   
# ncks -H -C -g g7g1 -v gds_var -d gds_crd,1,1,1 -d gds_crd,3,3,1 in_grp.nc 
#/g7/g7g1/gds_var
# gds_crd[1]=1 gds_var[1]=273.2 
# gds_crd[3]=3 gds_var[3]=273.4 

    $dsc_sng="(Groups) MSA -H -C -g g7g1 -v gds_var -d gds_crd,1,1,1 -d gds_crd,3,3,1";
    $tst_cmd[0]="ncks $nco_D_flg -H -C -g g7g1 -v gds_var -d gds_crd,1,1,1 -d gds_crd,3,3,1 $in_pth_arg in_grp.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="gds_crd[3]=3 gds_var[3]=273.4";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";    
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			  
    
#ncks #62
# Test "parallel" scope 
#ncks: INFO nco_bld_var_dmn() reports variable </g16/g16g1/lon1> with dimension coordinate [0]/g16/g16g1/lon1
# ncks -H -g g16g1 -v lon1 -d lon1,3,3,1  in_grp.nc
#/g16/g16g1/lon1
#lon1[3]=3 

    $dsc_sng="(Groups) Parallel scope MSA -g g16g1 -v lon1 -d lon1,3,3,1";
    $tst_cmd[0]="ncks $nco_D_flg -H -g g16g1 -v lon1 -d lon1,3,3,1  $in_pth_arg in_grp_3.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="lon1[3]=3";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";      
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			 

#ncks #63
# Test "parallel" scope 
#ncks: INFO nco_bld_var_dmn() reports variable </g16/g16g1/lon1_var> with dimension coordinate [0]/g16/g16g1/lon1

    $dsc_sng="(Groups) Parallel scope MSA -g g16g1 -v lon1_var -d lon1,1,1,1";
    $tst_cmd[0]="ncks $nco_D_flg -H -g g16g1 -v lon1_var -d lon1,1,1,1 $in_pth_arg in_grp_3.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="lon1[1]=1 lon1_var[1]=1";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";    
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			  

#ncks #64
# Test "parallel" scope 
#ncks: INFO nco_bld_var_dmn() reports variable </g16/g16g2/lon1_var> with dimension coordinate [0]/g16/g16g2/lon1

    $dsc_sng="(Groups) Parallel scope MSA -g g16g2 -v lon1_var -d lon1,1,1,1";
    $tst_cmd[0]="ncks $nco_D_flg -H -g g16g2 -v lon1_var -d lon1,1,1,1 $in_pth_arg in_grp_3.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="lon1[1]=4 lon1_var[1]=1";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";      
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			  
    
#ncks #65
# Test creation of variables with ancestor group dimensions
#ncks -O -g g6g1 -v area  in_grp.nc out.nc
#/g6/g6g1/area
#lat[1]=90 area[1]=50

    $dsc_sng="(Groups) Dimensions in ancestor groups -g g6g1 -v area";
    $tst_cmd[0]="ncks $nco_D_flg -O -g g6g1 -v area $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -H -C -v area -d lat,1,1 %tmp_fl_00%";
    $tst_cmd[2]="lat[1]=90 area[1]=50";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
    $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";     
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			

#ncks #66
# Test creation of variables with ancestor group dimensions, with GPE
#ncks -O -G o1 -g g6g1 -v area  in_grp.nc out.nc
#/o1/g6/g6g1/area
#lat[0]=-90 area[0]=40 
#lat[1]=90 area[1]=50 

    $dsc_sng="(Groups) GPE and Dimensions in ancestor groups -G o1 -g g6g1 -v area";
    $tst_cmd[0]="ncks $nco_D_flg -O -G o1 -g g6g1 -v area $in_pth_arg in_grp.nc %tmp_fl_00%";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks -H -C -v area -d lat,0,1 %tmp_fl_00%";
    $tst_cmd[2]="lat[1]=90 area[1]=50";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";    
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			      
         
#ncks #67
# Test "out of scope" coordinate
# ncks  -H  -v lon3_var  in_grp.nc

    $dsc_sng="(Groups) Out of scope coordinate -v lon3_var";
    $tst_cmd[0]="ncks $nco_D_flg -H -v lon3_var $in_pth_arg in_grp_3.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="lon3[3] lon3_var[3]=3";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";       
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			  
    
#ncks #68
# Test 2 "intermediate scope " coordinates
# ncks  -H  -v lon4_var  in_grp.nc

    $dsc_sng="(Groups) Order coordinates by group depth -v lon4_var";
    $tst_cmd[0]="ncks $nco_D_flg -C -H -v lon4_var $in_pth_arg in_grp_3.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="lon4[1]=4 lon4_var[1]=1";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			      
    
#ncks #69
# Test -X
# ncks -O -C -X 0.,1.,-30.,-29. -g g18 -v gds_3dvar in_grp_3.nc 

    $dsc_sng="(Groups) Auxiliary coordinates -C -X 0.,1.,-30.,-29. -g g18 -v gds_3dvar in_grp_3.nc";
    $tst_cmd[0]="ncks $nco_D_flg -C -X 0.,1.,-30.,-29. -g g18 -v gds_3dvar $in_pth_arg in_grp_3.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="time[9] gds_crd[1]=1 gds_3dvar[73]=282.2 meter";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			
	
#ncks #70
# Test -X with variables anywhere in file (g18g1 has variable)
# ncks -O -C -X 0.,1.,-30.,-29. -g g18g1 -v gds_3dvar in_grp_3.nc 

    $dsc_sng="(Groups) Auxiliary coordinates -X 0.,1.,-30.,-29. -g g18g1 -v gds_3dvar in_grp_3.nc";
    $tst_cmd[0]="ncks $nco_D_flg -C -X 0.,1.,-30.,-29. -g g18g1 -v gds_3dvar $in_pth_arg in_grp_3.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="time[9] gds_crd[1]=1 gds_3dvar[73]=282.2 meter";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			
	

#ncks #71
# ncks  -h -O  -C -v three_dmn_var_dbl  -d time,,2 -d lat,0,0 -d lon,0,0 -d lon,3,3 in.nc

    $dsc_sng="Limits -C -v three_dmn_var_dbl -d time,,2 -d lat,0,0 -d lon,0,0 -d lon,3,3 in.nc";
    $tst_cmd[0]="ncks $nco_D_flg -C -v three_dmn_var_dbl -d time,,2 -d lat,0,0 -d lon,0,0 -d lon,3,3 $in_pth_arg in.nc";
    $tst_cmd[1]="time[2]=3 lat[0]=-90 lon[3]=270 three_dmn_var_dbl[19]=20 watt meter-2";
    $tst_cmd[2]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 	

#ncks #72
#same as #70, with group
# ncks -h -O  -v three_dmn_var_dbl  -d time,,2 -d lat,0,0 -d lon,0,0 -d lon,3,3 in_grp_3.nc

    $dsc_sng="(Groups) Limits -C -v three_dmn_var_dbl -d time,,2 -d lat,0,0 -d lon,0,0 -d lon,3,3 in.nc";
    $tst_cmd[0]="ncks $nco_D_flg -C -g g19g3 -v three_dmn_var_dbl -d time,,2 -d lat,0,0 -d lon,0,0 -d lon,3,3 $in_pth_arg in_grp_3.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="time[2]=3 lat[0]=-90 lon[3]=270 three_dmn_var_dbl[19]=20 watt meter-2";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 	

#ncks #73
# ncks -v lat -d latitude,0,1,1 in_grp.nc

    $dsc_sng="(Groups) Invalid dimension input -v lat -d latitude,0,1,1 in_grp.nc";
    $tst_cmd[0]="ncks $nco_D_flg -v lat -d latitude,0,1,1 $in_pth_arg in_grp.nc";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[1]="ncks: ERROR dimension latitude is not in input file";
    $tst_cmd[2]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[1]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
    $tst_cmd[2]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 		

#ncks #74
# ncks -O -p ~/nco/data in_grp.nc ~/foo.nc
# ncks --cdl -g g8g1g1g1 ~/foo.nc | grep answer

    $dsc_sng="(Groups) Copy/print metadata-only leaf group";
    $tst_cmd[0]="ncks -O $nco_D_flg $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks --cdl -g g8g1g1g1 %tmp_fl_00% | grep answer";
    if($HAVE_NETCDF4_H == 1){
    $tst_cmd[2]="            :answer = \"Twerking\" ;";
    $tst_cmd[3]="SS_OK";   
    }elsif($HAVE_NETCDF4_H == 0){
     $tst_cmd[2]="nco_err_exit(): ERROR NCO will now exit with system call exit(EXIT_FAILURE)"; 
     $tst_cmd[3]="SS_OK";        
    }
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncks #75
# ncks -O -c in_grp.nc out.nc

    $dsc_sng="(Groups) -c Extract all coordinate variables";
    $tst_cmd[0]="ncks -O $nco_D_flg $in_pth_arg -c in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -v lat %tmp_fl_00% | grep 'lat size'";
    $tst_cmd[2]="lat size (RAM) = 2*sizeof(NC_FLOAT) = 2*4 = 8 bytes";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncks #76
# ncks -O in_grp.nc out.nc

    $dsc_sng="(Groups) Default input dataset";
    $tst_cmd[0]="ncks -O $nco_D_flg $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -v lat %tmp_fl_00% | grep 'lat size'";
    $tst_cmd[2]="lat size (RAM) = 2*sizeof(NC_FLOAT) = 2*4 = 8 bytes";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#ncks #77
# ncks -O in.nc out.nc

    $dsc_sng="Default input dataset";
    $tst_cmd[0]="ncks -O $nco_D_flg $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -v lat %tmp_fl_00% | grep 'lat size'";
    $tst_cmd[2]="lat size (RAM) = 2*sizeof(NC_FLOAT) = 2*4 = 8 bytes";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		

#ncks #78
#ncks -O -3 -G : -g /g27g1/ in_grp_3.nc out.nc
    
    $dsc_sng="(Groups) Test flatenning with parallel variables (-3 -G : -g)";
    $tst_cmd[0]="ncks -O -3 -G : -g /g27g1/  $nco_D_flg $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -v delta_time_start %tmp_fl_00%";
    $tst_cmd[2]="delta_time_start dimension 0: delta_time_start, size = 2 NC_FLOAT (Coordinate is delta_time_start)";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	

#ncks #79
#ncks -O -3 -G : -g g28 -v delta_time_stop  in_grp_3.nc out.nc	
# simpler case of ncks -O -D 2 -3 -G : -g /altimetry/${DATA}/hdf/mabel_l2_20130927t201800_008_1.h5 ~/foo_mabel.nc
    
    $dsc_sng="(Groups) Test CF extraction with flattening (-3 -G : -g)";
    $tst_cmd[0]="ncks -O -3 -G : -g g28 -v delta_time_stop  $nco_D_flg $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -v delta_time_start %tmp_fl_00%";
    $tst_cmd[2]="delta_time_start dimension 0: delta_time_start, size = 1 NC_FLOAT (Coordinate is delta_time_start)";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		
	
#ncks #80
#ncks -O -4 -L 0 --cnk_dmn lev,1  -v two_dmn_var in_grp_3.nc out.nc
#ncks -C -g g19g1g1 --hdn --cdl out.nc | grep _DeflateLevel
    
    $dsc_sng="(Groups) Chunking and uncompression -L 0 --cnk_dmn lev,1 -v two_dmn_var";
    $tst_cmd[0]="ncks -O -L 0 -4 --cnk_dmn lev,1 -v two_dmn_var $nco_D_flg $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -g /g19/g19g1/g19g1g1 -v two_dmn_var -m %tmp_fl_00%";
    $tst_cmd[2]="two_dmn_var dimension 1: lev, size = 3, chunksize = 1 (Non-coordinate dimension)";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	

#ncks #81
# there are 2 'two_dmn_var' beneath /g19/g19g1, chunking only 1
#ncks -O -4 --cnk_dmn /g19/g19g1/g19g1g1/lev,1 -v two_dmn_var in_grp_3.nc out.nc
#ncks -m -C -v /g19/g19g1/two_dmn_var --hdn out.nc
    
    $dsc_sng="(Groups) Chunking and full dimension names --cnk_dmn /g19/g19g1/g19g1g1/lev,1";
    $tst_cmd[0]="ncks -O -4 --cnk_dmn /g19/g19g1/g19g1g1/lev,1 -v two_dmn_var $nco_D_flg $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -C -v /g19/g19g1/two_dmn_var --hdn %tmp_fl_00%";
    $tst_cmd[2]="two_dmn_var attribute 3: _ChunkSizes, size = 2 NC_INT, value = 2, 3";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#ncks #82
# there are 2 'two_dmn_var' beneath /g19/g19g1, chunking only 1
#ncks -O -4 --cnk_dmn /g19/g19g1/g19g1g1/lev,1 -v two_dmn_var in_grp_3.nc out.nc
#ncks -m -C -v /g19/g19g1/g19g1g1/two_dmn_var --hdn out.nc
    
    $dsc_sng="(Groups) Chunking and full dimension names --cnk_dmn /g19/g19g1/g19g1g1/lev,1";
    $tst_cmd[0]="ncks -O -4 --cnk_dmn /g19/g19g1/g19g1g1/lev,1 -v two_dmn_var $nco_D_flg $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -C -v /g19/g19g1/g19g1g1/two_dmn_var --hdn %tmp_fl_00% | grep _ChunkSizes";
    $tst_cmd[2]="two_dmn_var attribute 1: _ChunkSizes, size = 2 NC_INT, value = 2, 1";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#ncks #83 (part 1)
# there are 2 'two_dmn_var' beneath /g19/g19g1, chunking both, one with absolute other with relative
# ncks -O -4 --cnk_dmn /g19/g19g1/g19g1g1/lev,1 --cnk_dmn lev,3 -v two_dmn_var in_grp_3.nc out.nc
# ncks -m -C -v /g19/g19g1/g19g1g1/two_dmn_var --hdn out.nc | grep _ChunkSizes
    
    $dsc_sng="(Groups) Chunking and full dimension names --cnk_dmn /g19/g19g1/g19g1g1/lev,1";
    $tst_cmd[0]="ncks -O -4 --cnk_dmn /g19/g19g1/g19g1g1/lev,1 -v two_dmn_var $nco_D_flg $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -C -v /g19/g19g1/g19g1g1/two_dmn_var --hdn %tmp_fl_00% | grep _ChunkSizes";
    $tst_cmd[2]="two_dmn_var attribute 1: _ChunkSizes, size = 2 NC_INT, value = 2, 1";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#ncks #84 (part 2)
# there are 2 'two_dmn_var' beneath /g19/g19g1, chunking the 2 , one with absolute other with relative
# ncks -O -4 --cnk_dmn /g19/g19g1/g19g1g1/lev,1 --cnk_dmn lev,3  -v two_dmn_var in_grp_3.nc out.nc
# ncks -m -C -v /g19/g19g1/two_dmn_var  --hdn out.nc | grep _ChunkSizes
    
    $dsc_sng="(Groups) Chunking and full dimension names --cnk_dmn /g19/g19g1/g19g1g1/lev,1";
    $tst_cmd[0]="ncks -O -4 --cnk_dmn /g19/g19g1/g19g1g1/lev,1 --cnk_dmn lev,3 -v two_dmn_var $nco_D_flg $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -C -v /g19/g19g1/two_dmn_var --hdn %tmp_fl_00% | grep _ChunkSizes";
    $tst_cmd[2]="two_dmn_var attribute 3: _ChunkSizes, size = 2 NC_INT, value = 2, 3";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		

#ncks #85
#ncks -O -L 0 --cnk_dmn lat,2 -v one in_grp.nc out.nc
#ncks -C -m --hdn -v one -g g13  out.nc | grep  _Storage
    
    $dsc_sng="(Groups) Chunking and shuffle filter -L 0 --cnk_dmn lat,2 -v one (expect failuse with netCDF 4.1.1-)";
    $tst_cmd[0]="ncks -O -4 -L 0 --cnk_dmn lat,2 -v one $nco_D_flg $in_pth_arg hdn.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -m --hdn -v one -g g13 %tmp_fl_00% | grep _Storage";
    $tst_cmd[2]="one attribute 1: _Storage, size = 10 NC_CHAR, value = contiguous";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#ncks #86
#ncks -h -O --fix_rec_dmn time52 -v one_dmn_rec_var in_grp.nc out.nc
#ncks -v time52 -m  out.nc
    
    $dsc_sng="(Groups) Change record dimension to fixed dimension --fix_rec_dmn time52 -v one_dmn_rec_var";
    $tst_cmd[0]="ncks -h -O --fix_rec_dmn time52 -v one_dmn_rec_var $nco_D_flg $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -v time52 -m %tmp_fl_00%";
    $tst_cmd[2]="time52 dimension 0: time52, size = 10 NC_DOUBLE (Coordinate is time52)";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	

#ncks #87
#ncks -O -v lat29 in_grp_3.nc out.nc
    
    $dsc_sng="(Groups) Test ";
    $tst_cmd[0]="ncks -O -v lat29  $nco_D_flg $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -v lat29 %tmp_fl_00%  | grep 'standard_name'";
    $tst_cmd[2]="lat29 attribute 0: standard_name, size = 8 NC_CHAR, value = latitude";
    $tst_cmd[3]="SS_OK";     
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		

#ncks #88
# Test -X writing (apply limits to all standard 'lat' 'lon')
# ncks -O  -X 0.,1.,-30.,-29. -g g18g1  -v gds_3dvar  in_grp_3.nc out.nc

    $dsc_sng="(Groups) Auxiliary coordinates writing -X 0.,1.,-30.,-29. -g g18g1  -v gds_3dvar";
    $tst_cmd[0]="ncks $nco_D_flg -X 0.,1.,-30.,-29. -g g18g1  -v gds_3dvar $in_pth_arg in_grp_3.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks $nco_D_flg -v lon_gds_1 %tmp_fl_00%";
    $tst_cmd[2]="gds_crd[0]=1 lon_gds_1[0]=0 degree";
    $tst_cmd[3]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 		

#ncks #89
# Test -X (writing associated coordinates) 
# ncks -O  -X 0.,1.,-30.,-29. -g g18 -v gds_3dvar in_grp_3.nc out.nc
# ncks  -g g18g2 -v lat_gds_2 out.nc 

    $dsc_sng="(Groups) Auxiliary coordinates (writing associated coordinates) -X 0.,1.,-30.,-29. -g g18 -v gds_3dvar in_grp_3.nc";
    $tst_cmd[0]="ncks $nco_D_flg -X 0.,1.,-30.,-29. -g g18 -v gds_3dvar $in_pth_arg in_grp_3.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks $nco_D_flg -g g18g2 -C -v lat_gds_2 %tmp_fl_00%";
    $tst_cmd[2]="gds_crd[0]=1 lat_gds_2[0]=-30 degree";
    $tst_cmd[3]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 				

   } #### Group tests	
   
	
#ncks #90
# Test -X writing
# ncks -O -X 0.,1.,-30.,-29. -v gds_3dvar  in.nc out.nc

    $dsc_sng="Auxiliary coordinates writing -X 0.,1.,-30.,-29. -v gds_3dvar";
    $tst_cmd[0]="ncks $nco_D_flg -X 0.,1.,-30.,-29. -v gds_3dvar $in_pth_arg in.nc %tmp_fl_00%";
	$tst_cmd[1]="ncks $nco_D_flg -v gds_crd %tmp_fl_00%";
    $tst_cmd[2]="gds_crd[0]=1 lon_gds[0]=0 degree";
    $tst_cmd[3]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 			   
    
#####################
#### ncpdq tests #### -OK !
#####################
    $opr_nm='ncpdq';
####################
    
#####################
#### ncpdq PERMUTE -a (reverse, re-order) tests 
#####################    
    
#ncpdq #1
    
    $tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -a -lat -v lat $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v lat -d lat,0 %tmp_fl_00%";
    $dsc_sng="Reverse coordinate -a -lat -v lat";
    $tst_cmd[2]="90";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array
    
#ncpdq #2
#three_dmn_var(lat,lev,lon);

    $tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -a -lat,-lev,-lon -v three_dmn_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,0 -d lev,0 -d lon,0 %tmp_fl_00%";
    $dsc_sng="Reverse 3D variable a -lat,-lev,-lon -v three_dmn_var";
    $tst_cmd[2]=23;
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array
    
#ncpdq #3 NEW NCO 4.3.2
    
    $tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -C -a lon,lat -v three_dmn_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,0 -d lev,2 -d lon,3 %tmp_fl_00%";
    $dsc_sng="Re-order 3D variable (no associated coordinates) -C -a lon,lat -v three_dmn_var ";
    $tst_cmd[2]="11";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array    
    
#ncpdq #4 
    
    $tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -a lon,lat -v three_dmn_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var -d lat,0 -d lev,2 -d lon,3 %tmp_fl_00%";
    $dsc_sng="Re-order 3D variable (associated coordinates) -a lon,lat -v three_dmn_var ";
    $tst_cmd[2]="11";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array
    
#ncpdq #5
#three_dmn_var_dbl(time,lat,lon);
#ncpdq -h -O -C -a lat,lon,time -v three_dmn_var_dbl -d time,0,3 -d time,9,9 -d lon,0,0 -d lon,3,3 in.nc
    
    $tst_cmd[0]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -a lat,lon,time -v three_dmn_var_dbl -d time,0,3 -d time,9,9 -d lon,0,0 -d lon,3,3 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var_dbl -d lat,0 -d lon,1 -d time,2 %tmp_fl_00%";
    $dsc_sng="Re-order 3D variable with MSA -C -a lat,lon,time -v three_dmn_var_dbl -d time,0,3 -d time,9,9 -d lon,0,0 -d lon,3,3";
    $tst_cmd[2]="20";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array  
 
#NEW NCO 4.3.2 
#ncpdq #6 (same run as #5) 
#make sure first dimension is record
    
    $tst_cmd[0]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -a lat,lon,time -v three_dmn_var_dbl -d time,0,3 -d time,9,9 -d lon,0,0 -d lon,3,3 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -v three_dmn_var_dbl %tmp_fl_00% | grep  'three_dmn_var_dbl dimension 0: lat, size = 2 (Record non-coordinate dimension)'";
    $dsc_sng="Re-order 3D variable with MSA -C -a lat,lon,time -v three_dmn_var_dbl -d time,0,3 -d time,9,9 -d lon,0,0 -d lon,3,3";
    $tst_cmd[2]="three_dmn_var_dbl dimension 0: lat, size = 2 (Record non-coordinate dimension)";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array
    
#NEW NCO 4.3.2 
#ncpdq #7 MSA stride
#ncpdq -h -O -a lat,lon,time  -d time,1,3,2 -d lat,1,1,1 -d lon,1,3,2 -v three_dmn_var_dbl  in.nc out.nc 
#ncks -C -H -v three_dmn_var_dbl -d lat,0,0 -d lon,1,1 -d time,0  out.nc
#lat[0]=90 lon[1]=270 time[0]=2 three_dmn_var_dbl[2]=16 

    $tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -a lat,lon,time -d time,1,3,2 -d lat,1,1,1 -d lon,1,3,2 -v three_dmn_var_dbl  $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -v three_dmn_var_dbl -d lat,0,0 -d lon,1,1 -d time,0 %tmp_fl_00%";
    $dsc_sng="Re-order 3D variable with MSA (stride) -a lat,lon,time -d time,1,3,2 -d lat,1,1,1 -d lon,1,3,2 -v three_dmn_var_dbl";
    $tst_cmd[2]="lat[0]=90 lon[1]=270 time[0]=2 three_dmn_var_dbl[2]=16";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array
   
#ncpdq #8

    $tst_cmd[0]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -a lat,lon,-time -v three_dmn_var_dbl -d time,0,3 -d time,9,9 -d lon,0,0 -d lon,3,3 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_var_dbl -d lat,1 -d lon,1 -d time,4 %tmp_fl_00%";
    $dsc_sng="Re-order 3D variable with MSA+ reversal of time dim -C -a lat,lon,-time -v three_dmn_var_dbl -d time,0,3 -d time,9,9 -d lon,0,0 -d lon,3,3";
    $tst_cmd[2]="8";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array

#ncpdq #9
    
     $tst_cmd[0]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -a lon,lat -v three_dmn_var_dbl -d time,0,2 -d time,4 -d lat,1 -d lat,1 --msa_usr_rdr $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%2.f,' -v three_dmn_var_dbl -d time,1 -d lon,0 %tmp_fl_00%";
    $dsc_sng="Re-order 3D variable with MSA and --msa_usr_rdr flag -C -a lon,lat -v three_dmn_var_dbl -d time,0,2 -d time,4 -d lat,1 -d lat,1 --msa_usr_rdr";
    $tst_cmd[2]="13,13";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array
    
#NEW NCO 4.3.2
#ncpdq #10
# two_dmn_var (lat,lev) no change
# ncpdq -O -C -a lat,lev -v two_dmn_var in.nc out.nc
# ncks  -d lat,1,1 -d lev,1,1 out.nc

  $tst_cmd[0]="ncpdq $omp_flg -O -C $fl_fmt $nco_D_flg -a lat,lev -v two_dmn_var $in_pth_arg in.nc %tmp_fl_00%";
  $tst_cmd[1]="ncks -v two_dmn_var -d lat,1,1 -d lev,1,1 %tmp_fl_00%";
  $dsc_sng="Re-order 2D variable (-C , no MSA, no-reorder) -v two_dmn_var -a lat,lev";
  $tst_cmd[2]="lat[1] lev[1] two_dmn_var[4]=17.5 fraction";
  $tst_cmd[3]="SS_OK";
  if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
  $#tst_cmd=0; # Reset array
    
#NEW NCO 4.3.2
#ncpdq #11
# two_dmn_var (lat,lev) -C, no MSA (no associated coordinates)
# ncpdq -O -C -a lev,lat -v two_dmn_var in.nc out.nc
# ncks -v two_dmn_var -d lat,1,1 -d lev,1,1 out.nc

  $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -O -C -a lev,lat -v two_dmn_var $in_pth_arg in.nc %tmp_fl_00%";
  $tst_cmd[1]="ncks -v two_dmn_var -d lat,1,1 -d lev,1,1 %tmp_fl_00%";
  $dsc_sng="Re-order 2D variable (-C , no MSA) -v two_dmn_var -a lev,lat";
  $tst_cmd[2]="lev[1] lat[1] two_dmn_var[3]=17.5 fraction";
  $tst_cmd[3]="SS_OK";
  if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
  $#tst_cmd=0; # Reset array   

#NEW NCO 4.3.2
#ncpdq #12
# two_dmn_var (lat,lev) no MSA (associated coordinates)
# ncpdq -O -a lev,lat -v two_dmn_var in.nc out.nc
# ncks  -C -d lat,1,1 -d lev,1,1 out.nc
#$tst_cmd[2]="lev[1]=500 lat[1]=90 two_dmn_var[3]=17.5 fraction";

  $tst_cmd[0]="ncpdq $omp_flg -O $fl_fmt $nco_D_flg -a lev,lat -v two_dmn_var $in_pth_arg in.nc %tmp_fl_00%";
  $tst_cmd[1]="ncks -C -v two_dmn_var -d lat,1,1 -d lev,1,1 %tmp_fl_00%";
  $dsc_sng="Re-order 2D variable (no -C , no MSA) -v two_dmn_var -a lev,lat";
  $tst_cmd[2]="lev[1]=500 lat[1]=90 two_dmn_var[3]=17.5 fraction";
  $tst_cmd[3]="SS_OK";
  if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
  $#tst_cmd=0; # Reset array     
   

#NEW NCO 4.3.2
#ncpdq #13
# two_dmn_var (lat,lev) -C, MSA (no associated coordinates)
# ncpdq -O -C -a lev,lat -d lat,1,1 -d lev,1,1 -v two_dmn_var in.nc out.nc
# ncks out.nc

  $tst_cmd[0]="ncpdq $omp_flg -O -C $fl_fmt $nco_D_flg -a lev,lat -d lat,1,1 -d lev,1,1 -v two_dmn_var $in_pth_arg in.nc %tmp_fl_00%";
  $tst_cmd[1]="ncks -v two_dmn_var %tmp_fl_00%";
  $dsc_sng="Re-order 2D variable (-C , MSA) -v two_dmn_var -a lev,lat -d lat,1,1 -d lev,1,1";
  $tst_cmd[2]="lev[0] lat[0] two_dmn_var[0]=17.5 fraction";
  $tst_cmd[3]="SS_OK";
  if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
  $#tst_cmd=0; # Reset array   
  
#NEW NCO 4.3.2
#ncpdq #14
# two_dmn_var (lat,lev) MSA (associated coordinates)
# ncpdq -O -a lev,lat -d lat,1,1 -d lev,1,1 -v two_dmn_var in.nc out.nc
# ncks out.nc

  $tst_cmd[0]="ncpdq $omp_flg -O $fl_fmt $nco_D_flg -a lev,lat -d lat,1,1 -d lev,1,1 -v two_dmn_var $in_pth_arg in.nc %tmp_fl_00%";
  $tst_cmd[1]="ncks -v two_dmn_var %tmp_fl_00%";
  $dsc_sng="Re-order 2D variable (no -C , MSA) -v two_dmn_var -a lev,lat -d lat,1,1 -d lev,1,1";
  $tst_cmd[2]="lev[0]=500 lat[0]=90 two_dmn_var[0]=17.5 fraction";
  $tst_cmd[3]="SS_OK";
  if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
  $#tst_cmd=0; # Reset array  

#NEW NCO 4.3.2
#ncpdq #15
# two_dmn_rec_var(time,lev) 2D variable with record  (-C , no MSA)
# ncpdq -O -C -a lev,time -v two_dmn_rec_var in.nc out.nc
# ncks  -d time,1,1 -d lev,1,1 out.nc

  $tst_cmd[0]="ncpdq $omp_flg -O $fl_fmt $nco_D_flg -C -a lev,time -v two_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
  $tst_cmd[1]="ncks -v two_dmn_rec_var  -d time,1,1 -d lev,1,1 %tmp_fl_00%";
  $dsc_sng="Re-order 2D variable with record (-C , no MSA) -v two_dmn_rec_var -C -a lev,time";
  $tst_cmd[2]="lev[1] time[1] two_dmn_rec_var[11]=2.1 watt meter-2";
  $tst_cmd[3]="SS_OK";
  if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
  $#tst_cmd=0; # Reset array  


#NEW NCO 4.3.2
#ncpdq #16
# two_dmn_rec_var(time,lev) 2D variable with record  (no MSA)
# ncpdq -O -a lev,time -v two_dmn_rec_var in.nc out.nc
# ncks  -d time,1,1 -d lev,1,1 out.nc

  $tst_cmd[0]="ncpdq $omp_flg -O $fl_fmt $nco_D_flg -a lev,time -v two_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
  $tst_cmd[1]="ncks -v two_dmn_rec_var -d time,1,1 -d lev,1,1 %tmp_fl_00%";
  $dsc_sng="Re-order 2D variable with record (no -C , no MSA) -v two_dmn_rec_var -a lev,time";
  $tst_cmd[2]="lev[1]=500 time[1]=2 two_dmn_rec_var[11]=2.1 watt meter-2";
  $tst_cmd[3]="SS_OK";
  if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
  $#tst_cmd=0; # Reset array  

#NEW NCO 4.3.2
#ncpdq #17
# two_dmn_rec_var(time,lev) 2D variable with record  (MSA)
# ncpdq -O -C -a lev,time -d time,1,1 -d lev,1,1 -v two_dmn_rec_var in.nc out.nc
# ncks  out.nc

  $tst_cmd[0]="ncpdq $omp_flg -O $fl_fmt $nco_D_flg -C -a lev,time -d time,1,1 -d lev,1,1 -v two_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
  $tst_cmd[1]="ncks -v two_dmn_rec_var %tmp_fl_00%";
  $dsc_sng="Re-order 2D variable with record (-C , MSA) -a lev,time -d time,1,1 -d lev,1,1 -v two_dmn_rec_var";
  $tst_cmd[2]="lev[0] time[0] two_dmn_rec_var[0]=2.1 watt meter-2";
  $tst_cmd[3]="SS_OK";
  if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
  $#tst_cmd=0; # Reset array   
  
#NEW NCO 4.3.2
#ncpdq #18
# two_dmn_rec_var(time,lev) 2D variable with record  (MSA)
# ncpdq -O -a lev,time -d time,1,1 -d lev,1,1 -v two_dmn_rec_var in.nc out.nc
# ncks  out.nc

  $tst_cmd[0]="ncpdq $omp_flg -O $fl_fmt $nco_D_flg -a lev,time -d time,1,1 -d lev,1,1 -v two_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
  $tst_cmd[1]="ncks -v two_dmn_rec_var %tmp_fl_00%";
  $dsc_sng="Re-order 2D variable with record (no -C , MSA) -a lev,time -d time,1,1 -d lev,1,1 -v two_dmn_rec_var";
  $tst_cmd[2]="lev[0]=500 time[0]=2 two_dmn_rec_var[0]=2.1 watt meter-2";
  $tst_cmd[3]="SS_OK";
  if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
  $#tst_cmd=0; # Reset array  

#NEW NCO 4.3.2
#ncpdq #19
#three_dmn_var_dbl(time,lat,lon);
#ncpdq -h -O -a lat,time -v three_dmn_var -d time,1,1 -d lat,1,1 -d lon,1,1 in.nc out.nc
    
    $tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -a -lat,-time -v three_dmn_var_dbl -d time,1,6,2 -d lat,0,1 -d lon,1,1 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks  -H -C -v three_dmn_var_dbl -d lat,1,1 -d time,2,2 %tmp_fl_00%";
    $dsc_sng="Reverse/Re-order 3D variable -a -lat,-time -v three_dmn_var_dbl -d time,1,1 -d lat,1,1 -d lon,1,1";
    $tst_cmd[2]="lat[1]=-90 time[2]=2 lon[0]=90 three_dmn_var_dbl[5]=10";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array  
    
#NEW NCO 4.3.2
#ncpdq #20
#three_dmn_rec_var(time,lat,lon);
#ty(time,lat);
#ncpdq -h -O -a lat,time -v ty,three_dmn_rec_var in.nc out.nc
    
    $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -h -O -a lat,time -v ty,three_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -v ty -d time,1,1 -d lat,1,1 %tmp_fl_00%";
    $dsc_sng="Re-order several variables -a lat,time -v ty,three_dmn_rec_var (no MSA) Test1";
    $tst_cmd[2]="lat[1]=90 time[1]=2 ty[11]=4";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array   

#NEW NCO 4.3.2
#ncpdq #21 (same run as #20)
#three_dmn_rec_var(time,lat,lon);
#ty(time,lat);
#ncpdq -h -O -a lat,time -v ty,three_dmn_rec_var in.nc out.nc
    
    $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -h -O -a lat,time -v ty,three_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -v three_dmn_rec_var -d time,1,1 -d lat,1,1 -d lon,1,1 %tmp_fl_00%";
    $dsc_sng="Re-order several variables -a lat,time -v ty,three_dmn_rec_var (no MSA) Test2";
    $tst_cmd[2]="lat[1]=90 time[1]=2 lon[1]=90 three_dmn_rec_var[45]=14";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array         
  
#NEW NCO 4.3.2
#ncpdq #22
#three_dmn_rec_var(time,lat,lon); MSA
#ty(time,lat);
#ncpdq -h -O -a lat,time -v ty,three_dmn_rec_var -d time,1,6,2 -d lat,1,1 in.nc out.nc
    
    $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -h -O -a lat,time -v ty,three_dmn_rec_var -d time,1,6,2 -d lat,1,1 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -v ty -d time,2,2 %tmp_fl_00%";
    $dsc_sng="Re-order several variables -a lat,time -v ty,three_dmn_rec_var (MSA) Test1 read ty";
    $tst_cmd[2]="lat[0]=90 time[2]=6 ty[2]=12";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array 

#NEW NCO 4.3.2
#ncpdq #23 (same run as #22)
#three_dmn_rec_var(time,lat,lon); MSA
#ty(time,lat);
#ncpdq -h -O -a lat,time -v ty,three_dmn_rec_var -d time,1,6,2 -d lat,1,1 in.nc out.nc
    
    $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -h -O -a lat,time -v ty,three_dmn_rec_var -d time,1,6,2 -d lat,1,1 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks  -C -H -v three_dmn_rec_var -d lat,0,0 -d time,1,1 -d lon,1,1 %tmp_fl_00%";
    $dsc_sng="Re-order several variables -a lat,time -v ty,three_dmn_rec_var (MSA) Test2 read three_dmn_rec_var";
    $tst_cmd[2]="lat[0]=90 time[1]=4 lon[1]=90 three_dmn_rec_var[5]=30";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array  

#NEW NCO 4.3.2
#ncpdq #24 (same run as #22) check if output is the right record
#three_dmn_rec_var(time,lat,lon); 
#ty(time,lat);
#ncpdq -h -O -a lat,time -v ty,three_dmn_rec_var in.nc out.nc
    
    $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -h -O -a lat,time -v ty,three_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -C -v three_dmn_rec_var %tmp_fl_00% | grep 'three_dmn_rec_var dimension 0: lat, size = 2 NC_FLOAT (Record coordinate is lat)'";
    $dsc_sng="Re-order several variables -a lat,time -v ty,three_dmn_rec_var Test3 check record for lat";
    $tst_cmd[2]="three_dmn_rec_var dimension 0: lat, size = 2 NC_FLOAT (Record coordinate is lat)";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array  

#NEW NCO 4.3.2
#ncpdq #25 (same run as #22) check if output is the right record
#three_dmn_rec_var(time,lat,lon); 
#ty(time,lat);
#ncpdq -h -O -a lat,time -v ty,three_dmn_rec_var in.nc out.nc
    
    $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -h -O -a lat,time -v ty,three_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -C -v three_dmn_rec_var %tmp_fl_00% | grep 'three_dmn_rec_var dimension 1: time, size = 10 NC_DOUBLE (Coordinate is time)'";
    $dsc_sng="Re-order several variables -a lat,time -v ty,three_dmn_rec_var Test4 check non record for time";
    $tst_cmd[2]="three_dmn_rec_var dimension 1: time, size = 10 NC_DOUBLE (Coordinate is time)";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array  

#NEW NCO 4.3.2
#ncpdq #26
#four_dmn_rec_var(time,lat,lev,lon); MSA
#ncpdq -h -O -a lev,time,-lon,-lat -v four_dmn_rec_var -d time,1,6,2 -d lat,1,1 -d lon,1,1 -d lev,1,1 in.nc out.nc
#ncks  -C -H -v four_dmn_rec_var -d time,2,2 out.nc

    $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -h -O -a lev,time,-lon,-lat -v four_dmn_rec_var -d time,1,6,2 -d lat,1,1 -d lon,1,1 -d lev,1,1 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks  -C -H -v four_dmn_rec_var -d time,2,2 %tmp_fl_00%";
    $dsc_sng="Reverse/Re-order 4D variable -a lev,time,-lon,-lat -v four_dmn_rec_var (MSA)";
    $tst_cmd[2]="lev[0]=500 time[2]=6 lon[0]=90 lat[0]=90 four_dmn_rec_var[2]=138";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array  
    
#NEW NCO 4.3.2
#ncpdq #27 : reorder 2 variables with -a lat,time and check a variable that only has 1 (lat)
#PS(time,lat,lon)
#three_dmn_var_crd(lev,lat,lon);
#ncpdq -h -O -a lat,time -v PS,three_dmn_var_crd -d lev,1,1 -d lat,1,1 -d lon,1,1 -d lev,1,1 in.nc out.nc
#ncks  -C -H -v three_dmn_var_crd out.nc

    $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -h -O -a lat,time -v PS,three_dmn_var_crd -d lev,1,1 -d lat,1,1 -d lon,1,1 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -H -C -v three_dmn_var_crd %tmp_fl_00%";
    $dsc_sng="Re-order 3D variable (only 1 in -a) -a lat,time -v PS,three_dmn_var_crd (MSA)";
    $tst_cmd[2]="lat[0]=90 lev[0]=500 lon[0]=90 three_dmn_var_crd[0]=17";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array  
 
#NEW NCO 4.3.2
#ncpdq #28 
#ncpdq -h -O -a lat,time -d time,1,6,2 -d lat,1,1 in.nc out.nc
#ncks  -C -H -v -d time,2,2 four_dmn_rec_var out.nc

    $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -h -O -a lat,time -d time,1,1 -d lat,1,1 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -v four_dmn_rec_var -d lon,2,2 -d lev,2,2 %tmp_fl_00%";
    $dsc_sng="Re-order all variables -a lat,time (MSA)";
    $tst_cmd[2]="lat[0]=90 time[0]=2 lev[2]=1000 lon[2]=180 four_dmn_rec_var[10]=47";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array       
    
#####################
#### ncpdq PACK tests 
#####################       
    
#ncpdq #29
    
    $tst_cmd[0]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -P all_new -v upk $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -P upk -v upk %tmp_fl_00% %tmp_fl_00%";
    $tst_cmd[2]="ncks -C -H -s '%g' -v upk %tmp_fl_00%";
    $dsc_sng="Pack and then unpack scalar (uses only add_offset) -P all_new -v upk";
    $tst_cmd[3]="3";
    $tst_cmd[4]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array

#ncpdq #30
    
    $tst_cmd[0]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -P upk -v rec_var_dbl_mss_val_dbl_pck -d time,0,4 -d time,6 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_dbl_mss_val_dbl_pck -d time,5 %tmp_fl_00%";
    $dsc_sng="Unpack 1D variable with MSA -C P upk -v rec_var_dbl_mss_val_dbl_pck -d time,0,4 -d time,6";
    $tst_cmd[2]="7";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array

#ncpdq #31

    $tst_cmd[0]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -P all_xst -v three_dmn_var_dbl -d time,0,2 -d time,8,9 -d lon,0 -d lon,1 -d lat,1 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%i' -v three_dmn_var_dbl -d time,2 -d lon,1 -d lat,0 %tmp_fl_00%";
    $dsc_sng="Pack 3D variable with MSA -C -P all_xst -v three_dmn_var_dbl -d time,0,2 -d time,8,9 -d lon,0 -d lon,1 -d lat,1"; 
    ##### TODO 880
    $tst_cmd[2]="17505";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array
	
	
	#### Group tests	
	if($HAVE_NETCDF4_H == 1){

#####################
#### ncpdq GROUP tests 
#####################    

#NEW NCO 4.3.2
#ncpdq #11
# two_dmn_var (lat,lev) -C, no MSA (no associated coordinates)
# ncpdq -O -C -a lev,lat -v two_dmn_var in.nc out.nc
# ncks -v two_dmn_var -d lat,1,1 -d lev,1,1 out.nc


# same as previous but with group
    
#NEW NCO 4.3.2
#ncpdq #32
# group: g19 { 
# dimensions:lat=2;lev=3;lon=4;time=unlimited;
#ncpdq #11
# two_dmn_var (lat,lev) -C, no MSA (no associated coordinates)
# ncpdq -O -C -g g19g1 -a lev,lat -v two_dmn_var in_grp_3.nc out.nc
# ncks -g g19g1 -v two_dmn_var -d lat,1,1 -d lev,1,1 out.nc


   $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -O -C -g g19g1 -a lev,lat -v two_dmn_var $in_pth_arg in_grp_3.nc %tmp_fl_00%";
   $tst_cmd[1]="ncks -g g19g1 -v two_dmn_var -d lat,1,1 -d lev,1,1 %tmp_fl_00%";
   $dsc_sng="(Groups) Re-order 2D variable (-C , no MSA) -v two_dmn_var -a lev,lat";
   $tst_cmd[2]="lev[1] lat[1] two_dmn_var[3]=17.5 fraction";
   $tst_cmd[3]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array    
 
 
#NEW NCO 4.3.2
#ncpdq #12
# two_dmn_var (lat,lev) no -C, no MSA
# ncpdq -O -a lev,lat -v two_dmn_var in.nc out.nc
# ncks  -C -d lat,1,1 -d lev,1,1 out.nc


# same as previous but with group
    
#NEW NCO 4.3.2
#ncpdq #33
# group: g19 { 
# dimensions:lat=2;lev=3;lon=4;time=unlimited;
#ncpdq #12
# two_dmn_var (lat,lev) no -C, no MSA 
# ncpdq -O -g g19g1 -a lev,lat -v two_dmn_var in_grp_3.nc out.nc
# ncks -C -g g19g1 -v two_dmn_var -d lat,1,1 -d lev,1,1 out.nc

   $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -O -g g19g1 -a lev,lat -v two_dmn_var $in_pth_arg in_grp_3.nc %tmp_fl_00%";
   $tst_cmd[1]="ncks -C -g g19g1 -v two_dmn_var -d lat,1,1 -d lev,1,1 %tmp_fl_00%";
   $dsc_sng="(Groups) Re-order 2D variable (no -C , no MSA) -v two_dmn_var -a lev,lat";
   $tst_cmd[2]="lev[1]=500 lat[1]=90 two_dmn_var[3]=17.5 fraction";
   $tst_cmd[3]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array    

#NEW NCO 4.3.2
#ncpdq #13
# two_dmn_var (lat,lev) -C, MSA (no associated coordinates)
# ncpdq -O -C -a lev,lat -d lat,1,1 -d lev,1,1 -v two_dmn_var in.nc out.nc
# ncks out.nc


# same as previous but with group
    
#NEW NCO 4.3.2
#ncpdq #34
# group: g19 { 
# dimensions:lat=2;lev=3;lon=4;time=unlimited;
#ncpdq #13
# two_dmn_var (lat,lev) -C, MSA (no associated coordinates)
# ncpdq -O -C -a lev,lat -d lat,1,1 -d lev,1,1 -v two_dmn_var in_grp_3.nc out.nc
# ncks out.nc


   $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -O -g g19g1 -C -a lev,lat -v two_dmn_var -d lat,1,1 -d lev,1,1 $in_pth_arg in_grp_3.nc %tmp_fl_00%";
   $tst_cmd[1]="ncks -g g19g1 -v two_dmn_var %tmp_fl_00%";
   $dsc_sng="(Groups) Re-order 2D variable (-C , MSA) -v two_dmn_var -a lev,lat";
   $tst_cmd[2]="lev[0] lat[0] two_dmn_var[0]=17.5 fraction";
   $tst_cmd[3]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array      

#NEW NCO 4.3.2
#ncpdq #14
# two_dmn_var (lat,lev) MSA (associated coordinates)
# ncpdq -O -a lev,lat -d lat,1,1 -d lev,1,1 -v two_dmn_var in.nc out.nc
# ncks out.nc
#$tst_cmd[2]="lev[0]=500 lat[0]=90 two_dmn_var[0]=17.5 fraction";

# same as previous but with group
    
#NEW NCO 4.3.2
#ncpdq #35
# group: g19 { 
# dimensions:lat=2;lev=3;lon=4;time=unlimited;
# two_dmn_var (lat,lev) MSA (associated coordinates)
# ncpdq -O -g g19g1 -a lev,lat -d lat,1,1 -d lev,1,1 -v two_dmn_var in_grp_3.nc out.nc
# ncks -C -g g19g1 -v two_dmn_var out.nc

   $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -O -g g19g1 -a lev,lat -v two_dmn_var -d lat,1,1 -d lev,1,1 $in_pth_arg in_grp_3.nc %tmp_fl_00%";
   $tst_cmd[1]="ncks -C -g g19g1 -v two_dmn_var %tmp_fl_00%";
   $dsc_sng="(Groups) Re-order 2D variable (no -C , MSA) -v two_dmn_var -a lev,lat";
   $tst_cmd[2]="lev[0]=500 lat[0]=90 two_dmn_var[0]=17.5 fraction";
   $tst_cmd[3]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array      

#NEW NCO 4.3.2
#ncpdq #15
# two_dmn_rec_var(time,lev) 2D variable with record  (-C , no MSA)
# ncpdq -O -C -a lev,time -v two_dmn_rec_var in.nc out.nc
# ncks  -d time,1,1 -d lev,1,1 out.nc

# same as previous but with group
    
#NEW NCO 4.3.2
#ncpdq #36
# group: g19 { 
# dimensions:lat=2;lev=3;lon=4;time=unlimited;
# ncpdq -O -g g19g2 -C -a lev,time -v two_dmn_rec_var in_grp_3.nc out.nc
# ncks  -d time,1,1 -d lev,1,1 out.nc

   $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -O -g g19g2 -C -a lev,time -v two_dmn_rec_var $in_pth_arg in_grp_3.nc %tmp_fl_00%";
   $tst_cmd[1]="ncks -g g19g2 -v two_dmn_rec_var -d time,1,1 -d lev,1,1 %tmp_fl_00%";
   $dsc_sng="(Groups) Re-order 2D variable with record (-C , no MSA) -v two_dmn_rec_var -C -a lev,time";
   $tst_cmd[2]="lev[1] time[1] two_dmn_rec_var[11]=2.1 watt meter-2";
   $tst_cmd[3]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array      

#NEW NCO 4.3.2
#ncpdq #37
# two_dmn_rec_var(time,lev) 2D variable with record  (no -C, no MSA)
# ncpdq -O -g g19g2 -a lev,time -v two_dmn_rec_var in_grp_3.nc out.nc
# ncks -C -g g19g2 -d time,1,1 -d lev,1,1 out.nc

# same #16 as previous but with group

   $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -O -g g19g2 -a lev,time -v two_dmn_rec_var $in_pth_arg in_grp_3.nc %tmp_fl_00%";
   $tst_cmd[1]="ncks -C -g g19g2 -d time,1,1 -d lev,1,1 %tmp_fl_00%";
   $dsc_sng="(Groups) Re-order 2D variable with record (no -C , no MSA) -v two_dmn_rec_var -C -a lev,time";
   $tst_cmd[2]="lev[1]=500 time[1]=2 two_dmn_rec_var[11]=2.1 watt meter-2";
   $tst_cmd[3]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array      

#NEW NCO 4.3.2
#ncpdq #38
# two_dmn_rec_var(time,lev) 2D variable with record  (MSA)
# ncpdq -O -C -a lev,time -d time,1,1 -d lev,1,1 -v two_dmn_rec_var in_grp_3.nc out.nc
# ncks -g g19g2 -v two_dmn_rec_var out.nc

# same as #17 but with group

   $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -O -C -g g19g2 -a lev,time -v two_dmn_rec_var -d time,1,1 -d lev,1,1 $in_pth_arg in_grp_3.nc %tmp_fl_00%";
   $tst_cmd[1]="ncks -g g19g2 -v two_dmn_rec_var %tmp_fl_00%";
   $dsc_sng="(Groups) Re-order 2D variable with record (-C , MSA) -v two_dmn_rec_var -C -a lev,time";
   $tst_cmd[2]="lev[0] time[0] two_dmn_rec_var[0]=2.1 watt meter-2";
   $tst_cmd[3]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array      

#NEW NCO 4.3.2
#ncpdq #39
# two_dmn_rec_var(time,lev) 2D variable with record  (MSA)
# ncpdq -O -g g19g2 -a lev,time -d time,1,1 -d lev,1,1 -v two_dmn_rec_var in_grp_3.nc out.nc
# ncks  -C -g g19g2 -v two_dmn_rec_var out.nc
# $tst_cmd[2]="lev[0]=500 time[0]=2 two_dmn_rec_var[0]=2.1 watt meter-2";

# same as #18 but with group
  
   $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -O -g g19g2 -a lev,time -v two_dmn_rec_var -d time,1,1 -d lev,1,1 $in_pth_arg in_grp_3.nc %tmp_fl_00%";
   $tst_cmd[1]="ncks -C -g g19g2 -v two_dmn_rec_var %tmp_fl_00%";
   $dsc_sng="(Groups) Re-order 2D variable with record (no -C , MSA) -v two_dmn_rec_var";
   $tst_cmd[2]="lev[0]=500 time[0]=2 two_dmn_rec_var[0]=2.1 watt meter-2";
   $tst_cmd[3]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array  

#NEW NCO 4.3.5
#ncpdq -O -a -lat -g g23,g24 ~/nco/data/in_grp_3.nc  out.nc
  
   $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -O -a -lat -g g23,g24 $in_pth_arg in_grp_3.nc %tmp_fl_00%";
   $tst_cmd[1]="ncks -v lat -g g23 %tmp_fl_00%";
   $dsc_sng="(Groups) Reverse -a lat -g g23 several groups";
   $tst_cmd[2]="lat[1]=-60 degrees_north";
   $tst_cmd[3]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array  

#NEW NCO 4.3.5
#ncpdq -O -a -lat -g g23,g24 ~/nco/data/in_grp_3.nc  out.nc
  
   $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -O -a -lat -g g23,g24 $in_pth_arg in_grp_3.nc %tmp_fl_00%";
   $tst_cmd[1]="ncks -v lat -g g24 %tmp_fl_00%";
   $dsc_sng="(Groups) Reverse -a lat -g g24 several groups";
   $tst_cmd[2]="lat[1]=-60 degrees_north";
   $tst_cmd[3]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array      

#####################
#### ncpdq PACK GROUP tests 
#####################   

#NEW NCO 4.3.2patch
#ncpdq #40
#ncpdq -h -O -P all_new -v upk in.nc out.nc
#ncpdq -h -O -P upk -v upk out.nc out.nc
#ncks -C -H -s '%g' -v upk out.nc

# same as #29 but with group

   $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -h -O -g g19g4 -P all_new -v upk $in_pth_arg in_grp_3.nc %tmp_fl_00%";
   $tst_cmd[1]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -P upk -v upk %tmp_fl_00% %tmp_fl_00%";
   $dsc_sng="(Groups) Pack and then unpack scalar (uses only add_offset) -P all_new -v upk";
   $tst_cmd[2]="ncks -C -H -s '%g' -v upk %tmp_fl_00%";
   $tst_cmd[3]="3";
   $tst_cmd[4]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array  

#NEW NCO 4.3.2patch
#ncpdq #41
# same as ncpdq #30
#ncpdq -g g19g4 -h -O -C -P upk -v rec_var_dbl_mss_val_dbl_pck -d time,0,4 -d time,6 in_grp_3.nc out.nc
#ncks -g g19g4 -C -H -s '%f' -v rec_var_dbl_mss_val_dbl_pck -d time,5 out.nc
#$tst_cmd[2]="7";

# same as #30 but with group

   $tst_cmd[0]="ncpdq $omp_flg $fl_fmt $nco_D_flg -g g19g4 -h -O -C -P upk -v rec_var_dbl_mss_val_dbl_pck -d time,0,4 -d time,6 $in_pth_arg in_grp_3.nc %tmp_fl_00%";
   $tst_cmd[1]="ncks -g g19g4 -C -H -s '%f' -v rec_var_dbl_mss_val_dbl_pck -d time,5 %tmp_fl_00%";
   $dsc_sng="(Groups) Unpack 1D variable with MSA -C P upk -v rec_var_dbl_mss_val_dbl_pck -d time,0,4 -d time,6";
   $tst_cmd[2]="7";
   $tst_cmd[3]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array  
   
   	
#NEW NCO 4.4.0
#ncpdq #46
# ncpdq -O -4  -a lon,lat --cnk_dmn lat,1 --cnk_dmn lon,2 -v lat_2D_rct in.nc out.nc

   $dsc_sng="Chunking -a lon,lat --cnk_dmn lat,1 --cnk_dmn lon,2 -v lat_2D_rct";
   $tst_cmd[0]="ncpdq $omp_flg -4 $nco_D_flg -a lon,lat --cnk_dmn lat,1 --cnk_dmn lon,2 -v lat_2D_rct $in_pth_arg in.nc %tmp_fl_00%";
   $tst_cmd[1]="ncks -m %tmp_fl_00% | grep 'lat_2D_rct dimension 0'";
   $tst_cmd[2]="lat_2D_rct dimension 0: lon, size = 4 NC_FLOAT, chunksize = 2 (Coordinate is lon)";
   $tst_cmd[3]="SS_OK";
   if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
   $#tst_cmd=0; # Reset array  	
	
	
	} ##### Group tests	
	
#ncpdq #42

    $tst_cmd[0]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -P upk -v pck_7 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -P upk -v pck_7 %tmp_fl_00% %tmp_fl_01%";
    $tst_cmd[2]="ncks -C -H -s '%f' -v pck_7 %tmp_fl_01%";
    $dsc_sng="Unpack 1D variable twice to verify first unpacking does not inadvertently store packing attributes";
    $tst_cmd[3]="7";
    $tst_cmd[4]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array
	
#ncpdq #43	

    $tst_cmd[0]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -P upk -v pck_7 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v pck_7 %tmp_fl_00%";
    $dsc_sng="Unpack 1D variable with netCDF convention";
    $tst_cmd[2]="7";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array
	
#ncpdq #44	

    $tst_cmd[0]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -P upk --hdf_upk -v pck_7 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v pck_7 %tmp_fl_00%";
    $dsc_sng="Unpack 1D variable with HDF convention";
    $tst_cmd[2]="-8";
    $tst_cmd[3]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array
	
#ncpdq #45	

    $tst_cmd[0]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -P xst_new --hdf_upk -v pck_7 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncpdq $omp_flg -h -O -C $fl_fmt $nco_D_flg -P upk -v pck_7 %tmp_fl_00% %tmp_fl_01%";
    $tst_cmd[2]="ncks -C -H -s '%f' -v pck_7 %tmp_fl_01%";
    $dsc_sng="Re-pack 1D variable from HDF convention to netCDF convention";
    $tst_cmd[3]="-8";
    $tst_cmd[4]="SS_OK";
    if($mpi_prc == 0 || ($mpi_prc > 0 && !($localhostname =~ /pbs/))){NCO_bm::tst_run(\@tst_cmd);} # ncpdq hangs with MPI TODO nco772
    $#tst_cmd=0; # Reset array	
	


####################
#### ncrcat tests ## OK !
####################
    $opr_nm='ncrcat';
####################

#ncrcat #01

#if($mpi_prc == 0){ # fxm test hangs because of ncrcat TODO 593
    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -d time,11 -s '%f' -v rec_var_flt_mss_val_dbl %tmp_fl_00%";
    $dsc_sng="Concatenate float with double missing values across two files";
    $tst_cmd[2]="2";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #02	

    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v one_dmn_rec_var $in_pth_arg in.nc in.nc -d time,2.,3. %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%d ' -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="Concatenate record coordinate-bounded variable across two non-monotonic files";
    $tst_cmd[2]="2 3 2 3";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #03	

    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v three_dmn_var_dbl $in_pth_arg -d time,,2 -d lat,0,0 -d lon,0,0 -d lon,3,3 in.nc in.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max %tmp_fl_00% %tmp_fl_01%"; 
    $tst_cmd[2]="ncks -C -H -s '%f' -v three_dmn_var_dbl %tmp_fl_01%";
    $dsc_sng="Concatenate float variable with multislabs across two files";
    $tst_cmd[3]="20";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #04	

    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v three_dmn_var_int $in_pth_arg -d time,,6 -d lat,0,0 -d lon,0,0 -d lon,3,3 in.nc in.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg %tmp_fl_00% %tmp_fl_01%"; 
    $tst_cmd[2]="ncks -C -H -s '%d' -v three_dmn_var_int %tmp_fl_01%";
    $dsc_sng="Concatenate int variable with multislabs across two files";
    $tst_cmd[3]="28";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #05	

    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v four_dmn_rec_var $in_pth_arg -d time,0,,4 -d lat,0,0 -d lon,0,1 -d lon,3,3  in.nc in.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncwa $omp_flg -h -y max -O $fl_fmt $nco_D_flg %tmp_fl_00% %tmp_fl_01%"; 
    $tst_cmd[2]="ncks -C -H -s '%f' -v four_dmn_rec_var %tmp_fl_01%";
    $dsc_sng="Concatenate float 4D variable with multislabs across two files";
    $tst_cmd[3]="204";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #06	

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time+=10;' $in_pth_arg in.nc %tmp_fl_00%";    
    $tst_cmd[1]="ncra -Y ncrcat -O $omp_flg $fl_fmt $nco_D_flg -C -v time -d time,0,,4 $in_pth/in.nc %tmp_fl_00% %tmp_fl_01% 2> %tmp_fl_02%";
    $tst_cmd[2]="ncks -C -H -s '%2.f,' -v time %tmp_fl_01%";
    $dsc_sng="Concatenate 1D variable with stride across two files";
    $tst_cmd[3]=" 1, 5, 9,13,17";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #07	

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time+=10;' $in_pth_arg in.nc %tmp_fl_00%"; 
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time+=20;' $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncra -Y ncrcat -O $omp_flg $fl_fmt $nco_D_flg -C -v time -d time,0,,8 $in_pth/in.nc %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% 2> %tmp_fl_03%";
    $tst_cmd[3]="ncks -C -H -s '%2.f,' -v time %tmp_fl_02%";
    $dsc_sng="Concatenate 1D variable with stride across three files";
    $tst_cmd[4]=" 1, 9,17,25";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #08	

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time+=10;' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks $fl_fmt $nco_D_flg -O -C -v time -d time,0,4 %tmp_fl_00% %tmp_fl_01%";  
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time+=20;' $in_pth_arg in.nc %tmp_fl_02%";
    $tst_cmd[3]="ncks $fl_fmt $nco_D_flg -O -C -v time -d time,5,7 %tmp_fl_02% %tmp_fl_03%";
    $tst_cmd[4]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,10,,2 $in_pth/in.nc %tmp_fl_01% %tmp_fl_03% %tmp_fl_04% 2> %tmp_fl_05%";
    $tst_cmd[5]="ncks -C -H -s '%2.f,' -v time %tmp_fl_04%";
    $dsc_sng="Concatenate 1D variable with stride across files with different record dimensions";
    $tst_cmd[6]="11,13,15,27";
    $tst_cmd[7]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #09	

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-03-22 12:09:00 -9:00\"' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time $in_pth/in.nc %tmp_fl_00% %tmp_fl_01% 2> %tmp_fl_05%";
    $tst_cmd[2]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[3]="time_ttl = 210";
    $dsc_sng="Concatenate 1D variable across two files no limits. Requires UDUnits.";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #10	

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-03-22 12:09:00 -9:00\"' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-04-01 12:09:00 -9:00\"' $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,8.0,9.0 $in_pth/in.nc %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% 2> %tmp_fl_05%";
    $tst_cmd[3]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tmp_fl_02% %tmp_fl_03%";
    $tst_cmd[4]="time_ttl = 17";
    $dsc_sng="Concatenate 1D variable across three files double limits. Requires UDUnits.";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #11	

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-03-22 12:09:00 -9:00\"' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-04-01 12:09:00 -9:00\"' $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,11.0,20.0 $in_pth/in.nc %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% 2> %tmp_fl_05%";
    $tst_cmd[3]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tmp_fl_02% %tmp_fl_03%";
    $tst_cmd[4]="time_ttl = 155";
    $dsc_sng="Concatenate 1D variable across three files double limits2. Requires UDUnits.";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #12	

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-03-22 12:09:00 -9:00\"' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time\@units=\"days since 1964-04-01 12:09:00 -9:00\"' $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,'1964-03-22 12:00','1964-03-25 12:00' $in_pth/in.nc %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% 2> %tmp_fl_05%";
    $tst_cmd[3]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tmp_fl_02% %tmp_fl_03%";
    $tst_cmd[4]="time_ttl = 33";
    $dsc_sng="Concatenate 1D variable across three files time stamp limits2. Requires UDUnits.";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #13	

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1\"' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 10:0:0\"' $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 20:0:0\"' $in_pth_arg in.nc %tmp_fl_02%";
    $tst_cmd[3]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,0.0,,11 %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% %tmp_fl_03% 2> %tmp_fl_05%";
    $tst_cmd[4]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tmp_fl_03% %tmp_fl_04%";
    $tst_cmd[5]="time_ttl = 33";
    $dsc_sng="Cat. 1D var over 3 files with large stride. Requires UDUnits.";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #14	

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1\"' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 10:0:0\"' $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 20:0:0\"' $in_pth_arg in.nc %tmp_fl_02%";
    $tst_cmd[3]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,22.0,,7 %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% %tmp_fl_03% 2> %tmp_fl_05%";
    $tst_cmd[4]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tmp_fl_03% %tmp_fl_04%";
    $tst_cmd[5]="time_ttl = 51";
    $dsc_sng="Cat. 1D var over 3 files coord limits+stride. Requires UDUnits.";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #15	

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1\"' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 10:0:0\"' $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 20:0:0\"' $in_pth_arg in.nc %tmp_fl_02%";
    $tst_cmd[3]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,'1990-1-1 22:0:0',,7 %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% %tmp_fl_03% 2> %tmp_fl_05%";
    $tst_cmd[4]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tmp_fl_03% %tmp_fl_04%";
    $tst_cmd[5]="time_ttl = 51";
    $dsc_sng="Cat. 1D var over three files date-stamp limits+stride. Requires UDUnits.";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #16	

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1\"' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 10:0:0\"' $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 20:0:0\"' $in_pth_arg in.nc %tmp_fl_02%";
    $tst_cmd[3]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,'1990-1-1 ',,9 %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% %tmp_fl_03% 2> %tmp_fl_05%";
    $tst_cmd[4]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tmp_fl_03% %tmp_fl_04%";
    $tst_cmd[5]="time_ttl = 54";
    $dsc_sng="Cat. 1D var over 3 files date-stamp limits+stride. Requires UDUnits.";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #17

    $tst_cmd[0]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1\"' $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 10:0:0\"' $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -v -s 'time=time-1;time\@units=\"hours since 1990-1-1 20:0:0\"' $in_pth_arg in.nc %tmp_fl_02%";
    $tst_cmd[3]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,,,20 %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% %tmp_fl_03% 2> %tmp_fl_05%";
    $tst_cmd[4]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tmp_fl_03% %tmp_fl_04%";
    $tst_cmd[5]="time_ttl = 20";
    $dsc_sng="Cat. 1D variable over 3 files dmn limits+stride. Requires UDUnits.";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #18	

    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-1' %tmp_fl_00%";   
    $tst_cmd[2]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,1 $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[3]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-2' %tmp_fl_01%";   
    $tst_cmd[4]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,3 $in_pth_arg in.nc %tmp_fl_02%";
    $tst_cmd[5]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-4' %tmp_fl_02%";   
    $tst_cmd[6]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,7 $in_pth_arg in.nc %tmp_fl_03%";
    $tst_cmd[7]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-8' %tmp_fl_03%";   
    $tst_cmd[8]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,7, %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% %tmp_fl_03% %tmp_fl_04% 2> %tmp_fl_06%";
    $tst_cmd[9]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tmp_fl_04% %tmp_fl_05%";
    $tst_cmd[10]="time_ttl = 92";
    $dsc_sng="Cat. 1D var over 4 files different record sizes. Requires UDUnits.";
    $tst_cmd[11]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #19	

    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-1' %tmp_fl_00%";   
    $tst_cmd[2]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,1 $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[3]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-2' %tmp_fl_01%";   
    $tst_cmd[4]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,3 $in_pth_arg in.nc %tmp_fl_02%";
    $tst_cmd[5]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-4' %tmp_fl_02%";   
    $tst_cmd[6]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,7 $in_pth_arg in.nc %tmp_fl_03%";
    $tst_cmd[7]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-8' %tmp_fl_03%";   
    $tst_cmd[8]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,,,4 %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% %tmp_fl_03% %tmp_fl_04% 2> %tmp_fl_06%";
    $tst_cmd[9]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tmp_fl_04% %tmp_fl_05%";
    $tst_cmd[10]="time_ttl = 28";
    $dsc_sng="Cat. 1D var over 4 files with srd=4 different rec sizes. Requires UDUnits.";
    $tst_cmd[11]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #20	

    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-1' %tmp_fl_00%";   
    $tst_cmd[2]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,1 $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[3]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-2' %tmp_fl_01%";   
    $tst_cmd[4]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,3 $in_pth_arg in.nc %tmp_fl_02%";
    $tst_cmd[5]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-4' %tmp_fl_02%";   
    $tst_cmd[6]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,7 $in_pth_arg in.nc %tmp_fl_03%";
    $tst_cmd[7]="ncatted -h -O $nco_D_flg -a units,time,o,c,'days since 1981-2-8' %tmp_fl_03%";   
    $tst_cmd[8]="ncra -Y ncrcat -O $fl_fmt $nco_D_flg -C -v time -d time,'1981-2-4 ','1981-2-20 ',3 %tmp_fl_00% %tmp_fl_01% %tmp_fl_02% %tmp_fl_03% %tmp_fl_04% 2> %tmp_fl_06%";
    $tst_cmd[9]="ncap2 -O -v -C -s 'time_ttl=time.total();print(time_ttl)' %tmp_fl_04% %tmp_fl_05%";
    $tst_cmd[10]="time_ttl = 45";
    $dsc_sng="Cat. 1D var over 4 files with srd=3 str/end timestamps. Requires UDUnits.";
    $tst_cmd[11]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #21	

    $tst_cmd[0]="ncks $omp_flg -h -O $fl_fmt $nco_D_flg -v one_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncrcat $omp_flg -h --no_tmp_fl --rec_apn $fl_fmt $nco_D_flg -v one_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[2]="ncks -C -H -s '%d' -d time,19 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="Append records to existing file without copying original";
    $tst_cmd[3]="10";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#### Group tests	
	if($HAVE_NETCDF4_H == 1){
	
#ncrcat #22	
# same as ncrcat #02 with group

    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -g g4 -v one_dmn_rec_var $in_pth_arg in_grp.nc in_grp.nc -d time,2.,3. %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%d ' -g g4 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="(Groups) Concatenate record coordinate-bounded variable across two non-monotonic files";
    $tst_cmd[2]="2 3 2 3";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#ncrcat #23	
# 2 groups each one with a record (part 1)
# ncra -Y ncrcat -h -O -g g25g1,g25g2 -v one_dmn_rec_var in_grp_3.nc in_grp_3.nc -d time,2.,3. out.nc

    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -g g25g1,g25g2 -v one_dmn_rec_var $in_pth_arg in_grp_3.nc in_grp_3.nc -d time,2.,3. %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%d ' -g g25g1 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="(Groups) 2 groups Concatenate record coordinate-bounded variable across two non-monotonic files";
    $tst_cmd[2]="2 3 2 3";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncrcat #24
# 2 groups each one with a record (part 2)
# ncra -Y ncrcat -h -O -g g25g1,g25g2 -v one_dmn_rec_var in_grp_3.nc in_grp_3.nc -d time,2.,3. out.nc

    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -g g25g1,g25g2 -v one_dmn_rec_var $in_pth_arg in_grp_3.nc in_grp_3.nc -d time,2.,3. %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%d ' -g g25g2 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="(Groups) 2 groups Concatenate record coordinate-bounded variable across two non-monotonic files";
    $tst_cmd[2]="2 3 2 3";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		
	
#ncrcat #25
#same as #03 

    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v three_dmn_var_dbl $in_pth_arg -d time,,2 -d lat,0,0 -d lon,0,0 -d lon,3,3 in_grp_3.nc in_grp_3.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max %tmp_fl_00% %tmp_fl_01%"; 
    $tst_cmd[2]="ncks -C -H -s '%f' -g g19g3 -v three_dmn_var_dbl %tmp_fl_01%";
    $dsc_sng="(Groups) Concatenate float variable with multislabs across two files";
    $tst_cmd[3]="20";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
	} #### Group tests	
	
# ncrcat #26
# Detect input_complete when stride skips user-specified idx_end_max
# ncrcat -O -C -v time -d time,0,10,9,1 -p ~/nco/data in.nc in.nc ~/foo.nc
# ncks -C -H -s '%g, ' -v time ~/foo.nc
    $dsc_sng="Detect input_complete when stride skips user-specified idx_end_max";
    $tst_cmd[0]="ncra -Y ncrcat -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,10,9,1 $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%g, ' -v time %tmp_fl_00%";
    $tst_cmd[2]="1, 10,";
    $tst_cmd[3]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 		

# ncrcat #27
# Another detect input_complete when stride skips user-specified idx_end_max
# ncks -O -C -v time -d time,0,2 ~/nco/data/in.nc ~/foo1.nc
# ncks -O -C -v time -d time,3,5 ~/nco/data/in.nc ~/foo2.nc
# ncra -Y ncrcat -D 3 -O -C -v time -d time,0,3,2,1 -p ~ foo1.nc foo2.nc ~/foo.nc
# ncks -C -H -s '%g, ' -v time ~/foo.nc
    $dsc_sng="Another detect input_complete when stride skips user-specified idx_end_max";
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,2 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -C -v time -d time,3,5 $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncra -Y ncrcat -h -O $fl_fmt $nco_D_flg -C -v time -d time,0,3,2,1 %tmp_fl_00% %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[3]="ncks -C -H -s '%g, ' -v time %tmp_fl_02%";
    $tst_cmd[4]="1, 3,";
    $tst_cmd[5]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 	
	
#### Group tests	
	if($HAVE_NETCDF4_H == 1){	

#ncrcat #28
#ncks -h -O -g g4 -v one_dmn_rec_var in_grp.nc in_grp1.nc
#ncrcat -h --no_tmp_fl --rec_apn -v one_dmn_rec_var in_grp.nc in_grp1.nc

    $tst_cmd[0]="ncks $omp_flg -h -O $fl_fmt $nco_D_flg -g g4 -v one_dmn_rec_var $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncrcat $omp_flg -h --no_tmp_fl --rec_apn $fl_fmt $nco_D_flg -g g4 -v one_dmn_rec_var $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[2]="ncks  -C -g g4 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="(Groups) Append records to existing file without copying original (Group with 1 record case)";
    $tst_cmd[3]="time[19]=10 one_dmn_rec_var[19]=10 kelvin";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncrcat #29
#ncks -h -O -g g5 -v one_dmn_rec_var,time51,time52 in_grp.nc in_grp1.nc
#ncrcat -h --no_tmp_fl --rec_apn -g g5 -v one_dmn_rec_var in_grp.nc in_grp1.nc

    $tst_cmd[0]="ncks $omp_flg -h -O $fl_fmt $nco_D_flg -g g5 -v one_dmn_rec_var,time51,time52 $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncrcat $omp_flg -h --no_tmp_fl --rec_apn $fl_fmt $nco_D_flg -g g5 -v one_dmn_rec_var $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[2]="ncks -C -g g5 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="(Groups) Append records to existing file without copying original (Group with 2 records case)";
    $tst_cmd[3]="time52[19]=10 one_dmn_rec_var[19]=10 kelvin";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#ncrcat #30
#ncks -h -O -g g5 -v one_dmn_rec_var,time51,time52 in_grp.nc in_grp1.nc 
#ncks -h -O -g g5 -v one_dmn_rec_var,time51,time52 in_grp.nc in_grp2.nc 
#ncrcat -O -h -g g5 -v one_dmn_rec_var in_grp1.nc in_grp2.nc out.nc
#ncks -C -g g5 -v one_dmn_rec_var  out.nc

	$tst_cmd[0]="ncks $omp_flg -h -O $fl_fmt $nco_D_flg -g g5 -v one_dmn_rec_var,time51,time52 $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks $omp_flg -h -O $fl_fmt $nco_D_flg -v one_dmn_rec_var,time51,time52 $in_pth_arg in_grp.nc %tmp_fl_01%";
    $tst_cmd[2]="ncrcat -h -O $fl_fmt $nco_D_flg -g g5 -v one_dmn_rec_var %tmp_fl_00% %tmp_fl_01% %tmp_fl_02%";
	$tst_cmd[3]="ncks -C -g g5 -v one_dmn_rec_var %tmp_fl_02%";
    $dsc_sng="(Groups) Group with 2 records case";
    $tst_cmd[4]="time52[19]=10 one_dmn_rec_var[19]=10 kelvin";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
		
	} #### End Group tests

#    } else { print "NB: Current mpncrcat test skipped because it hangs fxm TODO nco593.\n";}
    
####################
#### ncra tests #### OK!
####################
    $opr_nm='ncra';
####################

# ncra #01
    
#        if($mpi_prc == 0 || ($mpi_prc > 0 && $localhostname !~ /sand/)){ # test hangs because of ncrcat TODO nco772
    $tst_cmd[0]="ncra -Y ncrcat $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tmp_fl_01% 2> %tmp_fl_00%";
    $tst_cmd[1]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a time %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[2]="ncdiff $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl %tmp_fl_01% %tmp_fl_02% %tmp_fl_03%";
    $tst_cmd[3]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y rms -v rec_var_flt_mss_val_dbl %tmp_fl_03% %tmp_fl_00%";
    $tst_cmd[4]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl %tmp_fl_00%";
    $dsc_sng="record sdn of float with double missing values across two files";
    $tst_cmd[5]="2";
    $tst_cmd[6]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
#    } else { print "NB: Current mpncra test skipped on sand because mpncrcat step hangs fxm TODO nco593\n";}

# ncra #02
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v one_dmn_rec_var $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="record mean of int across two files";
    $tst_cmd[2]="6";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #03	
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl %tmp_fl_00%";
    $dsc_sng="record mean of float with double missing values";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #04	
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_mss_val_int $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_int %tmp_fl_00%";
    $dsc_sng="record mean of float with integer missing values";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #05	
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_int $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_int %tmp_fl_00%";
    $dsc_sng="record mean of integer with integer missing values";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #06	
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_int $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_int %tmp_fl_00%";
    $dsc_sng="record mean of integer with integer missing values across two files";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #07
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_flt $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_flt %tmp_fl_00%";
    $dsc_sng="record mean of integer with float missing values";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #08	
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_int_mss_val_flt $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v rec_var_int_mss_val_flt %tmp_fl_00%";
    $dsc_sng="record mean of integer with float missing values across two files";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #09	
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_dbl_mss_val_dbl_pck $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_dbl_mss_val_dbl_pck %tmp_fl_00%";
    $dsc_sng="record mean of packed double with double missing values (FAILURE with netCDF4 nco945) ";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #10	
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_dbl_pck $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_dbl_pck %tmp_fl_00%";
    $dsc_sng="record mean of packed double to test precision";
    $tst_cmd[2]="100.55";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #11	
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v rec_var_flt_pck $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%3.2f' -v rec_var_flt_pck %tmp_fl_00%";
    $dsc_sng="record mean of packed float to test precision";
    $tst_cmd[2]="100.55";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #12	
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -v pck,one_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v pck %tmp_fl_00%";
    $dsc_sng="pass through non-record (i.e., non-processed) packed data to output";
    $tst_cmd[2]="1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #13
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y avg -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl %tmp_fl_00%";
    $dsc_sng="record mean of float with double missing values across two files";
    $tst_cmd[2]="5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #14
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl %tmp_fl_00%";
    $dsc_sng="record min of float with double missing values across two files";
    $tst_cmd[2]="2";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #15	
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl %tmp_fl_00%";
    $dsc_sng="record max of float with double missing values across two files";
    $tst_cmd[2]="8";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #16
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v rec_var_flt_mss_val_dbl %tmp_fl_00%";
    $dsc_sng="record ttl of float with double missing values across two files";
    $tst_cmd[2]="70";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #17
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y rms -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%1.5f' -v rec_var_flt_mss_val_dbl %tmp_fl_00%";
    $dsc_sng="record rms of float with double missing values across two files";
    $tst_cmd[2]="5.38516";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #18
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -v time,one_dmn_rec_var $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v time %tmp_fl_00%";
    $dsc_sng="Return mean time coordinate (when computing totals) across two files";
    $tst_cmd[2]="5.5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #19
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -v time,one_dmn_rec_var $in_pth_arg in.nc in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="Return total record (when computing totals) across two files";
    $tst_cmd[2]="110";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #20	

    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -X 0.,180.,-30.,30. -v gds_3dvar $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%8.1f' -v gds_3dvar %tmp_fl_00%";
    $dsc_sng="Return record average of cell-based geodesic data";
    $tst_cmd[2]="   277.7   277.8   277.9   278.0   278.1   278.2";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #21
	
    $tst_cmd[0]="/bin/rm -f %tmp_fl_00%";
    $tst_cmd[1]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -C $in_pth_arg lrg_bm.nc lrg_bm1.nc %tmp_fl_00%";
    $tst_cmd[2]="ncra -t 1 -h -O $fl_fmt $nco_D_flg -C $in_pth_arg lrg_bm.nc lrg_bm1.nc %tmp_fl_01%";
    $tst_cmd[3]="ncbo -C -h -O %tmp_fl_00% %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[4]="ncwa -t 1 -O -C %tmp_fl_02% %tmp_fl_03% 2>%tmp_fl_05%";
    $dsc_sng="Check op with OpenMP";
    $tst_cmd[5]="ncap2 -t 1 -h -v -O -s 'n2= ((fabs(R)<0.01) && (fabs(S)<0.01) && (fabs(T)<0.01) && (fabs(U)<0.01) && (fabs(V)<0.01) && (fabs(W)<0.01) && (fabs(X)<0.01));print(n2)' %tmp_fl_03% %tmp_fl_04%";
    $tst_cmd[6]="n2 = 1";
    $tst_cmd[7]="NO_SS";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
# ncra #22
    
    $tst_cmd[0]="ncks $omp_flg -h -O $fl_fmt $nco_D_flg -v one_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncra $omp_flg -h --no_tmp_fl --rec_apn $fl_fmt $nco_D_flg -v one_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[2]="ncks -C -H -s '%d' -d time,10 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="Append records to existing file without copying original";
    $tst_cmd[3]="6";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#### Group tests	
	if($HAVE_NETCDF4_H == 1){	
	
# ncra #23
# same as ncra #02, for groups
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -g g4 -v one_dmn_rec_var $in_pth_arg in_grp.nc in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -g g4 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="(Groups) 1 group -g g4 -v one_dmn_rec_var record mean of int across two files";
    $tst_cmd[2]="time[0]=5.5 one_dmn_rec_var[0]=6 kelvin";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
	
# ncra #24
# (Groups) 1 record to process in 1 group
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -g g23 $in_pth_arg in_grp_3.nc in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H  -g g23 -v time %tmp_fl_00%";
    $dsc_sng="(Groups) 1 record to process in 1 group -g g23 (do variable and record in group) ";
    $tst_cmd[2]="time[0]=1.5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	

		
# ncra #25
# (Groups) 2 records to process in 2 groups (part 1)
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -g g23,g24 -v time $in_pth_arg in_grp_3.nc in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H  -g g23 -v time %tmp_fl_00%";
    $dsc_sng="(Groups) 2 records to process in 2 groups (part 1) (do record in group only)";
    $tst_cmd[2]="time[0]=1.5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	

# ncra #26
# (Groups) 2 records to process in 2 groups (part 2)
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -g g23,g24 -v time $in_pth_arg in_grp_3.nc in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H  -g g24 -v time %tmp_fl_00%";
    $dsc_sng="(Groups) 2 records to process in 2 groups (part 2) (do record in group only)";
    $tst_cmd[2]="time[0]=4";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	


# ncra #27
# (Groups) 1D var 2 records to process in 2 groups (part 1)
#  ncra -h -O  -g g25g1,g25g2 in_grp_3.nc in_grp_3.nc out.nc
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -g g25g1,g25g2 $in_pth_arg in_grp_3.nc in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -g g25g1 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="(Groups) 2 records to process in 2 groups (part 1) (1D variable and record in group)";
    $tst_cmd[2]="time[0]=5.5 one_dmn_rec_var[0]=6";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
# ncra #28 same as #27
# (Groups) 1D var 2 records to process in 2 groups (part 2)
#  ncra -h -O  -g g25g1,g25g2 in_grp_3.nc in_grp_3.nc out.nc
    
    $tst_cmd[0]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg -g g25g1,g25g2 $in_pth_arg in_grp_3.nc in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -g g25g2 -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="(Groups) 2 records to process in 2 groups (part 2) (1D variable and record in group)";
    $tst_cmd[2]="time[0]=5.5 one_dmn_rec_var[0]=6";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		

# ncra #29 Generate a file with 2 records
# ncecat -O  -g g25g1 in_grp_3.nc in1.nc -> generate "record"
# ncpdq -O -a time,record  in1.nc in2.nc -> switch "record" and "time"
# ncra -O  in2.nc out.nc
    
    $tst_cmd[0]="ncecat $omp_flg -h -O $fl_fmt $nco_D_flg -g g25g1 $in_pth_arg in_grp_3.nc %tmp_fl_00%";
	$tst_cmd[1]="ncpdq $omp_flg -h -O $fl_fmt $nco_D_flg -a time,record %tmp_fl_00% %tmp_fl_01%";
	$tst_cmd[2]="ncra $omp_flg -h -O $fl_fmt $nco_D_flg %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[3]="ncks -C -g g25g1 -v one_dmn_rec_var %tmp_fl_02%";
    $dsc_sng="(Groups) 2 records in 1 group ('time' same as record, 1st record)";
    $tst_cmd[4]="time[0]=5.5 record[0] one_dmn_rec_var[0]=6";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	

#ncra #30
#Operate files containing same variable in different orders
# ncks -O    -v time,one ~/nco/data/in.nc ~/foo1.nc
# ncks -O -a -v one,time ~/nco/data/in.nc ~/foo2.nc
# ncra -O -p ~ foo1.nc foo2.nc ~/foo3.nc
# ncks -C -H -v one ~/foo3.nc

    $dsc_sng="Concatenate variables with different ID ordering";
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -C    -v time,one $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -C -a -v one,time $in_pth_arg in.nc %tmp_fl_01%";
    $tst_cmd[2]="ncra -h -O $fl_fmt $nco_D_flg %tmp_fl_00% %tmp_fl_01% %tmp_fl_02%";
    $tst_cmd[3]="ncks -C -H -v one %tmp_fl_02%";
    $tst_cmd[4]="one = 1";
    $tst_cmd[5]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 		

   } #### Group tests	
	
# ncra #31
# Treatment of missing values with MRO and rth_dbl
# ncra -O -F -C -v one_dmn_rec_var_mss_val --mro -d time,1,2 ~/nco/data/in.nc ~/foo.nc
# ncks -C -H -s '%g' ~/foo.nc
    $dsc_sng="Treatment of missing values with MRO and rth_dbl";
    $tst_cmd[0]="ncra -h -O $fl_fmt $nco_D_flg -C -F -v one_dmn_rec_var_mss_val --mro -d time,1,2 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v one_dmn_rec_var_mss_val %tmp_fl_00%";
    $tst_cmd[2]="__";
    $tst_cmd[3]="SS_OK";   
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array 	

	#### Group tests	
	if($HAVE_NETCDF4_H == 1){
	
#NEW 4.4.2	
#ncra #32
#ncra -O -v time301 -C in_grp_3.nc out.nc
if (0){
    $dsc_sng="(Groups) Cell methods (Create) -v time301";
    $tst_cmd[0]="ncra $omp_flg $nco_D_flg -O -v time301 $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m %tmp_fl_00%";
    $tst_cmd[2]="time301 attribute 1: cell_methods, size = 13 NC_CHAR, value = time301: mean";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	}

	} #### Group tests		

	

#print "paused - hit return to continue"; my $wait=<STDIN>;
#print "<<<STOP>>>- hit return to continue"; my $wait=<STDIN>;
    
####################
#### ncwa tests #### OK!
####################
    $opr_nm='ncwa';
####################

#ncwa #1
    
    $tst_cmd[0]="ncks -h -O $fl_fmt $nco_D_flg -v lat_T42,lon_T42,gw_T42 $in_pth_arg in.nc %tmp_fl_03%";
    $tst_cmd[1]="ncrename -h -O $nco_D_flg -d lat_T42,lat -d lon_T42,lon -v lat_T42,lat -v gw_T42,gw -v lon_T42,lon %tmp_fl_03%";
    $tst_cmd[2]="ncap2 -h -O $fl_fmt $nco_D_flg -s 'one[lat,lon]=lat*lon*0.0+1.0' -s 'zero[lat,lon]=lat*lon*0.0' %tmp_fl_03% %tmp_fl_04%";
    $tst_cmd[3]="ncks -C -H -s '%g' -v one -F -d lon,128 -d lat,64 %tmp_fl_04%";
    $dsc_sng="Creating %tmp_fl_03% again (FAILURE netCDF4 ncrename nco821) ";
    $tst_cmd[4]="1";
    $tst_cmd[5]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.

#ncwa #2

    push(@tst_cmd, "ncks -h -O $fl_fmt $nco_D_flg -v lat_T42,lon_T42,gw_T42 $in_pth_arg in.nc %tmp_fl_03%");
    push(@tst_cmd, "ncrename -h -O $nco_D_flg -d lat_T42,lat -d lon_T42,lon -v lat_T42,lat -v gw_T42,gw -v lon_T42,lon %tmp_fl_03%");
    push(@tst_cmd, "ncap2 -h -O $fl_fmt $nco_D_flg -s 'one[lat,lon]=lat*lon*0.0+1.0' -s 'zero[lat,lon]=lat*lon*0.0' %tmp_fl_03% %tmp_fl_04%");
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a lat,lon -w gw -d lat,0.0,90.0 %tmp_fl_04% %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H -s '%g' -v one %tmp_fl_00%");
    $dsc_sng="normalize by denominator upper hemisphere (FAILURE netCDF4 ncrename nco821)";
    $prsrv_fl=1; # save previously generated files.
#	$nsr_xpc= 1;
# tst_run();
    push(@tst_cmd, 1);
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #3
    
# 20140207: Behavior changed to comply with documentation that coordinate variables always return averages, never extrema or other statistics
# Hence request for min(lat) should actually return avg(lat) = 0 != -90
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v lat $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v lat %tmp_fl_00%";
    $dsc_sng="return average coordinate coordinate variable during minimization for non-coordinate variables";
    $tst_cmd[2]="0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #4

    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a time -v pck,one_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v pck %tmp_fl_00%";
    $dsc_sng="pass through non-averaged (i.e., non-processed) packed data to output";
    $tst_cmd[2]="1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #5
    
    $tst_cmd[0]="ncwa -N $omp_flg -h -O $fl_fmt $nco_D_flg -a lat,lon -w gw $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v mask %tmp_fl_00%";
    $dsc_sng="do not normalize by denominator (FAILURE netCDF4 nco946)";
    $tst_cmd[2]="50";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #6
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a lon -v mss_val $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v mss_val %tmp_fl_00%";
    $dsc_sng="average with missing value attribute";
    $tst_cmd[2]="73";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #7
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a lon -v no_mss_val $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%g' -v no_mss_val %tmp_fl_00%";
    $dsc_sng="average without missing value attribute";
    $tst_cmd[2]="5.0e35";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #8
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lat -m lat -M 90.0 -T eq -a lat $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v lat %tmp_fl_00%";
    $dsc_sng="average masked coordinate";
    $tst_cmd[2]="90.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #9
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lat_var -m lat -M 90.0 -T eq -a lat $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v lat_var %tmp_fl_00%";
    $dsc_sng="average masked variable";
    $tst_cmd[2]="2.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #10
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lev -m lev -M 100.0 -T eq -a lev -w lev_wgt $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v lev %tmp_fl_00%";
    $dsc_sng="average masked, weighted coordinate";
    $tst_cmd[2]="100.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #11
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lev_var -m lev -M 100.0 -T gt -a lev -w lev_wgt $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v lev_var %tmp_fl_00%";
    $dsc_sng="average masked, weighted variable";
    $tst_cmd[2]="666.6667";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #12
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lat -a lat -w gw -d lat,0 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v lat %tmp_fl_00%";
    $dsc_sng="weight conforms to variable first time";
    $tst_cmd[2]="-90.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #13
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v mss_val_all -a lon -w lon $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H --no_blank -s '%g' -v mss_val_all %tmp_fl_00%";
    $dsc_sng="average all missing values with weights";
    $tst_cmd[2]="1.0e36";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #14
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v val_one_mss -a lat -w wgt_one $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v val_one_mss %tmp_fl_00%";
    $dsc_sng="average some missing values with unity weights";
    $tst_cmd[2]="1.0";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #15
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v msk_prt_mss_prt -m msk_prt_mss_prt -M 1.0 -T lt -a lon $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v msk_prt_mss_prt %tmp_fl_00%";
    $dsc_sng="average masked variable with some missing values";
    $tst_cmd[2]="0.5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #16

    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -w area -v area -a lat $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v area %tmp_fl_00%";
    $dsc_sng="average uniform field with uniform weights";
    $tst_cmd[2]="10";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #17
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -w area_asm -v area -a lat $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v area %tmp_fl_00%";
    $dsc_sng="average uniform field with asymmetric weights";
    $tst_cmd[2]="10";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #18
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -w area_asm -v area2 -a lat $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v area2 %tmp_fl_00%";
    $dsc_sng="average asymmetric field with asymmetric weights";
    $tst_cmd[2]="10";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #19
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v rec_var_flt_mss_val_dbl $in_pth_arg in.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%e' -v rec_var_flt_mss_val_dbl %tmp_fl_00%";
    $dsc_sng="min switch on type double, some missing values";
    $tst_cmd[2]="2";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #20
    
# will fail SS - ncks not the last cmd
    @tst_cmd=(); # really reset array. $#tst_cmd=0; sets last index=0 --> list has one element.
    push(@tst_cmd, "ncwa $omp_flg  -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_dbl -a lon $in_pth_arg in.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H --no_blank -s '%f' -v three_dmn_var_dbl -d time,3 -d lat,0 %tmp_fl_00%");
    # used to cut for field 7. (1 + 3x2 + 0x1=7)
    $dsc_sng="Dimension reduction with min switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #21
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg  -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_dbl -a lon $in_pth_arg in.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H -s '%f' -v three_dmn_var_dbl -d time,9 -d lat,1 %tmp_fl_00%");
    # used to cut for field 20. (1 + 9x2 + 1x1 = 20)
    $dsc_sng="Dimension reduction with min switch";
    $prsrv_fl=1; ## this is not needed anymore-- now independent from prev test-- remove this line soon
    push(@tst_cmd,"77");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #22
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_int -a lon $in_pth_arg in.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H --no_blank -s '%d' -v three_dmn_var_int -d time,2 -d lat,0 %tmp_fl_00%");
    # used to cut field 5: ( 1 + 2x2 + 0x1 = 5) 
    $dsc_sng="Dimension reduction on type int with min switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #23
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_int -a lon $in_pth_arg in.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H -s '%d' -v three_dmn_var_int -d time,3 -d lat,0 %tmp_fl_00%");
    # used to cut field 7: ( 1 + 3x2 + 0x1 = 7) 
    $dsc_sng="Dimension reduction on type int variable";
    $prsrv_fl=1;
    push(@tst_cmd, "25");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #24
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_sht -a lon $in_pth_arg in.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H --no_blank -s '%d' -v three_dmn_var_sht -d time,9 -d lat,1 %tmp_fl_00%");
    # used to cut field 20: ( 1 + 9x2 + 1x1 = 20) 
    $dsc_sng="Dimension reduction on type short variable with min switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #25
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_var_sht -a lon $in_pth_arg in.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H -s '%d' -v three_dmn_var_sht -d time,3 -d lat,1 %tmp_fl_00%");
    # used to cut field 8: ( 1 + 3x2 + 1x1 = 8) 
    $dsc_sng="Dimension reduction on type short variable";
    $prsrv_fl=1;
    push(@tst_cmd, "29");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #26
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_rec_var %tmp_fl_00%";
     $dsc_sng="Dimension reduction with min flag on type float variable";
    $tst_cmd[2]="1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #27
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v four_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v four_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="Max flag on type float variable";
    $tst_cmd[2]="240";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #28
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_dbl -a lat,lon $in_pth_arg in.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H --no_blank -s '%f' -v three_dmn_var_dbl -d time,3 %tmp_fl_00%");
    # used to cut field 4: ( 1 + 3x1=4) 
    $dsc_sng="Dimension reduction on type double variable with max switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #29
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_dbl -a lat,lon $in_pth_arg in.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H -s '%f' -v three_dmn_var_dbl -d time,4 %tmp_fl_00%"); 
    # used to cut field 5: ( 1 + 4x1=5) 
    $dsc_sng="Dimension reduction on type double variable";
    $prsrv_fl=1;
    push(@tst_cmd, "40");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #30
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_int -a lat $in_pth_arg in.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H --no_blank -s '%d' -v three_dmn_var_int -d time,2 -d lon,0 %tmp_fl_00%");
    # used to cut field 9: ( 1 + 2x4 + 0x1=9) 
    $dsc_sng="Dimension reduction on type int variable with min switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #31
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_int -a lat $in_pth_arg in.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H -s '%d' -v three_dmn_var_int -d time,3 -d lon,0 %tmp_fl_00%");
    # used to cut field 13 = 1 + 3x4 + 0x1
    $dsc_sng="Dimension reduction on type int variable";
    $prsrv_fl=1;
    push(@tst_cmd, "29");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #32
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_sht -a lat $in_pth_arg in.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H --no_blank -s '%d' -v three_dmn_var_sht -d time,9 -d lon,0 %tmp_fl_00%");
    # used to cut field 37 = 1 + 9x4 + 0x1
    $dsc_sng="Dimension reduction on type short variable with max switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #33
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -v three_dmn_var_sht -a lat $in_pth_arg in.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H -s '%d' -v three_dmn_var_sht -d time,8 -d lon,0 %tmp_fl_00%");
    # used to cut field 33 = 1 + 8x4 + 0x1
    $dsc_sng="Dimension reduction on type short, max switch variable";
    $prsrv_fl=1;
    push(@tst_cmd, "69");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #34
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y rms -w lat_wgt -v lat_cpy $in_pth_arg in.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v lat_cpy %tmp_fl_00%";;
    $dsc_sng="rms with weights";
    $tst_cmd[2]="90";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #35
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -w val_half_half -v val_one_one_int $in_pth_arg in.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v val_one_one_int %tmp_fl_00%";;
    $dsc_sng="weights would cause SIGFPE without dbl_prc patch";
    $tst_cmd[2]="1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #36
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y avg -v val_max_max_sht $in_pth_arg in.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v val_max_max_sht %tmp_fl_00%";;
    $dsc_sng="avg would overflow without dbl_prc patch";
    $tst_cmd[2]="17000";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #37
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -v val_max_max_sht $in_pth_arg in.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v val_max_max_sht %tmp_fl_00%";
    $dsc_sng="test wrapped ttl with dbl_prc patch (harmless failure expected/OK on all chips since wrap behavior is not IEEE-specified)";
#    $nsr_xpc= -31536 ; # Expected on Pentium IV GCC Debian 3.4.3-13, PowerPC xlc
#    $nsr_xpc= -32768 ; # Expected on Xeon GCC Fedora 3.4.2-6.fc3
#    $nsr_xpc= -32768 ; # Expected on Opteron
#    $nsr_xpc= -32768 ; # Expected on PentiumIII (Coppermine) gcc 3.4 MEPIS
#    $nsr_xpc= -31536 ; # Expected on Power4 xlc AIX
    $tst_cmd[2]="-31536";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #38
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -a lat -v lat_cpy -w gw $in_pth_arg in.nc %tmp_fl_00%";;
    $tst_cmd[1]="ncks -C -H -s '%g' -v lat_cpy %tmp_fl_00%";;
    $dsc_sng="min with weights";
    $tst_cmd[2]="-900";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #39
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -a lat -v lat_cpy -w gw $in_pth_arg in.nc %tmp_fl_00%";;
    $tst_cmd[1]="ncks -C -H -s '%g' -v lat_cpy %tmp_fl_00%";;
    $dsc_sng="max with weights";
    $tst_cmd[2]="900";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #40
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -a time -w one -v time,one_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%f' -v time %tmp_fl_00%";
    $dsc_sng="Return mean time coordinate (when computing totals)";
    $tst_cmd[2]="5.5";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #41
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y ttl -a time -w one -v time,one_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -s '%d' -v one_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="Return total record (when computing totals)";
    $tst_cmd[2]="55";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
	
#ncwa #42
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a lon -b -v lon $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -v lon %tmp_fl_00%";
    $dsc_sng="Retain degenerate dimensions (one variable)";
    $tst_cmd[2]="lon[0]=135";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#ncwa #43
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a lon -b  $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -v lon %tmp_fl_00%";
    $dsc_sng="Retain degenerate dimensions (all variables)";
    $tst_cmd[2]="lon[0]=135";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		
	
#### Group tests	
	if($HAVE_NETCDF4_H == 1){	
	
#ncwa #44
#NEW NCO 4.3.3
#same as #ncwa #25
#ncwa -h -O -y min -g g10 -v three_dmn_rec_var in_grp.nc out.nc
#ncks -C -H -s '%f' -g g10 -v three_dmn_rec_var out.nc
#$tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -v three_dmn_rec_var $in_pth_arg in.nc %tmp_fl_00% 2> %tmp_fl_02%";
#$tst_cmd[1]="ncks -C -H -s '%f' -v three_dmn_rec_var %tmp_fl_00%";
#$dsc_sng="Dimension reduction with min flag on type float variable";
#$tst_cmd[2]="1";
     
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y min -g g10 -v three_dmn_rec_var $in_pth_arg in_grp.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%f' -g g10 -v three_dmn_rec_var %tmp_fl_00%";
    $dsc_sng="(Groups) Dimension reduction with min flag on type float variable";
    $tst_cmd[2]="1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #45
#NEW NCO 4.3.3
#same as #ncwa #27
# ncwa -h -O -y max -g g19g3 -v three_dmn_var_dbl -a lat,lon in_grp_3.nc out.nc
# ncks -C -H --no_blank -s '%f' -g g19g3 -v three_dmn_var_dbl -d time,3 out.nc

# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -g g19g3 -v three_dmn_var_dbl -a lat,lon $in_pth_arg in_grp_3.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H --no_blank -s '%f' -g g19g3 -v three_dmn_var_dbl -d time,3 %tmp_fl_00%");
    # used to cut field 4: ( 1 + 3x1=4) 
    $dsc_sng="(Groups) Dimension reduction on type double variable with max switch and missing values";
    push(@tst_cmd, "-99");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #46
#NEW NCO 4.3.3
#same as #ncwa #28
# ncwa -h -O  -y max -g g19g3 -v three_dmn_var_dbl -a lat,lon in_grp_3.nc out.nc
# ncks -C -H -s '%f' -g g19g3 -v three_dmn_var_dbl -d time,4 out.nc 
    
# will fail SS - ncks not the last cmd
    push(@tst_cmd, "ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y max -g g19g3 -v three_dmn_var_dbl -a lat,lon $in_pth_arg in_grp_3.nc %tmp_fl_00%");
    push(@tst_cmd, "ncks -C -H -s '%f' -g g19g3 -v three_dmn_var_dbl -d time,4 %tmp_fl_00%"); 
    # used to cut field 5: ( 1 + 4x1=5) 
    $dsc_sng="(Groups) Dimension reduction on type double variable";
    $prsrv_fl=1;
    push(@tst_cmd, "40");
    push(@tst_cmd, "SS_OK");
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # Reset array

#ncwa #47
#NEW NCO 4.3.3
#same as #ncwa #33
# ncwa -h -O -y rms -w lat_wgt -g g20g1 -v lat_cpy  in_grp_3.nc out.nc
# ncks -C -H -s '%f' -g g20g1  -v lat_cpy out.nc
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -y rms -w lat_wgt -g g20g1 -v lat_cpy $in_pth_arg in_grp_3.nc %tmp_fl_00% 2> %tmp_fl_02%";
    $tst_cmd[1]="ncks -C -H -s '%f' -g g20g1  -v lat_cpy %tmp_fl_00%";;
    $dsc_sng="(Groups) rms with weights";
    $tst_cmd[2]="90";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array

#ncwa #48
#NEW NCO 4.3.6
#ncwa -O -w gw_lat -d lat,1,2 -d lon,0,1 -a lat,lon -g g26 in_grp_3.nc out.nc
    
    $tst_cmd[0]="ncwa $omp_flg -O $fl_fmt $nco_D_flg -w gw_lat -d lat,1,2 -d lon,0,1 -a lat,lon -g g26 $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -v a %tmp_fl_00%";;
    $dsc_sng="(Groups) Weights and hyperslabs";
    $tst_cmd[2]="a = 35.5803";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	
#ncwa #49
# ncwa -h -O -v lev -a lev -w lev_wgt in_grp_3.nc out.nc
# lev = 230.769 lev_wgt=10,2,1; /g19/lev
# lev = 241.667 lev_wgt=9,2,1;  /g8/lev
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lev -a lev -w lev_wgt $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -g g8 -v lev %tmp_fl_00%";
    $dsc_sng="(Groups) Weights and groups (relative weight name test 1)";
    $tst_cmd[2]="lev = 241.667";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#ncwa #50
# ncwa -h -O -v lev -a lev -w lev_wgt in_grp_3.nc out.nc
# lev = 230.769 lev_wgt=10,2,1; /g19/lev
# lev = 241.667 lev_wgt=9,2,1;  /g8/lev

    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lev -a lev -w lev_wgt $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -g g19 -v lev %tmp_fl_00%";
    $dsc_sng="(Groups) Weights and groups (relative weight name test 2)";
    $tst_cmd[2]="lev = 230.769";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#NEW 4.3.7	
#ncwa #51
# ncwa  -h -O -a time -b  -v time  in_grp.nc out.nc
# ncks out.nc | grep 'time dimension 0'

    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -a time -b  -v time $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks %tmp_fl_00% | grep 'time dimension 0:'";
    $dsc_sng="(Groups) -b degenerate a record dimension";
    $tst_cmd[2]="time dimension 0: time, size = 1 NC_DOUBLE, chunksize = 1 (Record coordinate is time)";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		
	
#NEW 4.4.0	
#ncwa #52 Use -w /g8/lev_wgt
# ncwa -h -O -v lev -a lev -w /g8/lev_wgt in_grp_3.nc out.nc
# lev = 230.769 lev_wgt=10,2,1; /g19/lev
# lev = 241.667 lev_wgt=9,2,1;  /g8/lev
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lev -a lev -w /g8/lev_wgt $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -g g8 -v lev %tmp_fl_00%";
    $dsc_sng="(Groups) Weights and groups (absolute weight name -w /g8/lev_wgt test 1)";
    $tst_cmd[2]="lev = 241.667";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	

#NEW 4.4.0		
#ncwa #53 Use -w /g8/lev_wgt
# ncwa -h -O -v lev -a lev -w /g19/lev_wgt in_grp_3.nc out.nc
# lev = 230.769 lev_wgt=10,2,1; /g19/lev
# lev = 241.667 lev_wgt=9,2,1;  /g8/lev
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lev -a lev -w /g8/lev_wgt $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -g g19 -v lev %tmp_fl_00%";
    $dsc_sng="(Groups) Weights and groups (absolute weight name -w /g19/lev_wgt test 2)";
    $tst_cmd[2]="lev = 241.667";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		
	
#NEW 4.4.0	
#ncwa #54 
#Use -a /g8/lev, -w /g8/lev_wgt
# ncwa -h -O -v lev -a /g8/lev -w /g8/lev_wgt in_grp_3.nc out.nc
# lev = 230.769 lev_wgt=10,2,1; /g19/lev
# lev = 241.667 lev_wgt=9,2,1;  /g8/lev
    
    $tst_cmd[0]="ncwa $omp_flg -h -O $fl_fmt $nco_D_flg -v lev -a /g8/lev -w /g8/lev_wgt $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -C -H -g g8 -v lev %tmp_fl_00%";
    $dsc_sng="(Groups) Weights and groups (absolute dimension name -a /g8/lev)";
    $tst_cmd[2]="lev = 241.667";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		
	
#NEW 4.4.0	
#ncwa #55
#ncwa -O -4  -a time --cnk_dmn lon,4 -v byt_3D_rec in.nc out.nc
    
    $dsc_sng="Chunking -a time --cnk_dmn lon,4 -v byt_3D_rec";
    $tst_cmd[0]="ncwa $omp_flg -O -4 $nco_D_flg -a time --cnk_dmn lon,4 -v byt_3D_rec $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m %tmp_fl_00% | grep 'byt_3D_rec dimension 1'";
    $tst_cmd[2]="byt_3D_rec dimension 1: lon, size = 4 NC_FLOAT, chunksize = 4 (Coordinate is lon)";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		
	
#NEW 4.4.0	
#ncwa #56
#ncwa -O -4 -a /time --cnk_dmn /time,1 -v time in.nc out.nc
    
    $dsc_sng="Chunking -a /time --cnk_dmn /time,1 -v time";
    $tst_cmd[0]="ncwa $omp_flg -O -4 $nco_D_flg -a /time --cnk_dmn /time,1 -v time $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m %tmp_fl_00% | grep 'time: type'";
    $tst_cmd[2]="time: type NC_DOUBLE, 0 dimensions, 5 attributes, chunked? no, compressed? no, packed? no";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		

#NEW 4.4.0	
#ncwa #57	
#ncwa -O -C -4 -D 12 --rdd -a lon,lat -v four_dmn_rec_var --cnk_dmn lat,2 --cnk_dmn lon,4  in.nc out.nc	
#ncks --hdn --cdl -v four_dmn_rec_var out.nc

    $dsc_sng="Chunking with --rdd -a lon,lat -v four_dmn_rec_var --cnk_dmn lat,2 --cnk_dmn lon,4 ";
    $tst_cmd[0]="ncwa $omp_flg -O -C -4 $nco_D_flg --rdd -a lon,lat -v four_dmn_rec_var --cnk_dmn lat,2 --cnk_dmn lon,4 $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks --hdn --cdl -v four_dmn_rec_var %tmp_fl_00% | grep 'four_dmn_rec_var:_ChunkSizes'";
    $tst_cmd[2]="      four_dmn_rec_var:_ChunkSizes = 1, 1, 3, 1 ;";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#NEW 4.4.2	
#ncwa #58
#ncwa -O -y avg -a time301 -v time301 -C in_grp_3.nc out.nc
#ncks -m out.nc
if (0){
    $dsc_sng="Cell methods (Create, average) -y avg -a time -v time";
    $tst_cmd[0]="ncwa $omp_flg $nco_D_flg -O -y avg -a time301 -v time301 -C $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m %tmp_fl_00%";
    $tst_cmd[2]="time301 attribute 1: cell_methods, size = 13 NC_CHAR, value = time301: mean";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	
#NEW 4.4.2	
#ncwa #59
#ncwa -O -y max -a time301 -v time301 -C in_grp_3.nc out.nc	  
#ncks -m out.nc	

    $dsc_sng="Cell methods (Create, maximum) -y max -a time -v time";
    $tst_cmd[0]="ncwa $omp_flg $nco_D_flg -O -y max -a time301 -v time301 -C $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m %tmp_fl_00%";
    $tst_cmd[2]="time301 attribute 1: cell_methods, size = 13 NC_CHAR, value = time301: mean";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array	
	}	

	
#NEW 4.4.2	
#ncwa #60 Append cell_methods
#ncwa -O -y max -a time302 -v time302 -C in_grp_3.nc out.nc
#ncks -m out.nc	
if (0) {
    $dsc_sng="Cell methods (Append) -y max -a time -v time";
    $tst_cmd[0]="ncwa $omp_flg $nco_D_flg -O -y max -a time302 -v time302 -C $in_pth_arg in_grp_3.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m %tmp_fl_00%";
    $tst_cmd[2]="time302 attribute 1: cell_methods, size = 13 NC_CHAR, value = time302: mean";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	} # if (0)
	
	} #### Group tests	


#NEW 4.4.2	
#ncwa #61 (part 1)
#ncwa  --op_typ=avg -O -v one,one_dmn_rec_var,three_dmn_rec_var -a time,lon in.nc out.nc	
#ncks -m -C -v three_dmn_rec_var out.nc
if (0){	
    $dsc_sng="Cell methods (Create, average) -v one,one_dmn_rec_var,three_dmn_rec_var -a time,lon";
    $tst_cmd[0]="ncwa $omp_flg $nco_D_flg --op_typ=avg -O -v one,one_dmn_rec_var,three_dmn_rec_var -a time,lon $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks  -m -C -v three_dmn_rec_var %tmp_fl_00%";
    $tst_cmd[2]="three_dmn_rec_var attribute 2: cell_methods, size = 15 NC_CHAR, value = time, lon: mean";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		

#NEW 4.4.2	
#ncwa #62 (part 2)
#ncwa  --op_typ=avg -O -v one,one_dmn_rec_var,three_dmn_rec_var -a time,lon in.nc out.nc	
#ncks -m -C -v one_dmn_rec_var out.nc
	
    $dsc_sng="Cell methods (Create, average) -v one,one_dmn_rec_var,three_dmn_rec_var -a time,lon";
    $tst_cmd[0]="ncwa $omp_flg $nco_D_flg --op_typ=avg -O -v one,one_dmn_rec_var,three_dmn_rec_var -a time,lon $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks  -m -C -v one_dmn_rec_var %tmp_fl_00%";
    $tst_cmd[2]="one_dmn_rec_var attribute 2: cell_methods, size = 10 NC_CHAR, value = time: mean";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array		

#NEW 4.4.2	
#ncwa #63 max
#ncwa  --op_typ=avg -O -v one,one_dmn_rec_var,three_dmn_rec_var -a time,lon in.nc out.nc	
#ncks -m -C -v one_dmn_rec_var out.nc
	
    $dsc_sng="Cell methods (Create, maximum) -v one,one_dmn_rec_var,three_dmn_rec_var -a time,lon";
    $tst_cmd[0]="ncwa $omp_flg $nco_D_flg --op_typ=max -O -v one,one_dmn_rec_var,three_dmn_rec_var -a time,lon $in_pth_arg in.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks  -m -C -v three_dmn_rec_var %tmp_fl_00%";
    $tst_cmd[2]="three_dmn_rec_var attribute 2: cell_methods, size = 18 NC_CHAR, value = time, lon: maximum";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
	}		
    
####################
#### ncrename tests #### OK!
####################
    $opr_nm='ncrename';
####################

#### Group tests	
	if($HAVE_NETCDF4_H == 1){	

#################### Attributes

#ncrename #1
#ncrename -O -a .nothing,new_nothing in_grp.nc out.nc 
#optional relative rename nothing to new_nothing (print warning)
    
    $dsc_sng="Attributes: Optional relative rename '.nothing' to 'new_nothing'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -a .nothing,new_nothing $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncrename: WARNING Attribute 'nothing' not renamed because not found in searched variable(s)";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	
	
#ncrename #2
#ncrename -O -a history,new_history in_grp.nc out.nc  
#relative rename history to new_history
    
    $dsc_sng="Attributes: Relative rename 'history' to 'new_history'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -a history,new_history $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks %tmp_fl_00% | grep 'History global attribute'";
    $tst_cmd[2]="Global attribute 3: new_history, size = 26 NC_CHAR, value = History global attribute.";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	

#ncrename #3
#ncrename -D 1 -O -a /g1@history,new_history in_grp.nc out.nc
#ncks -g g1 out.nc | grep 'History group attribute'  
# absolute rename /g1/history group/global att to /g1/new_history
# NB: use escape in '/g1\@history,new_history'
    
    $dsc_sng="Attributes: Absolute rename '/g1\@history' to 'new_history'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -a '/g1\@history,new_history' $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -g g1 %tmp_fl_00% | grep 'History group attribute'";
    $tst_cmd[2]="Group attribute 0: new_history, size = 25 NC_CHAR, value = History group attribute.";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	
	
	
#ncrename #4	
#ncrename -O -a global@history,new_history in_grp.nc out.nc 
# relative rename history group/global att to new_history

    $dsc_sng="Attributes: Relative rename 'global\@history' to 'new_history'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -a 'global\@history,new_history' $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks %tmp_fl_00% | grep 'History global attribute'";
    $tst_cmd[2]="Global attribute 3: new_history, size = 26 NC_CHAR, value = History global attribute.";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.		

#ncrename #5
#ncrename -O -a /g1/lon@units,new_units in_grp.nc out.nc 
# absolute rename /g1/lon@units att to /g1/lon@new_units

    $dsc_sng="Attributes: Absolute rename '/g1/lon\@units' to 'new_units'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -a '/g1/lon\@units,new_units' $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -g g1 %tmp_fl_00% | grep 'new_units'";
    $tst_cmd[2]="lon attribute 0: new_units, size = 12 NC_CHAR, value = degrees_east";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	
	
#ncrename #6	
#ncrename -D 1 -O -a /g1/lon@units,/g1/lon@new_units in_grp.nc out.nc 
#absolute rename /g1/lon@units att to /g1/lon@new_units

    $dsc_sng="Attributes: Absolute rename '/g1/lon\@units' to '/g1/lon\@new_units'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -a '/g1/lon\@units,/g1/lon\@new_units' $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -g g1 %tmp_fl_00% | grep 'new_units'";
    $tst_cmd[2]="lon attribute 0: new_units, size = 12 NC_CHAR, value = degrees_east";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.		
	
#ncrename #7	
#ncrename -O -a /g1/lon@.units,new_units in_grp.nc out.nc 
# optional absolute rename /g1/lon@.units att to new_units

    $dsc_sng="Attributes: Optional absolute rename '/g1/lon\@.units' to '/g1/lon\@new_units'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -a '/g1/lon\@.units,new_units' $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -g g1 %tmp_fl_00% | grep 'new_units'";
    $tst_cmd[2]="lon attribute 0: new_units, size = 12 NC_CHAR, value = degrees_east";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	
	
#################### Dimensions	

#ncrename #8	
#ncrename -O -d lat,new_lat in_grp.nc out.nc
# relative rename lat to new_lat

    $dsc_sng="Dimensions: Relative rename 'lat' to 'new_lat' (expect failure with netCDF < 4.3.1)";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -d lat,new_lat $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -v lat %tmp_fl_00% | grep 'new_lat'";
    $tst_cmd[2]="lat dimension 0: new_lat, size = 2 (Non-coordinate dimension)";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	
	
#ncrename #9	
#ncrename -O -d /lat,new_lat in_grp.nc out.nc
# Absolute rename /lat to new_lat

    $dsc_sng="Dimensions: Absolute rename '/lat' to 'new_lat' (expect failure with netCDF < 4.3.1)";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -d /lat,new_lat $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -v lat %tmp_fl_00% | grep 'new_lat'";
    $tst_cmd[2]="lat dimension 0: new_lat, size = 2 (Non-coordinate dimension)";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	

#ncrename #10	
#ncrename -O -d .lat,new_lat in_grp.nc out.nc
# optional relative rename lat to new_lat

    $dsc_sng="Dimensions: Optional existing relative rename '.lat' to 'new_lat' (expect failure with netCDF < 4.3.1)";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -d .lat,new_lat $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -v /g6/area %tmp_fl_00% | grep 'new_lat'";
    $tst_cmd[2]="area dimension 0: /new_lat, size = 2 (Non-coordinate dimension)";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	
	
#ncrename #11	
#ncrename -O -d /lat,new_lat in_grp.nc out.nc
# Absolute rename /lat to new_lat

    $dsc_sng="Dimensions: Optional existing absolute rename './lat' to 'new_lat' (expect failure with netCDF < 4.3.1)";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -d ./lat,new_lat $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -v lat %tmp_fl_00% | grep 'new_lat'";
    $tst_cmd[2]="lat dimension 0: new_lat, size = 2 (Non-coordinate dimension)";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	

#ncrename #12
#ncrename -O -d ./lat_non_existing,new_lat in_grp.nc out.nc
# Absolute non existing absolute rename

    $dsc_sng="Dimensions: Optional non-existing absolute rename './lat_non_existing,new_lat' to 'new_lat'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -d ./lat_non_existing,new_lat $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncrename: WARNING Dimension '/lat_non_existing' not present in ../data/in_grp.nc, skipping it.";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.		
	
#################### Variables	

#ncrename #13
#ncrename  -D 1 -O -v /g1/v1,new_v1 in_grp.nc out.nc
#Absolute rename /g1/v1 to /g1/new_v1 

    $dsc_sng="Variables: Absolute rename '/g1/v1' to '/g1/new_v1'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -v /g1/v1,new_v1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -g g1 -v new_v1 %tmp_fl_00% | grep /g1/new_v1";
    $tst_cmd[2]="/g1/new_v1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.		
	

#ncrename #14	
#ncrename  -D 1 -O -v v1,new_v1 in_grp.nc out.nc
#Relative rename v1 to new_v1 

    $dsc_sng="Variables: Relative rename 'v1' to 'new_v1'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -v v1,new_v1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -g g1 -v new_v1 %tmp_fl_00% | grep /g1/new_v1";
    $tst_cmd[2]="/g1/new_v1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	
	
#ncrename #15	
#ncrename  -D 1 -O -v ./g1/v1,new_v1 in_grp.nc out.nc
#Absolute rename /g1/v1 to /g1/new_v1 

    $dsc_sng="Variables: Optional absolute rename '/g1/v1' to '/g1/new_v1'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -v ./g1/v1,new_v1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -g g1 -v new_v1 %tmp_fl_00% | grep /g1/new_v1";
    $tst_cmd[2]="/g1/new_v1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.			

#ncrename #16
#ncrename  -D 1 -O -v .v1,new_v1 in_grp.nc out.nc
#Optional Relative rename v1 to new_v1 

    $dsc_sng="Variables: Optional relative rename 'v1' to 'new_v1'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -v .v1,new_v1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -g g1 -v new_v1 %tmp_fl_00% | grep /g1/new_v1";
    $tst_cmd[2]="/g1/new_v1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	
	
#ncrename #17
#ncrename  -D 1 -O -v ./g1/v1_not,new_v1 in_grp.nc out.nc
#Optional non-existing absolute rename '/g1/v1_not'

    $dsc_sng="Variables: Optional non-existing absolute rename './g1/v1_not' to '/g1/new_v1'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -v ./g1/v1_not,new_v1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncrename: WARNING Variable '/g1/v1_not' not present in ../data/in_grp.nc, skipping it.";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.		

#################### Groups

#ncrename #18
#ncrename -O -g g1,new_g1 in_grp.nc out.nc 
# relative rename g1 to new_g1

    $dsc_sng="Groups: Relative rename 'g1' to 'new_g1' (expect failure with netCDF < 4.3.1)";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -g g1,new_g1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m  -g new_g1 %tmp_fl_00% | grep new_g1";
    $tst_cmd[2]="/new_g1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	

#ncrename #19
#ncrename -O -g g1g1,new_g1g1 in_grp.nc out.nc  
#relative rename g1g1 to new_g1g1

    $dsc_sng="Groups: Relative rename 'g1g1' to 'new_g1g1' (expect failure with netCDF < 4.3.1)";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -g g1g1,new_g1g1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m  -g new_g1g1 %tmp_fl_00% | grep new_g1g1";
    $tst_cmd[2]="/g1/new_g1g1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	

#ncrename #20
#ncrename  -D 1 -O -g /g1/g1g1,new_g1g1 in_grp.nc out.nc
#Absolute rename -g /g1/g1g1,new_g1g1 

    $dsc_sng="Groups: Absolute rename '/g1/g1g1' to '/g1/new_g1g1' (expect failure with netCDF < 4.3.1)";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -g /g1/g1g1,new_g1g1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m  -g /g1/new_g1g1 %tmp_fl_00% | grep /g1/new_g1g1";
    $tst_cmd[2]="/g1/new_g1g1/v1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	

#ncrename #21	
#ncrename -O -g .gfoo,new_g1 in_grp.nc out.nc 
# optional relative rename gfoo to new_g1 (print warning)

    $dsc_sng="Groups: Optional relative rename '.gfoo' to 'new_g1'";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -g .gfoo,new_g1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncrename: WARNING Group 'gfoo' not present in ../data/in_grp.nc, skipping it.";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.	

	#################### Variables
	
#ncrename #22 (same as #13)
#ncrename  -D 1 -O -v /g1/v1,/g1/new_v1 in_grp.nc out.nc
#Absolute rename /g1/v1 to /g1/new_v1 

    $dsc_sng="Variables: Absolute rename '/g1/v1' to '/g1/new_v1' (input absolute name)";
    $tst_cmd[0]="ncrename -O $fl_fmt $nco_D_flg -v /g1/v1,/g1/new_v1 $in_pth_arg in_grp.nc %tmp_fl_00%";
    $tst_cmd[1]="ncks -m -g g1 -v new_v1 %tmp_fl_00% | grep /g1/new_v1";
    $tst_cmd[2]="/g1/new_v1";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    @tst_cmd=(); # really reset array.			
	
} #### Group tests	
    
#print "paused - hit return to continue"; my $wait=<STDIN>;
    
####################
##### net tests #### OK ! (ones that can be done by non-zender)
####################
    $opr_nm='net';
####################
if(0){ #################  SKIP THESE #####################
# test 1
    $tst_cmd[0]="/bin/rm -f /tmp/in.nc";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -s '%e' -v one -p ftp://dust.ess.uci.edu/pub/zender/nco -l /tmp in.nc | tail -1";
    $dsc_sng="Anonymous FTP protocol (requires anonymous FTP access to dust.ess.uci.edu)";
    $tst_cmd[2]="1.000000e+00";
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
# test 2
    my $sftp_url="sftp://dust.ess.uci.edu:/home/ftp/pub/zender/nco";
    if($dust_usr ne ""){ # if we need to connect as another user (hmangalm@esmf -> hjm@dust))
	$sftp_url =~ s/dust/$dust_usr\@dust/;
    }
#sftp://dust.ess.uci.edu:/home/ftp/pub/zender/nco
    $tst_cmd[0]="/bin/rm -f /tmp/in.nc";
    $tst_cmd[1]="ncks -O $nco_D_flg -v one -p $sftp_url -l /tmp in.nc";
    $tst_cmd[2]="ncks -H $nco_D_flg -s '%e' -v one -l /tmp in.nc";
    $dsc_sng="Secure FTP (SFTP) protocol (requires SFTP access to dust.ess.uci.edu)";
    $tst_cmd[3]="1.000000e+00";
    $tst_cmd[4]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
# test 3
    if($dust_usr ne ""){ # if we need to connect as another user (hmangalm@esmf -> hjm@dust))
	$pth_rmt_scp_tst=$dust_usr . '@' . $pth_rmt_scp_tst;
    }
    $tst_cmd[0]="/bin/rm -f /tmp/in.nc";
    $tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -s '%e' -v one -p $pth_rmt_scp_tst -l /tmp in.nc | tail -1";
    $dsc_sng="SSH protocol (requires authorized SSH/scp access to dust.ess.uci.edu)";
    $tst_cmd[2]=1;
    $tst_cmd[3]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
    $tst_cmd[0]="ncks -C -O -d lon,0 -s '%e' -v lon -p http://www.cdc.noaa.gov/cgi-bin/nph-nc/Datasets/ncep.reanalysis.dailyavgs/surface air.sig995.1975.nc";
    $dsc_sng="OPeNDAP protocol (requires OPeNDAP/DODS-enabled NCO)";
    $tst_cmd[1]="0";
    $tst_cmd[2]="SS_OK";
    NCO_bm::tst_run(\@tst_cmd);
    $#tst_cmd=0; # Reset array
    
    if($USER eq 'zender'){
	$tst_cmd[0]="/bin/rm -f /tmp/etr_A4.SRESA1B_9.CCSM.atmd.2000_cat_2099.nc";
	$tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -s '%e' -d time,0 -v time -p ftp://climate.llnl.gov//sresa1b/atm/yr/etr/ncar_ccsm3_0/run9 -l /tmp etr_A4.SRESA1B_9.CCSM.atmd.2000_cat_2099.nc";
	$dsc_sng="Password-protected FTP protocol (requires .netrc-based FTP access to climate.llnl.gov)";
	$tst_cmd[2]="182.5";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array
	
	$tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	$tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -v one -p mss:/ZENDER/nc -l /tmp in.nc";
	$tst_cmd[2]="ncks -C -H -s '%e' -v one %tmp_fl_00%";
	$dsc_sng="msrcp protocol (requires msrcp and authorized access to NCAR MSS)";
	$tst_cmd[3]="1";
	$tst_cmd[4]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array
	
    } else { print "WARN: Skipping net tests of mss: and password protected FTP protocol retrieval---user not zender\n";}
    
    if($USER eq 'zender' || $USER eq 'hjm'){
	$tst_cmd[0]="/bin/rm -f /tmp/in.nc";
	$tst_cmd[1]="ncks -h -O $fl_fmt $nco_D_flg -s '%e' -v one -p wget://dust.ess.uci.edu/nco -l /tmp in.nc";
	$dsc_sng="HTTP protocol (requires developers to implement wget in NCO nudge nudge wink wink)";
 	$tst_cmd[2]="1";
	$tst_cmd[3]="SS_OK";
	NCO_bm::tst_run(\@tst_cmd);
	$#tst_cmd=0; # Reset array
	
    } else { print "WARN: Skipping net test wget: protocol retrieval---not implemented yet\n";}
} #################  SKIP THESE #####################
    
} # end of perform_test()
