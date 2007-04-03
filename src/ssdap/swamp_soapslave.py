# $Header: /data/zender/nco_20150216/nco/src/ssdap/swamp_soapslave.py,v 1.1 2007-04-03 03:44:53 wangd Exp $
# Copyright (c) 2007 Daniel L. Wang
from swamp_common import *
from swamp_config import Config 
import cPickle as pickle
import logging
import SOAPpy

log = logging.getLogger("SWAMP")

class SimpleJobManager:
    def __init__(self, cfgName=None):
        if cfgName:
            self.config = Config(cfgName)
        else:
            self.config = Config()
        self.config.read()

        cfile = logging.FileHandler(self.config.logLocation)
        formatter = logging.Formatter('%(name)s:%(levelname)s %(message)s')
        cfile.setFormatter(formatter)
        log.addHandler(cfile)
        log.setLevel(self.config.logLevel)
        log.info("Swamp slave logging at "+self.config.logLocation)
        self.jobs = {} # dict: tokens -> jobstate
        self.localExec = LocalExecutor(FileMapper("slave%d"%os.getpid(),
                                                  self.config.execSourcePath,
                                                  self.config.execScratchPath ))

        self.token = 0
        pass

    def slaveExec(self, pickledCommand):
        cf = CommandFactory(self.config)
        p = cf.unpickleCommand(pickledCommand)
        print "I got", p, type(p),dir(p)
        fo = ExecutorForker(self.localExec)
        fo.forkCmd(p)
        token = self.token + 1
        self.jobs[token] = fo
        self.token = token
        return token

    def pollState(self, token):
        assert token in self.jobs
        fo = self.jobs[token]
        res = fo.pollResult()
        if res is not None:
            return res
        else:
            return None

    def pollOutputs(self, token):
        assert token in self.jobs
        fo = self.jobs[token]
        outs = fo.actualOutputs()
        return outs
        

    def startSlaveServer(self):
        SOAPpy.Config.debug =1
    
        server = SOAPpy.SOAPServer(("localhost", 8080))
        server.registerFunction(self.slaveExec)
        server.registerFunction(self.pollState)
        server.serve_forever()
        pass
    pass # end class SimpleJobManager

class ExecutorForker:
    def __init__(self, executor):
        self.pid = -1
        self.result = None
        self.pidFile = None
        self.executor = executor
        pass

    def clearPidFile(self):
        open(self.pidFile,"w")
        pass

    def writePidFile(self, retcode, actualOuts):
        log.debug("writing %s"%(str((retcode,actualOuts))))
        pickle.dump((retcode, actualOuts), open(self.pidFile,"w"))
        pass
    def checkPidFile(self):
        try:
            return pickle.load(open(self.pidFile, "r"))
        except IOError, e:
            return None
        except EOFError, e:
            return None
        pass

    def removePidFile(self):
        os.remove(self.pidFile)
        pass

    def updatePidname(self):
        self.pidFile = "/tmp/swamp_fork%d.return" % (self.pid)

    def detachStreams(self):
        """detach IO streams from the current process.
        Not a good idea to call this interactively. ;)"""
        devnull = os.open("/dev/null", os.O_RDONLY)
        os.dup2(devnull, sys.stdin.fileno())
        os.dup2(devnull, sys.stdout.fileno()) # silence stdout
        os.dup2(devnull, sys.stderr.fileno()) # silence stderr
        os.close(devnull)
        
    def forkCmd(self, cmd):
        assert self.pid == -1
        self.pid = os.fork()
        if self.pid > 0: #parent
            self.updatePidname()
            self.clearPidFile()
        else: #child
            self.detachStreams()
            self.pid = os.getpid()
            self.updatePidname()
            log.debug("child launching %s to %s"%(str(cmd),str(self.executor.launch)))
            self.executor.join("asdfareb")
            log.debug("still thinking...")
            token = self.executor.launch(cmd)
            retcode = self.executor.join(token)
            log.debug("child returned")
            # prevent lost update by ensuring empty before write
            while None != self.checkPidFile(): 
                time.sleep(0.5) # sleep for half a second
            self.writePidFile(retcode, self.executor.actualOutputs)
            log.error("%d %s" %(retcode, str(self.executor.actualOutputs)))
            sys.exit(0) # quick exit-- I just do what I'm told.
        pass
    def pollResult(self):
        if self.result is not None:
            return self.result

        tup = self.checkPidFile()
        print tup
        if isinstance(tup, tuple):
            self.result = tup[0]
            self.actualOutputs = tup[1]
            return int(f)
        else:
            return None
    def actualOutputs(self):
        """get the list of concrete outputs of this command
        have to call pollResult first, though"""
        return self.actualOutputs
    pass # end class ExecutorForker



def fakeCommand():
    cf = CommandFactory(Config.dummyConfig())
    ins = ["camsom1pdf/camsom1pdf.cam2.h1.0001-01-01-00000.nc"]
    outs = ["out.nc"]
    linenum = 20
    return cf.pickleCommand(cf.newCommand("ncwa", ({},[],ins+outs),(ins,outs), linenum))

def selfTest():
    pass

def clientTest():
    import SOAPpy
    server = SOAPpy.SOAPProxy("http://localhost:8080/")
    tok = server.slaveExec(fakeCommand())
    print "submitted, got token: ", tok
    while True:
        ret = server.pollState(tok)
        if ret is not None:
            print "finish, code ", ret
            break
        time.sleep(1)
    print "actual outs are at", server.pollOutputs(tok)

def main():
    selfTest()
    
    jm = SimpleJobManager("slave.conf")
    jm.startSlaveServer()

if __name__ == '__main__':
    main()

