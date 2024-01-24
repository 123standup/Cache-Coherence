/*******************************************************
                          cache.h
********************************************************/

#ifndef CACHE_H
#define CACHE_H

#include <cmath>
#include <iostream>

typedef unsigned long ulong;
typedef unsigned char uchar;
typedef unsigned int uint;

/****add new states, based on the protocol****/
enum {
   INVALID = 0,
   VALID,
   DIRTY,
   COPY, // for modified MSI protocol
   MODIFIED,
   EXCLUSIVE,
   SHARED_MODIFIED,
   SHARED_CLEAN
};
// BUS signal
enum {
   BusRdX = 0,
   BusRd,
   BusUpd,
   NONE
};

class cacheLine 
{
protected:
   ulong tag;
   ulong Flags;   // 0:invalid ...
   ulong seq; 
 
public:
   cacheLine()                { tag = 0; Flags = 0; }
   ulong getTag()             { return tag; }
   ulong getFlags()           { return Flags;}
   ulong getSeq()             { return seq; }
   void setSeq(ulong Seq)     { seq = Seq;}
   void setFlags(ulong flags) {  Flags = flags;}
   void setTag(ulong a)       { tag = a; }
   void invalidate()          { tag = 0; Flags = INVALID; } //useful function
   bool isValid()             { return ((Flags) != INVALID); }

};

class Cache
{
protected:
   ulong size, lineSize, assoc, sets, log2Sets, log2Blk, tagMask, numLines;
   ulong reads,readMisses,writes,writeMisses,writeBacks;

   //******///
   //add coherence counters here///
   ulong busRdX_cnt, invalid_cnt, mem_cnt, flush_cnt;
   ulong intervent_cnt, busUpd_cnt;
   //******///

   cacheLine **cache;
   ulong calcTag(ulong addr)     { return (addr >> (log2Blk) );}
   ulong calcIndex(ulong addr)   { return ((addr >> log2Blk) & tagMask);}
   ulong calcAddr4Tag(ulong tag) { return (tag << (log2Blk));}
   
public:
    ulong currentCycle;  
     
    Cache(int,int,int);
   ~Cache() { delete cache;}
   
   cacheLine *findLineToReplace(ulong addr);
   cacheLine *fillLine(ulong addr);
   cacheLine * findLine(ulong addr);
   cacheLine * getLRU(ulong);
   
   ulong getRM()     {return readMisses;} 
   ulong getWM()     {return writeMisses;} 
   ulong getReads()  {return reads;}       
   ulong getWrites() {return writes;}
   ulong getWB()     {return writeBacks;}
   
   void writeBack(ulong) {writeBacks++;}
   ulong Access(ulong,uchar,ulong,bool);
   void printStats(ulong, ulong);
   void updateLRU(cacheLine *);

   //******///
   //add other functions to handle bus transactions///
   ulong writeMiss(cacheLine *,ulong,bool);
   ulong readMiss(cacheLine *,ulong,bool);
   ulong writeHit(cacheLine *,ulong,bool);
   ulong readHit(cacheLine *,ulong,bool);
   void recvCore(ulong,ulong,ulong);
   void recvCore_DRAGON(cacheLine *,ulong);
   bool checkCopy(ulong, bool);
   //******///
};

#endif
