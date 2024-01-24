/*******************************************************
                          cache.cc
********************************************************/

#include <stdlib.h>
#include <assert.h>
#include "cache.h"
using namespace std;

Cache::Cache(int s,int a,int b)
{
   ulong i, j;
   reads = readMisses = writes = 0; 
   writeMisses = writeBacks = currentCycle = 0;
   busRdX_cnt = 0, invalid_cnt = 0;

   size       = (ulong)(s);
   lineSize   = (ulong)(b);
   assoc      = (ulong)(a);   
   sets       = (ulong)((s/b)/a);
   numLines   = (ulong)(s/b);
   log2Sets   = (ulong)(log2(sets));   
   log2Blk    = (ulong)(log2(b));   
  
   //*******************//
   //initialize your counters here//
   busRdX_cnt = invalid_cnt = mem_cnt = flush_cnt = 0;
   intervent_cnt = busUpd_cnt = 0;
   //*******************//
 
   tagMask =0;
   for(i=0;i<log2Sets;i++)
   {
      tagMask <<= 1;
      tagMask |= 1;
   }
   
   /**create a two dimentional cache, sized as cache[sets][assoc]**/ 
   cache = new cacheLine*[sets];
   for(i=0; i<sets; i++)
   {
      cache[i] = new cacheLine[assoc];
      for(j=0; j<assoc; j++) 
      {
         cache[i][j].invalidate();
      }
   }      
}

/**you might add other parameters to Access()
since this function is an entry point 
to the memory hierarchy (i.e. caches)**/
ulong Cache::Access(ulong addr,uchar op, ulong protocol, bool copy)
{
   currentCycle++;/*per cache global counter to maintain LRU order 
                    among cache ways, updated on every cache access*/
   ulong bus_signal = NONE;
   
   if(op == 'w') writes++;
   else          reads++;
   
   cacheLine * line = findLine(addr);
   if(line == NULL)/*miss*/
   {
      cacheLine *newline = fillLine(addr);
      // if(op == 'w') newline->setFlags(DIRTY); // do it in func
      if(op == 'w')
      {
         // writeMisses++; // do it in func
         bus_signal = writeMiss(newline, protocol, copy);
         mem_cnt++;
      }
      else
      {
         // readMisses++; // do it in func
         bus_signal = readMiss(newline, protocol, copy);
         mem_cnt++;
      }
   }
   else
   {
      /**since it's a hit, update LRU and update dirty flag**/
      updateLRU(line);
      if (op == 'w')
      {
         bus_signal = writeHit(line, protocol, copy);
      }
      else
      {
         bus_signal = readHit(line, protocol, copy);
      }
   }

   return bus_signal;
}

/*look up line*/
cacheLine * Cache::findLine(ulong addr)
{
   ulong i, j, tag, pos;
   
   pos = assoc;
   tag = calcTag(addr);
   i   = calcIndex(addr);
   // find if cahce exist (not invalid)
   for(j=0; j<assoc; j++)
   if(cache[i][j].isValid()) {
      if(cache[i][j].getTag() == tag)
      {
         pos = j; 
         break; 
      }
   }
   // return NULL if cannot find cache
   if(pos == assoc) {
      return NULL;
   }
   else {
      return &(cache[i][pos]); 
   }
}

/*upgrade LRU line to be MRU line*/
void Cache::updateLRU(cacheLine *line)
{
   line->setSeq(currentCycle);  
}

/*return an invalid line as LRU, if any, otherwise return LRU line*/
cacheLine * Cache::getLRU(ulong addr)
{
   ulong i, j, victim, min;

   victim = assoc;
   min    = currentCycle;
   i      = calcIndex(addr);
   
   // if find empty sapce (invalid) then return
   for(j=0;j<assoc;j++)
   {
      if(cache[i][j].isValid() == 0) { 
         return &(cache[i][j]); 
      }   
   }
   // else pick a least used as victim(be replaced to new comming)
   for(j=0;j<assoc;j++)
   {
      if(cache[i][j].getSeq() <= min) { 
         victim = j; 
         min = cache[i][j].getSeq();}
   } 

   assert(victim != assoc);
   
   return &(cache[i][victim]);
}

/*find a victim, move it to MRU position*/
cacheLine *Cache::findLineToReplace(ulong addr)
{
   cacheLine * victim = getLRU(addr);
   updateLRU(victim);
  
   return (victim);
}

/*allocate a new line*/
cacheLine *Cache::fillLine(ulong addr)
{ 
   ulong tag;
  
   cacheLine *victim = findLineToReplace(addr);
   assert(victim != 0);
   
   if(victim->getFlags() == MODIFIED || victim->getFlags() == SHARED_MODIFIED) {
      writeBack(addr);
      mem_cnt++;
   }
   tag = calcTag(addr);
   victim->setTag(tag);
   /**note that this cache line has been already 
      upgraded to MRU in the previous function (findLineToReplace)**/

   return victim;
}

void Cache::printStats(ulong i, ulong protocol)
{ 
   double missRate = ((double)(readMisses + writeMisses)/(double)(reads + writes));
   printf("============ Simulation results (Cache %lu) ============\n", i);
   /****print out the rest of statistics here.****/
   /****follow the ouput file format**************/
	if (protocol == 0)
   {
      printf("01. number of reads:                            %lu\n", reads);
      printf("02. number of read misses:                      %lu\n", readMisses);
      printf("03. number of writes:                           %lu\n", writes);
      printf("04. number of write misses:                     %lu\n", writeMisses);
      printf("05. total miss rate:                            %4.2f%%\n", missRate*100);
      printf("06. number of writebacks:                       %lu\n", writeBacks);
      printf("07. number of memory transactions:              %lu\n", mem_cnt);
      printf("08. number of invalidations:                    %lu\n", invalid_cnt);
      printf("09. number of flushes:                          %lu\n", flush_cnt);
      printf("10. number of BusRdX:                           %lu\n", busRdX_cnt);
   }
   else if (protocol == 1)
   {
      printf("01. number of reads:                            %lu\n", reads);
      printf("02. number of read misses:                      %lu\n", readMisses);
      printf("03. number of writes:                           %lu\n", writes);
      printf("04. number of write misses:                     %lu\n", writeMisses);
      printf("05. total miss rate:                            %4.2f%%\n", missRate*100);
      printf("06. number of writebacks:                       %lu\n", writeBacks);
      printf("07. number of memory transactions:              %lu\n", mem_cnt);
      printf("08. number of interventions:                    %lu\n", intervent_cnt);
      printf("09. number of flushes:                          %lu\n", flush_cnt);
      printf("10. number of Bus Transactions(BusUpd):         %lu\n", busUpd_cnt);
   }
}

// implement

bool Cache::checkCopy(ulong addr, bool send) // only used in dragon protocol
{
   cacheLine *line = findLine(addr);
   if (line == NULL) return false;
   if (send)
      recvCore_DRAGON(line, BusRd);

   return true;
}

ulong Cache::writeMiss(cacheLine *line, ulong protocol, bool copy)
{
   ulong bus_signal = NONE;
   if (protocol == 0) // modified MSI: write miss only happend when I -> M
   {
      bus_signal = BusRdX;
      line->setFlags(MODIFIED);
      writeMisses++;
      busRdX_cnt++;
   }
   else if (protocol == 1) // dragon
   {
      if (copy == true) // -> SM
      {
         bus_signal = BusUpd;
         line->setFlags(SHARED_MODIFIED);
         writeMisses++;
         busUpd_cnt++;
      }
      else if (copy == false) // -> M
      {
         bus_signal = BusRd;
         line->setFlags(MODIFIED);
         writeMisses++;
      }
   }
   return bus_signal;
}

ulong Cache::readMiss(cacheLine *line, ulong protocol, bool copy)
{
   ulong bus_signal = NONE;
   if (protocol == 0) // modified MSI: read miss only happend when I -> C
   {
      bus_signal = BusRd;
      line->setFlags(COPY);
      readMisses++;
   }
   else if (protocol == 1)
   {
      bus_signal = BusRd;
      if (copy == true)
      {
         line->setFlags(SHARED_CLEAN);
         readMisses++;
      }
      else if (copy == false)
      {
         line->setFlags(EXCLUSIVE);
         readMisses++;
      }
   }
   return bus_signal;
}

ulong Cache::writeHit(cacheLine *line, ulong protocol, bool copy)
{
   ulong bus_signal = NONE;
   // modified MSI: do nothing
   if (protocol == 0)
   {
      line->setFlags(MODIFIED);
   }
   else if (protocol == 1)
   {
      if (line->getFlags() == SHARED_MODIFIED)
      {
         if (copy == false)
            line->setFlags(MODIFIED);
         bus_signal = BusUpd;
         busUpd_cnt++;
      }
      else if (line->getFlags() == SHARED_CLEAN)
      {
         if (copy == true)
            line->setFlags(SHARED_MODIFIED);
         else if (copy == false)
            line->setFlags(MODIFIED);
         bus_signal = BusUpd;
         busUpd_cnt++;
      }
      else if (line->getFlags() == EXCLUSIVE)
      {
         line->setFlags(MODIFIED);
      }
   }

   return bus_signal;
}

ulong Cache::readHit(cacheLine *line, ulong protocol, bool copy)
{
   ulong bus_signal = NONE;
   return bus_signal;
}

void Cache::recvCore(ulong addr, ulong bus_signal, ulong protocol)
{
   if (bus_signal == NONE) return;
   cacheLine *line = findLine(addr);
   if (line == NULL) return;
   if (protocol == 0) // modified MSI
   {
      if (line->getFlags() == MODIFIED)
      {
         if (bus_signal == BusRdX)
         {
            line->invalidate();
				invalid_cnt++;
            writeBacks++;
            flush_cnt++;
            mem_cnt++;
         }
         else if (bus_signal == BusRd)
         {
            line->invalidate();
				invalid_cnt++;
            writeBacks++;
            flush_cnt++;
            mem_cnt++;
         }
      }
      else if (line->getFlags() == COPY)
      {
         if (bus_signal == BusRdX)
         {
            line->invalidate();
				invalid_cnt++;
         }
         else if (bus_signal == BusRd)
         {
            line->invalidate();
				invalid_cnt++;
         }
      }
   }
   else if (protocol == 1)
      recvCore_DRAGON(line, bus_signal);
}

void Cache::recvCore_DRAGON(cacheLine * line, ulong bus_signal)
{
   
   if (line->getFlags() == MODIFIED)
   {
      if (bus_signal == BusRd) // M -> SM
      {
         line->setFlags(SHARED_MODIFIED);
         flush_cnt++;
         mem_cnt++;
         intervent_cnt++;
         writeBacks++;
      }
   }
   else if (line->getFlags() == SHARED_MODIFIED)
   {
      if (bus_signal == BusRd) // SM -> SM
      {
         flush_cnt++;
         mem_cnt++;
         writeBacks++;
      }
      else if (bus_signal == BusUpd) // SM -> SC
      {
         line->setFlags(SHARED_CLEAN);
      }

   }
   else if (line->getFlags() == EXCLUSIVE)
   {
      if (bus_signal == BusRd)
      {
         line->setFlags(SHARED_CLEAN);
         intervent_cnt++;
      }
   }
}