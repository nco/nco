#!/usr/bin/env python

# $Id: scriptwrap2.py,v 1.1 2006-07-01 05:03:54 wangd Exp $
# want to support url-based specification of server.
#           ncra -n 100,1,2 -p http://server/foo/model_01.nc model_avg.nc
# this file was originally named scriptwrap.py

import os, shutil, sys, urllib

class local:
    (localPath, scriptName) = os.path.split(sys.argv[0])
    helpStr = """A_GREAT_NAME is designed to help you run your ssdap scripts on a server.
    Usage: A_GREAT_NAME <analysis.ssdap> <opendap_url> [output-file]

    -- analysis.ssdap is the name of a server-side script according to
       the proper server-side-dap script specification

    -- opendap_url is the base url of the dods server
       e.g. http://localhost:8000/cgi/nph-dods
       This means that if you wanted info about data/foo_02.nc , you
       could point your browser at:
       http://localhost:8000/cgi/nph-dods/data/foo_02.nc.info

    -- output_file is the name of the file to store the output.  Only
       one output script is currently supported."""
    helpStr = helpStr.replace("A_GREAT_NAME", scriptName)


def loglines(rawdata):
    lines = []
    for i in rawdata.splitlines():
        lines.append(i)
        if not i.endswith("\\"):
            yield "".join(lines)
            lines = []
    if len(lines) > 0: yield "".join(lines)
    pass


class SsdGenerator:
    """SsdGenerator is a simple class that understands how to generate
    ssdap scripts based on required information.
    This class is tagged to be put in a common module"""
    
    asyncCmd = "ssd_initsave"
    pollCmd = "ssd_poll"
    retrCmd = "ssd_retrieve"
    # use lambda to defer binding and allow args to be read later.
    cmdTable = { "poll" : lambda s,a : s.makePoll(a[2]),
                 "retr" : lambda s,a : s.makeRetr(a[2]),
                 "help" : lambda s,a : s.listCommands()
                 }
    def __init__(self):
        ## might put mode settings or defaults here.
        pass
    def makePoll(self, filenum):
        return self.pollCmd + " " + str(filenum) + "\n"
    def makeRetr(self, filenum):
        return self.retrCmd + " " + str(filenum) + "\n"
    def makeAsyncScript(self, orig):
        newscript = self.asyncCmd + "\n" + orig
        return newscript

    def guessScript(self, argv):
        """ this doesn't *really* belong in SsdGenerator, perhaps better
        in a client class of SsdGenerator"""
        return self.cmdTable[argv[1]](self, argv)
        
    def listCommands(self):
        print "available cmds", self.cmdTable.keys()
    pass
class SsdGeneffrator:
    """ dismiss this class """
    asyncCmd = "ssd_initsave"
    pollCmd = "ssd_poll"
    def __init__(self):
        ## might put mode settings or defaults here.
        pass
    def makePoll(self, filenum):
        return  self.pollCmd + " " + str(filenum) + "\n"
    def makeAsyncScript(self, orig):
        newscript = self.asyncCmd + "\n" + orig
        return newscript
        
    pass



class ScriptedTask:
    """a class that models a script as a task abstraction. Or so I had thought.
    ScriptedTask also bundles functionality to speak ssdap with the server.
    The common features badly need refactoring, once I have the prototype.
    Only with the prototype will the use case finally crystallize."""
    def __init__(self):
        ## this flag turns on when the script has multiple outputs
        self.asyncRequired = False
        self.originalScript = ""
        self.patchedScript = ""
        self.consistent = False # is the task ready to execute/process?
        self.outputFiles = {} # a dict mapping names-ids (for now)
        self.outLines = {} # a dict mapping script lines to output files
        self.asyncCmd = "ssd_initsave"
        pass
    @staticmethod
    def selectOutfile(line):
        """returns the output file part of a command line"""
        outfile = None
        lastarg = None
        import shlex # handy, handy shell lexer/tokenizer
        for x in shlex.split(line, True): # strip comments.
            if x == "": continue
            # do not allow pipes until we decide the right way to handle them
            assert(x[0] not in ["|","&"]) 
                
            if lastarg in ["-o", "--output", "--fl_out"]:
                assert (outfile == None) # trap dupe fileouts
                outfile = x
            lastarg = x
        if outfile == None: # steal last arg as output if no outfile yet
            outfile = lastarg
        # if the command was malformed, we might be stealing the input
        # file or another option.
        # error should be caught by real arg parser later.
        return outfile
    @staticmethod
    def needsReturning(filename):
        """Returns True if the file matches the outfile spec,
        i.e. %outfile_xxx%
        (actually, it returns a match object, which evaluates to true"""
        import sre
        pattern = """
        ^%      # % start delimiter
        outfile # magic outfile indicator
        (_\w+)? # outfile name suggestion
        %$      # end delimiter
        """
        print "checking ",filename,"to see if it needs returning"
        m = sre.search(pattern, filename, sre.VERBOSE)
        if m: return m.group(1)
        else: return
        
    def setFrom(self, script):
        self.originalScript = script
        
        # look for multifile
        scriptlines = []
        self.outLines = []
        #first, break script into logical lines-- merge backslashes.
        for line in loglines(script):
            ofid = ScriptedTask.selectOutfile(line)
            if ofid and ScriptedTask.needsReturning(ofid):
                    self.outLines.append((line,ofid))
        # for now, disallow these WAW hazards
        # -- maybe reparse away from WAW in a later version.
        assert( len(self.outLines)
                == len(dict(map(lambda (x,y) : (y,x), #swap line, id
                                self.outLines)) ) #so dict indexes by id
                )
                    
        # if found, switch to multi-mode
        if len(self.outLines) > 1:
            # construct async script
            # prepare to run two-stage approach
            self.asyncRequired = True
            g = SsdGenerator();
            self.patchedScript = g.makeAsyncScript(self.originalScript)
            self.consistent = True
            
            pass
        else: #simple script, pass and run in the old way.
            self.asyncRequired = False
            self.patchedScript = self.originalScript
            self.consistent = True
            pass

        pass

    def execute(self, execFunc, outStream):
        """execute this task with the specified exec function.
        function should have the following semantics:
        Given s = a string containing a valid ssdap script
        execFunc(s, out)
        ...should write the server response in out

        defOutStream should support file write semantics.
        """
        # simple way:
        if not self.asyncRequired:
            execFunc(self.patchedScript, outStream)
        else:
            from cStringIO import StringIO
            ostream = StringIO()
            execFunc(self.patchedScript, ostream)
            print "submit ok"
            rawtokens = ostream.getvalue()
            print "tokens:", rawtokens
            self.parseResponseTokens(rawtokens)
            print "fetching..."
            self.retrieveAllOutput(execFunc)
            #maybe report output back to user now.
        pass
    def parseResponseTokens(self, tokenraw):
        """ from the server response to the async init, extract the outfile
        ids so that we can retrieve the outputs later.  also make sure that
        the ids are matched to the outfiles we are tracking. should report
        a warning if a file we are expecting is not given an id."""
        tokenpattern = """
        SSDTOKEN<<\s* #magic delimiter
        logical=\" 
        (%\w+%) # the logical name
        \"\s*
        token=\"
        (\d+) # the numeric token
        \"\s*>> #end delimiter
        """
        import sre
        p ="SSDTOKEN<<\s*logical=\""
        q = "logical=\"(\w+)\""
        for m in sre.finditer(q,tokenraw):
            print "match "
        for m in sre.finditer(tokenpattern, tokenraw, sre.VERBOSE):
            logical = m.group(1) #first subgroup
            token = m.group(2) #second subgroup
            self.outputFiles[logical] = token
            print "add",logical,"->",token
            # ignore overwrite/duplicate for now
        pass
    def retrieveAllOutput(self, execFunc):
        """ with the knowledge of the files we are expecting, retrieve
        each file from the server using our ssdap protocol.  for now,
        choose a blocking one-at-a-time request protocol."""
        for t in self.outLines:
            logname = t[1]
            print "retrieving", logname
            #needsreturning strips outfile tag.
            nicename = self.needsReturning(logname)
            f = open(nicename, "wb")
            self.retrieveById(self.outputFiles[logname], execFunc, f)
            f.close()
            print "done getting", logname
            #will want to delegate file writing to a function/module
            #that can do whatever nice path fixups are needed

            # for now, don't trap file writing exceptions
            # -- let them fail things.
        pass
    def retrieveById(self, id, execFunc, outStream):
        g = SsdGenerator()
        # fixme: decide how to funnel url here.
        #url = "http://localhost/cgi-bin/nph-dods"
        script = g.makeRetr(id)
        execFunc(script, outStream)
        pass
    def makeRequest(self):
        """sends the script to be executed"""
        (path, name) = os.path.split(self.targetFile)
        url = self.serverBase + "/" + name
        url += ".dods?" + self.MAGIC_CONSTRAINT
        print "url is " + url
        
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
        
        pass
    pass
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
        s = ScriptedTask()
        s.setFrom(self.scriptData)
        s.execute(self.dispatchScriptOut, sys.stderr)
        #self.dispatchScript()


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

    def acceptArgs(self, scriptcontents, url):
        if len(scriptcontents) <= 0:
            return
        self.scriptData = scriptcontents
        if not self.acceptServer(url):
            return
        self.captureOutput = True
        self.targetFile = self.MAGIC_DUMMY
        self.ready = True
        pass

    def dispatchScript(self):
        if self.targetFile != self.MAGIC_DUMMY:
            target = open(self.targetFile, "wb") # open local result 
            self.dispatchScriptOut(self.scriptData, target)
            target.close() # done writing, ok to close
        else:
            self.dispatchScriptOut(self.scriptData, sys.stdout)
        pass
    def dispatchScriptOut(self, script, outstream):
        """sends the script to be executed"""
        (path, name) = os.path.split(self.targetFile)
        url = self.serverBase + "/" + name
        url += ".dods?" + self.MAGIC_CONSTRAINT
        print "url is " + url
        
        try:
            result = urllib.urlopen(url, script) # request from server
            shutil.copyfileobj(result, outstream) # funnel stuff to local
            result.close()
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
# selftest
