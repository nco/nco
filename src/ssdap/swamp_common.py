# $Header: /data/zender/nco_20150216/nco/src/ssdap/swamp_common.py,v 1.3 2007-03-16 23:32:48 wangd Exp $
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
import getopt
import logging
import os
import re
import shlex
import types
from pyparsing import *

# SWAMP imports
#

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
        print "try ", fragment
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
            assert isinstance(factory, NcoParser.CommandFactory)
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

    # Command and CommandFactory do not have dependencies on NCO things.
    class Command:
        """this is needed because we want to build a dependency tree."""
        def __init__(self, cmd, argtriple, inouts, referenceLineNum):
            self.cmd = cmd
            self.parsedOpts = argtriple[0] # adict
            self.argList = argtriple[1] # alist
            self.leftover = argtriple[2] # leftover
            self.inputs = inouts[0]
            self.outputs = inouts[1]
            self.referenceLineNum = referenceLineNum
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
        def __init__(self):
            self.commandByLogicalOut = {}
            self.logicalOutByScript = {}
            pass

        def mapInput(self, scriptFilename):
            try:
                return self.logicalOutByScript[scriptFilename]
            except KeyError:
                if self.allowedConcreteInput(scriptFilename):
                    return scriptFilename
                else:
                    logging.error("%s is not allowed as an input filename" %(s))
                    raise StandardError
            pass

        def mapOutput(self, scriptFilename):
            s = scriptFilename
            if scriptFilename in self.logicalOutByScript:
                s = self.nextOutputName(self.logicalOutByScript[s])
            else:
                s = self.cleanOutputName(s)
            self.logicalOutByScript[scriptFilename] = s
            return s
                    
        def allowedConcreteInput(self, inputFilename):
            # fix this...
            return True

        def cleanOutputName(self, scriptFilename):
            # I can't think of a "good" or "best" way, so for now,
            # we'll just take the last part and garble it a little
            (head,tail) = os.path.split(scriptFilename)
            if tail == "":
                logging.error("empty filename: %s"%(scriptFilename))
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

        def newCommand(self, cmd, argtriple,
                       inouts, referenceLineNum):
            # first, reassign inputs and outputs.
            newinputs = map(self.mapInput, inouts[0])
            newoutputs = map(self.mapOutput, inouts[1])

            c = NcoParser.Command(cmd, argtriple, inouts, referenceLineNum)
            
            for out in inouts[1]:
                self.commandByLogicalOut[out] = c
            return c
    pass




class ParserCommand:
    def __init__(self):
#         self.original = ""
#         self.command = ""
#         self.inputList = [""]
#         self.outputList = [""]
#         self.arglist = [""]
#         self.leftover = [""]
        pass
    pass
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
        pass

    def handlers(self, newHandlers):
        def newNcoCommand(argv):
            print "--".join(argv)
            return True
        pass

    def handlerDefaults(self):
        self.modules = [(NcoParser, None)]

        pass
        

    def parseScript(self, script, factory):
        """Parse and accept/reject commands in a script, where the script
        is a single string containing script lines"""
        vp = VariableParser()
        def accept(obj, argv):
            for mod in obj.modules:
                if mod[0].accepts(argv):
                    cmd = mod[0].parse(line, argv, obj.lineNum, factory)
                    return True
            return False
        for line in script.splitlines():
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
                continue
                
            line = vp.apply(line)
            if not isinstance(line, str):
                continue
            argv = shlex.split(line)
            command = accept(self, argv)
            if isinstance(command, types.InstanceType):
                command.referenceLineNum = self.lineNum
            if not command:
                logging.debug(" ".join(["reject:", str(len(argv)), str(argv)]))


        pass
    
    pass


class SwampInterface:

    def __init__(self):
        pass

    def submit(self, script):
        p = Parser()
        sch = Scheduler()
        p.commandHandler(sch)
        p.parseScript(script, NcoParser.CommandFactory)
        task = sch.taskId()
        return task

    def fileStatus(self, logicalname):
        state = 0
        # go check db for state
        return state


    def taskFileStatus(self, taskid):
        state = 0
        logname = "dummy.nc"
        url = "NA"
        # want to return list of files + state + url if available
        return [(state, logname, url)]
        
        

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
    cf = NcoParser.CommandFactory()
    p.parseScript(portion, cf)

def main():
    testParser2()
    #testExpand()

if __name__ == '__main__':
    main()
