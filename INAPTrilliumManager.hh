#ifndef _INTRILLIUMMANAGER_HH
#define _INTRILLIUMMANAGER_HH

#include "INDef.h"
#include "INMaps.hh"
#include "INMsgEvent.hh"
class INIncomingEventRep;

#define IN_TMR 10   
#define TRUE 1

typedef int8 InvokeId;
class INManager
{
  public:
	INManager();
	virtual ~INManager();
	
	static bool openReq(TCAPUserDialogueRep *);
        static bool oprReq(TCAPUserDialogue dlg); 
        static bool oprRsp(TCAPUserDialogue dlg ); 
	static bool abortReq(TCAPUserDialogue dlg);   
        static bool closeReq(Pst *pst,IeDlgId suDlgId,IeDlgId spDlgId, unsigned char );   
        static bool openRsp(IeDlgId suDlgId,IeDlgId spDlgId,IeOpenEv *ieopenEv);  
	bool fillOpenEvent(TCAPDialogueRep *, IeOpenEv *); 
        bool fillPst(Pst *);
	bool fillAbrtEv(TCAPDialogueRep *, IeAbrtEv *);  
	 
        static INMsgEventRep* MessageReceived(INMsg *inapMsg,CSAContext *csaContext , int InapMsgType)
	{
	  return instance()->doMessageReceived(inapMsg, csaContext, InapMsgType); 
	}

  	static bool setSuId(SuId suID);  
	static bool setSpId(SpId spID);   
	static SuId getSuId();            
	static SpId getSpId();            
  
	static INIncomingEventRep *getINinEvt(){ return instance()->doGetINinEvt();}
	static TCAPInstance getTCAPInstance(){return instance()->instance_;}   
 protected:
	//these functions are implemented in INManagerImpl
	virtual bool doSetSuId(SuId suId)=0;
	virtual bool doSetSpId(SpId spId)=0;
 	virtual bool doOpenReq(TCAPUserDialogueRep *)=0;
        virtual bool doOprReq(TCAPUserDialogue dlg)=0; 
	virtual bool doOprRsp(TCAPUserDialogue dlg)=0; 
	virtual bool doAbortReq(TCAPUserDialogue dlg)=0;
	virtual bool doCloseReq(Pst *pst,IeDlgId suDlgId,IeDlgId spDlgId,uint8)=0;   
        virtual bool doOpenRsp(IeDlgId suDlgId,IeDlgId spDlgId,IeOpenEv *ieOpenEv)=0; 
	virtual INMsgEventRep* doMessageReceived(INMsg *inapMsg,CSAContext *csaContext, int InapMsgType )=0;

	virtual SuId doGetSuId()=0;
        virtual SpId doGetSpId()=0;
        
        virtual INIncomingEventRep *doGetINinEvt()=0; 	

	OTEventDispatcher *ourDispatcher;
        TCAPInstance instance_; 
	SuId suId;
	SpId spId;

        INIncomingEventRep *inapInEvt;
private:
	static	INManager* instance();
	static	INManager* ourInstance;
};
#endif
