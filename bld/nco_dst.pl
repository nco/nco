#!/usr/bin/perl
				
# Purpose: Perform NCO distributions
# Script relies heavily on SSH connectivity between ${HOST}, ${CVSROOT}, and $www_mch

# Usage:
# Export tagged, public versions
# /usr/bin/scp ${DATA}/nco-4.9.8.tar.gz zender,nco@web.sf.net:/home/project-web/nco/htdocs/src

# ${HOME}/nco/bld/nco_dst.pl --dbg=2 --bld --cln nco-4.9.8 # Build, do not release on SF
# ${HOME}/nco/bld/nco_dst.pl --dbg=2 --bld --cln --sf nco-4.9.8 # Build, release on SF
# ${HOME}/nco/bld/nco_dst.pl --dbg=2 --cln --nst_all nco-4.9.8 # Install, do not build
# ${HOME}/nco/bld/nco_dst.pl --dbg=2 --bld --cln --nst_all nco-4.9.8 # Build and install
# ${HOME}/nco/bld/nco_dst.pl --dbg=2 --cln --acd_cnt nco-4.9.8
# ${HOME}/nco/bld/nco_dst.pl --dbg=2 --cln --acd_prs nco-4.9.8
# ${HOME}/nco/bld/nco_dst.pl --dbg=2 --cln --cgd_cnt nco-4.9.8
# ${HOME}/nco/bld/nco_dst.pl --dbg=2 --cln --cray_prs nco-4.9.8
# ${HOME}/nco/bld/nco_dst.pl --dbg=2 --cln --bbl_cnt nco-4.9.8
# ${HOME}/nco/bld/nco_dst.pl --dbg=2 --cln --blk_cnt nco-4.9.8
# ${HOME}/nco/bld/nco_dst.pl --dbg=2 --cln --dat_cnt nco-4.9.8
# ${HOME}/nco/bld/nco_dst.pl --dbg=2 --cln --ute_prs nco-4.9.8

# Export daily snapshot
# ${HOME}/nco/bld/nco_dst.pl --dbg=2 
# ${HOME}/nco/bld/nco_dst.pl --dbg=1 --cln --nst

# Machines requiring interactive builds
# cd ${HOME}/nco;cvs update;cd bld;make;make tst
# scp ${HOME}/nco/bld/nco_dst.pl dust.ess.uci.edu:/home/zender/nco/bld/nco_dst.pl

BEGIN{
    unshift @INC,$ENV{'HOME'}.'/perl'; # Location of csz.pl and DBG.pm HaS98 p. 170
} # end BEGIN

my $CVS_Header='$Header$';

# Specify modules
use strict; # Protect all namespaces
use Getopt::Long; # GNU-style getopt
use File::Basename; # For parsing filenames

# Personal modules
use DBG; # Debugging constants
require 'csz.pl'; # Contains date_time()

# Set output flushing to help debugging on hard crashes 
# These options update filehandle after every output statement
select((select(STDOUT),$|=1)[0]); # Camel book, p. 110
select((select(STDERR),$|=1)[0]); # Camel book, p. 110

# Timing information
my ($lcl_date_time,$srt_usr_tm,$srt_sys_tm,$srt_child_usr_tm,$srt_child_sys_tm);
($lcl_date_time,$srt_usr_tm,$srt_sys_tm,$srt_child_usr_tm,$srt_child_sys_tm)=time_srt();
printf STDOUT ("Start user time %f\n",$srt_usr_tm);

# Declare local variables
my ($idx,$rcd);
my ($prg_nm,$prg_dsc,$prg_vrs,$prg_date);
my ($pth_in,$fl_sfx);

my ($dst_vrs,$dst_fl,$doc_fl);
my ($dst_fl_chg,$dst_fl_deb,$dst_fl_doc,$dst_fl_dsc,$dst_fl_tgz);
my ($nco_vrs,$nco_vrs_mjr,$nco_vrs_mnr,$nco_vrs_pch);
my ($dly_snp);
my ($mk_cmd,$tar_cmd,$rmt_mch);
my ($rsh_cmd,$rcp_cmd,$cp_cmd,$rm_cmd,$mkdir_cmd,$cvs_cmd);

# Set defaults 
my $False=0;
my $True=1;

my $CVS_Date='$Date$';
my $CVS_Id='$Id$';
my $CVS_Revision='$Revision$';
my $CVSROOT='zender@nco.cvs.sf.net:/cvsroot/nco'; # CVS repository
my $DATA=$ENV{'DATA'};
my $HOME=$ENV{'HOME'};
my $HOST=$ENV{'HOST'};
my $PVM_ARCH=$ENV{'PVM_ARCH'};
my $bld=$False; # Option bld; Whether to rebuild netCDF distribution
my $cp_cmd='cp -p -f'; # Command that behaves like cp
my $cvs_cmd='cvs'; # Command that behaves like cvs (Use cvs -t for verbose output)
my $data_nm=$ENV{'DATA'};
my $main_trunk_tag='nco';
my $mkdir_cmd='mkdir -p'; # Command that behaves like mkdir
my $mdl_sng='nco'; # Base of module name
my $rm_cmd='rm -f'; # Command that behaves like rm
my $rcp_cmd='scp -p'; # Command that behaves like rcp
my $rcp_cmd_no_prs_prm='scp'; # Command that behaves like rcp and does not try to preserve permissions
my $rsh_cmd='ssh'; # Command that behaves like rsh
my $usr_nm=$ENV{'USER'};
my $vrs_tag='';
my $www_mch='dust.ess.uci.edu'; # WWW machine for package
my $www_drc='/var/www/html/nco'; # WWW directory for package
my $www_mch_mrr='web.sf.net'; # WWW machine for package mirror
my $www_drc_mrr='/home/project-web/nco/htdocs'; # WWW directory for package mirror

# Set defaults for command line arguments
my $sf=$False; # Release tarball and update SourceForge
my $cln=$True; # GNU standard Makefile option `clean'
my $dbg_lvl=0;
my $dst_cln=$False; # GNU standard Makefile option `distclean'
my $nst_all=$False; # Option nst_all; Install version on all machines
my $acd_cnt=$False; # Option acd_cnt; Install version in acd contrib
my $acd_prs=$False; # Option acd_prs; Install version in acd personal
my $bbl_cnt=$False; # Option bbl_cnt; Install version in babyblue contrib
my $blk_cnt=$False; # Option blk_cnt; Install version in blackforest contrib
my $bls_cnt=$False; # Option bls_cnt; Install version in bluesky contrib
my $dat_cnt=$False; # Option dat_cnt; Install version in dataproc contrib
my $ute_prs=$False; # Option ute_prs; Install version in ute personal
my $cgd_cnt=$False; # Option cgd_cnt; Install version in CGD contrib
my $cgd_prs=$False; # Option cgd_prs; Install version in CGD personal
my $cray_prs=$False; # Option cray_prs; Install version in Cray personal

# Derived fields
if($PVM_ARCH =~ m/SUN/){ # See Camel p. 81 for =~ and m//
    $tar_cmd='gtar';
    $mk_cmd='make';
}elsif($PVM_ARCH =~ m/CRAY/){
    $tar_cmd='tar';
    $mk_cmd='gnumake';
}else{
    $tar_cmd='tar';
    $mk_cmd='make';
} # endelse
if($data_nm eq ''){$data_nm='/data/'.$usr_nm;}
my $dst_pth_pfx=$data_nm; # Parent of build directory
if($dst_pth_pfx eq $HOME){die "$prg_nm: ERROR \$dst_pth_pfx eq $dst_pth_pfx";} # This could be disastrous
if($rm_cmd =~ m/( -r)|( -R)|( --recursive)/){die "$prg_nm: ERROR Dangerous setting \$rm_cmd eq $rm_cmd";} # This would be disastrous
#      $CVSROOT=':pserver:anonymous@nco.cvs.sf.net:/cvsroot/nco'; # CVS repository

$prg_dsc='NCO distribution maker'; # Program description
($prg_nm,$prg_vrs)=$CVS_Id =~ /: (.+).pl,v ([\d.]+)/; # Program name and version
$prg_vrs.='*' if length('$Locker$ ') > 12; # Tack '*' if it is not checked in into CVS.
($prg_nm,$pth_in,$fl_sfx)=fileparse($0,''); # $0 is program name Camel p. 136
if(length($CVS_Date) > 6){($prg_date)=unpack '@7 a19',$CVS_Date;}else{$prg_date='Unknown';}

# Parse command line arguments: '!' means Boolean, '|' is OR, '=' specifies required argument: 'i' is integer, 'f' is float, 's' is string
$rcd=GetOptions( # man Getopt::GetoptLong
		'acd_cnt!' => \$acd_cnt,
		'acd_prs!' => \$acd_prs,
		'bld!' => \$bld,
		'sf!' => \$sf,
		'bbl_cnt!' => \$bbl_cnt,
		'blk_cnt!' => \$blk_cnt,
		'cgd_cnt!' => \$cgd_cnt,
		'cgd_prs!' => \$cgd_prs,
		'clean!' => \$cln,
		'cln!' => \$cln,
		'cray_prs!' => \$cray_prs,
		'dat_cnt!' => \$dat_cnt,
		'dbg_lvl=i' => \$dbg_lvl,
		'distclean!' => \$dst_cln,
		'dst_cln!' => \$dst_cln,
		'nst_all!' => \$nst_all,
		'ute_prs!' => \$ute_prs,
		 ); # end GetOptions arguments

# Parse positional arguments, if present
if($#ARGV > 0){die "$prg_nm: ERROR Called with $#ARGV+1 positional arguments, need no more than 1\n";}
elsif($#ARGV == 0){$vrs_tag=$ARGV[0];} # Version name is first positional argument, if present. 

if($nst_all){
    $cgd_prs=$True;
    $cgd_cnt=$True;
    $acd_prs=$True;
    $acd_cnt=$True;
    $ute_prs=$True;
    $bbl_cnt=$True;
    $blk_cnt=$True;
    $dat_cnt=$True;
    $cray_prs=$True;
} # endif

# Print initialization state
if($dbg_lvl >= 1){print ("$prg_nm: $prg_dsc, version $prg_vrs of $prg_date\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$vrs_tag = $vrs_tag\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$dbg_lvl = $dbg_lvl\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$bld = $bld\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$sf = $sf\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$cln = $cln\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$dst_cln = $dst_cln\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$nst_all = $nst_all\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$mk_cmd = $mk_cmd\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$tar_cmd = $tar_cmd\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$acd_cnt = $acd_cnt\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$acd_prs = $acd_prs\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$cgd_cnt = $cgd_cnt\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$cgd_prs = $cgd_prs\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$cray_prs = $cray_prs\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$bbl_cnt = $bbl_cnt\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$blk_cnt = $blk_cnt\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$dat_cnt = $dat_cnt\n");} # endif dbg
if($dbg_lvl >= 2){print ("$prg_nm: \$ute_prs = $ute_prs\n");} # endif dbg

if($vrs_tag eq $main_trunk_tag || $vrs_tag eq ''){$dly_snp=$True;}else{$dly_snp=$False;}
# NCO is distributed using the `cvs export' command, so version tag to be distributed must be supplied to this script
if($dly_snp){
# The version tag is blank or of the form `nco'
    $nco_vrs=YYYYMMDD();
    $dst_vrs=$mdl_sng.'-'.$nco_vrs;
}else{
# The version tag is of the form `nco-1_2_0'
    my ($tag_sng);
    my ($nco_psn);

    $tag_sng=$vrs_tag;
    $tag_sng=~s/_/./g; # Use =~ to bind $tag_sng to s/// Camel p. 81
    $nco_vrs=substr($tag_sng,length($mdl_sng)+1,length($tag_sng)-length($mdl_sng)-1); # The +/- 1 accounts for the dash '-'
    ($nco_vrs_mjr,$nco_vrs_mnr,$nco_vrs_pch)=split(/\./,$nco_vrs);
    $dst_vrs=$mdl_sng.'-'.$nco_vrs;
    if($nco_vrs_mjr < 1 || $nco_vrs_mjr > 4){die "$prg_nm: ERROR $nco_vrs_mjr < 1 || $nco_vrs_mjr > 3"};
} # endelse
$doc_fl="$HOME/nco/doc/nco.dvi $HOME/nco/doc/nco.html $HOME/nco/doc/nco.ps $HOME/nco/doc/nco.pdf $HOME/nco/doc/nco.txt $HOME/nco/doc/nco.xml"; # Derived documentation

$dst_fl=$dst_vrs.'.tar.gz'; # Standard tarball distribution
$dst_fl_chg=$mdl_sng.'_'.$nco_vrs.'-1_*.changes'; # Debian changes
$dst_fl_deb=$mdl_sng.'_'.$nco_vrs.'-1_*.deb'; # Debian executables and libraries
$dst_fl_doc=$mdl_sng.'-doc_'.$nco_vrs.'-1_all.deb'; # Debian documentation (deprecated)
$dst_fl_dsc=$mdl_sng.'_'.$nco_vrs.'-1.dsc'; # Debian description
$dst_fl_tgz=$mdl_sng.'_'.$nco_vrs.'.orig.tar.gz'; # Debian tarball

my $dst_pth_bld=$dst_pth_pfx.'/'.$dst_vrs; # Build directory

if($dbg_lvl >= 1){		 
    print STDOUT "$prg_nm: Version to release: $vrs_tag\n";
    print STDOUT "$prg_nm: Distribution version: $dst_vrs\n";
    print STDOUT "$prg_nm: Distribution file: $dst_fl\n";
    print STDOUT "$prg_nm: NCO version: $nco_vrs\n";
    print STDOUT "$prg_nm: NCO major version: $nco_vrs_mjr\n";
    print STDOUT "$prg_nm: NCO minor version: $nco_vrs_mnr\n";
    print STDOUT "$prg_nm: NCO patch version: $nco_vrs_pch\n";
} # end if dbg 

# Build distribution from scratch
if($bld){
    cmd_prc("$rm_cmd -r $dst_pth_bld"); # Remove contents of current directory, if any
#    cmd_prc("$mkdir_cmd $dst_pth_bld"); # Create directory
    chdir $dst_pth_pfx or die "$prg_nm: ERROR unable to chdir to $dst_pth_pfx: $!\n"; # $! is system error string
    if($CVSROOT =~ m/pserver/){cmd_prc("$cvs_cmd -d $CVSROOT login");} # Login first
    if($dly_snp){
	cmd_prc("$cvs_cmd -d $CVSROOT export -kkv -D \"1 second ago\" -d $dst_vrs nco"); # Export
    }else{
	cmd_prc("$cvs_cmd -d $CVSROOT export -kkv -r $vrs_tag -d $dst_vrs nco"); # Export
    } # endelse
    cmd_prc("printf $nco_vrs > $dst_pth_bld/doc/VERSION"); # Stamp version in VERSION file in exported files
    cmd_prc("printf $nco_vrs > $HOME/nco/doc/VERSION"); # Stamp version in VERSION file in development directory
#    cmd_prc("ln -s $dst_pth_bld/bld/nco.spec $dst_pth_bld/bld/nco-$nco_vrs.spec"); # Stamp version in VERSION file
    
# Make sure documentation files are up to date
    my $bld_pth=$dst_pth_pfx.'/'."$dst_vrs".'/bld';
    chdir $bld_pth or die "$prg_nm: ERROR unable to chdir to $bld_pth: $!\n"; # $! is system error string
    cmd_prc("$mk_cmd doc"); 
    cmd_prc("$mk_cmd clean"); 

# Set up FTP server
    chdir $dst_pth_pfx or die "$prg_nm: ERROR unable to chdir to $dst_pth_pfx: $!\n"; # $! is system error string
    cmd_prc("$cp_cmd $doc_fl ./$dst_vrs/doc"); # Copy derived documentation to source directory
    cmd_prc("$tar_cmd cvzf $dst_fl --exclude='nco-4.9.8/debian*' --exclude='.cvsignore' --exclude=ncap_lex.c --exclude=ncap_yacc.[ch] ./$dst_vrs"); # Create gzipped tarfile
    cmd_prc("$rsh_cmd $www_mch $rm_cmd $www_drc/src/$dst_fl"); # Remove any distribution with same name
    if($dly_snp){cmd_prc("$rsh_cmd $www_mch $rm_cmd -r $www_drc/src/nco-????????.tar.gz");} # Remove previous daily snapshots from WWW server
    cmd_prc("$rcp_cmd $dst_fl $www_mch:$www_drc/src"); # Copy local tarfile to WWW server
    cmd_prc("$rcp_cmd $DATA/$dst_fl_chg $DATA/$dst_fl_deb $DATA/$dst_fl_dsc $DATA/$dst_fl_tgz $www_mch:$www_drc/src"); # Copy Debian files to WWW server
 
# Full release procedure (public releases only) includes update Web pages
    if(!$dly_snp){
	cmd_prc("$rsh_cmd $www_mch $rm_cmd $www_drc/nco.tar.gz");
	cmd_prc("$rsh_cmd $www_mch \"cd $www_drc; ln -s -f ./src/$dst_fl nco.tar.gz\"");
 	cmd_prc("$rcp_cmd $dst_pth_bld/doc/index.shtml $www_mch:$www_drc");
 	cmd_prc("$rcp_cmd $dst_pth_bld/doc/nco.png $www_mch:$www_drc");
	cmd_prc("$rcp_cmd $dst_pth_bld/doc/nco_news.shtml $www_mch:$www_drc");
	cmd_prc("$rcp_cmd $dst_pth_bld/doc/nco.html $dst_pth_bld/doc/nco.info* $dst_pth_bld/doc/nco.dvi $dst_pth_bld/doc/nco.pdf $dst_pth_bld/doc/nco.ps $dst_pth_bld/doc/nco.texi $www_mch:$www_drc");
	cmd_prc("$rcp_cmd $dst_pth_bld/doc/README $www_mch:$www_drc");
	cmd_prc("$rcp_cmd $dst_pth_bld/doc/TODO $www_mch:$www_drc");
	cmd_prc("$rcp_cmd $dst_pth_bld/doc/VERSION $www_mch:$www_drc");
	cmd_prc("$rcp_cmd $dst_pth_bld/doc/ChangeLog $www_mch:$www_drc");
    } # endif 
    
# Update SourceForge mirror
# Use rcp_cmd_no_prs_prm because scp appears to quit without notice if too many warnings errors occur copying first files
# Usually scp_cmd includes -p switch to preserve permissions and times, but sourceforge server does not allow this
# Hence it generates warnings when used with -p, and is not properly updated
    if($sf){
	cmd_prc("$rcp_cmd_no_prs_prm $dst_pth_bld/doc/index.shtml $dst_pth_bld/doc/nco.png $dst_pth_bld/doc/nco_news.shtml $dst_pth_bld/doc/README $dst_pth_bld/doc/TODO $dst_pth_bld/doc/VERSION $dst_pth_bld/doc/ChangeLog $usr_nm,nco\@$www_mch_mrr:$www_drc_mrr");
	cmd_prc("$rcp_cmd_no_prs_prm $dst_pth_bld/doc/nco.html $dst_pth_bld/doc/nco.info* $dst_pth_bld/doc/nco.dvi $dst_pth_bld/doc/nco.pdf $dst_pth_bld/doc/nco.ps $dst_pth_bld/doc/nco.ps $dst_pth_bld/doc/nco.texi $usr_nm,nco\@$www_mch_mrr:$www_drc_mrr");
	cmd_prc("$rcp_cmd_no_prs_prm $dst_pth_pfx/$dst_fl $usr_nm,nco\@$www_mch_mrr:$www_drc_mrr/src"); # Copy tarball to WWW server
# 20140529: Disable Debian copying until .debs built again
#	cmd_prc("$rcp_cmd_no_prs_prm $dst_pth_pfx/$dst_fl $DATA/$dst_fl_chg $DATA/$dst_fl_deb $DATA/$dst_fl_dsc $DATA/$dst_fl_tgz $usr_nm,nco\@$www_mch_mrr:$www_drc_mrr/src"); # Copy Debian files to WWW server
# Shell commands on SourceForge disabled 20081018
#	cmd_prc("$rsh_cmd $www_mch_mrr \"cd $www_drc_mrr; ln -s -f ./src/$dst_fl nco.tar.gz\"");
    } # endif SourceForge

# Housekeeping
    if($cln){cmd_prc("$rm_cmd $dst_pth_pfx/$dst_fl");} # Remove local tarfile
    if($dst_cln){cmd_prc("$rm_cmd -r $dst_pth_bld");} # Remove local distribution

# Sanity check
    cmd_prc("$rsh_cmd $www_mch ls -l $www_drc");
} # endif bld

if($acd_prs){
     $rmt_mch='acd.ucar.edu';
     print STDOUT "\n$prg_nm: Updating private NCO on $rmt_mch...\n";
     cmd_prc("$rsh_cmd $rmt_mch \"cd ~/nco;$cvs_cmd update\"");
     cmd_prc("$rsh_cmd $rmt_mch \"cd ~/nco/bld;make cln all tst\"");
# Unfortunately, sudo does not work at all with rsh
#    cmd_prc("$rsh_cmd $rmt_mch \"sudo cp /gs/zender/bin/LINUX/nc* /usr/local/bin\"");
     print STDOUT "$prg_nm: Done updating private NCO binaries on $rmt_mch\n\n";
} # endif acd_prs

if($acd_cnt){
    $rmt_mch='garcia.acd.ucar.edu';
    print STDOUT "\n$prg_nm: Updating private NCO on $rmt_mch...\n";
    cmd_prc("$rsh_cmd $rmt_mch \"cd ~/nco;/local/bin/$cvs_cmd update\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd ~/nco/bld;/local/bin/gmake cln all tst\"");
# Unfortunately, sudo does not work at all with rsh
#    cmd_prc("$rsh_cmd $rmt_mch \"sudo cp /a1/zender/bin/ALPHA/nc* /usr/local/bin\"");
    print STDOUT "$prg_nm: Done updating private NCO binaries on $rmt_mch\n\n";
} # endif acd_cnt

if($cgd_cnt){
    $rmt_mch='sanitas.cgd.ucar.edu';
#    $rsh_cmd $rmt_mch 'printf $PVM_ARCH'
    print STDOUT "\n$prg_nm: Updating contrib NCO on $rmt_mch...\n";
    cmd_prc("$rsh_cmd $rmt_mch \"$rm_cmd -r /usr/tmp/$usr_nm/nco*\"");
    cmd_prc("$rsh_cmd $rmt_mch \"$mkdir_cmd /usr/tmp/$usr_nm/$dst_vrs/obj\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd /usr/tmp/$usr_nm;gtar -xvzf nco.tar.gz;rm nco.tar.gz\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd /usr/tmp/$usr_nm/$dst_vrs/bld; setenv MY_BIN_DIR /contrib/nco-1.1/bin; setenv MY_LIB_DIR /contrib/nco-1.1/lib; setenv MY_OBJ_DIR /usr/tmp/$usr_nm/$dst_vrs/obj; gmake cln all test\"");
    print STDOUT "$prg_nm: Done updating contrib NCO on $rmt_mch\n\n";
} # endif cgd_cnt

if($dat_cnt){
    $rmt_mch='dataproc.ucar.edu';
#    $rsh_cmd $rmt_mch 'printf $PVM_ARCH'
    print STDOUT "\n$prg_nm: Updating contrib NCO on $rmt_mch...\n";
    cmd_prc("$rsh_cmd $rmt_mch \"$rm_cmd -r /usr/tmp/$usr_nm/nco*\"");
    cmd_prc("$rsh_cmd $rmt_mch \"$mkdir_cmd /usr/tmp/$usr_nm/$dst_vrs/obj\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd /usr/tmp/$usr_nm;tar -xvzf nco.tar.gz;rm nco.tar.gz\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd /usr/tmp/$usr_nm/$dst_vrs/bld; setenv MY_BIN_DIR /contrib/nco-1.1/bin; setenv MY_LIB_DIR /contrib/nco-1.1/lib; setenv MY_OBJ_DIR /usr/tmp/$usr_nm/$dst_vrs/obj; gmake cln all tst\"");
    print STDOUT "$prg_nm: Done updating contrib NCO on $rmt_mch\n\n";
} # endif dat_cnt

if($bbl_cnt){
    $rmt_mch='babyblue.ucar.edu';
#    $rsh_cmd $rmt_mch 'printf $PVM_ARCH'
    print STDOUT "\n$prg_nm: Updating contrib NCO on $rmt_mch...\n";
    cmd_prc("$rsh_cmd $rmt_mch \"$rm_cmd -r /usr/tmp/$usr_nm/nco*\"");
    cmd_prc("$rsh_cmd $rmt_mch \"$mkdir_cmd /usr/tmp/$usr_nm/$dst_vrs/obj\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd /usr/tmp/$usr_nm;gunzip nco.tar.gz;tar -xvf nco.tar;rm nco.tar\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd /usr/tmp/$usr_nm/$dst_vrs/bld; setenv MY_BIN_DIR /home/blackforest/$usr_nm/bin/AIX; setenv MY_LIB_DIR /home/blackforest/$usr_nm/lib/AIX; setenv MY_OBJ_DIR /home/blackforest/$usr_nm/obj/AIX; setenv NETCDF_INC /usr/local/include; setenv NETCDF_LIB /usr/local/lib32/r4i4; gmake cln all tst\"");
    print STDOUT "$prg_nm: Done updating contrib NCO on $rmt_mch\n\n";
} # endif bbl_cnt

if($blk_cnt){
    $rmt_mch='blackforest.ucar.edu';
#    $rsh_cmd $rmt_mch 'printf $PVM_ARCH'
    print STDOUT "\n$prg_nm: Updating contrib NCO on $rmt_mch...\n";
    cmd_prc("$rsh_cmd $rmt_mch \"$rm_cmd -r /usr/tmp/$usr_nm/nco*\"");
    cmd_prc("$rsh_cmd $rmt_mch \"$mkdir_cmd /usr/tmp/$usr_nm/$dst_vrs/obj\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd /usr/tmp/$usr_nm;gunzip nco.tar.gz;tar -xvf nco.tar;rm nco.tar\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd /usr/tmp/$usr_nm/$dst_vrs/bld; setenv MY_BIN_DIR /home/blackforest/$usr_nm/bin/AIX; setenv MY_LIB_DIR /home/blackforest/$usr_nm/lib/AIX; setenv MY_OBJ_DIR /home/blackforest/$usr_nm/obj/AIX; setenv NETCDF_INC /usr/local/include; setenv NETCDF_LIB /usr/local/lib32/r4i4; gmake cln all tst\"");
    print STDOUT "$prg_nm: Done updating contrib NCO on $rmt_mch\n\n";
} # endif blk_cnt

if($ute_prs){
    $rmt_mch='utefe.ucar.edu'; # utefe and ute are cross-mounted, utefe is for interactive logins
#    $rsh_cmd $rmt_mch 'printf $PVM_ARCH'
    print STDOUT "\n$prg_nm: Updating personal NCO on $rmt_mch...\n";
    cmd_prc("$rsh_cmd $rmt_mch \"$rm_cmd -r /usr/tmp/$usr_nm/nco*\"");
    cmd_prc("$rsh_cmd $rmt_mch \"$mkdir_cmd /usr/tmp/$usr_nm/$dst_vrs/obj\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd /usr/tmp/$usr_nm;tar -xvzf nco.tar.gz;rm nco.tar.gz\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd /usr/tmp/$usr_nm/$dst_vrs/bld; setenv MY_BIN_DIR /home/ute/$usr_nm/bin/SGIMP64/bin; setenv MY_LIB_DIR /home/ute/$usr_nm/bin/SGIMP64/lib; setenv MY_OBJ_DIR /usr/tmp/$usr_nm/$dst_vrs/obj; gmake cln all tst\"");
    print STDOUT "$prg_nm: Done updating contrib NCO on $rmt_mch\n\n";
} # endif ute_prs

if($cray_prs){
    $rmt_mch='ouray.ucar.edu';
    print STDOUT "\n$prg_nm: Updating private NCO on $rmt_mch...\n";
    cmd_prc("$rsh_cmd $rmt_mch \"$rm_cmd -r /usr/tmp/$usr_nm/nco*\"");
    cmd_prc("$rsh_cmd $rmt_mch \"$mkdir_cmd /usr/tmp/$usr_nm/$dst_vrs/obj\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd /usr/tmp/$usr_nm;gunzip nco.tar.gz;tar -xvf nco.tar;rm nco.tar*\"");
    cmd_prc("$rsh_cmd $rmt_mch \"cd /usr/tmp/$usr_nm/$dst_vrs/bld; setenv MY_BIN_DIR /home/ouray0/$usr_nm/bin/CRAY; setenv MY_LIB_DIR /usr/tmp/$usr_nm/$dst_vrs/lib; setenv MY_OBJ_DIR /usr/tmp/$usr_nm/$dst_vrs/obj; gnumake cln all tst\"");
    print STDOUT "$prg_nm: Done updating contrib NCO on $rmt_mch\n\n";
} # endif cray_prs
