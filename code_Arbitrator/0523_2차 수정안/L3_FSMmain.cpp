#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"


//FSM state -------------------------------------------------
//#define L3STATE_IDLE                0
#define Collect_participants        1 
#define Question_transmission       2
#define Answer_transmission         3

//state variables
static uint8_t main_state = L3STATE_IDLE; //protocol state
static uint8_t prev_state = main_state;

//parameter
uint8_t partiID[2];

//SDU (input)
static uint8_t originalWord[1030];
static uint8_t wordLen=0;

static uint8_t sdu[1030];

//PDU context/size
uint8_t L3_[200];
uint8_t pduSize;

//serial port interface
static Serial pc(USBTX, USBRX);
static uint8_t myDestId;
static uint8_t answer_id;

static uint8_t flag_aid=0;

//application event handler : generating SDU from keyboard input
static void L3service_processInputWord(void)
{
    char c = pc.getc();
    //if (!L3_event_checkEventFlag(L3_event_dataToSend))

    if (main_state == Question_transmission)
    {
        if (c == '\n' || c == '\r')
        {
            originalWord[wordLen++] = '\0';

            if (flag_aid == 0)
                L3_event_setEventFlag(L3_event_answerIDinput);
            else
                L3_event_setEventFlag(L3_event_questSDUinput);
            debug_if(DBGMSG_L3,"word is ready! ::: %s\n", originalWord);
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
        //         //L3_LLI_dataReqFunc(sdu, wordLen, myDestId);

        //         debug_if(DBGMSG_L3, "[L3] sending msg....\n");
        //         wordLen = 0;

        //         pc.printf("Give a word to send : ");

        //         L3_event_clearEventFlag(L3_event_dataToSend);
        //     }
        //     break;
        case Collect_participants:
            if (L3_event_checkEventFlag(L3_event_quizstartPDUSend))
            {
                main_state = Collect_participants;
                L3_event_clearEventFlag(L3_event_quizstartPDUSend);
            }
            else if(L3_event_checkEventFlag(L3_event_demandPDURcvd))
            {   
                partiID[partiNum++] = L3_LLI_getSrcId();
                if (partiNum >= 2)
                {
                    // Collect PDU 전송 // "collect complement" 질문
                    //encode PDU/ PDU 부호화
                    //L3_LLI_dataReqFunc(sdu, wordLen, myDestId); sdu에 pdu 들어가도 되는지 가능
                    //아래 코드(2줄): 데이터 부호화 이부분 맞는지 확인 필요
                    uint8_t pduSize_CollectPDU = L3_msg_encodeData(L3_MSG_CollectPDU, uint8_t* data, int seq, int len, uint8_t);
                    L3_LLI_dataReqFunc(pdu, wordLen, myDestId);//수정필요
                    flag_aid = 0;
                    main_state = Question_transmission;
                    L3_event_clearEventFlag(L3_event_demandPDURcvd);
                }
                else
                    main_state = Collect_participants;
                    L3_event_clearEventFlag(L3_event_demandPDURcvd);
                //}
                //else
                break;
            }
            // #if 0
            // else if(L3_event_checkEventFlag(L3_event_ComPartiCollect))
            // {
            //         main_state = Question_transmission;
            //         L3_event_clearEventFlag(L3_event_ComPartiCollect);
            //         L3_event_setEventFlag(L3_event_QuestionStart)
            // }
            // #endif
        break;
        case Question_transmission:
            if (L3_event_checkEventFlag(L3_event_answerIDinput)) // Quest 와 ID입력 
            {
                answer_id = atoi(originalWord);
                flag_aid = 1;
                debug("QTX state : AID : %i\n", answer_id);
                main_state = Answer_transmission;
                L3_event_clearEventFlag(L3_event_answerIDinput) // 안 되면 나눠서 하기

            }
            else if (L3_event_checkEventFlag(L3_event_questSDUinput)) // Quest 와 ID입력 
            {
                L3_timer_startTimer(); 
                //uint8_t* Question = L3_event_questSDUinput();
                //uint8_t Answer = L3_event_answerIDinput();
                //QuestPDU 만드는 코드 아래
                //int QuestionSize =  sizeof(Question) / sizeof(Question[0]);
                //int AnswerSize = sizeof(Answer) / sizeof(Answer[0]);
                //int QuestSize = QuestionSize + AnswerSize;
                //int QuestPDU[QuestSize]; // QuestPDU = Question+ AnswerID

                //std::copy(Question, Question+QuestionSize, QuestPDU);
                //std::copy(Answer, Answer+AnswerSize, QuestPDU + QuestionSize);

                L3_MSG_encode_QUESTION(sdu, answer_id, wordLen, originalWord);
                debug("QTX state : Q transmitted - (AID:%i, len:%i) %s\n", answer_id, wordLen, originalWord);
                
                L3_LLI_dataReqFunc(sdu, QuestSize, 255);
                main_state = Answer_transmission;
                L3_event_clearEventFlag(L3_event_questSDUinput) // 안 되면 나눠서 하기

            }
            break;
        case Answer_transmission:
            if(L3_event_checkEventFlag(L3_event_answerPDU))
            {
                uint8_t Srcid =L3_LLI_getSrcId();
                if (Answer ==Srcid)
                {
                    Srcid = L3_LLI_getSrcId();
                    uint8_t* AnswerPtr = L3_LLI_getMsgPtr();
                    uint8_t Size = L3_LLI_getSize();
                    //답변 저장 후 전송, timerstop
                    //totalPDU 만들기(배열변수 합치기(Srcid 배열과 Answer 배열)
                    int SrcidSize = sizeof(Srcid) / sizeof(Srcid[0]);
                    int AnswerPtrSize = sizeof(AnswerPtr) / sizeof(AnswerPtr[0]);                        totalSize = AnswerPtrSize + SrcIDSize;
                    int totalSize = SrcidSize + AnswerPtrSize;
                    int totalPDU[totalSize];
                    std::copy(Srcid, Srcid + SrcidSize, totalPDU);
                    std::copy(AnswerPtr, AnswerPtr+AnswerPtrSize, totalPDU + SrcidSize);
            
                    L3_LLI_dataReqFunc(totalPDU , totalSize, Parti[]);

                    L3_timer_stopTimer();
                    main_state = Collect_participants;
                    L3_event_clearEventFlag(L3_event_answerPDU);
                }
                break;
        
            }
            else if (L3_event_checkEventFlag(L3_event_timeout))
            {
                //답변이 없습니다. PDU 전송
                noAnswerPDU// 정의하기
                int noAnswerPDUSize = sizeof(noAnswerPDU) / sizeof(noAnswerPDU[0]);
                int AnswerSize = sizeof(Answer) / sizeof(Answer[0]);
                int totalSize = noAnswerPDUSize + AnswerSize;
                int totalPDU[totalSize];
                std::copy(noAnswerPDU, noAnswerPDU +noAnswerPDUSize, totalPDU);
                std::copy(Answer, Answer+AnswerSize, totalPDU + noAnswerPDUSize);

                L3_LLI_dataReqFunc(totalPDU , totalSize, Parti[]);
            }
            break;
        break;
        default :
            break;
    }
}