#!/usr/bin/env python
       
# $Id: ssdap_dbutil.py,v 1.3 2006-09-21 20:22:42 wangd Exp $
# This is:  -- a module for managing state persistence for the dap handler.
#           -- Uses a SQLite backend.
from pysqlite2 import dbapi2 as sqlite
import sys

class JobPersistence:
    defaultDbFilename = "/tmp/mydb_ssdap"
    fileStateMap = {1 : "planned",
                    2 : "running",
                    3 : "saved",
                    4 : "removed"
                    }
    insertInoutTemplate = """insert into inouttable
        (cmdid, output, logicalname, concretename, state)
        values ('%s',%d,'%s','%s',%d);""" 
    insertFilestateTemplate = """insert into filestate (concretename, state)
        values ('%s',%d);"""
    selectFileIdTemplate = "select rowid from filestate where concretename=\"%s\";"
    
    def __init__(self, dbfile = defaultDbFilename):
        self.connected = False
        if dbfile == None: self.dbFilename = self.defaultDbFilename
        else: self.dbFilename = dbfile
        self.dbconnection = None
        self.dbcursor = None
        self.inOutList = []
        pass
    def connection(self):
        """only for intra-class use"""
        if not self.connected:
            # Create a connection to the database file "mydb"
            self.dbconnection = sqlite.connect(self.dbFilename)
            self.dbcursor = None
            self.connected = True
        return self.dbconnection
    def cursor(self):
        """only for intra-class use"""
        if self.dbcursor == None:
            # Get a Cursor object that operates in the context
            # of my Connection:
            self.dbcursor = self.connection().cursor()
        return self.dbcursor
    def commit(self):
        """force a db commit"""
        if self.connected:
            self.dbconnection.commit()
        else:
            raise "Error committing transaction: unconnected."
        

    def close(self):
        """force closing the db and releasing of db handles"""
        if self.connected:
            self.dbconnection.commit()
            self.dbcursor.close()
            self.dbconnection.close()
            self.connected = False
        self.dbcursor = None
        self.dbconnection = None
            
    def buildTables(self):
        ##
        ## consider having a table to store the actual script
        ## this would help for debugging and output caching
        ##
        taskcommand = ["create table tasktable (",
                       "  taskid varchar(8),",
                       "  linenum integer(8),", # 8 digits of lines is a lot
                       "  cmdid varchar(16),", #for now, concat taskid, linenum
                       "  date date",
                       ");"]
        cmdcommand = ["create table cmdtable (",
                       "  cmdid varchar(16),",
                       "  cmd varchar(80),", # e.g. ncwa, ncra
                       "  cmdline varchar(256)", # command line(with paths)
                       ");"]
        inoutcommand = ["create table inouttable (",
                        "  cmdid varchar(16),",
                        "  output tinyint(1),",
                        "  logicalname varchar(128),", # 
                        "  concretename varchar(192),", #
                        "  state tinyint(2)" # track file state 
                        ");"]
        filestate = ["create table filestate (",
                       "  concretename varchar(192),"
                       "  state tinyint(2)",
                       ");"]

                       
        # files can be planned, active, saved, removed, etc.
        

        cur = self.cursor()
        try:
            cmd = "".join(taskcommand + ["\n"] + cmdcommand +
                          ["\n"] + inoutcommand + ["\n"] + filestate)
            cur.executescript("".join(cmd))
            #cur.executescript("".join(cmdcommand))
            #cur.executescript("".join(inoutcommand))
            print >>sys.stderr, "made tables in db (uncommit)"
        except sqlite.OperationalError:
            print >>sys.stderr, "error making tables in DB"
        pass
    
    def deleteTables(self):
        deletecmd = [ "drop table tasktable;",
                      "drop table cmdtable;",
                      "drop table inouttable;",
                      "drop table filestate;", 
                    ];
        cur = self.cursor()
        try:
            cur.executescript("".join(deletecmd))
            print >>sys.stderr, "tables dropped ok (uncommit)"
        except sqlite.OperationalError:
            print >>sys.stderr, "error dropping tables from DB"
        pass
    def insertTask(self, taskid,linenum,cmdid):
        """inserts a task into the right tables"""
        sqlcmd = """insert into tasktable (taskid, linenum, cmdid, date)
        values ('%s',%d,'%s','%s');""" 
        #print >>sys.stderr, sqlcmd
        import time
        #today = "%04d%02d%02d" % time.localtime()[:3]
        # date in yyyy-mm-dd hh:mm format
        today = "%04d-%02d-%02d %02d:%02d" % time.localtime()[:5]
        cur = self.cursor()

        cur.execute(sqlcmd % (taskid, linenum, cmdid, today))
        #print >>sys.stderr, "inserted task in db (uncommit)"
        pass
    def insertCmd(self, cmdid, cmd, cmdline):
        sqlcmd = """insert into cmdtable (cmdid, cmd, cmdline)
        values ('%s','%s',\"%s\");""" 

        cur = self.cursor()
        #print >>sys.stderr, sqlcmd % (cmdid, cmd, cmdline)
        cur.execute(sqlcmd % (cmdid, cmd, cmdline))
        pass
    def insertInout(self, cmdid, logical, concrete, output, state):
        """insert a tuple in the inout table.
        cmdid -- id of relevant command
        logical -- logical filename
        concrete -- concrete-mapped filename
        output -- True if this is an output, False, otherwise
        state -- filestate (planned(1), saved(2), removed(3)

        returns a token to allow later retrieval of output file
        """
        # compare performance for deferred commit
        return self.insertInout2(cmdid, logical, concrete, output, state)
        import sys
        fileid = None
        statecmd = self.insertFilestateTemplate
        numcmd = self.selectFileIdTemplate
        cur = self.cursor()
        outnum = [0,1][output == True]
        sqlcmd = self.insertInoutTemplate % (cmdid, outnum, logical, concrete, state)
        #print >>sys.stderr, sqlcmd
        cur.execute(sqlcmd)
        if output: # insert into the token table if it's an output
            cur.execute(statecmd % (concrete, state))
            cur.execute(numcmd % concrete)
            row = cur.fetchall()
            fileid = row[0][0]
            pass
        return fileid
    def insertInOut2(self, cmdid, logical, concrete, output, state):
        """insert a tuple in the inout table.
        cmdid -- id of relevant command
        logical -- logical filename
        concrete -- concrete-mapped filename
        output -- True if this is an output, False, otherwise
        state -- filestate (planned(1), saved(2), removed(3)

        returns a token to allow later retrieval of output file
        """
        import sys
        fileid = None
        statecmd = self.insertFilestateTemplate
        numcmd = self.selectFileIdTemplate
        cur = self.cursor()
        outnum = [0,1][output == True]
        self.inOutList.append("'%s',%d,'%s','%s',%d" % (cmdid, outnum, logical, concrete, state))
        #defer this to a batch insert right before committing.
        #sqlcmd = self.insertInoutTemplate % (cmdid, outnum, logical, concrete, state)
        #print >>sys.stderr, sqlcmd
        #cur.execute(sqlcmd)

        if output: # insert into the token table if it's an output
            cur.execute(statecmd % (concrete, state))
            cur.execute(numcmd % concrete)
            row = cur.fetchall()
            fileid = row[0][0]
            pass
        return fileid
    def insertInOut2Commit(self):
        substTemp = """insert into inouttable 
        (cmdid, output, logicalname, concretename, state) values (?)"""
        cur = self.cursor()
        cur.executemany(substTemp, self.inOutList)
        self.inOutList = []

    def pollFileState(self, id):
        cur = self.cursor()
        cur.execute("select state from filestate where rowid=%d" % id)
        states = cur.fetchall()
        if states == []:
            return
        if len(states) == 1:
            return states[0][0]
        else:
            return
    def setFileState(self, id, state):
        cur = self.cursor()
        template = "update filestate set state=%d where rowid=%d" 
        cur.execute(template % (state,id))
    def pollFilename(self, id):
        cur = self.cursor()
        cur.execute("select concretename from filestate where rowid=%d" % id)
        names = cur.fetchall()
        if names == []:
            return
        if len(names) == 1:
            return names[0][0]
        else:
            return
        
    def showState(self):
        cur = self.cursor()
        # look for tasks
        cur.execute("select * from tasktable;")
        ttable = cur.fetchall()
        taskdict = {}
        if ttable == []:
            print "No Tasks in DB"
        else:
            for row in ttable:
                taskid = row[0]
                if taskid not in taskdict:
                    taskdict[taskid] = (1, row[3])
                else:
                    entry = taskdict[taskid]
                    taskdict[taskid] = (entry[0] + 1, entry[1])
            for tid in taskdict:
                entry = taskdict[tid]
                print "TaskId:", tid, "has", entry[0], "lines, dated",
                print entry[1]
                self.showTask(tid)
        # look for filestates
        self.showFileTable()
        #print >>sys.stderr, ttable
    def showFileTable(self):
        cur = self.cursor()
        cur.execute("select rowid,concretename,state from filestate")
        fstate = cur.fetchall()
        if fstate == []:
            print "No Files in DB"
        else:
            for row in fstate:
                print row[0], "Concrete", row[1], "with state", row[2],
                print "(", JobPersistence.fileStateMap[row[2]], ")"
        #print >>sys.stderr, ttable
        
    def showTask(self, tid):
        cur = self.cursor()
        cmd = 'select cmdid from tasktable where taskid="%s"' % (tid)
        cur.execute(cmd)
        for cidtuple in cur.fetchall():
            self.showCmd(cidtuple[0])
        pass

    def showCmd(self, cid):
        cur = self.cursor()
        cmd = 'select * from cmdtable where cmdid="%s"' % (cid)
        cur.execute(cmd)
        for fields in cur:
            print '  ID %s is cmd "%s" with cmdline "%s"' % fields
        cmd = 'select * from inouttable where cmdid="%s" and output=0' % (cid)
        cur.execute(cmd)
        for fields in cur:
            print "  Inputfile: logical=%s, real=%s, state=%d" % fields[2:]
        cmd = 'select * from inouttable where cmdid="%s" and output=1' % (cid)
        cur.execute(cmd)
        for fields in cur:
            print "  Outputfile: logical=%s, real=%s, state=%d" % fields[2:]
        pass
        
        
    pass

def selfTest():
    print "doing basic internal build/delete/build/delete test."
    j = JobPersistence("sometest_db")
    j.buildTables()
    j.close()
    j.deleteTables()
    j.close()
    j.buildTables()
    j.close()
    j.deleteTables()
    j.close()
    pass
def quickShow(dbfilename = None):
    if dbfilename == []:
        dbfilename = None
    elif type(dbfilename) == type([]):
        dbfilename = dbfilename[0]
    j = JobPersistence(dbfilename)
    j.showState()
    j.close()
def fileShow():
    j = JobPersistence()
    j.showFileTable()
    j.close()

def deleteTables():
    j = JobPersistence()
    print "ok, deleting tables from ssdap"
    j.deleteTables()
    j.close()
    pass
def buildTables():
    j = JobPersistence()
    print "ok, building new tables for ssdap"
    j.buildTables()
    j.close()
    pass
def parseArgs():
    jumptable = {"show" : quickShow,
                 "deletedb" : deleteTables,
                 "builddb" : buildTables,
                 "selftest" : selfTest
                 }
    import sys
    if (len(sys.argv) > 1) and sys.argv[1] in jumptable:
        if len(sys.argv) > 2:
            jumptable[sys.argv[1]](sys.argv[2:])
        else:
            jumptable[sys.argv[1]]()
    else:
        print sys.argv[0], "is a handy tool to manage your ",
        print "ssdap persistent state."
        print "available options:"
        for tup in jumptable:
            print tup, ",",
        print ""
        
if __name__ == '__main__':
    parseArgs()
    #test()

############################################################
# spare code section
# -- delete when stable.
############################################################
exit # so the interpreter won't look at the following
#self.buildTables()
#cur.execute("select * from tasktable;")
#print >>sys.stderr, cur.fetchall()
