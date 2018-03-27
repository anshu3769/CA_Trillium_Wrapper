#include "INManager.hh"
#include <IN_TCAP/InvokeOutgoingInitialDP.hh>
#include <IN_TCAP/InvokeOutgoingEventReportBCSM.hh>
#include <IN_TCAP/InvokeOutgoingCallInformationReport.hh> 

#include <IN_TCAP/InvokeOutgoingApplyChargingReport.hh>

#include <IN_TCAP/InvokeOutgoingEventNotificationCharging.hh>

#include <IN_TCAP/ErrorOutgoingMissingParameter.hh>

#include <GenericComponent/GenericComponent.hh>
#include "INIncomingEvent.hh"
#include <cstring>

#include <CS2Common/CS2Config.hh> 
#include <OTCommon/OTStringUtil.hh> 

#include <asn1c++/asn-incl.h>
#define MAX_MSG_SIZE	1024  

long int selfPC;

INManager* INManager::ourInstance = 0;

int asNum( char c )
{
   if (std::isdigit(c)) return c - '0';
   return c - 'a' + 10;
}

INManager::INManager()
{
   TRACE_LOG(INLog,"INManager::INManager()");
   assert(ourInstance == 0);   

   ourInstance = this;
   ourDispatcher = 0;  
}

INManager::~INManager()
{
   TRACE_LOG(INLog,"INManager::~INManager()");

   if(ourInstance == this);
   ourInstance = 0;
}

INManager*
INManager::instance()
{
	TRACE_LOG(INLog,"INManager::instance()");

	assert(ourInstance != 0);
	return ourInstance;
}

bool 
INManager::setSuId(SuId suId)
{
	TRACE_LOG(INLog,"INManager::setSuId(SuId suId)");
	return instance()->doSetSuId(suId);
}

bool 
INManager::setSpId(SpId spId)
{
	TRACE_LOG(INLog,"INManager::setSpId(SpId spId)");
	return instance()->doSetSpId(spId);
}

bool
INManager::openReq(TCAPUserDialogueRep *dlg)
{
	TRACE_LOG(INLog,"INManager::Open_Req(TCAPUserDialogueRep *dlg)");
	return instance()->doOpenReq(dlg);
}

bool
INManager::openRsp(IeDlgId suDlgId,IeDlgId spDlgId,IeOpenEv *ieopenEv)
{
        TRACE_LOG(INLog,"INManager::Open_Rsp(IeDlgId suDlgId,IeDlgId spDlgId,IeOpenEv *ieopenEv)");
        return instance()->doOpenRsp(suDlgId,spDlgId,ieopenEv);
}

bool
INManager::oprReq(TCAPUserDialogue dlg)
{
	TRACE_LOG(INLog,"INManager::Opr_Req(TCAPUserDialogue *dlg)");
	return instance()->doOprReq(dlg);
}

bool
INManager::oprRsp(TCAPUserDialogue dlg )
{
	TRACE_LOG(INLog,"INManager::Opr_Rsp(TCAPUserDialogue dlg)");
	return instance()->doOprRsp(dlg);
}

bool
INManager::abortReq( TCAPUserDialogue dlg )
{
	TRACE_LOG(INLog,"INManager::Abrt_Req(TCAPUserDialogue dlg)");
	return instance()->doAbortReq(dlg);
}

bool
INManager::closeReq(Pst *pst,IeDlgId suDlgId,IeDlgId spDlgId, unsigned char endType)
{
	TRACE_LOG(INLog,"INManager::Close_Req(Pst *pst,IeDlgId suDlgId,IeDlgId spDlgId)");
	return instance()->doCloseReq(pst,suDlgId,spDlgId,endType);
}

SuId INManager::getSuId()    
{
   TRACE_LOG(INLog,"INManager::getSuId()");

   return instance()->doGetSuId();
}

SpId INManager::getSpId()            
{
  TRACE_LOG(INLog,"INManager::getSpId()");

  return instance()->doGetSpId();
}

bool
INManager::fillOpenEvent( TCAPDialogueRep *dlg, IeOpenEv *ieOpenEv)
{
  TRACE_LOG(INLog,"INManager::fillOpenEvent( TCAPDialogueRep )");

  ieOpenEv->pres = TRUE; 
  ieOpenEv->srcAddr.pres = TRUE; 
  ieOpenEv->srcAddr.sw= SW_ITU ; 
  ieOpenEv->srcAddr.ssfPres = TRUE; 
  ieOpenEv->srcAddr.ssf = SSRV_NAT;    
  ieOpenEv->srcAddr.niInd = INAT_IND;  
  ieOpenEv->srcAddr.rtgInd= RTE_SSN;
  ieOpenEv->srcAddr.pcInd = TRUE; 
  ieOpenEv->srcAddr.ssnInd = TRUE;;
  ieOpenEv->srcAddr.ssn = dlg->subSystemNumber();
  ieOpenEv->srcAddr.gt.format = 0;

  LOG(logDBG2)<< "SSN is = " << (int)ieOpenEv->srcAddr.ssn << "." <<endl;

  ieOpenEv->srcAddr.pc = selfPC; 
  LOG(logDBG2)<< "Self PC = " << (int)ieOpenEv->srcAddr.pc << "." <<endl;

  ieOpenEv->destAddr.pres = TRUE;
  ieOpenEv->destAddr.sw= SW_ITU ;
  ieOpenEv->destAddr.ssfPres = TRUE;
  ieOpenEv->destAddr.ssf = SSRV_NAT; 
  ieOpenEv->destAddr.niInd = INAT_IND; 
  LOG(logDBG2)<< "Switch is = " << (int)ieOpenEv->destAddr.sw << "." <<endl;
	
  ieOpenEv->destAddr.rtgInd= RTE_SSN;
  ieOpenEv->destAddr.pcInd = TRUE;   
  ieOpenEv->destAddr.ssnInd =TRUE;  
  ieOpenEv->destAddr.ssn = dlg->subSystemNumber();   
  ieOpenEv->destAddr.pc = dlg->signallingPointCode();
  ieOpenEv->destAddr.gt.format = 0;
  LOG(logDBG2)<< "Destination PC = " << (int)ieOpenEv->destAddr.pc << "." <<endl;
	
  ieOpenEv->priority = SN_PRI3;  

  return TRUE;
}

bool
INManager::fillPst(Pst *pst)
{
	TRACE_LOG(INLog,"INManager::fillPst()");

	pst->dstEnt = ENTIE;
        LOG(logDBG2)<<"destination entity is "<< (int) pst->dstEnt <<endl;

        pst->dstInst = 0;
        pst->srcInst = 0;

  	pst->srcEnt = ENTIU;
        LOG(logDBG2)<<"source entity is "<< (int) pst->srcEnt <<endl;

        pst->region = DFLT_REGION;
        LOG(logDBG2)<<"default region is "<< (int) pst->region <<endl;

        pst->pool = DFLT_POOL;
        LOG(logDBG2)<<"pool is "<< (int) pst->pool <<endl;

        pst->prior = PRIOR0;
        LOG(logDBG2)<<"priority is "<< (int) pst->prior <<endl;

        pst->route = RTESPEC;
        LOG(logDBG2)<<"route is "<< (int) pst->route <<endl;

        pst->dstProcId = SFndProcId();
        LOG(logDBG2)<<"destination procId is "<< (int) pst->dstProcId <<endl;

        pst->srcProcId = SFndProcId();
        LOG(logDBG2)<<"source procId is "<< (int) pst->srcProcId <<endl;

        pst->selector = 0;
        LOG(logDBG2)<<"selector value is "<< (int) pst->selector <<endl;
        return TRUE;
}

bool
INManager::fillAbrtEv(TCAPDialogueRep *dlg, IeAbrtEv *ieAbrtEv)
{
  TRACE_LOG(INLog, "INManager::fillAbrtEv()");

  ieAbrtEv->pres = TRUE;
	
  ieAbrtEv->srcAddr.pres = TRUE;
  ieAbrtEv->srcAddr.sw= SW_ITU ;
  ieAbrtEv->srcAddr.ssfPres = TRUE;
  ieAbrtEv->srcAddr.ssf = SSRV_NAT;    

  ieAbrtEv->srcAddr.niInd = INAT_IND;   
  ieAbrtEv->srcAddr.rtgInd= RTE_SSN;  
  ieAbrtEv->srcAddr.pcInd = TRUE; 
  ieAbrtEv->srcAddr.ssnInd =TRUE; 
  ieAbrtEv->srcAddr.ssn = dlg->subSystemNumber(); 
  ieAbrtEv->srcAddr.pc = selfPC; 

  ieAbrtEv->destAddr.pres = TRUE;
  ieAbrtEv->destAddr.sw= SW_ITU ;
  ieAbrtEv->destAddr.ssfPres = TRUE;
  ieAbrtEv->destAddr.ssf = SSRV_NAT; 
  ieAbrtEv->destAddr.niInd = INAT_IND;   
  ieAbrtEv->destAddr.rtgInd= RTE_SSN;
  ieAbrtEv->destAddr.pcInd = TRUE;
  ieAbrtEv->destAddr.ssnInd = TRUE; 
  ieAbrtEv->destAddr.ssn = dlg->subSystemNumber();  
  ieAbrtEv->destAddr.pc = dlg->signallingPointCode(); 

  ieAbrtEv->priority = SN_PRI3;

  return TRUE; 
}
