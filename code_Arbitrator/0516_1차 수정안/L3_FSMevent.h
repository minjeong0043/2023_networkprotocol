typedef enum L3_event
{
    L3_event_msgRcvd = 2,
    L3_event_dataToSend = 4,
    L3_event_dataSendCnf = 5,
    L3_event_recfgSrcIdCnf = 6,
    L3_event_quizstartPDUSend = 7,//event a
    L3_event_demandPDURcvd = 8,//event b
    L3_event_compPartiCollect = 9, // event c 참여자 다 모임
    L3_event_questinput = 10, // event d Question 입력받음
    L3_event_timerexpiry = 11, // event e
    L3_event_answerRcvd = 12 // event f
} L3_event_e;


void L3_event_setEventFlag(L3_event_e event);
void L3_event_clearEventFlag(L3_event_e event);
void L3_event_clearAllEventFlag(void);
int L3_event_checkEventFlag(L3_event_e event);