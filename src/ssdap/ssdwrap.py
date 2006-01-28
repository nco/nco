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
# version info: $Id: ssdwrap.py,v 1.2 2006-01-28 00:49:45 wangd Exp $
########################################################################


# configurable params
serverBase = "http://localhost:8000/cgi/nph-dods"

# params probably unchanged

# some of these are probably unacceptable... FIXME
acceptableNcCommands = ["ncap", "ncatted", "ncbo", "ncdiff",
                        "ncea", "ncecat", "ncflint", "ncks",
                        "ncpack", "ncpdq", "ncra", "ncrcat",
                        "ncrename", "ncunpack", "ncwa"]
# should probably do some basic sanity check on the options

class Command:
    def __init__(self, cmd, argvlist):
        """construct a command, which is a primitive-ish operation
        over netcdf files.  in the future, we can query the command
        for its attributes (i.e. complexity, dependencies, etc.)"""
        self.cmd = cmd
        self.cmdline = self.build(argvlist)
        pass
    def build(self, argvlist):
        """look for output filename, replace with magic key for remote"""
        # some of these options do not make sense in this context,
        # and some have meanings that necessarily need changing.
        shortopts = "4ACcD:d:Ffhl:n:Oo:p:Rrs:S:t:v:xy:"
        longopts = ["4", "netcdf4", "apn", "append", "crd", "coords",
                    "nocoords", "dbg_lvl=", "debug-level=",
                    "dmn=", "dimension=", "ftn", "fortran",
                    "fnc_tbl", "prn_fnc_tbl", "hst", "history",
                    "lcl=", "local=", "nintap", 
                    "output=", "fl_out=",
                    "ovr", "overwrite", "pth=", "path=",
                    "rtn", "retain", "revision", "vrs", "version",
                    "spt=", "script=", "fl_spt=", "script-file=",
                    "thr_nbr=", "threads=", "omp_num_threads=",
                    "xcl", "exclude"
                    "variable=", "op_typ=", "operation=" ]
        (arglist, leftover) = getopt.getopt(argvlist, shortopts, longopts)
        argdict = dict(arglist)
        ofname = ""
        if "--output" in argdict:
            ofname = argdict["--output"]
        for x in ["-o", "--fl_out"]:
            if x in argdict:
                assert ofname == ""
                ofname = argdict[x]
                # convert alt specs to --output
                argdict["--output"] = ofname
            if ofname == "": # i.e. haven't gotten a parameterixed outfilename
                ofname = leftover[-1]
                argdict["--output"] = ofname # and add to dict.
                assert len(leftover) > 1 # assume in.nc, out.nc, at least
                leftover = leftover[:-1] # take only first element

        assert ofname != ""  # make sure we got one

            
        self.outfilename = ofname # save outfilename
        argdict["--output"] = "%outfile%" # patch with magic script hint
        # hack since ncbo doesn't support --output option
        argdict["-o"] = argdict.pop("--output")
        
        
        #patch infiles with -p option
        self.infilename = self.patchInfiles(argdict, leftover)
        
        # now, build script command line
        return self.rebuildCommandline(argdict, self.infilename)

    def patchInfiles(self, argdict, filelist):
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
            if (len(v) > 0) and \
                   v[0] not in (string.letters + string.digits + "%"):
                line += " " + k + "='" + v + "'"
            else:
                line += " " + k + " " + v
        for name in infilename:
            line += " " + name
        return line
    
    def scriptLineSub(self):
        return self.cmdline
    def outputFile(self):
        return self.outfilename
    

class RemoteScript:
    serverBase = serverBase
    def __init__(self):
        self.cmdList = []
        #
        pass

    def addCommand(self, cmd):
        assert isinstance(cmd, Command)
        self.cmdList.append(cmd)
        # might consider building dep tree here.
        return True
        
    def run(self):
        """sends off its current batch of commands off to the server to run"""
        script = self.buildScript()
        print "script is " + script
        self.executeBuilt(script)

    def buildScript(self):
        """builds a textual script to send off to the server processor"""
        script = ""
        for x in self.cmdList:
            script += x.scriptLineSub()
        return script

    def executeBuilt(self, script):
        """sends the script to be executed"""
        filename = self.cmdList[0].outputFile()
        url = serverBase + "/" + filename
        url += ".dods?superduperscript11"
        print "url is " + url
        print "and script is " + script
        #return True
        try:
            result = urllib.urlopen(url, script) # request from server
            target = open(filename, "wb") # open local result
            shutil.copyfileobj(result, target) # funnel stuff to local
            target.close() # done writing, ok to close
            result.close() #done copying, ok to close
        except AttributeError:
            print "odd error in fetching url/writing file."
        # should be done now
        return True
    
    pass


def printUsage():
    print "Usage: " + sys.argv[0] + " <cmd> [cmd args...]"
    print "... where <cmd> is one of: ",
    for c in acceptableNcCommands: print c,
    print
    print "... and cmd args are the args you want for the command"

if len(sys.argv) < 4:  # we'll use the heuristic that we have at least:
                       # ssdwrap.py ncsomething in.nc out.nc
    printUsage()
    sys.exit(1)

ncCommand = sys.argv[1]
if ncCommand not in acceptableNcCommands:
    printUsage()
    sys.exit(1)


rs = RemoteScript()
#line = ""
#for x in sys.argv[1:]:
#    line += x + " "
rs.addCommand(Command(sys.argv[1], sys.argv[2:]))
rs.run()
    
# ssdwrap ncbadf src dest.nc
#wget serverbase/virtdest.nc.dods?superduperscript11

#script has ncecat foo.nc %outfile%
# outfile written to dest.nc

#bin/dap_nc_handler_hack -L -o dods -r /usr/tmp -e superduperscript11 -c -u http://localhost:8000/cgi/nph_dods/nc/foo_T42.nc -v DAP2/3.5.3 /home/wangd/opendap/aolserver4/servers/aoldap/pages/nc/foo_T42.nc <simplescript.ssdap

