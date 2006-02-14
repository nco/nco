#!/usr/bin/env python

# $Id: scriptwrap.py,v 1.3 2006-02-14 01:53:23 wangd Exp $
# want to support url-based specification of server.
#           ncra -n 100,1,2 -p http://server/foo/model_01.nc model_avg.nc
# this file was originally named scriptwrap.py

import os, shutil, sys, urllib

class local:
    (localPath, scriptName) = os.path.split(sys.argv[0])
    helpStr = """A_GREAT_NAME is designed to help you run your ssdap scripts on a server.
    Usage: A_GREAT_NAME <opendap_url> <analysis.ssdap> [output-file]

    -- opendap_url is the base url of the dods server
       e.g. http://localhost:8000/cgi/nph-dods
       This means that if you wanted info about data/foo_02.nc , you
       could point your browser at:
       http://localhost:8000/cgi/nph-dods/data/foo_02.nc.info
       
    -- analysis.ssdap is the name of a server-side script according to
       the proper server-side-dap script specification

    -- output_file is the name of the file to store the output.  Only
       one output script is currently supported."""
    helpStr = helpStr.replace("A_GREAT_NAME", scriptName)


class ScriptRunner:
    def __init__(self):
        self.MAGIC_CONSTRAINT = "superduperscript11"
        self.MAGIC_DUMMY = "DUMMY.nc"
        self.ready = False

    ## public (externally-usable) methods ####################
        
    def parseArgs(self, argv):
        if (len(argv) > 3) or (len(argv) < 2):
            return # not of spec... so continue
        if not self.acceptScript(argv[0]):
            return
        if not self.acceptServer(argv[1]):
            return
        if len(argv) == 3:
            if not self.acceptOutput(argv[2]):
                return
        else: # write to stdout
            self.targetFile = self.MAGIC_DUMMY
        self.ready = True
        return

    def isReady(self):
        return self.ready
    def run(self):
        self.dispatchScript()


    ## private (only called from within class) methods ######

    # validate script existence.  may do basic sanity checks
    def acceptScript(self, scriptname):
        if os.access(scriptname, os.R_OK):
            self.script = scriptname
            self.scriptData = open(scriptname).read()
            #print "got script " + self.scriptData
            return True
        return False

    # validate server url... just look for http:// prefix for now
    # don't really want to contact server for better validation
    def acceptServer(self, serverurl):
        if 0 == serverurl.find("http://"): # look for http at beginning
            self.serverBase = serverurl
            return True
        return False

    # check that we can write to the output file.
    def acceptOutput(self, outname):
        # only check two cases for now.  Doesn't worry (much) about dirs
        if not os.access(outname, os.F_OK):
            # check if dir is writable
            (path,name) = os.path.split(outname)
            if path == "": path = "."  ## fix for current dir writing.
            # check if the current dir is writable
            if not os.access(path, os.W_OK):
                return False
        elif not os.access(outname, os.W_OK):
            return False
        #should be okay now.
        self.targetFile = outname
        return True

    def dispatchScript(self):
        """sends the script to be executed"""
        (path, name) = os.path.split(self.targetFile)
        url = self.serverBase + "/" + name
        url += ".dods?" + self.MAGIC_CONSTRAINT
        #print "url is " + url
        s
        try:
            result = urllib.urlopen(url, self.scriptData) # request from server
            if self.targetFile != self.MAGIC_DUMMY:
                target = open(self.targetFile, "wb") # open local result
                shutil.copyfileobj(result, target) # funnel stuff to local
                target.close() # done writing, ok to close
            else:
                shutil.copyfileobj(result, sys.stdout) # funnel stuff to local
            result.close() #done copying, ok to close
        except AttributeError:
            print "odd error in fetching url/writing file."
        # should be done now
        return True
    
######################################################################
# end of declarations, start of code
######################################################################

s = ScriptRunner()
s.parseArgs(sys.argv[1:])
if not s.isReady():
    print local.helpStr
    
else:
    s.run()
    pass
