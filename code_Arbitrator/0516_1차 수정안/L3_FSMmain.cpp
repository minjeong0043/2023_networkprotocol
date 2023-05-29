#include "L3_FSMevent.h"
#include "L3_msg.h"
#include "L3_timer.h"
#include "L3_LLinterface.h"
#include "protocol_parameters.h"
#include "mbed.h"


//FSM state -------------------------------------------------
#define L3STATE_IDLE                  0
#define L3STATE_Collect_participants  1
#define L3STATE_Question_transmission 2
#define L3STATE_Answer_transmission   3


//state variables
static uint8_t main_state = L3STATE_IDLE; //protocol state
static uint8_t prev_state = main_state;

//PDU context/ size
uint8_t Quizstart_PDU[200]; //00
uint8_t total_PDU[200]; //11
uint8_t Collect_PDU[200]; //01
uint8_t Quiz_PDU[200]; //10
//uint8_t Demand_PDU[200];

uint8_t pduSize



//SDU (input)
static uint8_t originalWord[1030];
static uint8_t wordLen=0;

static uint8_t sdu[1030];

//serial port interface
static Serial pc(USBTX, USBRX);
static uint8_t myDestId;

// Arbitrator's parameters ----------------------------------
uint8_t partiNum = 0;       //참여자 ID 저장 배열
uint8_t Collect_PDU[5]      //참여자 모집이 완료됐고, 참여자로 선택됐다는 PDU
uint8_t Quizstart_PDU[5]    //퀴즈 프로토콜이 시작됐다는 PDU
uint8_t partiID_1[5];       //1번 참여자 아이디
uint8_t partiID_2[5];       //2번 참여자 아이디

//application event handler : generating SDU from keyboard input
static void L3service_processInputWord(void)
{
    char c = pc.getc();
    if (!L3_event_checkEventFlag(L3_event_dataToSend))
    {
        if (c == '\n' || c == '\r')
        {
            originalWord[wordLen++] = '\0';
            L3_event_setEventFlag(L3_event_dataToSend);
            debug_if(DBGMSG_L3,"word is ready! ::: %s\n", originalWord);
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
        case L3STATE_IDLE: //IDLE state description
            
            if (L3_event_checkEventFlag(L3_event_msgRcvd)) //if data reception event happens
            {
                //Retrieving data info.
                uint8_t* dataPtr = L3_LLI_getMsgPtr();
                uint8_t size = L3_LLI_getSize();

                debug("\n -------------------------------------------------\nRCVD MSG : %s (length:%i)\n -------------------------------------------------\n", 
                            dataPtr, size);
                
                pc.printf("Give a word to send : ");
                
                L3_event_clearEventFlag(L3_event_msgRcvd);
            }
            else if (L3_event_checkEventFlag(L3_event_dataToSend)) //if data needs to be sent (keyboard input)
            {
                //msg header setting
                strcpy((char*)sdu, (char*)originalWord);
                debug("[L3] msg length : %i\n", wordLen);
                L3_LLI_dataReqFunc(sdu, wordLen, myDestId);

                debug_if(DBGMSG_L3, "[L3] sending msg....\n");
                wordLen = 0;

                pc.printf("Give a word to send : ");

                L3_event_clearEventFlag(L3_event_dataToSend);
            }
            break;
        case L3STATE_Collect_participants:
            if (L3_event_checkEventFlag(L3_event_quizstartPDUSend))
            {
                //Quizstart PDU 전송 주변 모두에게(destID =255로 설정하면 모든 단말에게 전송)
                endNode_ID = 0
                dest_ID = 255; // 모두에게
                pduSize = Msg_encodeData(Quizstart_PDU, originalWord, seqNum, wordLen);
                L3_LLI_SendData(Quizstart_PDU, pduSize, dest_ID);
                //Quizstart_PDU 정의하기 숫자를 통해
                main_state = L3STATE_Collect_participants;
                L3_event_clearEventFlag(L3_event_quizstartPDUSend);
            }
            esle if (L3_event_checkEventFlag(L3_event_demandPDURcvd))
            {
                //demand PDU 받음
                if (size(part) <2)
                {
                    uint8_t* dataPtr = L3_LLI_getMsgPtr();
                    uint8_t size = L3_LLI_getSize();
                    uint8_t srcId = L3_LLI_getSrcId();

                    if (L3_LLI_getMsgPtr(dataPtr) == L3_LLI_getMsgPtr(demandPDU))
                    {
                        parti.insert(srcID);
                        //참가자 어레이에 아이디 저장
                        debug("\n -------------------------------------------------\nRCVD MSG from %i : %s (length:%i) \n -------------------------------------------------\n", 
                            srcId, dataPtr, size);
                            // 위에 debug에서 (dataPtr or Msg_getWord(dataPtr)??)
                        main_state = L3STATE_Collect_participants;
                        L3_event_checkEventFlag(L3_event_demandPDURcvd);
                    }
                    break;
                }
                break;
        
            }
            else if (L3_event_checkEventFlag(L3_event_CompPartiCollect))
            {
                // Collect_PDU 전송 선착순 두 명에게만 받음. 메시지를 받아오는 거
                // 참가자 array 저장 후 전송
                

                main_state = L3STATE_Question_transmission;
                L3_event_clearEventFlag(L3_event_CompPartiCollect);
            }
            break;
        break;
        
        case L3STATE_Question_transmission:

        break;

        case L3STATE_Answer_transmission:

        break;
        default :
            break;
    }
}