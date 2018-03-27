#ifndef _INTRILLIUMMANAGERIMPL_HH
#define _INTRILLIUMMANAGERIMPL_HH

#include "INManager.hh" 
#include "INIncomingEvent.hh"

class INManagerImpl: public INManager
{
  public:

  INManagerImpl(); 	
  virtual ~INManagerImpl();
 
  //To establish a dialouge with the peer entity.it will always be called when InitialDP method for an IN call is hit.
  bool doOpenReq(TCAPUserDialogueRep *dlg); 

  bool doOprReq(TCAPUserDialogue dlg); 
 
  bool doOprRsp(TCAPUserDialogue dlg); 

  bool doAbortReq(TCAPUserDialogue dlg);   

  bool doCloseReq(Pst *pst,IeDlgId suDlgId,IeDlgId spDlgId, unsigned char); 

  bool doOpenRsp(IeDlgId suDlgId,IeDlgId spDlgId,IeOpenEv *ieOpenEv);

  bool registerINinEv(INIncomingEventRep *inapInEv){ inapInEvt = inapInEv; }
  void setInstance(TCAPInstance instance) { instance_ = instance; }  

  bool setDispatcher(OTEventDispatcher *theDispatcher) ; 
  virtual INMsgEventRep* doMessageReceived(INMsg *inapMsg,CSAContext *csaContext, int InapMsgType ); 

  virtual bool doSetSuId(SuId suId){this->suId= suId;}
  virtual bool doSetSpId(SpId spId){this->spId = spId;}

  virtual SuId doGetSuId() {return suId;}
  virtual SpId doGetSpId() {return spId;}


  INIncomingEventRep *doGetINinEvt(){ return inapInEvt;}
	
};
#endif // _INTRILLIUMMANAGERIMPL_HH
