
# $Header: /data/zender/nco_20150216/nco/src/ssdap/swamp_common.py,v 1.31 2007-07-09 22:58:37 wangd Exp $
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
import fnmatch
import glob
from heapq import * # for minheap implementation
import itertools
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
import threading
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

def appendListMulKey(aDict, keys, val):
    for k in keys:
        l = aDict.get(k,[])
        l.append(val)
        aDict[k] = l

def isRemote(filepath):
    return filepath.startswith("http://")

class VariableParser:
    """treats shell vars "a=b" and env vars "export a=b" as the same for now.

    Bourne shell grammar at:
    The Open Group Base Specifications Issue 6
    IEEE Std 1003.1, 2004 Edition
    http://www.opengroup.org/onlinepubs/009695399/toc.htm
    http://www.opengroup.org/onlinepubs/007904975/toc.htm
    """
    ident = "[A-Za-z]\w*"
    quoteclass = "[\"\']"
    identifier = Word(alphas, alphanums+"_")
    unpReference = Literal("$").suppress() + identifier
    protReference = Literal("${").suppress() + identifier + Literal("}").suppress()
    sReference = protReference | unpReference
    # should support double-quoted references
    reference = sReference
    backtickString = Literal("`") + CharsNotIn("`") + Literal("`")
    
    def __init__(self, varmap):
        self.varMap = varmap ## varMap is exposed/public.
        reference = VariableParser.reference.copy()
        # lookup each substitution reference in the dictionary
        reference.setParseAction(lambda s,loc,toks: [self.varMap[t] for t in toks])
        dblQuoted = dblQuotedString.copy()
        dblQuoted.setParseAction(lambda s,loc,toks: [self.varSub(t) for t in toks])
        
        sglQuoted = sglQuotedString.copy()
        sglQuoted.setParseAction(lambda s,loc,toks: [t.strip("'") for t in toks])
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
            return
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
    pass #End of class VariableParser

def testExpand():
    """testExpand() is a convenience tester for the VariableParser class"""
    vp = VariableParser({})
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
            print ":( no factory."
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

class LoopParser:
    """LoopParser is a parser module to use with the main parser.
    Handling loops require handling some state, so we need to plug
    an *instance* into the Parser, and not the static class, as
    with NcoParser.
    """
    def __init__(self):
        pass

    def parse(self, original, argv, lineNumber=0, factory=None):
        pass

    def accepts(self, argv):
        if len(argv) < 1:
            return False
        else:
            return (argv[0] in NcoParser.commands)    
        

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
        #self.cmdLine = filter(lambda x: x != '', cmdLine)
        #return self.cmdLine

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
        temps = fnmatch.filter(self.scriptOuts, scriptFilename)
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
        # WARN: this may not be threadsafe! 
        save = os.getcwd()
        os.chdir(self.config.execSourcePath)
        # no absolute paths in input filename!
        s = inputFilename.lstrip("/")
        res = glob.glob(s)
        print "glob: ", res, s, self.config.execSourcePath
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
            return m.group(1) + str(1 + int(m.group(2)))
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

    def realOuts(self):
        """return tuples of (scriptname, logname)
        for each file that is a final output """
        # technically, we should ask just for its keys,
        #but random access to a dict is faster.
        temps =  self.commandByLogicalIn

        if False:
            # original
            # get the final set of logical outs
            realouts = filter(lambda x: x not in temps,
                              self.commandByLogicalOut.keys())
            # then, take the final set and then find the script names
            scriptByLogical = dict([[v,k] for k,v
                                    in self.logicalOutByScript.items()])
            # ? 
        else: # new method:
            # Get the script outs, and then map them to logical outs.
            rawscriptouts = self.logicalOutByScript.items()
            scriptAndLogicalOuts = filter(lambda x: x[1] not in temps,
                              rawscriptouts)
            

        return scriptAndLogicalOuts

    pass # end of CommandFactory class




class Parser:
    """Parse a SWAMP script for NCO(for now) commands"""

    # Try to keep parse state SMALL!  Don't track too much for parsing.
    # Only a few things will cause us to keep state across multiple lines:
    # - \ continuation
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

    loopStart = Parser.LoopContext.Expr.ForHeading
    ifStart = Parser.BranchContext.Expr.IfHeading
    ourFlowParsers = [(loopStart.parseString, Parser.LoopContext),
                      (ifStart.parseString, Parser.BranchContext)]
    
    def __init__(self):
        self.handlers = {} # a lookup table for parse accept handlers
        self.variables = {} # a lookup table for variables
        self.lineNum=0
        self.modules = []
        self.handlerDefaults()
        self.handleFunc = None
        self.commands = []
        # FIXME: should be able to push variableparser into the context.
        self.variableParser = VariableParser(self.variables) 
        self._context = Parser.BaseContext(self)
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

    def stripComments(self, line):
        original = line.strip()
        comment = None
        # for now, do not detect sh dialect 
        linesp = original.split("#", 1) # chop up comments
        
        if len(linesp) > 1:
            line, comment = linesp
        else:
            line = original
            
        if (len(line) < 1): # skip comment-only line
            return ""
        return line
                
        
    def parseScriptLine2(self, line):
        """factory should probably *not* change between calls-- it
        doesn't seem to make sense to do so.  FIXME: make more sense
        of this interface.

        No return value.  To see what commands got constructed and
        handled, wrap self.handleFunc .
        """
        self.lineNum += 1
        line = self.stripComments(line)
        #Try 1: match context-creating constructs
        
        #Try 2: pass to current context.
        
        (nextContext, result) = self._context.acceptStatefully(line)
        if result is 3:
            logging.debug("reject: " + line)
        self._context = nextContext
        return nextContext
        pass
    
    def parseScriptLine(self, factory, line):
        """parse a single script line"""
        # for now, force to test alternate parsing
        return self.parseScriptLine2(line)
        def accept(obj, argv):
            for mod in obj.modules:
                if mod[0].accepts(argv):
                    cmd = mod[0].parse(line, argv, obj.lineNum, factory)
                    return cmd
            return False
        self.lineNum += 1
        line = self.stripComments(line)
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
        self._factory = factory
        self._context.setFactory(factory)
        lineCount = 0
        for line in script.splitlines():
            self.parseScriptLine(factory, line)
            lineCount += 1
            if (lineCount % 500) == 0:
                log.debug("%d lines parsed" % (lineCount))
   
        #log.debug("factory cmd_By_log_in: " + str(factory.commandByLogicalIn))
        #log.debug("factory uselist " + str(factory.scrFileUseCount))
        
        pass

    ## a few helper classes for Parser.
    class BaseContext:
        loopStart = Parser.LoopContext.Expr.ForHeading
        ifStart = Parser.BranchContext.Expr.IfHeading
        ourFlowParsers = [(loopStart.parseString, Parser.LoopContext),
                          (ifStart.parseString, Parser.BranchContext)]
        
        def __init__(self, parser):
            """handler is a function that accepts a Command instance
            as input (it's probably Scheduler.schedule)."""
            self.canPop = False
            # _parser gets exposed to subclasses, because they need
            # to invoke the the variable parser.  *sigh*
            self._parser = parser 
            self._factory = None

            pass

        def setFactory(self, f):
            self._factory = f
            pass

        @staticmethod
        def extParse(context, line):
            for (test, newContext) in BaseContext.ourFlowParsers:
                try:
                    parseResult = test(line)
                except ParseException, e:
                    continue # didn't find a looping expression.
                nextContext = newContext(context, parseResult)
                return nextContext
            return None
                
        def stdAccept(self, argv):
            for mod in self._parser.modules:
                if mod[0].accepts(argv):
                    cmd = mod[0].parse(line, argv,
                                       self._parser.lineNum, self._factory)
                    return cmd
            print "stdAccept reject", argv
            return False

        def stdParse(self, line):
            argv = shlex.split(line)
            command = self.stdAccept(argv)
            if isinstance(command, types.InstanceType):
                print "stdParse, insert", command
                command.referenceLineNum = self._parser.lineNum
                #command.original = original
                command.original = line
                print "not saving original, please fix"
                self._parser.handleFunc(command)
            return (self, True)

        def acceptSubParse(self, line):
            print "subparse",line
            line = self._parser.variableParser.apply(line)
            nextCon = self.extParse(line)
            if nextCon != None:
                return (nextCon, True)
                
            if not isinstance(line, str):
                return (self, True)

            return self.stdParse(line)
        
        def acceptStatefully(self, line):

            line = self._parser.variableParser.apply(line)
            print "line after", line
            if line == None: # already parsed line.
                return (self, True)
            return self.acceptSubParse(line)

        def variableMap(self):
            #possible to add a wrapper parser  here.
            return self._parser.variables            
        pass

    class BranchContext:
        # basically, the branching context is aware of a branch,
        # and is able to evaluate and take one branch versus another.
        class Expr:
            If = Literal("if")
            Elif = Literal("elif")

            Reference = VariableParser.reference

            PlainString = Word(alphanums + "_,")
            Squoted = Literal("'") + OneOrMore(PlainString) + Literal("'")
            Dquoted = Literal('"').suppress() + OneOrMore(PlainString | Reference) + Literal('"').suppress()
            BinaryOp = MatchFirst([Literal("="), Literal("=="), Literal("!="),
                                  Literal("<"), Literal(">")])
            String = MatchFirst([Dquoted, Squoted, PlainString, Reference])
            Expression = String # for now, just strings
            Boolean = Expression.setResultsName("lhs") + BinaryOp.setResultsName("op") + Expression.setResultsName("rhs")
            Term = Boolean.setResultsName("boolean")
            Condition = Literal("[") + Term + Literal("]")
            
            IfHeading = If + Condition.setResultsName("condition") + Literal(";") + Literal("then")
            ElifHeading = Elif + Condition + Literal(";") + Literal("then")
            ElseHeading = Literal("else")
            IfClosing = Literal("fi")
            pass
        
        def __init__(self, parentContext, parseResult):
            self.canPop = True
            self._parentContext = parentContext
            self._savedLines = []
            self._branchHeading = parseResult
            self._varMap = parentContext.variableMap()
            self._variableParser = parentContext._parser.variableParser
            self._factory = None

            self._state = 1 # startup in state 1 (seen an if statement)
            self._parseFunc = { 1: self._state1Parse,
                                2: self._state2Parse }
            self._taking = self._evaluateCondition(parseResult)
            self._took = False
            print "new branch"
            pass

        def setFactory(self, f):
            self._factory = f
            pass

        def acceptStatefully(self, line):
            """acceptStatefully, in the context of a branch.
            we save a list of the command lines."""
            # should be able to push this logic back to the Parser
            original = line.strip()
            comment = None
            # for now, do not detect magic #! interpreter
            linesp = original.split("#", 1) # chop up comments
        
            if len(linesp) > 1:
                line, comment = linesp
            else:
                line = original
            
            if (len(line) < 1): # skip comment-only line
                return (self, True)
            # apply normal parse.
            nextContext =  self._branchParse(line)
            if nextContext:
                return (nextContext, True)
            else:
                raise StandardError("Error while parsing if-else block")
            pass

        def _state1Parse(self, line):
            # In state 1, the last statement we saw was an if or elif,
            # so the only valid keywords are 'else' and 'elif' or 'fi'
            print "state 1",line
            try:
                subline = self._variableParser.apply(line)
                return self._tryElif(subline) 
            except ParseException, e:
                pass
            except KeyError, e:
                pass # passthrough variable lookup errors.
            try:
                return self._tryElse(line)
            except ParseException, e:
                pass
            try:
                return self._tryFi(line)
            except ParseException, e:
                pass
            # now, save it, if we are in the taken body, or discard
            # if it is not taken.
            if self._taking:
                self._savedLines.append(line)
            else:
                pass # don't save it.
            return self # okay to stay in the same context.
        
        def _state2Parse(self, line):
            # In state 2, the last statement seen is 'else' so the only
            # valid keyword is 'fi'
            print "state 2",line
            try:
                assert (self._taking or self._took)
                return self._tryFi(line)
            except ParseException, e:
                pass
            # now, save it, if we are in the taken body, or discard
            # if it is not taken.
            if self._taking:
                self._savedLines.append(line)
            else:
                pass # don't save it.
            return self

        def _tryElif(self, line):
            check = self.Expr.ElifHeading.parseString(line)
            print "elif parsing", check
            if not (self._taking or self._took):
                self._taking = self._evaluateCondition(check)
            else: # stop taking...
                self._taking = False
                self._took = True                
            return self

        def _tryElse(self, line):
            check = self.Expr.ElseHeading.parseString(line)
            if not (self._taking or self._took):
                self._taking = True
            else:
                self._taking = False
                self._took = True
            return self


        def _tryFi(self, line):
            check = self.Expr.IfClosing.parseString(line)
            parent = self._parentContext #save my parent.
            print "supposed to dump to", parent, "lines now", self._savedLines
            for l in self._savedLines:
                n = BaseContext.extParse(self, l)
                if n:
                    context = n
                    parent = parent.acceptSubParse(l)
                print parent
            assert parent is self._parentContext # should return from any nesting.

            print "subdump results", rlist
            return self._parentContext


        def _branchParse(self, line):
            # FSM:
            # 0: Nothing (not in this context)
            #  -> 1: receive if[] ; then
            #  -> 0: anything else
            # 1: In "taken" body
            #  -> 1: receive elif [] ; then
            #  -> 3: receive fi
            #  -> 2: receive else 
            # 2: In "not taken" body
            #  -> 3: receive fi
            #  -> 2: anything else
            # 3: (pseudostate)Resolution: evalutate branch condition
            #  -> 0: return to parent context.
            return self._parseFunc[self._state](line)
            pass
            
        def _evaluateCondition(self, condition):
            parsed = condition
            boolean = parsed["boolean"]
            rhs = boolean["rhs"]
            op = parsed["op"]
            lhs = parsed["lhs"]
            rhs = parsed["rhs"]
            resolver = {"=" :  lambda l,r: l == r,
                        "==" : lambda l,r: l == r,
                        "!=" : lambda l,r: l != r,
                        "<" :  lambda l,r: l < r,
                        ">" :  lambda l,r: l > r,
                        }
            print "resolved", lhs, op, rhs, resolver[op](lhs,rhs)
            self._ifResult = resolver[op](lhs,rhs)
            return self._ifResult
            # FIXME: Figure out what sort of errors are possible here.
            pass
            
        def variableMap(self):
            return self._varMap

        pass
        
    
    class LoopContext:
        # basically, a loopcontext is aware of loops, so that when the 'end loop' token is parsed, the loop body is replicated for each value of the looping variable.  Is this (having a class) the right way to do it?  Are we just a glorified stack?  I think more correctly, loopcontext functions as the parse node for a loop.  
        class Expr: # move this to the context
            For = Literal('for')
            In = Literal('in')
            Do = Literal('do')
            Done = Literal('done')
            Semicolon = Literal(';')
            Identifier = VariableParser.identifier.copy()
            Identifier = Identifier.setResultsName("identifier")
            Value = Word(alphanums).setResultsName("realValue")
            CommandLine = OneOrMore(Word(alphanums + '_-'))
            BacktickExpr = Literal('`') + CommandLine + Literal('`')
            SubValue = BacktickExpr.copy().setResultsName("indValue")
            
            Range = OneOrMore(Value ^ SubValue).setResultsName("range")
            ForHeading = For + Identifier + In + Range + Semicolon + Do
            ForHeading2_1 = For + Identifier + In + Range
            ForHeading2_2 = Do
            ForClosing = Done
            pass
            
        def __init__(self, parentContext, parseResult):
            self.canPop = True
            self._parentContext = parentContext
            self._savedLines = []
            self._loopHeading = parseResult
            self._varMap = parentContext.variableMap()
            self._factory = None
            print "new loop"
            pass

        def setFactory(self, f):
            self._factory = f
            pass

        def acceptStatefully(self, line):
            """acceptStatefully, in the context of a loop.
            When in a loop context, variable substitutions and command
            generations are deferred until the loop closure.  That way,
            the entire loop can be unrolled at once.  So until the closing
            statement, we save a list of the command lines."""
            print "incontext accept ", line
            # should be able to push this logic back to the Parser
            original = line.strip()
            comment = None
            # for now, do not detect magic #! interpreter
            linesp = original.split("#", 1) # chop up comments
        
            if len(linesp) > 1:
                line, comment = linesp
            else:
                line = original
            
            if (len(line) < 1): # skip comment-only line
                return (self, True)
            # check if it's the end:
            check = self._loopParse(line)
            if check:
                #unroll Loop
                self._unroll()
                return (self._parentContext, True)
            else:
                # if not the end, save the line
                self._savedLines.append(line)
                return (self, True)

        def _unroll(self):
            #print "loopvar:", self._loopHeading["identifier"]
            #print "range:", self._loopHeading["range"]
            # At this point, we may want to eval/expand
            # subexpressions within the range.
            varname = self._loopHeading["identifier"]
            for val in self._loopHeading["range"]:
                self._varMap[varname] = val
                self._iterateLoop()
            return

        def _iterateLoop(self):
            for l in self._savedLines:
                r = self._parentContext.acceptSubParse(l)

        def _loopParse(self, line):
            try:
                check = self.Expr.ForClosing.parseString(line)
                return "End"
            except ParseException, e:
                return None
            pass
            
            
        def variableMap(self):
            return self._varMap


        pass
    
    pass

class Scheduler:
    def __init__(self, config, executor=None, graduateHook=lambda x: None):
        self.config = config
        self.transaction = None
        self.env = {}
        self.taskId = self.makeTaskId()
        self.executor = executor
        self.cmdList = []
        self.fileLocations = {}
        self._graduateHook = graduateHook
        self.result = None # result is True on success, or a string/other-non-boolean on failure.
        
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
        if self.transaction != None:
            self.transaction.finish()
        pass

    def graduateHook(self, hook):
        """hook is a unary void function that accepts a graduating command
        as a parameter.  Invocation occurs sometime after the command
        commits its output file to disk."""
        self._graduateHook = hook
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
                self.result = "ret was %s, error running %s" %(str(ret), c)
                break
    def executeParallelAll(self, executors=None):
        if executors is None:
            executors = [self.executor]
        self.pd = ParallelDispatcher(self.config, executors)
        self.fileLocations = self.pd.dispatchAll(self.cmdList,
                                                 self._graduateHook)
        self.result = self.pd.result
        pass
    pass # end of class Scheduler

class EarlyExecutor(threading.Thread):
    def __init__(self, executor, cmdlist, stopFunc):
        Thread.__init__(self) 
        self.executor = executor
        self.cmdList = cmdlist
        self.stopFunc = stopFunc
        pass
    def run(self):
        while not self.stopFunc():
            tok = self.executor.launch(cmd)
            retcode = self.executor.join(tok)
        # NOT FINISHED
            

class ParallelDispatcher:
    def __init__(self, config, executorList):
        self.config = config
        self.executors = executorList
        self.finished = {}
        self.okayToReap = True
        # not okay to declare file death until everything is parsed.
        self.execLocation = {} # logicalout -> executor
        self.sleepTime = 0.100 # originally set at 0.200
        self.running = {} # (e,etoken) -> cmd
        self.execPollRR = None
        self.execDispatchRR = None
        self.result = None
        pass

    def fileLoc(self, file):
        """ return url of logical output file"""
        execs = self.execLocation[file]
        if len(execs) > 0:
            try:
                return execs[0].actual[file] # return the first one
            except KeyError:
                i = 0
                emap = {}
                msg = ["#Key error for %s" % (file),
                       "badkey = '%s'" %(file)]
                for e in self.executors:
                    nicename = "e%dactual" % i
                    emap[e] = nicename
                    msg.append("%s = %s" %(nicename, str(e.actual)))
                    i += 1
                msg.append("#Key %s maps to %s" %(file,
                                                 map(lambda e: emap[e],
                                                     self.execLocation[file])))
                locstr = str(self.execLocation)
                for (k,v) in emap.items():
                    locstr = locstr.replace(str(k),v)
                msg.append("eLoc = " + locstr) 
                open("mykeyerror.py","w").write("\n".join(msg))
                log.error("writing debug file to mykeyerror.py")
                execs[0].actual[file] # re-raise error.
        else:
            return None
        
    def isDead(self, file, consumingCmd):
        # Find the producing cmd, see if all its children are finished.
        return 0 == len(filter(lambda c: c not in self.finished,
                               consumingCmd.factory.commandByLogicalIn[file]))

    def isReady(self, cmd):
        # if there are no unfinished parents...
        return 0 == len(filter(lambda c: c not in self.finished, cmd.parents))

    def _allBusy(self):
        for e in self.executors:
            if not e.busy():
                return False
        return True
        

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
    
    def dispatch(self, cmd):
        log.info("dispatching %s %d" %(cmd.cmd, cmd.referenceLineNum))
        inputLocs = map(lambda f:(f, self.fileLoc(f)),
                        cmd.inputsWithParents)
        # look for a more appropriate executor:
        # pick host of first file if not busy.
        execs = map(lambda f:self.execLocation[f], cmd.inputsWithParents)
        executor = None
        for e in execs:
            if not e[0].busy():
                executor = e[0]
                break
        if not executor:
            executor = self._nextFreeExecutor()
        etoken = executor.launch(cmd, inputLocs)
        self.running[(executor, etoken)] = cmd

    def releaseFiles(self, files):
        if len(files) == 0:
            return
        log.debug("ready to delete " + str(files))
        # collect by executors

        if False: # old way deletes files by where they were produced
            eList = {}
            map(lambda f: appendListMulKey(eList, self.execLocation[f],f),
                files)
            for (k,v) in eList.items():
                log.debug("Discard: "+str(k)+"  :  "+str(v))
                k.discardFiles(v)
        else: # new way looks at where files reside (may have been downloaded)
            map(lambda e: e.discardFilesIfHosted(files), self.executors)
            
            
        # cleanup execLocation
        map(self.execLocation.pop, files)
#        map(lambda f: map(lambda e: e.discardFile(f),
#                          self.execLocation[f]),
#            files)
        pass


    def _graduate(self, token, code, hook):
        cmd = self.running.pop(token)
        if code != 0:
            origline = ' '.join([cmd.cmd] + map(lambda t: ' '.join(t), cmd.argList) + cmd.leftover)
            s = "Bad return code %s from cmdline %s %d outs=%s" % (
                code, origline, cmd.referenceLineNum, str(cmd.outputs))
            log.error(s)
            # For nicer handling, we should find the original command line
            # and pass it back as the failing line (+ line number)
            # It would be nice to trap the stderr for that command, but that
            # can be done later, since it's a different set of pipes
            # to connect.

            self.result = "Error at line %d : %s" %(cmd.referenceLineNum,
                                                    origline)
            return
            #raise StandardError(s)
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
            map(lambda o: appendList(self.execLocation, o, e), cmd.outputs)
            execs = self.execLocation[cmd.outputs[0]]

            #self._publishFiles(filter(lambda o: self.isOutput(o),
            #                         cmd.outputs))
            execs[0].actual[cmd.outputs[0]] # FIXME: can I delete this?
            # hardcoded for now.

            # call this to migrate files to final resting places.
            # Use a generic hook passed from above.
            hook(cmd)
            pass

    def _pollAny(self, hook):
        for i in range(len(self.executors)):
            e = self.execPollRR.next()
            # should be faster to iterate over self.executors
            # than self.running
            r = e.pollAny()
            if r is not None:
                log.debug("dispatcher pollany ok: "+str(r))
                self._graduate((e, r[0]), r[1], hook)
                return ((e, r[0]), r[1])
        return None

    def _waitAnyExecutor(self, hook):
        while True:
            r = self._pollAny(hook)
            if r is not None:
                return r
            time.sleep(self.sleepTime)

        
    def _nextFreeExecutor(self):
        """Return next free executor. Try to pursue a policy of
        packing nodes tight so as to maximize locality.
        We'll go fancier some other time."""
        for ei in self.executors:
            e = self.execDispatchRR.next()
            if not e.busy():
                return e
        return None

    def dispatchAll(self, cmdList, hook=lambda f:None):
        log.debug("dispatching cmdlist="+str(cmdList))
        self.running = {} # token -> cmd
        self.execPollRR = itertools.cycle(self.executors)
        self.execDispatchRR = itertools.cycle(self.executors)
        (self.ready, self.queued) = self.extractReady(cmdList)

        # Consider 'processor affinity' to reduce data migration.
        while True:
            # if there are free slots in an executor, run what's ready
            if self._allBusy() or not self.ready:
                if not self.running: # done!
                    break
                log.debug("waiting")
                r = self._waitAnyExecutor(hook)
                log.debug("wakeup! %s" %(str(r)))
                if r[1] != 0:
                    # let self.result bubble up.
                    return
                #self._graduate(token, code)
            else:
                # not busy + jobs to run, so 'make it so'
                cmd = heappop(self.ready)
                self.dispatch(cmd)
            continue # redundant, but safe
        # If we got here, we're finished.
        self.result = True
        pass # end def dispatchAll

    def earlyStart(self, cmdlist):
        # NOT FINISHED
        pass
    pass # end class ParallelDispatcher

class SwampTask:
    def __init__(self, defExec, remote, config, script, outMap):
        self.config = config
        self.parser = Parser()
        #FIXME: need to define publishIfOutput
        self.scheduler = Scheduler(config, defExec, self._publishIfOutput)
        self.parser.commandHandler(self.scheduler.schedule)
        self.commandFactory = CommandFactory(self.config)
        self.fail = None
        try:
            self.parser.parseScript(script, self.commandFactory)
            self.scheduler.finish()
            self.remoteExec = remote
            self.scrAndLogOuts = self.commandFactory.realOuts()
            self.logOuts = map(lambda x: x[1], self.scrAndLogOuts)
            log.debug("outs are " + str(self.scrAndLogOuts))
            self.outMap = LinkedMap(outMap, self.taskId())
        except StandardError, e:
            self.fail = str((e,e.__doc__, str(e)))
        
        pass
    
    def _publishIfOutput(self, obj):
        """object can be either a logical filename or a command,
        or a list of either"""
        if isinstance(obj, Command):
            actfiles = obj.actualOutputs
        else:
            #don't know how to publish.
            pass
        log.debug("raw outs are %s" %(str(actfiles)))
        files = filter(lambda f: f[0] in self.logOuts, actfiles)
        log.debug("filtered is %s" %(str(files)))
        targetfiles = map(lambda ft: (ft[0], ft[1],
                                      self.outMap.mapWriteFile(ft[0])),
                          files)        
        # fork a thread for this in the future.
        self._publishHelper(targetfiles)


    def _publishHelper(self, filetuples):
        for t in filetuples:
            log.debug("publish " + str(t))
            actual = t[1]
            target = t[2]
            if isRemote(actual):
                #download, then add to local map (in db?)
                log.debug("Error, not doing actual download right now.")
            else: #it's local!
                # this will break if we request a read on the file
                #
                # remove the file from the old mapping (discard)
                log.debug("start file move")
                shutil.move(actual, target)
                # FIXME: ping the new mapper so that it's aware of the file.
                log.debug("end file move.")
        pass
    def taskId(self):
        return self.scheduler.taskId
    def run(self):
        if not self.result():
            log.debug("Starting parallel dispatcher")
            self.scheduler.executeParallelAll(self.remoteExec)
        else: # refuse to run if we have a failure logged.
            log.debug("refusing to dispatch: " + str(self.result()))
            pass
        pass
    def result(self):
        if self.fail:
            return self.fail
        else:
            return self.scheduler.result

    def selfDestruct(self):
        log.debug("Task %d is self-destructing" %(str(self.scheduler.taskId)))
        
        
class SwampInterface:

    def __init__(self, config, executor=None):
        self.config = config
        cfile = logging.FileHandler(self.config.logLocation)
        formatter = logging.Formatter('%(name)s:%(levelname)s %(message)s')
        cfile.setFormatter(formatter)
        log.addHandler(cfile)
        log.setLevel(self.config.logLevel)
        log.info("Swamp master logging at "+self.config.logLocation)
        self.config.dumpSettings(log, logging.DEBUG)
        
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
        self.tasks = []
        pass

    def submit(self, script, outputMapper):
        t = SwampTask(self.executor, self.remote, self.config,
                      script, outputMapper)
        self.tasks.append(t)
#         p = Parser()
#         sch = Scheduler(self.config, self.executor)
#         p.commandHandler(sch.schedule)
#         cf = CommandFactory(self.config)
#         p.parseScript(script, cf)
#         sch.finish()
        log.info("after parse: " + time.ctime())
        t.run()
#        sch.executeParallelAll(self.remote)
#        task = sch.taskId

        return t

        

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
    def launch(self, cmd, locations = []):
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
        self.token = 10
        self.tokenLock = threading.Lock()
        self.fetchLock = threading.Lock()
        self.fetchFile = None
        self.rFetchedFiles = {}
        self.pollCache = []
        self.actual = {}
        pass

    @staticmethod
    def newInstance(config):
        return LocalExecutor(NcoBinaryFinder(config),
                             FileMapper("swamp%d"%os.getpid(),
                                        config.execSourcePath,
                                        config.execScratchPath,
                                        config.execBulkPath))
    def busy(self):
        # soon, we should put code here to check for process finishes and
        # cache their results.
        return len(self.running) >= self.slots

    def launch(self, cmd, locations=[]):
        self.tokenLock.acquire()
        self.token += 1
        token = self.token
        self.tokenLock.release()
        # make sure our inputs are ready
        missing = filter(lambda f: not self.filemap.existsForRead(f),
                         cmd.inputs)
        fetched = self._fetchLogicals(missing, cmd.inputSrcs)
        self.rFetchedFiles[token] = fetched
        # doublecheck that remaining logicals are available.
        fetched = self._verifyLogicals(set(cmd.inputs).difference(missing))
        self.rFetchedFiles[token] += fetched
        
        cmdLine = cmd.makeCommandLine(self.filemap.mapReadFile,
                                      self.filemap.mapWriteFile)
        log.debug("%d-exec-> %s" % (token," ".join(cmdLine)))
        # make sure there's room to write the output
        self.clearFiles(map(lambda t: t[1], cmd.actualOutputs))
        pid = self.resistErrno513(os.P_NOWAIT,
                                  self.binaryFinder(cmd), cmdLine)
        log.debug("child pid: "+str(pid))

        self.running[token] = pid
        self.cmds[token] = cmd
        return token

    def poll(self, token):
        if self.pollCache:
            #for now, be a little wasteful
            for i in range(len(self.pollCache)):
                if token == t[i][0]:
                    return self.pollCache.pop(t[i][1])
        return self._poll(token)
                    

    def _poll(self, token):
        if token in self.finished:
            return self.finished[token]
        if token in self.running:
            pid = self.running[token]
            (pid2, status) = os.waitpid(pid,os.WNOHANG)
            if (pid2 != 0) and os.WIFEXITED(status):
                code = os.WEXITSTATUS(status)
                self._graduate(token, code)
                return code
            else:
                return None
        else:
            raise StandardError("Tried to poll non-running job")

    def pollAny(self):
        if self.pollCache:
            return self.pollCache.pop()
        f = self.pollAll()
        if f:
            log.debug("pollall ok: "+str(f))
            top = f.pop()
            self.pollCache += f
            return top
        return None

    def pollAll(self):
        fins = []
        for (token, pid) in self.running.items():
            code = self._poll(token)
            if code == 0:
                fins.append((token,code))
            elif isinstance(code, int):
                log.debug("token %s gave: %s, what should I do?" %
                          (str(token),str(code)))
                fins.append((token,code)) # special handling for fail codes?
        if fins:
            return fins
        return None

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

    def fetchedSrcs(self,token):
        return self.rFetchedFiles[token]

    def discardFile(self, file):
        log.debug("req discard of %s on %s" %(file, self.url))
        self.actual.pop(file)
        self.rpc.discardFile(file)

    def discardFilesIfHosted(self, files):
        return self.clearFiles(files)

    def clearFiles(self, filelist):
        for f in filelist:
            if os.access(f, os.F_OK):
                if os.access(f, os.W_OK):
                    os.remove(f)
                else:
                    raise StandardError("Tried to unlink read-only %s"
                                        % (fname))
            pass
        pass

    def _verifyLogicals(self, logicals):
        if len(logicals) == 0:
            return []
        for f in logicals:
            while f == self.fetchFile:
                time.sleep(0.1)
        return []
                
    def _fetchLogicals(self, logicals, srcs):
        fetched = []
        if len(logicals) == 0:
            return []
        log.info("need fetch for %s from %s" %(str(logicals),str(srcs)))
        
        d = dict(srcs)
        for lf in logicals:
            self.fetchLock.acquire()
            if self.filemap.existsForRead(lf):
                self.fetchLock.release()
                log.debug("satisfied by other thread")
                continue
            self.fetchFile = lf
            #phy = self.filemap.mapBulkFile(lf) # 
            phy = self.filemap.mapWriteFile(lf)
            log.debug("fetching %s from %s" % (lf, d[lf]))
            self._fetchPhysical(phy, d[lf])
            fetched.append((lf, phy))
            self.fetchFile = None
            self.fetchLock.release()
        return fetched

    def _fetchPhysical(self, physical, url):
        #urllib.urlretrieve(d[lf], phy)
        # urlretrieve dies on interrupt signals
        # Use curl: fail silently, silence output, write to file
        tries = 1
        maxTries = 3
        pid = None
        while pid is None:
            try:
                pid = os.spawnv(os.P_NOWAIT, '/usr/bin/curl',
                                ['curl', "-f", "-s", "-o", physical, url])
            except OSError, e:
                if not (e.errno == 513):
                    raise
                pass #retry on ERESTARTNOINTR
        rc = None
        while rc is None:
            try:
                (p,rc) = os.waitpid(pid,0)
                rc = os.WEXITSTATUS(rc)
            except OSError, e:
                if not (e.errno == 4):
                    raise
                log.info("Retry, got errno 4 (interrupted syscall)")
                continue
            if rc != 0:
                raise StandardError("error fetching %s (curl code=%d)" %
                                    (url, rc))

    def _graduate(self, token, retcode):
        ## fix this! pulled from remoteexecutor (1/2 finish)
        log.debug("finish token %d with code %d" %(token,retcode))
        self.finished[token] = retcode
        self.running.pop(token)
        
        outputs = self.cmds[token].actualOutputs
        for x in outputs:
            self.actual[x[0]] = x[1]


    def resistErrno513(self, option, binPath, arglist):
        pid = None
        while pid is None:
            try:
                pid = os.spawnv(option, binPath, arglist)
            except OSError, e:
                if not (e.errno == 513):
                    raise
                pass #retry on ERESTARTNOINTR
        return pid
        
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
        self.token = 100
        self.sleepTime = 0.2
        self.actual = {}
        self.pollCache = []
        pass

    def busy(self):
        # soon, we should put code here to check for process finishes and
        # cache their results.
        return len(self.running) >= self.slots
    
    def launch(self, cmd, locations=[]):
        cmd.inputSrcs = locations
        log.debug("launch %s %d to %s" %(cmd.cmd,
                                         cmd.referenceLineNum, self.url))
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

    def discardFilesIfHosted(self, fileList):
        hosted = filter(lambda f: f in self.actual, fileList)
        if hosted:
            return self.discardFiles(hosted)
        

    def discardFiles(self, fileList):
        log.debug("req discard of %s on %s" %(str(fileList), self.url))
        map(self.actual.pop, fileList)
        self.rpc.discardFiles(fileList)

    def pollAny(self):
        if self.pollCache:
            return self.pollCache.pop()
        f = self.pollAll()
        if f:
            top = f.pop()
            self.pollCache += f
            return top
        return None

    def pollAll(self):
        lTokens = []
        rTokens = []
        fins = []
        for (token, rToken) in self.running.items():
            lTokens.append(token)
            rTokens.append(rToken)
        states = self.rpc.pollStateMany(rTokens)
        for i in range(len(lTokens)):
            if states[i] is not None:
                self._graduate(lTokens[i], states[i])
                fins.append((lTokens[i], states[i]))
        if fins:
            return fins
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
        rToken = self.running.pop(token)
        self.finished[token] = retcode
        outputs = self.rpc.actualOuts(rToken)
        log.debug("adding %s from %s" % (str(outputs), str(rToken)))
        for x in outputs:
            self._addFinishOutput(x[0],x[1])

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
    def __init__(self, name, readParent, writeParent, bulkParent,
                 panicSize=3000000000):
        # we assume that logical aliases are eliminated at this point.
        self.physical = {} # map logical to physical
        self.logical = {} # map physical to logical
        self.readPrefix = readParent + os.sep 
        self.writePrefix = writeParent + os.sep + name + "_"
        self.writeParent = writeParent
        self.bulkPrefix = bulkParent + os.sep + name + "_"
        self.bulkParent = bulkParent
        self.panicSize = panicSize
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
        pass

    def spaceLeft(self, fname):
        (head,tail) = os.path.split(fname)
        s = os.statvfs(head)
        space = (s.f_bavail * s.f_bsize)
        #log.info("ok to write %s because %d free" %(fname,space))
        # Enough space left if (available > paniclimit)
        return (s.f_bavail * s.f_bsize) > self.panicSize


    def mapWriteFile(self, f, altPrefix=None):
        if altPrefix is not None:
            pf = altPrefix + f
        else:
            pf = self.writePrefix + f
        if not self.spaceLeft(pf):
            pf = self.bulkPrefix + f
            log.info("mapping %s to bulk at %s" %(f,pf))
           
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

class LinkedMap(FileMapper):
    """LinkedMap is a filemap that is linked to a parent map.
    This class exists for those cases where an additional context
    is needed within the parent map that can be disambiguated.
    """
    def __init__(self, parent, pref):
        self.parent = parent
        self.pref = pref
        self.private = set()
        pass
        
    def existsForRead(self, f):
        if f in self.private:
            return self.parent.existsForRead(self.pref + f)
        else:
            return self.parent.existsForRead(f)
        pass
    
    def mapReadFile(self, f):
        if f in self.private:
            return self.parent.mapReadFile(self.pref + f)
        else:
            return self.parent.mapReadFile(f)
        pass
    
    def spaceLeft(self, fname):
        return self.parent.spaceLeft(fname)
        
    def mapWriteFile(self, f, altPrefix=None):
        self.private.add(f)
        return self.parent.mapWriteFile(self.pref + f, altPrefix)

    def mapBulkFile(self, f):
        self.private.add(f)
        return self.parent.mapBulkFile(self.pref + f)

    def discardLogical(self, f):
        if f in self.private:
            self.private.remove(f)
            return self.parent.discardLogical(self.pref + f)
        else:
            log.debug("tried to discard unmapped file " + f)
        pass

    def cleanPhysicals(self):
        map(lambda f: self.parent.discardLogical(self.pref + f),
            self.private)
        #assume discard success.
        self.private.clear()
        pass



class ActionBag:
    """ActionBag contains Threads.  The bag's containment allows it
        to peform cleanup routines as needed.  Threads are reclaimed
        as soon as they are free-- do not expect thread.local data to
        live after Thread.run terminates."""

    class Thread(threading.Thread):
        """ActionBag.Thread is a wrapper class to wrap up a function
        call so that it can be executed in a non-blocking manner. A
        callback is used to signify completion.
        
        An ActionThread is attached to an ActionBag, """
        def __init__(self, action, callback):
            self.action = action
            self.callback = callback
            pass

        def run(self):
            rv = self.action()
            self.callback(rv)

    def __init__(self):
        self.running = []        
        pass
    
    def executeAction(self, action, callback):
        t = ActionBag.Thread(action, callback)
        t.start()
        self.running.append(t)
        pass
    
    def houseclean(self):
        # iterate through running threads and reclaim if done
        # for now, this means getting rid of dead threads in the list
        self.running = filter(lambda t: t.isAlive(), self.running)
            
    pass



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

ncrcat -O camsom1pdf/camsom1pdf.cam2.h1.0002*.nc camsom1pdf_00020101_00021231.nc # pack the year into a single series
 ncap -O -s "loctime[time,lon]=float(0.001+time+(lon/360.0))" -s "mask2[time,lon]=byte(ceil(0.006000-abs(loctime-floor(loctime)-0.25)))" camsom1pdf_00020101_00021231.nc yrm_0002am.nc
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

profref = None

def testParser2():
    logging.basicConfig(level=logging.DEBUG,
                        format='%(asctime)s %(levelname)s %(message)s',
                        datefmt='%d%b%Y %H:%M:%S')
    wholelist = open("full_resamp.swamp").readlines()
    test = [ "".join(wholelist[:10]),
             "".join(wholelist[:400]),
             "".join(wholelist),
             testScript4]
    
    p = Parser()
    cf = CommandFactory(Config.dummyConfig())
    p.parseScript(test[1], cf)


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
    #logging.basicConfig(level=logging.DEBUG)
    wholelist = open("full_resamp.swamp").readlines()
    test = [ "".join(wholelist[:10]),
             "".join(wholelist[:6000]),
             "".join(wholelist),
             testScript4]

    c = Config("swamp.conf")
    c.read()
    fe = FakeExecutor()
    le = LocalExecutor.newInstance(c)
    #si = SwampInterface(fe)
    si = SwampInterface(c, le)
    log.info("after configread at " + time.ctime())

    #evilly force the interface to use a remote executor
    assert len(si.remote) > 0
    si.executor = si.remote[0]
    taskid = si.submit(test[1])
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
