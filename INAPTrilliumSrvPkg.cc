/*
// ============================================================================
//
// = LIBRARY
//     INAP-TrilliumWrapper
//
// = FILENAME
//     INAPTrilliumSrvPkg.cc
//
// ============================================================================
*/

extern "C"
{
#include <INAPTrilliumWrapper/INAPTrilliumDef.h>
} // extern "C"


extern "C"
{
   PUBLIC S16 iuActvTsk(Pst* pst, Buffer* mBuf);
   EXTERN  SSTskId         sTskId; 
} // extern "C"
#include "INAPTrilliumSrvPkg.hh"
#include "INAPTrilliumManager.hh"
#define SM_GOACTIVE 1   //23nov...anshu..FT
#define SM_GOSBY 2      //25/11/2010 ..anshu..FT
#define SM_PEER_NODE_DOWN 3      //13/12/2010 ..anshu..FT
INAPTrilliumSrvPkg::INAPTrilliumSrvPkg()
{
  S16	ret;

 // Register the Tapa Task (INAP User)
    SRegTTsk(ENTIU, 0, TTNORM, PRIOR0, 0, iuActvTsk);
 
 // Attach Tapa Task to the previously created System Task 
 // SAttachTTsk(ENTIU, 0, sTskIdIU);
 
  ret = SAttachTTsk(ENTIU, 0, sTskId);
}


INAPTrilliumSrvPkg::~INAPTrilliumSrvPkg()
{
// Deregister Tapa Task
  SDeregTTsk(ENTIU, 0);

// Detach Tapa Task
  SDetachTTsk(ENTIU, 0);
}

#if 1
void INAPTrilliumSrvPkg::goActive()
{
   
  SuId suId = 0;
  SpId spId = 0;  

  Pst           inapPst;
  memset(&inapPst,0,sizeof(Pst));

  Pst           *pst = &inapPst;

  pst->dstEnt = ENTIE;
  pst->srcEnt = ENTIU;
  pst->event = IET_EVTBNDREQ;
  pst->dstInst = 0;
  pst->srcInst = 0;

  pst->region = DFLT_REGION;
  pst->pool = DFLT_POOL;
  pst->prior = PRIOR0;
  pst->route = RTESPEC;
  pst->dstProcId = SFndProcId();
  pst->srcProcId = SFndProcId();
  pst->selector = 0; 

  INAPTrilliumManager::setSuId(suId);
  INAPTrilliumManager::setSpId(spId);

   for( int i=0: i<NO_OF_SSN; i++)
      {
        suId=i;
        spId=i;
       IuLiIetBndReq(pst,suId,spId);
     }

}

int INAPTrilliumSrvPkg::stackGoActive()
{
  Buffer *mBuf;
  Pst           stackPst;
  memset(&stackPst,0,sizeof(Pst));

  Pst           *pst = &stackPst;

  pst->dstEnt = ENTAL;
  pst->srcEnt = ENTIU;
  pst->event = SM_GOACTIVE;
  pst->dstInst = 0;
  pst->srcInst = 0;

  pst->region = DFLT_REGION;
  pst->pool = DFLT_POOL;
  pst->prior = PRIOR0;
  pst->route = RTESPEC;
  pst->dstProcId = SFndProcId();
  pst->srcProcId = SFndProcId();
  pst->selector = 0; // priyatos LC

   S16 ret,res = 0;
   
   if ((res = SGetMsg(pst->region, pst->pool, &mBuf)) != ROK)
        {
            //printf("SGetMsg(pst->region, pst->pool, &mBuf))..error");
	    return res;
        }

   //CMCHKPKLOG( cmPkSpId, 0, mBuf, EIET146, pst);
   ret = SPstTsk(pst, mBuf);
   return ret;
}

int INAPTrilliumSrvPkg::stackGoStandby()
{
//  printf("Inside stackGoStandby()...%s::%d\n",__FILE__,__LINE__);
  Buffer *mBuf=NULL;
  Pst           stackPst;
  memset(&stackPst,0,sizeof(Pst));

  Pst           *pst = &stackPst;
  //The entity changed from ENTSM to ENTAl to avoid confusion with OT SM and Trillium SM..
  pst->dstEnt = ENTAL;  
  pst->srcEnt = ENTIU;

  pst->srcEnt = ENTIU;
  pst->event = SM_GOSBY;
  pst->dstInst = 0;
  pst->srcInst = 0;

  pst->region = DFLT_REGION;
  pst->pool = DFLT_POOL;
  pst->prior = PRIOR0;
  pst->route = RTESPEC;
  pst->dstProcId = SFndProcId();
  pst->srcProcId = SFndProcId();
  pst->selector = 0; // priyatos LC

  S16 ret,res = 0;
   
   if ((res = SGetMsg(pst->region, pst->pool, &mBuf)) != ROK)
        {
            //printf("SGetMsg(pst->region, pst->pool, &mBuf))..error");
	    return res;
        }

   ret = SPstTsk(pst, mBuf);
}

bool
INAPTrilliumSrvPkg::notifyActiveTrilliumStack()
{
//printf("Inside ..notifyActiveTrilliumStack().%s::%d\n",__FILE__,__LINE__);
  Buffer *mBuf;
  Pst           stackPst;
  memset(&stackPst,0,sizeof(Pst));

  Pst           *pst = &stackPst;

  pst->dstEnt = ENTAL;
  pst->srcEnt = ENTIU;
  pst->event = SM_PEER_NODE_DOWN;
  pst->dstInst = 0;
  pst->srcInst = 0;

  pst->region = DFLT_REGION;
  pst->pool = DFLT_POOL;
  pst->prior = PRIOR0;
  pst->route = RTESPEC;
  pst->dstProcId = SFndProcId();
  pst->srcProcId = SFndProcId();
  pst->selector = 0; // priyatos LC

   S16 ret,res = 0;
   
   if ((res = SGetMsg(pst->region, pst->pool, &mBuf)) != ROK)
        {
            //printf("SGetMsg(pst->region, pst->pool, &mBuf))..error");
	    return res;
        }

   ret = SPstTsk(pst, mBuf);
   return ret;
}

#endif
/* ------------------------------------------------------------------------- */
