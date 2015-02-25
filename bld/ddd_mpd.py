#!/usr/bin/env python
# spawns a ddd session for each one found in mpilistjobs
# nco-specific: sends SIGUSR1 to nodeid>0 processes
# $Id$ 

# Usage:
#    ./ddd_mpd.py
#         -- spawn ddd sessions for each MPI process found via
# mpdlistjobs for a particular MPI job.  Before spawning, send SIGUSR1
# to the processes of rank > 0 (the non-manager nodes) to reduce user
# tedium.  If multiple jobs are found on MPD, prompt user to select
# one of jobs.
#         -- If no jobs owned by the current user are found in mpd,
# complain and exit.
#
# Prefer something else than ddd? Try changing the argument at the
# call to mainthing(...).  Support hasn't been checked for
# command-line arguments other than ddd/gdb, so you may have to hack
# spawnDebugger(...) as a short-term solution.
# 
# Please direct feedback to the sourceforge forums for NCO.  Thanks, -Daniel

from  os import environ, getuid, getpid, path, getcwd, popen3
import os

class Job:
    pass

def newJob(jid, username, host, pid, sid, rank, path):
    job = Job()
    job.jobid = jid
    job.username = username
    job.host = host
    job.pid = pid
    job.sid = sid
    job.rank = rank
    job.pgm = path
    return job

def readJobInfo(jobtext):
    job = Job()
    #print "got job ", jobtext
    lines = jobtext.split("\n")
    for line in lines:
        pair = line.split("=")
        if len(pair) > 1:
            key,value = pair
            #print key.strip(),value.strip()
            exec "job."+key.strip()+"="+"\""+value.strip()+"\""
    return job;

def readJobListMPD():
    joblist = []
    jobI, jobO, jobE = popen3("mpdlistjobs")
    allinfo = jobO.read()
    for jobtext in allinfo.split("\n\n"):
        joblist.append(readJobInfo(jobtext))
    return joblist

def makeSampleJobList(): ## dummy for testing
    joblist = []
    
    joblist.append(newJob( "21@dirt_3734", "wangd", "dirt",
                              "11931", "11926", "0",
                              "/home/wangd/nco/nco/mpi_bin/mpncwa"))
    joblist.append(newJob( "21@dirt_3734", "wangd", "dirt",
                              "11929", "11927", "1",
                              "/home/wangd/nco/nco/mpi_bin/mpncwa"))
    joblist.append(newJob( "21@dirt_3734", "wangd", "dirt",
                              "11930", "11928", "2",
                              "/home/wangd/nco/nco/mpi_bin/mpncwa"))
    # next two jobs should be culled
    joblist.append(newJob( "10@dirt_3734", "somebody", "dirt",
                              "112", "110", "0",
                              "/badpath/mpncbo"))
    joblist.append(newJob( "10@dirt_3734", "somebody", "dirt",
                              "119", "111", "1",
                              "/badpath/mpncbo"))
    joblist.append(newJob( "24@dirt_3734", "wangd", "dirt",
                              "11951", "11946", "0",
                              "/home/wangd/nco/nco/mpi_bin/mpncbo"))
    joblist.append(newJob( "24@dirt_3734", "wangd", "dirt",
                              "11949", "11947", "1",
                              "/home/wangd/nco/nco/mpi_bin/mpncbo"))
    joblist.append(newJob( "24@dirt_3734", "wangd", "dirt",
                              "11950", "11948", "2",
                              "/home/wangd/nco/nco/mpi_bin/mpncbo"))

    return joblist


def cutoutJobs(joblist, jobids, paths):
    print "There seems to be more than one job in MPD.  Which would you like?"
    pick = ""
    index = -1
    while not pick in jobids:
        for i in range(len(jobids)):
            print i,jobids[i], paths[i]
        print "which would you like(0..",len(jobids)-1,")?",
        typed = raw_input()
        try:
            index = int(typed)
        except ValueError:
            pass
        if index not in range(len(jobids)):    
            print "Sorry, bad choice: ",typed, " Try another."
        else:
            pick = jobids[index]
    print "Chose id:", pick, " path:",paths[index]

    # remove job objects that do not match
    joblist = filter((lambda j: j.jobid == pick), joblist)
    return joblist

def spawnDebugger(progname, joblist, debug):
    shI, shO, shE = popen3("sh")

    children = filter(lambda j: j.rank != "0", joblist)
    for c in children: # resume the children first
        s = "kill -USR1 %s\n" % (c.pid)
        if not debug: shI.write(s)
        else: print s,
    print "Node 0 is pid:",filter(lambda j: j.rank == "0",joblist)[0].pid
    shI.write("\n")
    for job in joblist:
        try:
            #print job.jobid, job.pid, job.rank, job.pgm
            dummy = job.pgm
            dummy = job.pid
            s = "%s %s %s &\n" % (progname, job.pgm, job.pid)
            if not debug: shI.write(s)
            #else:
            print s,
            
        except AttributeError:
            continue

    shI.close()
    print shO.read().strip()
    print shE.read().strip()
    

def mainthing(dbgprogname):
    joblist = readJobListMPD()
    #joblist = makeSampleJobList() # for simple testing
    # now, go ahead and spawn ddd jobs.
    # for now, just spit out the command line so we don't have to deal
    # with the process management. *sigh*
    ####print len(joblist)
    jobids = []
    paths = []
    joblist = filter(lambda j: hasattr(j, "jobid"), joblist)
    myname = os.popen("whoami").read().strip()
    joblist = filter(lambda j: j.username == myname, joblist)
    if len(joblist) < 1:
        print "No acceptable jobs found."
        return
    for job in joblist: ## check to see how many jobs there are
        if not job.jobid in jobids:
            jobids.append( job.jobid )
            paths.append( job.pgm )
    if len(jobids) > 1:
        joblist = cutoutJobs(joblist, jobids, paths)

    spawnDebugger(dbgprogname, joblist, False) #False for no debug
        

## -----------------
## main program body
## -----------------
mainthing("ddd")

# if you like gdb or dbx, you may wish to try something like:
#
# mainthing ("xterm -e gdb")
#
# You will probably want to spawn xterms for each gdb to avoid
# managing multiple gdb sessions from a single terminal window.

    
