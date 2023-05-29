#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"


//FSM state -------------------------------------------------
#define L3STATE_Collect_Participants      0
#define L3STATE_Question_Transmission     1
#define L3STATE_Answer_Transmission       2


//state variables
//static uint8_t main_state = L3STATE_IDLE; //protocol state
static uint8_t main_state = L3STATE_CollectParticipants; //protocol state
static uint8_t prev_state = main_state;

//parameter
uint8_t PartiID[2];
uint8_t partiNum;
//uint8_t seqNum = 0; // sequence number???

//PDU contect/size
uint8_t quizStart_Pdu[200];
uint8_t collectComplete_Pdu[200];
uint8_t partiConfirmed_Pdu[200];
uint8_t noAnswer_Pdu[200];
uint8_t total_Pdu[200];
uint8_t pduSize;

static uint8_t pdu[1030];

//SDU (input)
static uint8_t originalWord[1030];
static uint8_t wordLen=0;

static uint8_t quizStart_Sdu[] = "QnA protocol Start!! Please send you 'demand PDU'";
static uint8_t Len_quizStart = sizeof(quizStart_Sdu);

static uint8_t partiConfirmed_Sdu[] = "You have become a participant!!";
static uint8_t Len_partiConfirmed = sizeof(partiConfirmed_Sdu);

static uint8_t collectComplete_Sdu[] = "Participant recruitment completed. Let's start the quiz!!";
static uint8_t Len_collectComplete = sizeof(collectComplete_Sdu);

static uint8_t noAnswer_Sdu[] = "There is no answer....";
static uint8_t Len_noAnswer = sizeof(noAnswer_Sdu);

static uint8_t sdu[1030];

//serial port interface
static Serial pc(USBTX, USBRX);
static uint8_t myDestId;
static uint8_t answer_id;

static uint8_t flag_aid = 0;

//application event handler : generating SDU from keyboard input
static void L3service_processInputWord(void)
{
    char c = pc.getc();
    if (main_state == L3STATE_Collect_Participants)
    {
        if (c == '\n' || c == '\r')
        {
            originalWord[wordLen++] = '\0';
            L3_event_setEventFlag(L3_event_quizstartPDUSend);
        }
        else
        {
            originalWord[wordLen++] = c;
            #if 0
            if (wordLen >= L3_MAXDATASIZE-1)
            {
                originalWord[wordLen++] = '\0';
                L3_event_setEventFlag(L3_event_dataToSend);
                pc.printf("\n max reached! word forced to be ready :::: %s\n", originalWord);
            }
            #endif
        }
    }
    else if (main_state == L3STATE_Question_Transmission)
    {
        if (c == '\n' || c == '\r')
        {
            originalWord[wordLen++] = '\0';

            if (flag_aid == 0)
                L3_event_setEventFlag(L3_event_answerIDinput);
            else
                L3_event_setEventFlag(L3_event_questSDUinput);
            debug_if(DBGMSG_L3, "word is ready! ::: %s\n", originalWord);
        }
        else
        {
            originalWord[wordLen++] = c;
            #if 0
            if (wordLen >= L3_MAXDATASIZE-1)
            {
                originalWord[wordLen++] = '\0';
                L3_event_setEventFlag(L3_event_dataToSend);
                pc.printf("\n max reached! word forced to be ready :::: %s\n", originalWord);
            }
            #endif
        }
        
    }
}



void L3_initFSM(uint8_t destId)
{

    myDestId = destId;
    //initialize service layer
    pc.attach(&L3service_processInputWord, Serial::RxIrq);

    pc.printf("Give a word to send : ");
}

void L3_FSMrun(void)
{  
    //initialization
    pc.printf("------------------ QnA protocol starts! --------------------------\n");
    pc.printf("-----------------It starts when you press any type----------------\n")
    arqEvent_clearAllEventFlag();

    if (prev_state != main_state)
    {
        debug_if(DBGMSG_L3, "[L3] State transition from %i to %i\n", prev_state, main_state);
        prev_state = main_state;
    }

    //FSM should be implemented here! ---->>>>
    switch (main_state)
    {
        case L3STATE_Collect_Participants:
            if(L3_event_checkEventFlag(L3_event_quizstartPDUSend)) //quizstartPDU보냈을 때
            {
                // 아무 문자나 치게 되면 자동으로 QuizstartPDU전송되는 거
                pduSize = L3_MSG_encode_QUIZSTART(quizStart_Pdu, quizStart_Sdu, Len_quizStart);
                debug("COLLECT PARTI state : quizStartPDU transmitted - (len:%i) %s\n", Len_quizStart, quizStart_Sdu);
                L3_LLI_dataReqFunc(quizStart_Pdu, pduSize, 255); //sendData
                partiNum = 0;
                main_state = L3STATE_Collect_Participants;
                L3_event_clearEventFlag(L3_event_quizstartPDUSend);
            }
            else if(L3_event_checkEventFlag(L3_event_demandPDURcvd))
            {
                uint8_t srcId = L3_LLI_getSrcId();
                PartiID[partiNum++] = srcId;

                if(partiNum >= 2)
                {
                    //조원과 공유필요) "너가 참여자다"를 알려주는 PDU 전송 단계 추가함.
                    //partiConfirmed Pdu
                    pduSize = L3_MSG_encode_PARTICONFIRMED(partiConfirmed_Pdu, partiConfirmed_Sdu, seqNum, Len_partiConfirmed);
                    debug("COLLECT PARTI state : partiConfirmedPDU transmitted - (len:%i) %s\n", Len_partiConfirmed, partiConfirmed_Sdu);
                    L3_LLI_dataReqFunc(partiConfirmed_Pdu, pduSize, srcId);

                    //collectComplete Pdu
                    pduSize = L3_MSG_encode_COLLECTCOMPLETE(collectComplete_Pdu, collectComplete_Sdu, seqNum, Len_collectComplete);
                    debug("COLLECT PARTI state : collectComplete transmitted - (len:%i) %s\n", Len_collectComplete, collectComplete_Sdu);
                    L3_LLI_dataReqFunc(collectComplete_Pdu, pduSize, 255);
                    //조원과 공유필요) partiNum으로 지정참여자에게만 보내는 게 어렵다고 하심. 참가자 부분에서 거르도록코드 짜는 게 어떻냐..

                    pc.printf("INPUT Answer ID :\n");
                    main_state = L3STATE_Question_Transmission;
                    L3_event_clearEventFlag(L3_event_demandPDURcvd);
                }
                else
                {
                    //partiConfirmed Pdu
                    pduSize = L3_MSG_encode_PARTICONFIRMED(partiConfirmed_Pdu, partiConfirmed_Sdu, seqNum, Len_partiConfirmed);
                    debug("COLLECT PARTI state : partiConfirmedPDU transmitted - (len:%i) %s\n", Len_partiConfirmed, partiConfirmed_Sdu);
                    L3_LLI_dataReqFunc(partiConfirmed_Pdu, pduSize, srcId);

                    main_state = L3STATE_Collect_Participants;
                    L3_event_clearEventFlag(L3_event_demandPDURcvd);

                }
                break;
            }
            break;
        case L3STATE_Question_Transmission:
            if (L3_event_checkEventFlag(L3_event_answerIDinput))
            {
                answer_id = atoi(originalWord); // atoi 문자열을 정수로 변환
                flag_aid= 1;
                debug("QTX state : AID : %i\n". answer_id);
                pc.printf("INPUT Question :\n");
                main_state = L3STATE_Question_Transmission;
                L3_event_clearEventFlag(L3_event_answerIDinput);
            }
            else if (L3_event_checkEventFlag(L3_event_questSDUinput))
            {
                L3_MSG_encode_QUESTION(pdu, seqNum, answer_id, wordLen, originalWord);
                debug("QTX state : Q transmitted - (AID:%i, len:%i) %s\n", answer_id, wordLen, originalWord);
                uint8_t questSize = sizeof(pdu);
                L3_LLI_dataReqFunc(pdu, questSize, 255);
                L3_timer_startTimer(); //Q) 타이머 간격 정의하기

                main_state = L3STATE_Answer_Transmission;
                L3_event_clearEventFlag(L3_event_questSDUinput);
            }
            break;
        case L3STATE_Answer_Transmission:
            if (L3_event_answerPDURcvd)
            {// Q) PDU에서 SrcId 어떻게 뽑아와?
                srcId = L3_LLI_getSrcId();
                sdu = L3_LLI_getMsgPtr();
                if (answer_id == srcId)
                {
                    L3_timer_stopTimer();
                    uint8_t total_Sdu[] = sdu;
                    uint8_t Len_total = sizeof(total_Sdu);
                    pduSize = L3_MSG_encode_TOTAL(total_Pdu, total_Sdu, Len_total);
                    debug("ATX state : total PDU transmitted - (AID:%i, len:%i) %s\n", answer_id, Len_total, total_Sdu);
                    L3_LLI_dataReqFunc(total_Pdu, pduSize, 255);

                    main_state = L3STATE_Collect_Participants;
                    L3_event_clearEventFlag(L3_event_answerPDURcvd);
                }
                else
                break;

            }
            else (L3_event_timeout)
            {
                pduSize = L3_MSG_encode_NOANSWER(noAnswer_Pdu, noAnswer_Sdu, Len_noAnswer);
                debug("ATX state : noAnswer PDU transmitted - (AID:%i, len:%i) %s\n", answer_id, Len_noAnswer, noAnswer_Sdu);
                L3_LLI_dataReqFunc(noAnswer_Pdu, pduSize, 255);
                main_state = L3STATE_Collect_Participants;
                L3_event_clearEventFlag(L3_event_timeout);
            }
            break;

        default :
            break;
    }
}