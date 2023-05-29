typedef enum L3_event
{   
    // #if 0
    // L3_event_msgRcvd = 2,
    // L3_event_dataToSend = 4,
    // L3_event_dataSendCnf = 5,
    // L3_event_recfgSrcIdCnf = 6
    // #endif
    L3_event_quizstartPDUSend = 2,
    L3_event_demandPDURcvd = 4,
    L3_event_questSDUinput = 5,
    L3_event_answerIDinput = 6,
    L3_event_answerPDURcvd = 7,
    L3_event_timeout = 8
} L3_event_e;


void L3_event_setEventFlag(L3_event_e event);
void L3_event_clearEventFlag(L3_event_e event);
void L3_event_clearAllEventFlag(void);
int L3_event_checkEventFlag(L3_event_e event);