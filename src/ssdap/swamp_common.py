# $Header: /data/zender/nco_20150216/nco/src/ssdap/swamp_common.py,v 1.14 2007-04-12 02:23:53 wangd Exp $
# swamp_common.py - a module containing the parser and scheduler for SWAMP
#  not meant to be used standalone.
# 
# Copyright (c) 2007, Daniel L. Wang
# Licensed under the GNU General Public License v2

"""Parser and scheduler module for SWAMP
This provides high level access to SWAMP parsing and scheduling functionality.
"""
__author__ = "Daniel L. Wang <wangd@uci.edu>"
#__all__ = ["Parser"] # I should fill this in when I understand it better

# Python imports
import cPickle as pickle
import copy # for shallow object copies for pickling
import getopt
from fnmatch import fnmatch
import glob
from heapq import * # for minheap implementation
import logging
import md5
import os
import operator
import re
import shlex
import shutil
import struct
import subprocess
import time
import types
import urllib
from pyparsing import *
from SOAPpy import SOAPProxy # for remote execution        



# SWAMP imports
#
from swamp_dbutil import JobPersistence
from swamp_config import Config

# define global swamp logger
log = logging.getLogger("SWAMP")
#
#

#local module helpers:
def appendList(aDict, key, val):
    l = aDict.get(key,[])
    l.append(val)
    aDict[key] = l


class VariableParser:
    """treats shell vars "a=b" and env vars "export a=b" as the same for now.
    """
    ident = "[A-Za-z]\w*"
    quoteclass = "[\"\']"
    identifier = Word(alphas, alphanums+"_")
    unpReference = Literal("$").suppress() + identifier
    protReference = Literal("${").suppress() + identifier + Literal("}").suppress()
    reference = protReference | unpReference
    backtickString = Literal("`") + CharsNotIn("`") + Literal("`")
    
    def __init__(self):
        self.varMap = {}
        reference = VariableParser.reference.copy()
        # lookup each substitution reference in the dictionary
        reference.setParseAction(lambda s,loc,toks: [self.varMap[t] for t in toks])
        dblQuoted = dblQuotedString.copy()
        dblQuoted.setParseAction(lambda s,loc,toks: [self.varSub(t) for t in toks])
        sglQuoted = sglQuotedString.copy()
        self.reference = reference
        self.dblQuoted = dblQuoted
        self.sglQuoted = sglQuoted
        varValue = (self.dblQuoted
                    | self.sglQuoted
                    | Word(alphanums+"-_/")
                    | CharsNotIn(" "))
        
        varDefinition = Optional(Literal("export ")).suppress() \
                        + VariableParser.identifier \
                        + Literal("=").suppress() + varValue

        def assign(lhs, rhs):
            self.varMap[lhs] = rhs
            logging.debug("assigning %s = %s" % (lhs,rhs))
        varDefinition.setParseAction(lambda s,loc,toks:
                                     assign(toks[0], toks[1]))
        self.arithExpr = self.makeCalcGrammar()
        varArithmetic = Literal("let ").suppress() \
                        + VariableParser.identifier \
                        + Literal("=").suppress() + self.arithExpr
        def arithAssign(lhs,rhs):
            assign(lhs, str(eval("".join(rhs))))
        varArithmetic.setParseAction(lambda s,loc,toks:
                                     arithAssign(toks[0], toks[1:]))
                   

        self.varArithmetic = varArithmetic
        self.varDefinition = varDefinition

    @staticmethod
    def parse(original, argv):
        pass

    def accepts(argv):
        if argv[0] == "export":
            return isAssign(argv[1:])
        return isAssign(argv)
    def isAssign(argv):
        pass
    def makeCalcGrammar(self):
        # from:
        # http://pyparsing.wikispaces.com/space/showimage/simpleArith.py
        # and 
        # http://pyparsing.wikispaces.com/space/showimage/fourFn.py
        # Copyright 2006, by Paul McGuire
        integer = Word(nums).setParseAction(lambda t:int(t[0]))

        snum = Word( "+-"+nums, nums )
        lpar  = Literal( "(" )
        rpar  = Literal( ")" )
        addop  = Literal( "+" ) | Literal( "-" )
        multop = Literal("*") | Literal("/")
        identifier = VariableParser.identifier.copy()
        identifier.setParseAction(lambda s,loc,toks:
                                  [self.varMap[t] for t in toks])
        expr = Forward()
        atom = (Optional("-") + (snum | identifier|(lpar+expr+rpar)))

        # by defining exponentiation as "atom [ ^ factor ]..." instead of "atom [ ^ atom ]...", we get right-to-left exponents, instead of left-to-righ
        # that is, 2^3^2 = 2^(3^2), not (2^3)^2.
        term = atom #+ ZeroOrMore(multop + atom)
        expr << term + ZeroOrMore(addop + term)

        return expr


    @staticmethod
    def expand(token):
        return
        x = re.match(VariableParser.quoteclass, token)
        if x is not None:
            (start,end) = x.span()
            if x[start] == "\"":
                # doublequote
                close = x.find("\"", end)
                if close is None:
                    return StandardError("Parse error.")
                substr = x[start:end]
                return token[:start] 
            pass
        pass

    def varSub(self, fragment):
        #print self.reference.transformString(fragment)
        
        return self.reference.transformString(fragment)
    def varDef(self, fragment):        
        try:
            return [x for x in self.varDefinition.parseString(fragment)]
        except ParseException, e:
            return 
        pass
    def varLet(self, fragment):        
        try:
            return [x for x in self.varArithmetic.parseString(fragment)]
        except ParseException, e:
            return 
        pass
    def expand2(self, fragment):
        return self.varSub(fragment)
    def quoteSplit(self, fragment):
        quoted = sglQuoted | doubleQuoted
        scanString
    def apply(self, fragment):
        fragment = self.expand2(fragment)
        #print "try ", fragment
        identifier = VariableParser.identifier.copy()
        def tryconvert(v):
            if v in self.varMap:
                return self.varMap[v]
            return v
        identifier.setParseAction(lambda s,loc,toks:
                                  [tryconvert(t) for t in toks])

        #print "".join([str(x) for x in identifier.scanString(fragment)])
        #print "".join([str(x) for x in self.varArithmetic.scanString(fragment)])

        result = self.varLet(fragment)
        if result:
            return # we consumed, so nothing left after applying
        result = self.varDef(fragment)
        if result:
            return # we consumed, so nothing left after applying
        return fragment
def testExpand():
    vp = VariableParser()
    vp.varMap = {"sub" : "<SUB>", "sub3" : "-SUB3-", "sub2" : "Two"}
    test1 = "This is a simple $sub test with Joe$sub3 and ${sub2}blah"
    test2 = "export x=adf"
    test3 = "Now x should be substituted, so x = $x or ${x}"
    test4 = "CASEID=$1"
    test5 = 'John="$CASEID1 double quote"'
    test6 = "Josiah='$CASEID1 single quote'"
    test7 = 'Jerry=asdf"blahblah"'
    test8 = "Junior=asdf'blahblah'"
    test9 = "let Y1=0000+1"

    print vp.varSub(test1)
    print vp.varMap    
    print vp.varDef(test2)
    print vp.varMap
    print vp.varSub(test3)
    print vp.varDef(test4)
    print vp.varLet(test9)

    return
    print VariableParser.expand("blahblah")
    print VariableParser.expand("'blahblah'")
    print VariableParser.expand('"blahblah"')
    print VariableParser.expand('asdf"blahblah"asdf')

class NcoParser:
    """stuff that should be identical between client and server code"""
    commands = ["ncap", "ncatted", "ncbo", "ncdiff",
                "ncea", "ncecat", "ncflint", "ncks",
                "ncpack", "ncpdq", "ncra", "ncrcat",
                "ncrename", "ncunpack", "ncwa"]
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
                     "mmr_cln", # only on recent NCO release
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

    ncflintShortOpt = parserShortOpt.replace("hl","hi:l")
    ncflintLongOpt = parserLongOpt[:]
    ncflintLongOpt.extend(['ntp', 'interpolate'])

    # special handling for ncwa's params (mask-related)
    # B: msk_cnd= mask_condition= (rplc)
    # b rdd degenerate-dimensions (rplc)
    # I: wgt_msk_crd_var= 
    # M: msk_val= mask_value=  mask-value= (rplc)
    # m: msk_nm= msk_var= mask_variable= mask-variable= (rplc)
    # N nmr numerator
    # T: mask_comparitor= msk_cmp_typ= op_rlt=
    ncwaShortOpt = parserShortOpt.replace("Bb:","B:b")
    ncwaShortOpt = ncwaShortOpt.replace("h","hI:")
    ncwaShortOpt = ncwaShortOpt.replace("Mmn:","M:m:Nn:")
    ncwaShortOpt = ncwaShortOpt.replace("t:","T:t:")
    ncwaLongOpt = parserLongOpt[:]
    ncwaLongOpt.extend(["msk_cnd=", "mask_condition=",
                        "rdd", "degenerate-dimensions",
                        "wgt_msk_crd_var=", 
                        "msk_val=", "mask_value=",  "mask-value=",
                        "msk_nm=", "msk_var=", "mask_variable=",
                        "mask-variable=",
                        "nmr", "numerator",
                        "mask_comparitor=", "msk_cmp_typ=", "op_rlt="])
    
    @staticmethod
    def specialGetOpt(argv):
        """argv is the argvlist formed like you would get from sys.argv .
        It should include the nco command (e.g. ncwa) in the 0th index."""
        #consider special-case for ncwa ncflint -w: option
        # wgt_var, weight also for ncflint/ncwa
        cmd = argv[0]
        argvlist = argv[1:]
        if cmd == "ncap": # ncap has a different format
            return getopt.getopt(argvlist,
                                 NcoParser.ncapShortOpt,
                                 NcoParser.ncapLongOpt)
        elif cmd in ["ncpdq", "ncpack", "ncunpack"]:
            # ncpdq/ncpack/ncunpack have a different format too
            return getopt.getopt(argvlist,
                                 NcoParser.ncpackShortOpt,
                                 NcoParser.ncpackLongOpt)
        elif cmd == "ncks":
            return getopt.getopt(argvlist,
                                 NcoParser.ncksShortOpt,
                                 NcoParser.ncksLongOpt)
        elif cmd == "ncflint":
            return getopt.getopt(argvlist,
                                 NcoParser.ncflintShortOpt,
                                 NcoParser.ncflintLongOpt)
        elif cmd == "ncwa":
            return getopt.getopt(argvlist,
                                 NcoParser.ncwaShortOpt,
                                 NcoParser.ncwaLongOpt)
        else:
            return getopt.getopt(argvlist,
                                 NcoParser.parserShortOpt,
                                 NcoParser.parserLongOpt)
        pass

    @staticmethod
    def parse(original, argv, lineNumber=0, factory=None):
        cmd = argv[0]
        (argList, leftover) = NcoParser.specialGetOpt(argv)
        argDict = dict(argList)
        (ins, outs) = NcoParser.findInOuts(cmd, argDict, argList, leftover)
        # logging.debug(" ".join([str(x)
#                                 for x in ["accept!", argv[0],
#                                           argList, leftover]
#                                 ]))
        if factory is not None:
            # assert for now to catch stupid mistakes.
            assert isinstance(factory, CommandFactory)
            return factory.newCommand(cmd,
                                      (argDict, argList, leftover),
                                      (ins,outs), lineNumber)
        else:
            return cmd

    @staticmethod
    def findInOuts(cmd, adict, alist, leftover):
        # look for output file first
        ofname = ""
        inPrefix = None
        for x in  ["-o", "--fl_out", "--output"]:
            if x in adict:
                assert ofname == ""
                keys = [y[0] for y in alist]
                o = alist.pop(keys.index(x)) # o is a tuple.
                ofname = adict.pop(x)
                
                assert o[1] == ofname
                
        if ofname == "":
            # don't steal output if it's actually the input.
            if len(leftover) > 1:
                ofname = leftover[-1] # take last arg
                leftover = leftover[:-1] # and drop it off

        inlist = leftover # Set input list.

        # Detect input prefix.
        if "-p" in adict: 
            inPrefix = adict["-p"] 
            keys = [x[0] for x in alist]
            p = alist.pop(keys.index("-p"))
            adict.pop("-p")
        # handle ncap -S script input
        if (cmd == "ncap") and adict.has_key("-S"):
            inlist.append(adict["-S"])
        # now patch all the inputs
        if inPrefix is not None:
            inlist = [(inPrefix + x) for x in inlist]
        return (inlist,[ofname])
        pass

            
    
    @staticmethod
    def accepts(argv):
        if len(argv) < 1:
            return False
        else:
            return (argv[0] in NcoParser.commands)    

class NcoBinaryFinder:
    def __init__(self, config):
        self.config = config
        pass
    def __call__(self, cmd):
        # for now, always pick one nco binary,
        # regardless of netcdf4 or opendap.
        return self.config.execNcoDap + os.sep + cmd.cmd
    
        

# Command and CommandFactory do not have dependencies on NCO things.
class Command:
    """this is needed because we want to build a dependency tree."""

    def __init__(self, cmd, argtriple, inouts, parents, referenceLineNum):
        self.cmd = cmd
        self.parsedOpts = argtriple[0] # adict
        self.argList = argtriple[1] # alist
        self.leftover = argtriple[2] # leftover
        self.inputs = inouts[0]
        self.outputs = inouts[1]
        self.parents = parents
        self.referenceLineNum = referenceLineNum
        self.actualOutputs = []
        self.children = []
        self.factory = None
        self.inputSrcs = []
        pass

    # sort on referencelinenum
    def __cmp__(self, other):
        return cmp(self.referenceLineNum, other.referenceLineNum)

    def __hash__(self): # have to define hash, since __cmp__ is defined
        return id(self)

    def pickleNoRef(self):
        safecopy = copy.copy(self)
        safecopy.parents = None
        safecopy.children = None
        safecopy.factory = None
        return pickle.dumps(safecopy)


    # need to map all non-input filenames.
    # apply algorithm to find temps and outputs
    # then mark them as remappable.
    # this can be done in the mapping function.
    def remapFiles(self, mapFunction):
        raise StandardError("unimplemented function Command.remapFiles")
    
    def remapFile(self, logical, mapInput, mapOutput):
        if logical in self.inputs:
            return mapInput(logical)
        elif logical in self.outputs:
            phy = mapOutput(logical)
            self.actualOutputs.append((logical, phy))
            return phy
        else:
            return logical

    def makeCommandLine(self, mapInput, mapOutput):
        cmdLine = [self.cmd]
        # the "map" version looks a little funny.
        #cmdLine += map(lambda (k,v): [k, self.remapFile(v,
        #                                            mapInput,
        #                                            mapOutput)],
        #               self.argList)
        cmdLine += reduce(lambda a,t: a+[t[0], self.remapFile(t[1],
                                                              mapInput,
                                                              mapOutput)],
                          self.argList, [])
            
        cmdLine += map(lambda f: self.remapFile(f, mapInput, mapOutput),
                       self.leftover)
        # don't forget to remove the '' entries
        # that got pulled in from the arglist
        return filter(lambda x: x != '', cmdLine)

    pass # end of Command class
        
class CommandFactory:
    """this is needed because we want to:
    a) connect commands together
    b) rename outputs.
    To create a new command, we need:
    a) which commands created my inputs?
    b) what should I remap the file to?
    So:
    -- a mapping: script filename -> producing command
    -- script filename -> logical name (probably the same,
    but may be munged)
    -- logical name -> producing command.  This is important
    for finding your parent.
    For each output, create a new scriptname->logical name mapping,
    incrementing the logical name if a mapping already exists.  
    
    """
    def __init__(self, config):
        self.config = config
        self.commandByLogicalOut = {}
        self.commandByLogicalIn = {}
        self.logicalOutByScript = {}
        self.scriptOuts = set() # list of script-defined outputs
        # scriptOuts is logicalOutByScript.keys(), except in ordering
        self.scrFileUseCount = {} 
        pass

    def mapInput(self, scriptFilename):
        temps = filter(lambda f: fnmatch(f, scriptFilename),
                       self.scriptOuts)
        # FIXME: should really match against filemap, since
        # logicals may be renamed
        if temps:
            # we need to convert script filenames to logicals
            # to handle real dependencies
            return map(lambda s: self.logicalOutByScript[s], temps)
        else:
            inList = self.expandConcreteInput(scriptFilename)
            if inList:
                return inList
            else:
                log.error("%s is not allowed as an input filename"
                              % (scriptFilename))
                raise StandardError("Input illegal or nonexistant %s"
                                    % (scriptFilename))
        pass

    def mapOutput(self, scriptFilename):
        s = scriptFilename
        if scriptFilename in self.logicalOutByScript:
            s = self.nextOutputName(self.logicalOutByScript[s])
        else:
            s = self.cleanOutputName(s)
        self.logicalOutByScript[scriptFilename] = s
        return s
                    
    def expandConcreteInput(self, inputFilename):
        save = os.getcwd()
        os.chdir(self.config.execSourcePath)
        res = glob.glob(inputFilename)
        #logging.error("globbing %s from %s"%(inputFilename,os.curdir))
        #logging.error("-- %s --%s"%(str(res),str(glob.glob("camsom1pdf/*"))))
        os.chdir(save)
        return res

    def cleanOutputName(self, scriptFilename):
        # I can't think of a "good" or "best" way, so for now,
        # we'll just take the last part and garble it a little
        (head,tail) = os.path.split(scriptFilename)
        if tail == "":
            log.error("empty filename: %s"%(scriptFilename))
            raise StandardError
        if head != "":
            # take the last 4 hex digits of the head's hash value
            head = ("%x" % hash(head))[-4:]
        return head+tail

    def nextOutputName(self, logical):
        # does the logical name end with .1 or .2 or .3 or .99?
        # (has it already been incremented?)
        m = re.match("(.*\.)(\d+)$", logical)

        if m is not None:
            # increment the trailing digit(s)
            return m.group(1) + str(1 + int(m.group(1)))
        else:
            return logical + ".1"

    @staticmethod
    def incCount(aDict, key):
        aDict[key] = 1 + aDict.get(key,0)

    @staticmethod
    def appendList(aDict, key, val):
        l = aDict.get(key,[])
        l.append(val)
        aDict[key] = l

    def newCommand(self, cmd, argtriple,
                   inouts, referenceLineNum):
        # first, reassign inputs and outputs.
        scriptouts = inouts[1]
        newinputs = reduce(operator.add, map(self.mapInput, inouts[0]))
        newoutputs = map(self.mapOutput, inouts[1])
        inouts = (newinputs, newoutputs)

        # patch arguments: make new leftovers
        argtriple = (argtriple[0], argtriple[1], newinputs + newoutputs)

        # link commands: first find parents 
        inputsWithParents = filter(self.commandByLogicalOut.has_key, newinputs)
        map(lambda f: CommandFactory.incCount(self.scrFileUseCount,f),
            inputsWithParents)
        
        parents = map(lambda f: self.commandByLogicalOut[f], inputsWithParents)

        
        # build cmd, with links to parents
        c = Command(cmd, argtriple, inouts, parents, referenceLineNum)

        # then link parents back to cmd
        map(lambda p: p.children.append(c), parents)

        # update scriptOuts 
        self.scriptOuts.update(scriptouts)
        
        # update parent tracker.
        for out in inouts[1]:
            self.commandByLogicalOut[out] = c

        # link inputs to find new cmd
        map(lambda f: CommandFactory.appendList(self.commandByLogicalIn,f,c),
            inputsWithParents)

        # additional cmd properties to help delete tracking
        c.factory = self
        c.inputsWithParents = inputsWithParents
        return c

    def unpickleCommand(self, pickled):
        # not sure this needs to be done in the factory
        # -- maybe some fixup code though
        return pickle.loads(pickled)

    pass # end of CommandFactory class




class Parser:
    """Parse a SWAMP script for NCO(for now) commands"""

    # Try to keep parse state SMALL!  Don't track too much for parsing.
    # Only a few things will cause us to keep state across multiple lines:
    # - "\" continuation
    # - for-loop constructs
    # - defined variables
    # - if-then-else constructs not an impl priority.

    # Summary of support:
    # backticks.  be very careful about this.
    # nco commands.  allow quotes.
    # comments. must start the line or be >=1 space separated
    # matching quotes.  this might be annoying
    # output capture: we can actually do this:
    #           [> out.txt [2>&1]]
    #           We don't need to be fancy like bash and support alternate
    #              orderings and syntaxes.

    # Anti-support:
    # meta-commands.  Don't support constructs that will break
    #                 on a plain-old shell script interpreter

    # build with default handlers: print statements

    # Primary function:
    # From a well-formed script,
    # * extract an ordered list of command tuples
    # * command tuple = (original, command, inputlist, outputlist,
    #                    arglist, leftover)
    #
    # Modules are used to isolate logic pertaining to specific binaries.
    # A module *accepts* a command line and *parses* it.    
    
# put in a sanity check
#     if len(leftover) <= 1:
#         # only one leftover...leave it to be captured by the inputter
#         logger.warning("warning, unbound output for "
#                        + self.original)
# handle dep checking in scheduler, not parser.
#        self.env.tinlist.update(map(lambda x:(x,1),leftover))

    
    def __init__(self):
        self.handlers = {} # a lookup table for parse accept handlers
        self.variables = {} # a lookup table for variables
        self.lineNum=0
        self.modules = []
        self.handlerDefaults()
        self.handleFunc = None
        self.commands = []
        self.variableParser = VariableParser()
        pass

    def handlers(self, newHandlers):
        def newNcoCommand(argv):
            print "--".join(argv)
            return True
        pass

    def handlerDefaults(self):
        self.modules = [(NcoParser, None)]

        pass
        
    def commandHandler(self, handle):
        """handler is a function reference.  the function is a unary
        function: handle(ParserCommand)"""
        self.handleFunc = handle
        pass
    def parseScriptLine(self, factory, line):
        def accept(obj, argv):
            for mod in obj.modules:
                if mod[0].accepts(argv):
                    cmd = mod[0].parse(line, argv, obj.lineNum, factory)
                    return cmd
            return False
        self.lineNum += 1
        original = line.strip()
        comment = None
        # for now, do not detect sh dialect 
        linesp = original.split("#", 1) # chop up comments
        
        if len(linesp) > 1:
            line, comment = linesp
        else:
            line = original
            
        if (len(line) < 1): # skip comment-only line
            return
                
        line = self.variableParser.apply(line)
        if not isinstance(line, str):
            return None
        argv = shlex.split(line)
        command = accept(self, argv)
        if isinstance(command, types.InstanceType):
            command.referenceLineNum = self.lineNum
            command.original = original
        if not command:
            logging.debug(" ".join(["reject:", str(len(argv)), str(argv)]))
        elif self.handleFunc is not None:
            self.handleFunc(command)
        return command
    
    def parseScript(self, script, factory):
        """Parse and accept/reject commands in a script, where the script
        is a single string containing script lines"""
        vp = VariableParser()
        for line in script.splitlines():
            self.parseScriptLine(factory, line)
        log.debug("factory cmd_By_log_in: " + str(factory.commandByLogicalIn))
        log.debug("factory uselist " + str(factory.scrFileUseCount))
        
        pass
    
    pass

class Scheduler:
    def __init__(self, config, executor=None):
        self.config = config
        self.transaction = None
        self.env = {}
        self.taskId = self.makeTaskId()
        self.executor = executor
        self.cmdList = []
        pass
    def makeTaskId(self):
        # As SWAMP matures, we should rethink the purpose of a taskid
        # It's used now to disambiguate different tasks in the database
        # and to provide a longer-lived way to reference a specific task.

        # if we just need to disambiguate, just get some entropy.
        # this should get us enough entropy
        digest = md5.md5(str(time.time())).digest()
        # take first 4 bytes, convert to hex, strip off 0x and L
        ## assume int is 4 bytes. works on dirt (32bit) and tephra(64bit)
        assert struct.calcsize("I") == 4 
        taskid = hex(struct.unpack("I",digest[:4])[0])[2:10] 
        return taskid
    
    def instanceJobPersistence(self):
        """finds the class's instance of a JobPersistence object,
        creating if necessary if it doesn't exist, and caching for
        future use."""
        if self.env.has_key("JobPersistence"):
            o = self.env["JobPersistence"] 
            if o != None:
                return o
        o = JobPersistence(self.config.dbFilename, True)
        self.env["JobPersistence"] = o
        return o


    def initTransaction(self):
        assert self.transaction is None
        jp = self.instanceJobPersistence()
        trans = jp.newPopulationTransaction()
        self.persistedTask = trans.insertTask(self.taskId)
        assert self.persistedTask is not None
        self.transaction = trans
        pass
    
    def schedule(self, parserCommand):
        if self.transaction is None:
            self.initTransaction()
        self.transaction.insertCmd(parserCommand.referenceLineNum,
                                   parserCommand.cmd, parserCommand.original)
        #concrete = logical # defer concrete mapping
        def insert(f, isOutput):
            self.transaction.insertInOutDefer(parserCommand.referenceLineNum,
                                          f, f, isOutput, 1)
            pass
        map(lambda f: insert(f, False), parserCommand.inputs)
        map(lambda f: insert(f, True), parserCommand.outputs)
        self.cmdList.append(parserCommand)

        pass
    def finish(self):
        self.transaction.finish()
        pass
    def executeSerialAll(self):
        def run(cmd):
            if self.executor:
                tok = self.executor.launch(cmd)
                retcode = self.executor.join(tok)
                return retcode
        for c in self.cmdList:
            ret = run(c)
            if ret != 0:
                log.debug( "ret was "+str(ret))
                log.error("error running command %s" % (c))
                break
    def executeParallelAll(self, executors=None):
        if executors is None:
            executors = [self.executor]
        pd = ParallelDispatcher(self.config, executors)
        pd.dispatchAll(self.cmdList)
        pass
    pass # end of class Scheduler

class ParallelDispatcher:
    def __init__(self, config, executorList):
        self.config = config
        self.executors = executorList
        self.finished = {}
        self.okayToReap = True
        self.tempFiles = {} # logicalout -> (useCount, )
        # not okay to declare file death until everything is parsed.
        self.execLocation = {} # logicalout -> executor
        self.sleepTime = 0.2
        self.running = {} # (e,etoken) -> cmd
        pass

    def fileLoc(self, file):
        """ return url of logical output file"""
        execs = self.execLocation[file]
        if len(execs) > 0:
            return execs[0].actual[file] # return the first one
        else:
            return None
        
    def isDead(self, file, consumingCmd):
        # Find the producing cmd, see if all its children are finished.
        return 0 == len(filter(lambda c: c not in self.finished,
                               consumingCmd.factory.commandByLogicalIn[file]))

    def isReady(self, cmd):
        # if there are no unfinished parents...
        return 0 == len(filter(lambda c: c not in self.finished, cmd.parents))


    def findReady(self, queued):
        return filter(self.isReady, queued)

    def extractReady(self, queued):
        """returns a tuple (ready,queued)
        filter is elegant to generate r, but updating q is O(n_r*n_q),
        whereas this generates r and q in O(n_q)
        """
        r = []
        q = []
        for c in queued:
            if self.isReady(c):
                r.append(c)
            else:
                q.append(c)
        return (r,q)
        
        
    def findMadeReady(self, justFinished):
        """Generally, len(justFinished.children) << len(queued).
        n_j * n_q'  << n_q, since n_j << n_q
        and n_q'(position number of justfinishedchild << n_q, so
        it's generally cheaper just to find+remove the ready
        elements instead of iterating over the entire list."""
        assert justFinished in self.finished
        return filter(self.isReady, justFinished.children)
    
    def dispatch(self, executor, cmd):
        log.debug("dispatching %s %d" %(cmd.cmd, cmd.referenceLineNum))
        etoken = executor.launch(cmd, map(lambda f:(f, self.fileLoc(f)),
                                          cmd.inputsWithParents))
        self.running[(executor, etoken)] = cmd

    def releaseFiles(self, files):
        log.info("ready to delete " + str(files))
        map(lambda f: map(lambda l: l.discardFile(f),
                          self.execLocation[f]),
            files)

    def _graduate(self, token, code):
        cmd = self.running.pop(token)

        if code != 0:
            s = "Bad return code %s from cmdline %s" % (code, cmd.cmd)
            log.error(s)
            raise StandardError(s)
        else:
            # figure out which one finished, and graduate it.
            self.finished[cmd] = code
            log.debug("graduating %s %d" %(cmd.cmd,
                                                  cmd.referenceLineNum))
            # update the readylist
            newready = self.findMadeReady(cmd)
            map(lambda x: heappush(self.ready,x), newready)
            # delete consumed files.
            if self.okayToReap:
                self.releaseFiles(filter(lambda f: self.isDead(f, cmd),
                                         cmd.inputsWithParents))
            e = token[0] # token is (executor, etoken)
            log.debug("linking %s to executor %s" %(str(cmd.outputs), str(e)))
            map(lambda o: appendList(self.execLocation, o, e), cmd.outputs)
            # hardcoded for now.

    def _pollAny(self):
        for e in self.executors:
            # should be faster to iterate over self.executors
            # than self.running
            r = e.pollAny()
            if r is not None:
                self._graduate((e, r[0]), r[1])
                return ((e, r[0]), r[1])
        return None

    def _waitAnyExecutor(self):
        while True:
            r = self._pollAny()
            if r is not None:
                return r
            time.sleep(self.sleepTime)

        
    def _nextFreeExecutor(self):
        """Return next free executor. Try to pursue a policy of
        packing nodes tight so as to maximize locality.
        We'll go fancier some other time."""
        for e in self.executors:
            if not e.busy():
                return e
        return None

    def dispatchAll(self, cmdList):
        self.running = {} # token -> cmd
        (self.ready, self.queued) = self.extractReady(cmdList)

        # Consider 'processor affinity' to reduce data migration.
        while True:
            # if there are free slots in an executor, run what's ready
            e = self._nextFreeExecutor() 
            if e is None or not self.ready:
                if not self.running: # done!
                    break
                r = self._waitAnyExecutor()
                #self._graduate(token, code)
            else:
                # not busy + jobs to run, so 'make it so'
                cmd = heappop(self.ready)
                self.dispatch(e, cmd)
            continue # redundant, but safe
        pass # end def dispatchAll
    pass # end class ParallelDispatcher
        
class SwampInterface:

    def __init__(self, config, executor=None):
        self.config = config
        if executor:
            self.executor = executor
        else:
            self.executor = FakeExecutor()

        if config.execSlaveNodes > 0:
            self.remote = []
            for i in range(config.execSlaveNodes):
                s = config.slave[i]
                self.remote.append(RemoteExecutor(s[0], s[1]))
        else:
            self.remote = None
        pass

    def submit(self, script):
        p = Parser()
        sch = Scheduler(self.config, self.executor)
        p.commandHandler(sch.schedule)
        cf = CommandFactory(self.config)
        p.parseScript(script, cf)
        sch.finish()
        sch.executeParallelAll(self.remote)
        task = sch.taskId
        #print len(task)
        return task

        

    def fileStatus(self, logicalname):
        """return state of file by name"""

        # go check db for state
        jp = JobPersistence(self.config.dbFilename)
        poll = jp.newPollingTransaction()
        i = poll.pollFileStateByLogical(logicalname)
        jp.close()
        if i is not None:
            return i
        else:
            return -32768
        pass


    def taskFileStatus(self, taskid):
        state = 0
        logname = "dummy.nc"
        url = "NA"
        # go check db for state
        jp = JobPersistence(self.config.dbFilename)
        poll = jp.newPollingTransaction()
        i = poll.pollFileStateByTaskId(taskid)
        jp.close()
        if i is not None:
            return i
        else:
            return -32768
        pass

        # want to return list of files + state + url if available
        return [(state, logname, url)]

    pass

class FakeExecutor:
    def __init__(self):
        self.running = []
        self.fakeToken = 0
        pass
    def launch(self, cmd):
        cmdLine = cmd.makeCommandLine(lambda x: x, lambda y:y)
        print "fakeran",cmdLine
        self.fakeToken += 1
        self.running.append(self.fakeToken)
        return self.fakeToken
    def join(self, token):
        if token in self.running:
            self.running.remove(token)
            return True  # return False upon try-but-fail
        else:
            raise StandardError("Tried to join non-running job")
    pass

class LocalExecutor:
    def __init__(self, binaryFinder, filemap, slots=1):
        self.binaryFinder = binaryFinder
        self.filemap = filemap
        self.slots = slots
        self.running = {}
        self.finished = {}
        self.cmds = {}
        self.token = 0

        pass
    def busy(self):
        # soon, we should put code here to check for process finishes and
        # cache their results.
        return len(self.running) >= self.slots

    def launch(self, cmd):
        # make sure our inputs are ready
        missing = filter(lambda f: not self.filemap.existsForRead(f),
                         cmd.inputs)
        self._fetchLogicals(missing, cmd.inputSrcs)
        cmdLine = cmd.makeCommandLine(self.filemap.mapReadFile,
                                      self.filemap.mapWriteFile)
        log.debug("-exec-> %s"% " ".join(cmdLine))
        # make sure there's room to write the output
        self.clearFiles(map(lambda t: t[1], cmd.actualOutputs))
        pid = os.spawnv(os.P_NOWAIT, self.binaryFinder(cmd), cmdLine)
        log.debug("child pid: "+str(pid))
        self.token += 1
        self.running[self.token] = pid
        self.cmds[self.token] = cmd
        return self.token

    def poll(self, token):
        if token in self.finished:
            return self.finished[token]
        if token in self.running:
            pid = self.running[token]
            (pid2, status) = os.waitpid(pid,os.WNOHANG)
            
            if (pid2 != 0) and os.WIFEXITED(status):
                log.debug("poll pid %d (%d) -> %d"%(pid, pid2, status))
                code = os.WEXITSTATUS(status)
                self.finished[token] = code
                self.running.pop(token)
                return code
            else:
                return None
        else:
            raise StandardError("Tried to poll non-running job")

    def join(self, token):
        if token in self.running:
            pid = self.running.pop(token)
            (pid, status) = os.waitpid(pid,0)
            
            log.debug("got "+str(pid)+" " +str(status)+"after spawning")
            if os.WIFEXITED(status):
                status = os.WEXITSTATUS(status)
            else:
                status = -1
            self.finished[token] = status
            return status
        else:
            raise StandardError("Tried to join non-running job")
    def actualOuts(self, token):
        return self.cmds[token].actualOutputs

    def clearFiles(self, filelist):
        for f in filelist:
            if os.access(f, os.F_OK):
                if os.access(f, os.W_OK):
                    os.remove(fname)
                else:
                    raise StandardError("Tried to unlink read-only %s"
                                        % (fname))
            pass
        pass

    def _fetchLogicals(self, logicals, srcs):
        log.info("srcs: " + str(srcs))
        if len(logicals) == 0:
            return
        log.info("need fetch for %s from %s" %(str(logicals),str(srcs)))
        d = dict(srcs)
        for lf in logicals:
            phy = self.filemap.mapBulkFile(lf)
            log.info("fetching %s from %s" % (lf, d[lf]))
            rf = urllib.urlopen(d[lf])
            target = open(phy, "wb")
            shutil.copyfileobj(rf, target)
            target.close()
        pass
    
    pass # end class LocalExecutor

class RemoteExecutor:
    def __init__(self, url, slots):
        """ url: SOAP url for SWAMP slave
            slots: max number of running slots"""
        self.url = url
        self.slots = slots
        self.rpc = SOAPProxy(url)
        log.debug("reset slave at %s with %d slots" %(url,slots))
        self.rpc.reset()
        self.running = {}
        self.finished = {}
        self.token = 0
        self.sleepTime = 0.2
        self.actual = {}
        pass

    def busy(self):
        # soon, we should put code here to check for process finishes and
        # cache their results.
        return len(self.running) >= self.slots
    
    def launch(self, cmd, locations=[]):
        cmd.inputSrcs = locations 
        remoteToken = self.rpc.slaveExec(cmd.pickleNoRef())
        self.token += 1        
        self.running[self.token] = remoteToken
        return self.token

    def discard(self, token):
        assert token in self.finished
        self.finished.pop(token)
        # consider releasing files too.

    def discardFile(self, file):
        log.debug("req discard of %s on %s" %(file, self.url))
        self.actual.pop(file)
        self.rpc.discardFile(file)

    def pollAny(self):
        for (token, rToken) in self.running.items():
            state = self._pollRemote(rToken)
            if state is not None:
                self._graduate(token, state)
                return (token, state)
        return None

    def waitAny(self):
        """wait for something to happen. better be something running,
        otherwise you'll wait forever."""
        while True:
            r = self.pollAny()
            if r is not None:
                return r
            time.sleep(self.sleepTime)
        pass

    def poll(self, token):
        if token in self.finished:
            return self.finished[token]
        elif token in self.running:
            state = self._pollRemote(self.running[token])
            if state is not None:
                self._graduate(token, state)
        else:
            raise StandardError("RemoteExecutor.poll: bad token")

    def _pollRemote(self, remoteToken):
        state = self.rpc.pollState(remoteToken)
        if state is not None:
            if state != 0:
                log.error("slave %s error while executing" % self.url)
        return state # always return, whether None, 0 or nonzero

    def _addFinishOutput(self, logical, actual):
        self.actual[logical] = actual

    def _graduate(self, token, retcode):
        self.running.pop(token)
        self.finished[token] = retcode
        outputs = self.rpc.pollOutputs(token)
        log.debug("adding " + str(outputs))
        for x in outputs:
            self._addFinishOutput(x[0],x[1])
        log.debug("new actual: " + str(self.actual))

    def _waitForFinish(self, token):
        """helper function"""
        remoteToken = self.running[token]
        while True:
            state = self._pollRemote(remoteToken)
            if state is not None:
                return state
            time.sleep(self.sleepTime) # sleep for a while.  need to tune this.
        pass
        
    def join(self, token):
        if token in self.running:
            ret = self._waitForFinish(token)
            self._graduate(token, ret)
            return ret
        elif token in self.finished:
            return self.finished[token]
        else:
            raise StandardError("RemotExecutor.join: bad token")
    pass # end class RemoteExecutor 


class FileMapper:
    def __init__(self, name, readParent, writeParent, bulkParent):
        # we assume that logical aliases are eliminated at this point.
        self.physical = {} # map logical to physical
        self.logical = {} # map physical to logical
        self.readPrefix = readParent + os.sep 
        self.writePrefix = writeParent + os.sep + name + "_"
        self.bulkPrefix = bulkParent + os.sep + name + "_"
        pass

    def clean():
        pass
    def existsForRead(self, f):
        return os.access(self.mapReadFile(f), os.R_OK)

    def mapReadFile(self, f):
        if f in self.physical:
            return self.physical[f]
        else:
            return self.readPrefix + f
        return self.prefix + f

    def mapWriteFile(self, f, altPrefix=None):
        if altPrefix is not None:
            pf = altPrefix + f
        else:
            pf = self.writePrefix + f
        self.logical[pf] = f
        self.physical[f] = pf
        return pf

    def mapBulkFile(self, f):
        return self.mapWriteFile(f, self.bulkPrefix)

    def _cleanPhysical(self, p):
        try:
            if os.access(p, os.F_OK):
                os.unlink(p)
            f = self.logical.pop(p)
            self.physical.pop(f)
        except IOError:
            pass

    def discardLogical(self, f):
        p = self.physical.pop(f)
        if os.access(p, os.F_OK):
            os.unlink(p)
            log.debug("Unlink OK: %s" %(f))
        self.logical.pop(p)
    
    def cleanPhysicals(self):
        physicals = self.logical.keys()
        map(self._cleanPhysical, physicals)
        
    
######################################################################
######################################################################
testScript4 = """!/usr/bin/env bash
ncrcat -O camsom1pdf/camsom1pdf.cam2.h1.0001*.nc camsom1pdf_00010101_00011231.nc # pack the year into a single series
 ncap -O -s "loctime[time,lon]=float(0.001+time+(lon/360.0))" -s "mask2[time,lon]=byte(ceil(0.006000-abs(loctime-floor(loctime)-0.25)))" camsom1pdf_00010101_00011231.nc yrm_0001am.nc
 ncwa -O --rdd -v WINDSPD -a time -d time,0,143 -B "mask2 == 1" yrm_0001am.nc yr_0001d0_am.nc.deleteme
ncap -O -h -s "time=0.25+floor(time)" yr_0001d0_am.nc.deleteme yr_0001d0_am.nc

ncwa -O --rdd -v WINDSPD -a time -d time,144,287 -B "mask2 == 1" yrm_0001am.nc yr_0001d1_am.nc.deleteme
ncap -O -h -s "time=0.25+floor(time)" yr_0001d1_am.nc.deleteme yr_0001d1_am.nc

ncwa -O --rdd -v WINDSPD -a time -d time,288,431 -B "mask2 == 1" yrm_0001am.nc yr_0001d2_am.nc.deleteme
ncap -O -h -s "time=0.25+floor(time)" yr_0001d2_am.nc.deleteme yr_0001d2_am.nc

ncwa -O --rdd -v WINDSPD -a time -d time,432,575 -B "mask2 == 1" yrm_0001am.nc yr_0001d3_am.nc.deleteme
ncap -O -h -s "time=0.25+floor(time)" yr_0001d3_am.nc.deleteme yr_0001d3_am.nc

ncwa -O --rdd -v WINDSPD -a time -d time,576,719 -B "mask2 == 1" yrm_0001am.nc yr_0001d4_am.nc.deleteme
ncap -O -h -s "time=0.25+floor(time)" yr_0001d4_am.nc.deleteme yr_0001d4_am.nc

ncwa -O --rdd -v WINDSPD -a time -d time,720,863 -B "mask2 == 1" yrm_0001am.nc yr_0001d5_am.nc.deleteme
ncap -O -h -s "time=0.25+floor(time)" yr_0001d5_am.nc.deleteme yr_0001d5_am.nc

ncwa -O --rdd -v WINDSPD -a time -d time,864,1007 -B "mask2 == 1" yrm_0001am.nc yr_0001d6_am.nc.deleteme

"""

        
def testParser():
    test1 = """#!/usr/local/bin/bash
# Analysis script for CAM/CLM output.
#export CASEID1=camsomBC_1998d11
ncwa in.nc out.nc
CASEID1=$1
DATA=nothing
export ANLDIR=${DATA}/anl_${CASEID1}
export STB_YR=00
export FRST_YR=0000
export LAST_YR=0014

export MDL=clm2
let Y1=$FRST_YR+1
let Y9=1
let Y9=1+1

mkdir -p ${ANLDIR} #inline comment
mkdir -p ${DATA}/${CASEID1}/tmp

# Move /tmp files into original directories (strictly precautionary):
mv ${DATA}/${CASEID1}/tmp/* ${DATA}/${CASEID1}/

# Move data from year 0 out of the way
mv ${DATA}/${CASEID1}/${CASEID1}.${MDL}.h0.${FRST_YR}-??.nc ${DATA}/${CASEID1}/tmp/



# STEP 1: Create ensemble annual and seasonal means

# Bring in December of year 0 for seasonal mean
mv ${DATA}/${CASEID1}/tmp/${CASEID1}.${MDL}.h0.${FRST_YR}-12.nc ${DATA}/${CASEID1}/

for yr in `seq $Y1 $LAST_YR`; do
    YY=`printf "%04d" ${yr}`
    let yrm=yr-1
    YM=`printf "%04d" ${yrm}`
    ncrcat -O ${DATA}/${CASEID1}/${CASEID1}.${MDL}.h0.${YM}-12.nc ${DATA}/${CASEID1}/${CASEID1}.${MDL}.h0.${YY}-??.nc ${ANLDIR}/foo2.nc

"""
    logging.basicConfig(level=logging.DEBUG,
                        format='%(asctime)s %(levelname)s %(message)s',
                        datefmt='%d%b%Y %H:%M:%S')
#                    filename='/tmp/myapp.log',
#                    filemode='w')
    p = Parser()
    p.parseScript(test1, None)

def testParser2():
    logging.basicConfig(level=logging.DEBUG,
                        format='%(asctime)s %(levelname)s %(message)s',
                        datefmt='%d%b%Y %H:%M:%S')

    p = Parser()
    portionlist = open("full_resamp.swamp").readlines()[:10]
    portion = "".join(portionlist)
    cf = CommandFactory(Config.dummyConfig())
    p.parseScript(portion, cf)

def testParser3():
    logging.basicConfig(level=logging.DEBUG,
                        format='%(asctime)s %(levelname)s %(message)s',
                        datefmt='%d%b%Y %H:%M:%S')

    p = Parser()
    portionlist = ["ncwa in.nc temp.nc", "ncwa temp.nc temp.nc",
                   "ncwa temp.nc out.nc"]
    portion = "\n".join(portionlist)
    cf = CommandFactory(Config.dummyConfig())
    p.parseScript(portion, cf)
 

def testSwampInterface():
    logging.basicConfig(level=logging.DEBUG)
    wholelist = open("full_resamp.swamp").readlines()
    portionlist = wholelist[:10]
    test = [ "".join(portionlist),
             "".join(wholelist),
             testScript4]

    c = Config("swamp.conf")
    c.read()
    log.info("after configread at " + time.ctime())
    fe = FakeExecutor()
    le = LocalExecutor(NcoBinaryFinder(c),
                       FileMapper("swampTest%d"%os.getpid(),
                                  c.execSourcePath,
                                  c.execScratchPath,
                                  c.execBulkPath))
    
    #si = SwampInterface(fe)
    si = SwampInterface(c, le)

    #evilly force the interface to use a remote executor
    assert len(si.remote) > 0
    si.executor = si.remote[0]
    taskid = si.submit(test[2])
    log.info("finish at " + time.ctime())
    print "submitted with taskid=", taskid
def main():
    #testParser3()
    #testExpand()
    testSwampInterface()

if __name__ == '__main__':
    main()

#timing results:
#
# case 1: 1.2Ghz Pentium M laptop, 512MB,
# complete full_resamp parse/submit w/command-line building
# swamp_common.py,v 1.6
#
# real    2m54.891s
# user    2m52.979s
# sys     0m1.724s
