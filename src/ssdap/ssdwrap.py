#!/usr/bin/env python
import getopt, os, sys, tempfile, shutil
import string, time
import urllib

########################################################################
# This is ssdwrap.py, a python script meant to execute an operation on
# a "remote" opendap-with-hackplugin server.  The operation is specified
# as arguments to this script, maintaining a syntax similar to the
# operation run locally.
#
# Note:  option parsing is only barely tested.
# Report option passing problems so I can fix this.  Not all nco
# commands have been tested.
#
# Note: --ncks option added. (will only work with multi-line capable server)
#       Try:
#       ./ssdwrap.py --ncks=P01 ncecat nc/foo_T42.nc
#         (but variable P01 is not in foo_T42, so this won't really work.
#
#
# version info: $Id: ssdwrap.py,v 1.15 2006-03-11 01:57:02 wangd Exp $
########################################################################


# Administrator configurable params
#serverBase = "http://localhost:8000/cgi/nph-dods"
# FIXME: this way of organizing the config has to be changed so that
# the server url can be passed in via an option or the config can reside
# in a std config file
class local:
    serverBase = "http://sand.ess.uci.edu:80/cgi-bin/dods/nph-dods"

    # params probably unchanged

    # some of these are probably unacceptable... FIXME
    acceptableNcCommands = ["ncap", "ncatted", "ncbo", "ncdiff",
                            "ncea", "ncecat", "ncflint", "ncks",
                            "ncpack", "ncpdq", "ncra", "ncrcat",
                            "ncrename", "ncunpack", "ncwa"]
    @staticmethod
    def readConfigFile():
        # can add other mappings from config file to local settings"
        cfgmap = [("serverBase", "ssd-server", "url")]
        try:
            import ConfigParser
            config = ConfigParser.ConfigParser()
            filename = "ssdwrap.conf"
            
            # look in the same place as the script is located... 
            # should I check current working directory instead?
            filepath = os.path.join(os.path.split(sys.argv[0])[0], filename)
            config.read(filepath)
            for m in cfgmap:
                if config.has_option(m[1], m[2]):
                    setattr(local, m[0], config.get(m[1],m[2]))
                
            # dump entire config file (to remember the interface)
            #for section in config.sections():
            #    print section
            #    for option in config.options(section):
            #        print " ", option, "=", config.get(section, option)
            pass
        except:
            # no config file, probably safe to silently ignore
            pass
    pass


    pass
# should probably do some basic sanity check on the options


# this class should stay identical in client/server.  If it gets big,
# we should split it into some python module to be imported.
class SsdapCommon:
    """stuff that should be identical between client and server code"""
    parserShortOpt = "4Aa:Bb:CcD:d:FfHhl:Mmn:Oo:Pp:QqRrs:S:s:t:uv:w:xY:y:"
    parserLongOpt = ["4", "netcdf4", "apn", "append",
                     "attribute=", #ncatted, ncrename
                     "avg=", "average=" #ncwa
                     "bnr", "binary",
                     "fl_bnr=", "binary-file=",
                     "crd", "coords",
                     "nocoords", "dbg_lvl=", "debug-level=",
                     "dmn=", "dimension=", "ftn", "fortran",
                     "huh", "hmm",
                     "fnc_tbl", "prn_fnc_tbl", "hst", "history",
                     "Mtd", "Metadata", "mtd", "metadata",
                     "lcl=", "local=",
                     "nintap", 
                     "output=", "fl_out=",
                     "ovr", "overwrite", "prn", "print", "quiet",
                     "pth=", "path=",
                     "rtn", "retain", "revision", "vrs", "version",
                     "spt=", "script=", "fl_spt=", "script-file=",
                     "sng_fmt=", "string=",
                     "thr_nbr=", "threads=", "omp_num_threads=",
                     "xcl", "exclude",
                     "variable=", "wgt_var=", "weight=",
                     "op_typ=", "operation=" ]

    # special handling for ncap's parameters
    ncapShortOpt = parserShortOpt.replace("v:","v")
    ncapLongOpt = parserLongOpt[:]
    ncapLongOpt.remove('variable=')
    ncapLongOpt.append('variable')

    ncpackShortOpt = parserShortOpt.replace("M","M:")
    ncpackShortOpt = ncpackShortOpt.replace("P","P:")
    ncpackShortOpt = ncpackShortOpt.replace("u","Uu")
    
    ncpackLongOpt = parserLongOpt[:]
    ncpackLongOpt.extend(['arrange','permute','reorder', 'rdr',
                         'pck_map', 'map', 'pck_plc','pack_policy',
                         'upk', 'unpack'])
    ncksShortOpt = parserShortOpt.replace("a:","a")
    ncksLongOpt = parserLongOpt[:]
    ncksLongOpt.extend(['abc', 'alphabetize'])
    
    @staticmethod
    def specialGetOpt(cmd, argvlist):
        #consider special-case for ncwa ncflint -w: option
        # wgt_var, weight also for ncflint/ncwa
        if cmd == "ncap": # ncap has a different format
            return getopt.getopt(argvlist,
                                 SsdapCommon.ncapShortOpt,
                                 SsdapCommon.ncapLongOpt)
        elif cmd in ["ncpdq", "ncpack", "ncunpack"]:
            # ncpdq/ncpack/ncunpack have a different format too
            return getopt.getopt(argvlist,
                                 SsdapCommon.ncpackShortOpt,
                                 SsdapCommon.ncpackLongOpt)
        elif cmd == "ncks":
            return getopt.getopt(argvlist,
                                 SsdapCommon.ncksShortOpt,
                                 SsdapCommon.ncksLongOpt)            
        else:
            return getopt.getopt(argvlist,
                                 SsdapCommon.parserShortOpt,
                                 SsdapCommon.parserLongOpt)
    pass

##################################################

class Command:
    NCKS_OPT = "--ncks"
    NCKS_TEMP = "%tempf_SCRIPTncks%"
    
    def __init__(self, argvlist):
        """construct a command, which is a primitive-ish operation
        over netcdf files.  in the future, we can query the command
        for its attributes (i.e. complexity, dependencies, etc.)"""
        self.children = []

        newList = self.preProcess(argvlist)
        self.cmdline = self.build(newList)

        pass
    def preProcess(self, argvlist):
        longopts = ["ncks="]
        shortopts = ''
        (arglist, newlist) = getopt.getopt(argvlist,shortopts, longopts)
        argdict = dict(arglist)
        # if we just want ncks on the results, build a two line script.
        if Command.NCKS_OPT in argdict:
            # ncks on outfile is desired... create a new command line
            # FIXME: not sure what sort of options we want on ncks
            optlist = ["ncks", "-CH", "-s%f",
                       "-v", argdict[Command.NCKS_OPT], Command.NCKS_TEMP,
                       "%stdout%"]
            self.children.append( Command(optlist))
            newlist.append(Command.NCKS_TEMP) #create output for first line
            
        return newlist

    def specialOutput(self, fname):
        return '%' == fname[0] == fname[-1]
    
    def build(self, argvlist):
        """look for output filename, replace with magic key for remote"""
        # pull of cmd first.
        if argvlist[0] not in local.acceptableNcCommands:
            raise "Bad NCO command"
        self.cmd = argvlist[0]

        # some of these options do not make sense in this context,
        # and some have meanings that necessarily need changing.
        (arglist, leftover) = SsdapCommon.specialGetOpt(self.cmd,
                                                        argvlist[1:])

        argdict = dict(arglist)

        ofname = ""
        for x in ["-o", "--fl_out", "--output"]:
            if x in argdict:
                assert ofname == ""
                ofname = argdict[x]
                # convert alt specs to --output
                argdict["--output"] = argdict.pop(x)
        if ofname == "": # i.e. haven't gotten a parameterixed outfilename
            ofname = leftover[-1]
            argdict["--output"] = ofname # and add to dict.
            assert len(leftover) > 1 # assume in.nc, out.nc, at least
            leftover = leftover[:-1] # take only first element

        assert ofname != ""  # make sure we got one
        self.outfilename = ofname # save outfilename

        # do not patch output if it's special already.
        if self.specialOutput(ofname) :
            argdict.pop("--output")
            # leave as special
        else: 
            argdict["--output"] = "%outfile%" # patch with magic script hint
            # hack since ncbo doesn't support --output option
            argdict["-o"] = argdict.pop("--output")
            
        #patch infiles with -p option
        self.infilename = self.patchInfiles(argdict, leftover)

        # now, build script command line
        return self.rebuildCommandline(argdict, self.infilename)

    def patchInfiles(self, argdict, filelist):
        return filelist ## turn off path prefixing since it is done server-side
        # delete this function after testing.

        # find path prefixer.
        prefix = ""
        opts = ["-p", "--pth", "--path"]
        for p in opts:
            if p in argdict:
                prefix = argdict[p]
                break
        if prefix == "":
            return filelist
        newlist = []
        for n in filelist:
            newlist.append(prefix + os.sep + n)
        # now, delete prefix option from arguments, so it doesn't get applied twice.
        for p in opts:
            if p in argdict:
                argdict.pop(p)  # ignore return value
        return newlist

    def rebuildCommandline(self, argdict, infilename):
        line = self.cmd
        for (k,v) in argdict.items():
            #special value handling for --op_typ='-'
            needsProt = False
            safe = string.letters + string.digits + "%"
            for x in v: needsProt |= (x not in safe)
                
            if needsProt:    line += " " + k + " '" + v + "'"
            elif len(v) > 0: line += " " + k + " " + v
            else:            line += " " + k
        line += ''.join([" " + name for name in infilename])
        if "-o" not in argdict:
            line += " " + self.outfilename
        return line

    def childCommands(self):
        """Commands can have children.  A child is a command that should
        be executed after the parent.  Often times, in parsing and
        building a command, children are discovered."""
        return self.children

    def scriptLineSub(self):
        return self.cmdline
    def outputFile(self):
        if self.specialOutput(self.outfilename):
            return None
        return self.outfilename


class RemoteScript:
    serverBase = local.serverBase
    def __init__(self):
        self.cmdList = []
        #
        pass

    def addCommand(self, cmd):
        assert isinstance(cmd, Command)
        self.cmdList.append(cmd)

        for c in cmd.childCommands():
            self.addCommand(c)
        
        # might consider building dep tree here.
        return True

    def run(self):
        """sends off its current batch of commands off to the server to run"""
        script = self.buildScript()
        #print "script is " + script

        self.executeBuilt(script)

    def buildScript(self):
        """builds a textual script to send off to the server processor"""
        script = ""
        script = "".join([x.scriptLineSub() + "\n" for x in self.cmdList])
        return script

    def executeBuilt(self, script):
        """sends the script to be executed"""

        # pick the outfile of the last cmd... not sure this is right.
        # but it seems better than picking the one from the first
        filename = self.cmdList[-1].outputFile()
        try:
            target = None
            needToClose = False
            if not filename:
                filename = "DUMMY.nc"
                target = sys.stdout
            else:
                target = open(filename, "wb") # open local result
                needToClose = True

            url = local.serverBase + "/" + filename
            url += ".dods?superduperscript11"
            
            print "url is " + url
            print "and script is " + script
            #return True # uncomment this to halt before server connect

            result = urllib.urlopen(url, script) # request from server

            shutil.copyfileobj(result, target) # funnel stuff to local
            if needToClose: target.close() # done writing, ok to close
            result.close() #done copying, ok to close
        except AttributeError:
            print "odd error in fetching url/writing file."
        # should be done now
        return True

    pass


def printUsage():
    print "Usage: " + sys.argv[0] + " <cmd> [cmd args...]"
    print "... where <cmd> is one of: ",
    for c in local.acceptableNcCommands: print c,
    print
    print "... and cmd args are the args you want for the command"
    print "Note that you have to have both an input and output file specified,"
    print "unless you're using --ncks=var, where var is the name of the"
    print "variable you want from ncks."


##################################################
#    "main function" begins here."
##################################################

if len(sys.argv) < 4:  # we'll use the heuristic that we have at least:
                       # ssdwrap.py ncsomething in.nc out.nc
    printUsage()
    sys.exit(1)

local.readConfigFile()

# defer command checking to the command itself.  
passedCommand = None
try:
    passedCommand = Command(sys.argv[1:])
except:
    print "Unexpected error:", sys.exc_info()[0]
    sys.excepthook(sys.exc_info()[0], sys.exc_info()[1], sys.exc_info()[2])
    printUsage()
    sys.exit(1)


rs = RemoteScript()
#line = ""
#for x in sys.argv[1:]:
#    line += x + " "
rs.addCommand(passedCommand)
rs.run()

# ssdwrap ncbadf src dest.nc
#wget serverbase/virtdest.nc.dods?superduperscript11

#script has ncecat foo.nc %outfile%
# outfile written to dest.nc

#bin/dap_nc_handler_hack -L -o dods -r /usr/tmp -e superduperscript11 -c -u http://localhost:8000/cgi/nph_dods/nc/foo_T42.nc -v DAP2/3.5.3 /home/wangd/opendap/aolserver4/servers/aoldap/pages/nc/foo_T42.nc <simplescript.ssdap

