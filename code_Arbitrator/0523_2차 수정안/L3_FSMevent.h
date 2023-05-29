typedef enum L3_event
{
    L3_event_msgRcvd = 2,
    L3_event_dataToSend = 4,
    L3_event_dataSendCnf = 5,
    L3_event_recfgSrcIdCnf = 6,
    L3_event_quizstartPDUSend = 7,
    L3_event_demandPDURcvd = 8,
    L3_event_questSDUinput = 9,
    L3_event_answerIDinput = 10,
    L3_event_answerPDU = 11,
    L3_event_timeout = 12
} L3_event_e;


void L3_event_setEventFlag(L3_event_e event);
void L3_event_clearEventFlag(L3_event_e event);
void L3_event_clearAllEventFlag(void);
int L3_event_checkEventFlag(L3_event_e event);