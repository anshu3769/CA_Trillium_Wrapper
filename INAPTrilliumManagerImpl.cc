#include "INManagerImpl.hh"
#include  <string.h>
#include  <TCAP/TCAPUserDialogue.hh>
#include  <CallSegmentAssociation/CSAContext.hh>

INManagerImpl::INManagerImpl():INManager()
{
  TRACE("INManagerImpl::INManagerImpl()");
}

INManagerImpl::~INManagerImpl()
{
  TRACE("INManagerImpl::~INManagerImpl");
}

bool INManagerImpl::setDispatcher(OTEventDispatcher* dispatcher)
{
   TRACE("INManagerImpl::setDispatcher");

   if(ourDispatcher==0)
   {
      // no existsing dispatcher, so set
      ourDispatcher = dispatcher;
      return true;
   }
   else
   {
      // dispatcher already exists, fail
	return false;
   }

   return true;
}   

INMsgEventRep* INManagerImpl::doMessageReceived(INMsg *inapMsg,CSAContext *csaContext, int InapMsgType)
{
  TRACE("INManagerImpl::doMessageReceived()");
  INMsgEventRep* ev = new INMsgEventRep(ourDispatcher,
								instance_,
								inapMsg,
								csaContext,
								InapMsgType);
  
  return ev;

}

bool
INManagerImpl::doOpenReq(TCAPUserDialogueRep *dialog)
{
  TRACE("INManagerImpl::doOpen_Req(TCAPUserDialogueRep *dlg)");

  TCAPDialogueRep *dlg = dynamic_cast<TCAPDialogueRep*>(dialog);

  S16 ret = ROK;
  Pst pst; 
  IeDlgId suDlgId=0;
  IeDlgId spDlgId=0;
  IeOpenEv ieOpenEv;
/* KOMAL for SSN 2 */
 SpId spId=0;
 for( int i=0: i<NO_OF_SSN; i++)
   {
    if(dlg->subSystemNumber() == arr_ssn[i])
     {
      spId=i;
      break;
     }
   }
  memset(&pst,0,sizeof(Pst));
  memset(&ieOpenEv,0,sizeof(IeOpenEv));
 
  pst.event = IET_EVTOPENREQ;
  LOG(logDBG2)<<"Event = "<< (int) pst.event<< endl;	

  if(fillOpenEvent(dlg, &ieOpenEv) && fillPst(&pst)) 
  {	
    suDlgId = dlg->localDialogueId();
    
    //call to primitive of IN User/wrapper lower inteface that will map to corresponding primitive of IN upper inteface. 

    //if(!(ret = IuLiIetOpenReq(&pst, getSpId(), suDlgId, spDlgId,&ieOpenEv)))
    if(!(ret = IuLiIetOpenReq(&pst, spId, suDlgId, spDlgId,&ieOpenEv)))
   {
     return TRUE;
   }
   else
   {
     LOG( logERRO ) << "Operation not Successful !!" << endl ;	
     return FALSE;
   }
  }
  else
  {
    LOG( logERRO ) << "fillOpenEvent() and fillPst() not Successful !!" << endl ;	
    return FALSE;
  }
}

bool
INManagerImpl::doOprReq(TCAPUserDialogue dialog)
{
  TRACE("INManagerImpl::doOprReq(TCAPUserDialogueRep )");
  S16 ret = ROK; 
  IeInvokeId lnkId;
  lnkId.pres = 0; 
  InvokeId invkId;
  IeInvokeId invId;
  IeOprCode oprCode;
  U8 oprClass;
  U16 tmrVal;
  Buffer *uBuf;
  Pst pst;

  TCAPDialogueRep *dlg = dynamic_cast<TCAPDialogueRep*>(dialog.cast());

  memset(&pst,0,sizeof(Pst));
  fillPst(&pst);
    	
  invkId = dlg->opClassCodeEvent.invkId;
  LOG(logDBG2)<< "Invoke ID = " << (int)invkId << endl;
          
  invId.pres = TRUE;
  invId.val = invkId;

  oprCode.pres = TRUE;
  oprCode.u.oprInt.pres = TRUE;
  oprCode.oprCodeSpec = IET_INIT_DP;
  oprCode.oprCodeType = IET_TKN_INT;
  oprCode.u.oprInt.val =  dlg->opClassCodeEvent.oprCode;
  oprClass = (unsigned char)(dlg->opClassCodeEvent.oprClass);

  IeDlgId suDlgId = dlg->localDialogueId();
  IeDlgId spDlgId = dlg->remoteDialogueId();

  LOG(logDBG2)<< "LocalDialogueId() = " << (int)dlg->localDialogueId() << endl;
  LOG(logDBG2)<< "RemoteDialogueId() = " << (int)dlg->remoteDialogueId() << endl;
  LOG(logDBG2)<< "Service Key = " << (int)dlg->opClassCodeEvent.ieOprEv.invEv.initDpInv.svcKey.val << endl;
  tmrVal = 2; 
  uBuf = NULLP;
	
  pst.event = IET_EVTOPRREQ;
 /* KOMAL for SSN 2 */
 SpId spId=0;
 for( int i=0: i<NO_OF_SSN; i++)
   {
    if(dlg->subSystemNumber() == arr_ssn[i])
     {
      spId=i;
      break;
     }
   }

 // if(!(ret = IuLiIetOprReq(&pst,getSpId(),suDlgId,spDlgId,&invId,&lnkId,&oprCode,oprClass,tmrVal,(U64 )(&(dlg->opClassCodeEvent.ieOprEv)),uBuf)))
  if(!(ret = IuLiIetOprReq(&pst, spId,suDlgId,spDlgId,&invId,&lnkId,&oprCode,oprClass,tmrVal,(U64 )(&(dlg->opClassCodeEvent.ieOprEv)),uBuf)))
  {
    // reInitialize the event structure
    memset(&(dlg->opClassCodeEvent.ieOprEv),0,sizeof(IeOprEv));   
    return TRUE;	
  }
      
  // reInitialize the event structure
  memset(&(dlg->opClassCodeEvent.ieOprEv),0,sizeof(IeOprEv));   

  return TRUE;      
}

bool
INManagerImpl::doOprRsp(TCAPUserDialogue dialog)
{
  TRACE("INManagerImpl::doOprRsp(TCAPUserDialogue dialog)");
  S16 ret = ROK;
  IeInvokeId lnkId;
  lnkId.pres = FALSE;  
  IeDlgId suDlgId=0;
  IeDlgId spDlgId=0;
  Pst pst;
  U8 oprClass;
  U16 tmrVal;
  //SpId spId;
  IeInvokeId invId;
  Buffer *uBuf;
  IeOprCode oprCode;
  IeOprErr oprErr;
  IeOprEv oprEv;
  InvokeId invkId;

  memset(&oprEv,0,sizeof(IeOprEv));
  memset(&oprErr,0,sizeof(IeOprErr));

  TCAPDialogueRep *dlg = dynamic_cast<TCAPDialogueRep*>(dialog.cast());

  oprErr.pres = FALSE;

  invkId = dlg->opClassCodeEvent.invkId;
  LOG(logDBG2)<< "Invoke ID = " << (int)invkId << endl;
          
  invId.pres = TRUE;
  invId.val = invkId;

  oprClass = (unsigned char)(dlg->opClassCodeEvent.oprClass);

  suDlgId = dlg->localDialogueId();
  spDlgId = dlg->remoteDialogueId();

  LOG(logDBG2)<< "dlg->localDialogueId() = " << (int)dlg->localDialogueId() << endl;
  LOG(logDBG2)<< "dlg->remoteDialogueId() = " << (int)dlg->remoteDialogueId() << endl;

  memset(&pst,0,sizeof(Pst));
  fillPst(&pst);

 // oprClass =  IET_OPR_CLASS_4;
  tmrVal = IN_TMR;  
/* KOMAL for SSN 2 */
 SpId spId=0;
 for( int i=0: i<NO_OF_SSN; i++)
   {
    if(dlg->subSystemNumber() == arr_ssn[i])
     {
      spId=i;
      break;
     }
   }

  uBuf = NULLP;	
   	     
    oprCode.pres = TRUE;
    oprCode.u.oprInt.pres = TRUE;
    oprCode.oprCodeType = IET_TKN_INT;

    LOG(logDBG2)<<"Operation with opcode "<< (int)dlg->opClassCodeEvent.oprCode<<endl;

    switch((int)dlg->opClassCodeEvent.oprCode)
   {
      case IET_EVT_RPT_BCSM:
        oprCode.oprCodeSpec = IET_EVT_RPT_BCSM;
        oprCode.u.oprInt.val = dlg->opClassCodeEvent.oprCode;
	pst.event = IET_EVTOPRREQ;   
      break;  

      case IET_CALL_INFO_RPT:
        oprCode.oprCodeSpec = IET_CALL_INFO_RPT;
        oprCode.u.oprInt.val = dlg->opClassCodeEvent.oprCode;
        pst.event = IET_EVTOPRREQ;   
      break;

      case IET_EVT_NOT_CHG:
        oprCode.oprCodeSpec = IET_EVT_NOT_CHG;
        oprCode.u.oprInt.val = dlg->opClassCodeEvent.oprCode;
        pst.event = IET_EVTOPRREQ;   
      break;
	  
      case IET_ACTV_TST:  
        oprCode.oprCodeSpec = IET_ACTV_TST;
        oprCode.u.oprInt.val = dlg->opClassCodeEvent.oprCode;
        pst.event = IET_EVTOPRRSP;  
      break;

      case IET_APP_CHG_RPT:
        oprCode.oprCodeSpec = IET_APP_CHG_RPT;
        oprCode.u.oprInt.val = dlg->opClassCodeEvent.oprCode;
	pst.event = IET_EVTOPRREQ;   
      break;

      case IET_RE_MIS_PARAM:
        pst.event = IET_EVTOPRRSP;

        LOG(logDBG2)<<"OprRsp for opcode = " << (int)oprCode.u.oprInt.val << endl;
  	oprErr.pres = TRUE;
        oprErr.evnt = IET_ERR_RE;

        oprErr.usrErr.pres = TRUE;
        oprErr.usrErr.errCodeSpec = IET_RE_MIS_PARAM;

        oprErr.usrErr.usrErrType = IET_TKN_INT;
        oprErr.usrErr.u.errInt.pres = TRUE;
        oprErr.usrErr.u.errInt.val = IET_RE_MIS_PARAM;

        oprErr.prvErr.pres = FALSE;

        oprCode.pres = FALSE;
        pst.event = IET_EVTOPRRSP;
      break;

      case IET_PROMPT_CUI:
        oprCode.oprCodeSpec = IET_PROMPT_CUI;
        oprCode.u.oprInt.val = dlg->opClassCodeEvent.oprCode;
        pst.event = IET_EVTOPRRSP;
      break; 

      case IET_RE_IMP_CALL_RSP:
        oprErr.pres = TRUE;
        oprErr.evnt = IET_ERR_RE;
        oprErr.usrErr.pres = TRUE;
        oprErr.usrErr.errCodeSpec = IET_RE_IMP_CALL_RSP;
              
        oprErr.usrErr.usrErrType = IET_TKN_INT;
        oprErr.usrErr.u.errInt.pres = TRUE;
        oprErr.usrErr.u.errInt.val = IET_RE_IMP_CALL_RSP;

        oprErr.prvErr.pres = FALSE;
        oprCode.pres = FALSE;

        pst.event = IET_EVTOPRRSP;
      break;
      
      case IET_RE_UNAVAIL_RES:
        oprErr.pres = TRUE;
        oprErr.evnt = IET_ERR_RE;
        oprErr.usrErr.pres = TRUE;
        oprErr.usrErr.errCodeSpec = IET_RE_UNAVAIL_RES;
              
        oprErr.usrErr.usrErrType = IET_TKN_INT;
        oprErr.usrErr.u.errInt.pres = TRUE;
        oprErr.usrErr.u.errInt.val = IET_RE_UNAVAIL_RES;

        oprErr.prvErr.pres = FALSE;
        oprCode.pres = FALSE;

        pst.event = IET_EVTOPRRSP;
      break;

      case IET_SPEC_RES_RPT:
        oprCode.oprCodeSpec = IET_SPEC_RES_RPT;
        oprCode.u.oprInt.val = dlg->opClassCodeEvent.oprCode;
	pst.event = IET_EVTOPRREQ;   
        lnkId.pres = TRUE;  
        lnkId.val  = invId.val; 
      break;
   }
	 
   memcpy(&oprEv,&(dlg->opClassCodeEvent.ieOprEv),sizeof(IeOprEv));

   LOG(logDBG2)<<" IuLiIetOprReq() with opcode : " << (int)dlg->opClassCodeEvent.oprCode << endl;

   if(pst.event == IET_EVTOPRREQ)
   {
     if((!(ret = IuLiIetOprReq(&pst,spId,suDlgId,spDlgId,&invId,&lnkId,&oprCode,oprClass,tmrVal,(U64)(&oprEv),uBuf))))
     {
       LOG(logDBG2)<<" IuLiIetOprReq() Successful" << endl;

      // reInitialize the event structure
      memset(&(dlg->opClassCodeEvent.ieOprEv),0,sizeof(IeOprEv));   

      return TRUE;
     }
     else
     {
       LOG(logDBG2)<<" IuLiIetOprReq() Failed." << endl;
      // reInitialize the event structure
      memset(&(dlg->opClassCodeEvent.ieOprEv),0,sizeof(IeOprEv));   

      return FALSE;   
     }
   }
   else
   if(pst.event == IET_EVTOPRRSP)
   {
     if((!(ret = IuLiIetOprRsp(&pst,spId,suDlgId,spDlgId,&invId,&oprCode,&oprErr,(U64)(&oprEv),uBuf))))
     {
       ret = IuLiIetDelimReq(&pst,spId,suDlgId,spDlgId);
       // reInitialize the event structure
       memset(&(dlg->opClassCodeEvent.ieOprEv),0,sizeof(IeOprEv));   
 
       return TRUE;
     } 
     else
     { 
       LOG(logDBG2)<<" IuLiIetOprRsp() Failed." << endl;
       // reInitialize the event structure
       memset(&(dlg->opClassCodeEvent.ieOprEv),0,sizeof(IeOprEv));   

       return FALSE;
     }
   }
   else
   {
    LOG(logERRO)<<" Invalid Request. "<<endl;

    // reInitialize the event structure
    memset(&(dlg->opClassCodeEvent.ieOprEv),0,sizeof(IeOprEv));   

    return FALSE; 
   } 

}

bool
INManagerImpl::doAbortReq(TCAPUserDialogue dialog)
{
  TRACE("INManagerImpl::doAbortReq(TCAPUserDialogue dialog)");
  S16 ret = ROK;
  IeAbrtEv ieAbrtEv;
  //SuId suId;
  Pst pst;

  TCAPDialogueRep *dlg = dynamic_cast<TCAPDialogueRep*>(dialog.cast());

  memset(&ieAbrtEv,0,sizeof(IeAbrtEv));
  memset(&pst,0,sizeof(Pst));

  fillPst(&pst);

  U32 localDialogue = 0;
  U32 tcapDialogue = 0;

  localDialogue = dlg->localDialogueId();
  tcapDialogue = dlg->remoteDialogueId();

  LOG(logDBG2)<<"LocalDialogue "<< (int)localDialogue <<endl;
  LOG(logDBG2)<<"RemoteDialogue "<< (int)tcapDialogue <<endl;
   	 
  pst.event = IET_EVTABRTREQ;
  //suId = getSuId();
/* KOMAL for SSN 2 */
 SuId suId=0;
 for( int i=0: i<NO_OF_SSN; i++)
   {
    if(dlg->subSystemNumber() == arr_ssn[i])
     {
      suId=i;
      break;
     }
   }

  fillAbrtEv(dlg, &ieAbrtEv);

  ret = IuLiIetAbrtReq(&pst, suId, localDialogue, tcapDialogue, &ieAbrtEv);

  if(ret==0)
    LOG(logDBG2)<<" IuLiIetAbrtReq():Successful" <<endl;
  else
    LOG(logDBG2)<<" IuLiIetAbrtReq():Failed " <<endl;

   return ret; //added by Shruti:30/12/14 
   }

bool
INManagerImpl::doCloseReq(Pst *post,IeDlgId suDlgId,IeDlgId spDlgId,unsigned char endType)
{
  TRACE("INManagerImpl::doCloseReq(Pst *pst,IeDlgId suDlgId,IeDlgId spDlgId)");
  S16 ret  = ROK;
  SpId spId = getSpId();
  Pst pst;
  IeCloseEv ieCloseEv;

  memset(&ieCloseEv,0,sizeof(IeCloseEv));	
  memset(&pst,0,sizeof(Pst));	
  fillPst(&pst);
  pst.event = IET_EVTCLOSEREQ; 

  ieCloseEv.pres = TRUE;
  ieCloseEv.endType = endType; 

  ret  = IuLiIetCloseReq(&pst,spId,suDlgId,spDlgId,&ieCloseEv);

  return TRUE; 
}

