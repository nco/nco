#$Id: swamp_config.py,v 1.2 2007-04-12 01:32:51 wangd Exp $
# swamp_config.py
# Copyright (c) 2007 Daniel L. Wang
import ConfigParser
import logging
import os
import sys

log = logging.getLogger("SWAMP")
class Config:
    """class Config: contains our local configuration information
    settings get written as attributes to an instance"""

    #defaults
    CFGMAP = [("logDisable", "log", "disable", False),
              ("logLocation", "log", "location",
               "/home/wangd/opendap/iroot/ssdap.log"),
              ("logLevel", "log", "level", logging.DEBUG),


              ("execNco4", "exec", "nco4",
               "/home/wangd/opendap/iroot/nco_ncdf4_bin"),
              ("execNcoDap", "exec", "ncodap",
               "/home/wangd/opendap/iroot/nco_dap_bin"),
              ("execScratchPath", "exec", "scratchPath", "."),
              ("execResultPath", "exec", "resultPath", "."),
              ("execSourcePath", "exec", "sourcePath", "."),
              ("execBulkPath", "exec", "bulkPath", "."),
              

              ("execLocalSlots", "exec", "localSlots", 2),
              ("execSlaveNodes", "exec", "slaveNodes", 0),

              ("slaveHostname", "slave", "hostname", "localhost"),
              ("slavePort", "slave", "port", 8080),
              ("slaveSoapPath", "slave", "soapPath", "SOAP"),
              ("slavePubPath", "slave", "pubPath", "pub"),

              ("dbFilename", "db", "filename", None),

              ]
    REMAP = {"True" : True, "False" : False, "Yes" : True, "No" : False,
             "" : None,
             "CRITICAL" : logging.CRITICAL,
             "ERROR" : logging.ERROR,
             "WARNING" : logging.WARNING,
             "INFO" : logging.INFO,
             "DEBUG" : logging.DEBUG,
             "NOTSET" : logging.NOTSET}

    staticInstance = None

    def __init__(self, fname = "swamp.conf"):
        self.config = ConfigParser.ConfigParser()
        self.slave = []
        if len(os.path.split(fname)[0]) == 0:
            # look in the same place as the script is located...
            # should I check current working directory instead?
            self.filepath = os.path.join(os.path.split(sys.argv[0])[0],
                                    fname)
        else:
            self.filepath = fname
        pass
    
    def read(self):
        log.info("Reading configfile %s" % (self.filepath))
        self.config.read(self.filepath)
        for m in Config.CFGMAP:
            val = m[3] # preload with default
            if self.config.has_option(m[1], m[2]):
                val = self.config.get(m[1],m[2])
                if val in Config.REMAP:
                    val = Config.REMAP[val]
                elif not isinstance(val, type(m[3])):
                    val = type(m[3])(val) # coerce type to match default
            setattr(self, m[0], val)
            log.debug("set config %s to %s", m[0], str(val))
            pass
        self.postReadFixup()
        pass

    def postReadFixup(self):
        if self.execSlaveNodes > 0:
            urlStr = "slave%dUrl"
            slotStr = "slave%dSlots"
            self.slave = []
            for i in range(1, self.execSlaveNodes+1):
                u = self.config.get("exec", urlStr % i)
                s = int(self.config.get("exec", slotStr % i))
                self.slave.append((u, s))
                log.debug("Added slave: url=%s slots=%d" %(u,s))
            pass
        pass

    def dumpSettings(self, logger, level):
        template = "config %s = %s"
        for m in Config.CFGMAP:
            val = getattr(self, m[0])
            logger.log(level, template % (m[0], str(val)))
            
    @staticmethod
    def dummyConfig():
        """Make a placeholder dummy config.  Our dummy config is one
        that is loaded with the default values."""
        c = Config()
        for m in Config.CFGMAP:
            val = m[3] 
            setattr(c, m[0], val)
        return c
        

def testConfigReader():
    logging.basicConfig(level=logging.DEBUG)
    c = Config()
    c.read()

def main():
    testConfigReader()

if __name__ == '__main__':
    main()
