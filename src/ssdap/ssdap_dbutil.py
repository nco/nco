#!/usr/bin/env python
       
# $Id: ssdap_dbutil.py,v 1.9 2006-10-04 23:45:01 wangd Exp $
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
    insertInoutTemplate = """insert into cmdFileRelation
        (taskrow, linenum, output, logicalname, concretename, state)
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
        self.inOutList = [] # cache inserts to batch until commit
        self.cmdList = [] # cache inserts to batch until commit
        self.stateList = [] # cache inserts to batch until commit
        
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
        self.insertInOut2Commit()
        self.commitCmds()
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
        """Builds the set of tables needed for async operation"""
        ##
        ## consider having a table to store the actual script
        ## this would help for debugging and output caching
        ##
        taskcommand = ["create table tasks (",
                       "  taskid varchar(8),",
                       "  date date",
                       ");"]
        cmdcommand = ["create table cmds (",
                      "  taskrow integer(8),", 
                      "  linenum integer(8),", # 8 digits of lines is a lot
                      "  cmd varchar(80),", # e.g. ncwa, ncra
                      "  cmdline varchar(256)", # command line(with paths)
                       ");"]
        inoutcommand = ["create table cmdFileRelation (",
                        "  taskrow integer(8),",
                        "  linenum integer(8),",
                        "  output tinyint(1),",
                        "  logicalname varchar(128),", # 
                        "  concretename varchar(192)", #
                        ");"]  
        filestate = ["create table fileState (",
                       "  concretename varchar(192),"
                       "  state tinyint(2)", #need this consistent with other table?
                       ");"]
        readylist = ["create table readyList (",
                     " taskrow integer(8),",
                     " linenum integer(8),",
                     " concretename varchar(192)", # track concrete,
                     # so we can update the filestate without querying.
                     ");"]

                       
        # files can be planned, active, saved, removed, etc.
        

        cur = self.cursor()
        try:
            cmd = "".join(taskcommand + ["\n"] + cmdcommand +
                          ["\n"] + inoutcommand + ["\n"] + filestate
                          + ["\n"] + readylist)
            cur.executescript("".join(cmd))
            #print "trying to execute cmd:",cmd
            #cur.executescript("".join(cmdcommand))
            #cur.executescript("".join(inoutcommand))
            print >>sys.stderr, "made tables in db (uncommit)"
        except sqlite.OperationalError:
            print >>sys.stderr, "error making tables in DB"
        pass
    
    def deleteTables(self):
        deletecmd = [ "drop table tasks;",
                      "drop table cmds;",
                      "drop table cmdFileRelation;",
                      "drop table fileState;",
                      "drop table readyList;"
                    ];
        cur = self.cursor()
        try:
            cur.executescript("".join(deletecmd))
            print >>sys.stderr, "tables dropped ok (uncommit)"
        except sqlite.OperationalError:
            print >>sys.stderr, "error dropping tables from DB"
        pass
    def insertTask(self, taskid):
        """inserts a task into the right tables,
        returns resultant rownumber for use in other tables"""
        sqlcmd = """insert into tasks (taskid, date)
        values ('%s','%s');""" 
        # print >>sys.stderr, sqlcmd
        import time
        # today = "%04d%02d%02d" % time.localtime()[:3]
        # date in yyyy-mm-dd hh:mm format
        today = "%04d-%02d-%02d %02d:%02d" % time.localtime()[:5]
        cur = self.cursor()

        cur.execute(sqlcmd % (taskid, today))
        sqlcmd = """select rowid from tasks where taskid=\"%s\";""";
        cur.execute(sqlcmd % (taskid))
        row = cur.fetchall()
        rownum = row[0][0]
        assert len(row) == 1
        #print >>sys.stderr, "inserted task in db (uncommit)"
        return rownum 

    def insertCmd(self, taskrow, linenum, cmd, cmdline):
        """Inserts a command (i.e. a script line) into the right tables"""


        sqlcmd = """insert into cmds (taskrow, linenum, cmd, cmdline)
        values (%d, %d,'%s',\"%s\");""" 

        self.cmdList.append((taskrow, linenum, str(cmd), str(cmdline)))
        #print >>sys.stderr, sqlcmd % (cmdid, cmd, cmdline)
        #cur = self.cursor()
        #cur.execute(sqlcmd % (cmdid, cmd, cmdline))
        
        pass
    def commitCmds(self):
        """Commit whatever command entries are currently queued."""
        if(len(self.cmdList) > 0):
            cur = self.cursor()
            cur.executemany("""insert into cmds (taskrow, linenum, cmd, cmdline)
            values (?,?,?,?)""", self.cmdList)
            self.cmdList = []
            
                        
    def insertInout(self, taskrow, linenum, logical, concrete, output, state, isTemp):
        """insert a tuple in the inout table.
        cmdid -- id of relevant command
        logical -- logical filename
        concrete -- concrete-mapped filename
        output -- True if this is an output, False, otherwise
        state -- filestate (planned(1), saved-needed(2),
                            saved-deletable(3), removed(4)

        returns a token to allow later retrieval of output file
        """
        # compare performance for deferred commit
        
        return self.insertInOut2(taskrow, linenum,
                                 logical, concrete, output, state, isTemp)
        import sys
        fileid = None
        statecmd = self.insertFilestateTemplate
        numcmd = self.selectFileIdTemplate
        cur = self.cursor()
        outnum = [0,1][output == True]
        sqlcmd = self.insertInoutTemplate % (taskrow, linenum,
                                             outnum, logical, concrete, state)
        #print >>sys.stderr, sqlcmd
        cur.execute(sqlcmd)
        if output: # insert into the token table if it's an output
            cur.execute(statecmd % (concrete, state))
            cur.execute(numcmd % concrete)
            row = cur.fetchall()
            fileid = row[0][0]
            pass
        return fileid
    def insertInOut2(self, taskrow, linenum, logical, concrete, output, state, isTemp):
        """insert a tuple in the inout table.
        cmdid -- id of relevant command
        logical -- logical filename
        concrete -- concrete-mapped filename
        output -- True if this is an output, False, otherwise
        state -- filestate (planned(1), saved(2), removed(3)

        Returns a token to allow later retrieval of output file,
          but only if isTemp is false.
        """
        import sys
        fileid = None
        statecmd = self.insertFilestateTemplate
        numcmd = self.selectFileIdTemplate
        cur = self.cursor()
        outnum = [0,1][output == True]
        self.inOutList.append((taskrow, linenum, outnum,
                               str(logical), str(concrete)))
        #defer this to a batch insert right before committing.
        #sqlcmd = self.insertInoutTemplate % (cmdid, outnum, logical, concrete, state)
        #print >>sys.stderr, sqlcmd
        #cur.execute(sqlcmd)

        if output: # insert into the token table if it's an output
            if not isTemp: # only insert real files now, defer temps.
                # skipping 'select' of temp fileids
                # performance of  persistCommand from 64s to 6s
                cur.execute(statecmd % (concrete, state))
                cur.execute(numcmd % concrete)
                row = cur.fetchall()
                fileid = row[0][0]
            else:
                self.stateList.append((str(concrete),state))
            pass
        return fileid
    def insertInOut2Commit(self):
        """Commit those inserts that were queued earlier during insertInOut2"""
        substTemp = """insert into cmdFileRelation
        (taskrow, linenum, output, logicalname, concretename)
        values (?,?,?,?,?)"""
        cur = self.cursor()
        print "inout commit has %d inout and %d state to commit" % (
            len(self.inOutList), len(self.stateList))
        if(len(self.inOutList) > 0):
            cur.executemany(substTemp, self.inOutList)
            self.inOutList = []
        if(len(self.stateList) > 0):
            cur.executemany("""insert into filestate (concretename, state)
            values (?,?)""", self.stateList)
            self.stateList = []
        pass

    def pollFileState(self, id):
        """Check the state of a file with the supplied id
        Returns: the state of the file, if it exists."""
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
    def setFileStateByName(self, concretename, state):
        cur = self.cursor()
        template = "update filestate set state=%d where concretename=\'%s\'" 
        cur.execute(template % (state,concretename))
        
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
    def cmdsWithInput(self, concretename):
        """Return a list of cmdids that have inputid as one of their input"""
        cur = self.cursor()
        # first find the affected commands
        cur.execute("""select taskrow,linenum from cmdFileRelation where
        output=0 AND concretename='%s';""" % (concretename))
        rows = cur.fetchall()
        return rows # should return list of (taskrow, linenum) tuples
    def makeReady(self, cmdList):
        """input: cmdList is a list of tuples (taskrow,linenum)
        for each tuple, check to see if it has any unready input files
        if there are no unready input files, add job to the ready list

        Idea: If this results in a ready job, might just return the job
        instead of putting it on the list--> avoid the db update and
        continue working.
        """
        # for each cmd in the list, check the filestates of its input files
        # might want LEFT JOIN
        getfiles = """select concretename,state from cmdFileRelation
        LEFT JOIN fileState USING (concretename)
        where taskrow=%d AND linenum=%d AND output=0 AND state IS NOT NULL;"""
        getoutput = """select concretename from cmdFileRelation
        JOIN fileState USING (concretename)
        where taskrow=%d AND linenum=%d AND output=1;"""
        readyTemplate = """insert into readyList
        (taskrow,linenum,concretename) values (?,?,?)"""

        cur = self.cursor()
        addReady = []
        # consider batching up the select command if db access is expensive
        for (taskrow, linenum) in cmdList:
            cur.execute(getfiles % (taskrow, linenum))
            ready = True
            f = cur.fetchall()
            for t in f:
                if t[1] != 3: # if state is not saved
                    ready = False
                    print "cmdList: %d,%d has %s in %s --> not ready" % (
                        taskrow, linenum, t[0], str(t[1]))
                    break
            if ready:
                cur.execute(getoutput % (taskrow, linenum))
                f = cur.fetchall()
                assert len(f) == 1
                addReady.append((taskrow,linenum,f[0][0]))
            pass
        if len(addReady) > 0: cur.executemany(readyTemplate, addReady)
        return len(addReady)
    def initMakeReady(self, taskrow):
        """Find all ready jobs and put them on the readylist
        Warning: logic duplication between this and makeReady.
        Refactoring these two is a priority.
        """
        sql = """select linenum,output,concretename,state from cmds LEFT JOIN
        cmdFileRelation USING (taskrow,linenum) LEFT JOIN fileState
        USING (concretename)
        WHERE taskrow=%d"""
        ## this SQL command is not appropriate!!! fix!
        cur = self.cursor()
        cur.execute(sql % taskrow)
        readyCmds = {}
        f = cur.fetchall()
        for (linenum, output,concretename, state) in f:
            #print "cmdList: %d,%d has %s in %s" % (
            #    taskrow, linenum, concretename, state)
            ready = readyCmds.get(linenum, (True,None))
            if not output: #output is either missing or false(input)
                if (state is not None) and (state != 3): #if not saved?
                    readyCmds[linenum] = (False,ready[1])
            #        print "Not Ready %d,%d has %s in %s" % (
            #            taskrow, linenum, concretename, state)
            else: readyCmds[linenum] = (ready[0],concretename)
        i = readyCmds.items()
        # filter for only the true values, and then pick the first
        # half of the tuple
        i = map(lambda x:(taskrow,x[0],x[1][1]), filter(lambda x: x[1][0], i))
        i.sort()
        sql = "insert into readyList VALUES(?,?,?)"
        cur.executemany(sql,i)
        
        pass
    def fetchAndLockNextCmd(self,taskrow):
        # fetch a cmd from ready list
        cmd = """SELECT readyList.rowid,concretename,cmdLine
        FROM readyList JOIN cmds
        USING (taskrow,linenum) WHERE taskrow=%d LIMIT 1;"""
        cur = self.cursor()
        cur.execute(cmd % taskrow)
        rows = cur.fetchall()
        if len(rows) < 1:
            return None 
        mycommand = rows[0]
        # drop the cmd from the list
        cmd = """delete from readyList where rowid=%d;"""
        cur.execute(cmd % mycommand[0])
        # update the filestate to running
        # hope filename doesn't have any quotes!
        cmd = "update fileState set state=2 where concretename='%s';"
        cur.execute(cmd % mycommand[1])
        # return cmdline and output concretename
        return (mycommand[2],mycommand[1]) 
    def showState(self):
        cur = self.cursor()
        # look for tasks
        cur.execute("select taskid,rowid,date from tasks LIMIT 200;")
        ttable = cur.fetchall()
        taskdict = {}
        if ttable == []:
            print "No Tasks in DB"
        else:
            for row in ttable:
                taskid = row[0]
                if taskid not in taskdict:
                    taskdict[taskid] = row[1:]
                else:
                    print "warning, duplicate task in tasks table, id=",
                    print taskid, " Ignoring..."
            for tid in taskdict:
                entry = taskdict[tid]
                print "TaskId:", tid, "is dated",
                print entry[1]
                #self.showTaskById(tid)
                self.showTaskCommandsByRow(entry[0])
        # look for filestates
        self.showFileTable()
        self.showReadyList()
        #print >>sys.stderr, ttable
    def showFileTable(self):
        cur = self.cursor()
        cur.execute("select rowid,concretename,state from filestate LIMIT 200")
        fstate = cur.fetchall()
        if fstate == []:
            print "No Files in DB"
        else:
            for row in fstate:
                print row[0], "Concrete", row[1], "with state", row[2],
                print "(", JobPersistence.fileStateMap[row[2]], ")"
        #print >>sys.stderr, ttable
        
    def showTaskCommandsByRow(self, taskrow):
        cur = self.cursor()
        cmd = 'select * from cmds where taskrow="%s" LIMIT 200' % (taskrow)
        cur.execute(cmd)
        for cidtuple in cur.fetchall():
            self.showCmdTuple(cidtuple)
        pass

    def showCmdTuple(self, tuple):
        """Pretty-prints a row from the cmds table"""
        cmdtemplate = "row %d, line %d, cmd %s, cmdline= %s"
        print cmdtemplate % tuple
        cur = self.cursor()
        cmd = 'select * from cmdFileRelation LEFT JOIN fileState \
        using (concretename) where taskrow=%d and linenum=%d;'
        cur.execute(cmd % (tuple[0],tuple[1]) )
        for t in cur:
            print "  ",["Inputfile","Outputfile"][t[2]],
            state = self.fileStateMap.get(t[5], "NULL")
            print "logical=%s, real=%s, state=%s (%s)" % (t[3],t[4],str(t[5]),state)
                                                          
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
        
    def showReadyList(self):
        sql = "select * from readyList JOIN cmds USING (taskrow,linenum) LIMIT 200;"
        cur = self.cursor()
        cur.execute(sql)
        for r in cur.fetchall():
            print "ready cmd: task=%d line=%d, out=%s, cmd=%s, cmdline=%s" % (
                r[0], r[1], r[2], r[3], r[4])

            
    pass


def selfTest(args=[]):
    buildOnly = False
    if len(args) > 0:
        if "build" in args:
            buildOnly = True
    print "doing basic internal build/fill/run/delete/build/delete test."
    j = JobPersistence("sometest_db")
    j.buildTables()
    j.close()
    print " build and close"
    row = j.insertTask("AABBCCDD")
    # a command with no input and one independent output (ready to go)
    j.insertCmd(row, 2, "ncap", "ncap -o %outf_indep.nc%")
    j.insertInout(row, 2, "%outf_indep.nc%", "/tmp/temp1111outf_indep.nc", 
                  True, 1, False)
    # a command with one input(orig) and one output(temp) (ready to go)
    j.insertCmd(row, 5, "ncwa", "ncwa in.nc %tempf_other.nc%")
    j.insertInout(row, 5, "in.nc", "in.nc", 
                  False, 1, False)
    j.insertInout(row, 5, "%tempf_other.nc%", "/tmp/temp0000tempf_other.nc", 
                  True, 1, True)
    # a command with two inputs (orig+depend) and one output
    j.insertCmd(row, 10, "ncwa", "ncwa in.nc %tempf_other.nc% %outf_out.nc%")
    j.insertInout(row, 10, "in.nc", "in.nc", 
                  False, 1, False)
    j.insertInout(row, 10, "%tempf_other.nc%", "/tmp/temp0000tempf_other.nc", 
                  False, 1, True)
    j.insertInout(row, 10, "%outf_out.nc%", "/tmp/temp1111outf_out.nc", 
                  True, 1, False)
    # a command with two inputs (orig+depend) and one output
    j.insertCmd(row, 12, "ncap", "ncwa in.nc %outf_out.nc% %outf_out2.nc%")
    j.insertInout(row, 12, "in.nc", "in.nc", 
                  False, 1, False)
    j.insertInout(row, 12, "%outf_out.nc%", "/tmp/temp1111outf_out.nc", 
                  False, 1, False)
    j.insertInout(row, 12, "%outf_out2.nc%", "/tmp/temp2222outf_out2.nc", 
                  True, 1, False)
    j.commit()
    j.initMakeReady(row)
    #    clist = j.cmdsWithInput("/tmp/temp1111tempf_other.nc")
    #j.makeReady(clist)
    j.close()
    j.showState()
    # now, pretend like we're executing.
    while True:
        try:
            (cline,outname) = j.fetchAndLockNextCmd(row)
        except TypeError:
            print ":::no more lines to run!"
            break
        print ":::pretending to run %s" % (cline)
        j.showState()
        print ":::fake produce %s" % (outname)
        affected = j.cmdsWithInput(outname)
        j.setFileStateByName(outname, 3) # mark completed
        j.makeReady(affected)
        j.close()
        j.showState()
    

    if buildOnly:
        return
    j.deleteTables()
    j.close()
    print " delete and closed"
    j.buildTables()
    j.close()
    print " build and closed"
    j.deleteTables()
    j.close()
    print " delete and closed"
    pass
def fixDbFilename(dbfilename):
    if dbfilename == []:
        return None
    elif type(dbfilename) == type([]):
        return dbfilename[0]
    else:
        return dbfilename
    
def quickShow(dbfilename = None):
    j = JobPersistence(fixDbFilename(dbfilename))
    j.showState()
    j.close()
def fileShow(dbfilename = None):
    j = JobPersistence(fixDbFilename(dbfilename))
    j.showFileTable()
    j.close()

def deleteTables(dbfilename = None):
    realdb = fixDbFilename(dbfilename)
    j = JobPersistence(realdb)
    print "ok, deleting tables from ssdap @ %s" % (str(realdb))
    j.deleteTables()
    j.close()
    pass
def buildTables(dbfilename = None):
    realdb = fixDbFilename(dbfilename)
    j = JobPersistence(realdb)
    print "ok, building new tables for ssdap @ %s" % (str(realdb))
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
