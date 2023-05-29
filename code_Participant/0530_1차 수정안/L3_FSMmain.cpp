#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"


//FSM state -------------------------------------------------
#define L3STATE_ConnectArbitrator   0
#define L3STATE_QuestionReception   1
#define L3STATE_AnswerTransmission  2
#define L3STATE_AnswerReception     3


//state variables
static uint8_t main_state = L3STATE_IDLE; //protocol state
static uint8_t prev_state = main_state;

//PDU context/size
uint8_t demand_Pdu[200];
uint8_t answer_Pdu[200];
uint8_t pduSize;

static uint8_t pdu[1030];

//SDU (input)
static uint8_t originalWord[1030];
static uint8_t wordLen=0;

static uint8_t sdu[1030];

static uint8_t demand_Sdu[] = "I want to enjoy QnA protocol!!";
static uint8_t Len_demand = sizeof(demand_Sdu);


//serial port interface
static Serial pc(USBTX, USBRX);
static uint8_t myDestId;
static uint8_t arbiId = 1;

//application event handler : generating SDU from keyboard input
static void L3service_processInputWord(void)
{
    char c = pc.getc();
    if (main_state == L3STATE_ConnectArbitrator)
    {
        if (c == '\n' || c == '\r')
        {
            originalWord[wordLen++] = '\0';
            L3_event_setEventFlag(L3_event_quizstartPduRcvd);
            debug_if(DBGMSG_L3, "word is ready! ::: %s\n", originalWord);
        }
        else if (main_state == L3STATE_AnswerTransmission)
        {
            if (c == '\n' || c == '\r')
            {
                originalWord[wordLen++] = '\0';
                L3_event_setEventFlag(L3_event_answerInput);
            }
            
        }
        else
        {
            originalWord[wordLen++] = c;
            if (wordLen >= L3_MAXDATASIZE-1)
            {
                originalWord[wordLen++] = '\0';
                L3_event_setEventFlag(L3_event_dataToSend);
                pc.printf("\n max reached! word forced to be ready :::: %s\n", originalWord);
            }
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
    if (prev_state != main_state)
    {
        debug_if(DBGMSG_L3, "[L3] State transition from %i to %i\n", prev_state, main_state);
        prev_state = main_state;
    }

    //FSM should be implemented here! ---->>>>
    switch (main_state)
    {
        // case L3STATE_IDLE: //IDLE state description
            
        //     if (L3_event_checkEventFlag(L3_event_msgRcvd)) //if data reception event happens
        //     {
        //         //Retrieving data info.
        //         uint8_t* dataPtr = L3_LLI_getMsgPtr();
        //         uint8_t size = L3_LLI_getSize();

        //         debug("\n -------------------------------------------------\nRCVD MSG : %s (length:%i)\n -------------------------------------------------\n", 
        //                     dataPtr, size);
                
        //         pc.printf("Give a word to send : ");
                
        //         L3_event_clearEventFlag(L3_event_msgRcvd);
        //     }
        //     else if (L3_event_checkEventFlag(L3_event_dataToSend)) //if data needs to be sent (keyboard input)
        //     {
        //         //msg header setting
        //         strcpy((char*)sdu, (char*)originalWord);
        //         debug("[L3] msg length : %i\n", wordLen);
        //         L3_LLI_dataReqFunc(sdu, wordLen, myDestId);

        //         debug_if(DBGMSG_L3, "[L3] sending msg....\n");
        //         wordLen = 0;

        //         pc.printf("Give a word to send : ");

        //         L3_event_clearEventFlag(L3_event_dataToSend);
        //     }
        //     break;
        case L3STATE_ConnectArbitrator:
            if (L3_event_checkEventFlag(L3_event_quizstartPduRcvd)) //Q) event set
            { //Q) 나의 아이디 입력
                sdu = L3_LLI_getMsgPtr();
                uint8_t srcId = L3_LLI_getSrcId();
                pc.printf("\n -------------------------------------------------\nRCVD from %i : %s\n -------------------------------------------------\n", 
                                srcId, sdu);

                if (originalWord == "Y")
                {
                    pduSize = L3_MSG_encode_DEMAND(demand_Pdu, demand_Sdu, Len_demand);
                    debug("CONNETARBITRATOR state : demandPDU transmitted - (len:%i) %s\n", Len_demand, demand_Sdu);
                    L3_LLI_dataReqFunc(demand_Pdu, pduSize, arbiId);
                    
                    main_state = L3STATE_ConnectArbitrator;
                    L3_event_clearEventFlag(L3_event_quizstartPduRcvd);
                }
                else if (L3_event_partiConfirmedPduRcvd) //Q) partiConfirmedPduRcvd || collectCompletePduRcvd 가능?
                {
                    if(L3_event_collectCompletePduRcvd)
                    {
                        main_state = L3STATE_QuestionReception;
                        L3_event_clearEventFlag(L3_event_partiConfirmedPduRcvd);
                        L3_event_clearEventFlag(L3_event_collectCompletePduRcvd);
                    }
                }
                break;   
            }
        break;

        case L3STATE_QuestionReception:
            if (L3_event_checkEventFlag(L3_event_questionPduRcvd))
            {
                sdu = L3_LLI_getMsgPtr();
                srcId = L3_LLI_getSrcId();
                pc.printf("\n -------------------------------------------------\nRCVD from %i : %s\n -------------------------------------------------\n", 
                                srcId, sdu);
                main_state = L3STATE_AnswerReception;
                L3_event_checkEventFlag(L3_event_questionPduRcvd);
            }
        break;
        
        case L3STATE_AnswerTransmission:
            if (L3_event_checkEventFlag(L3_event_totalPduRcvd))
            {
                sdu = L3_LLI_getMsgPtr();
                srcId = L3_LLI_getSrcId();
                pc.printf("\n -------------------------------------------------\nRCVD from %i : %s\n -------------------------------------------------\n", 
                                srcId, sdu);
                main_state = L3STATE_ConnectArbitrator;
                L3_event_clearEventFlag(L3_event_totalPduRcvd);
            }
            else if(L3_event_checkEventFlag(L3_event_noAnswerPduRcvd))
            {
                sdu = L3_LLI_getMsgPtr();
                srcId = L3_LLI_getSrcId();
                pc.printf("\n -------------------------------------------------\nRCVD from %i : %s\n -------------------------------------------------\n", 
                                srcId, sdu);
                main_state = L3STATE_ConnectArbitrator;
                L3_event_clearEventFlag(L3_event_noAnswerPduRcvd);
            }
            else if (L3_event_checkEventFlag(L3_event_answerInput))
            {
                L3_MSG_encode_ANSWER(pdu, seqNum, originalWord, wordLen);
                debug("ATX state : A transmitted - (len:%i) %s\n", wordLen, originalWord);
                uint8_t answerSize = sizeof(pdu);
                L3_LLI_dataReqFunc(pdu, answerSize, arbiId);

                main_state = L3STATE_AnswerReception;
                L3_event_clearAllEventFlag(L3_event_answerInput);
            }
            break;
        break;
        case L3STATE_AnswerReception:
            if (L3_event_checkEventFlag(L3_event_totalPduRcvd))
            {
                sdu = L3_LLI_getMsgPtr();
                srcId = L3_LLI_getSrcId();
                pc.printf("\n -------------------------------------------------\nRCVD from %i : %s\n -------------------------------------------------\n", 
                                srcId, sdu);
                main_state = L3STATE_ConnectArbitrator;
                L3_event_clearEventFlag(L3_event_totalPduRcvd);
            }
            else if(L3_event_checkEventFlag(L3_event_noAnswerPduRcvd))
            {
                sdu = L3_LLI_getMsgPtr();
                srcId = L3_LLI_getSrcId();
                pc.printf("\n -------------------------------------------------\nRCVD from %i : %s\n -------------------------------------------------\n", 
                                srcId, sdu);
                main_state = L3STATE_ConnectArbitrator;
                L3_event_clearEventFlag(L3_event_noAnswerPduRcvd);
            }
        break;
        default :
            break;
    }
}