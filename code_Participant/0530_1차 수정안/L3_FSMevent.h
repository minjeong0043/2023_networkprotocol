typedef enum L3_event
{
    L3_event_quizstartPduRcvd = 2,
    L3_event_partiConfirmedPduRcvd = 4,
    L3_event_collectCompletePduRcvd = 5,
    L3_event_questionPduRcvd = 6,
    L3_event_totalPduRcvd = 7,
    L3_event_noAnswerPduRcvd =8,
    L3_event_answerInput = 9,
} L3_event_e;


void L3_event_setEventFlag(L3_event_e event);
void L3_event_clearEventFlag(L3_event_e event);
void L3_event_clearAllEventFlag(void);
int L3_event_checkEventFlag(L3_event_e event);