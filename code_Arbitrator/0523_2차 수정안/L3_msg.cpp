#include "mbed.h"
#include "L3_msg.h"

//checkIF 받는 거
int L3_checkIfDemand(uint8_t* msg)
{
    return (msg[L3_OFFSET_TYPE] == L3_MSG_TYPE_DEMAND);
}

int L3_CheckIfAnswer(uint8_t* msg)
{
    return (msg[L3_OFFSET_TYPE] == L3_MSG_TYPE_ANS);
}

//encode START
uint8_t L3_MSG_encode_START(uint8_t* msg_data, uint8_t* data, int seq, int len)
{//uint8_t: 부호없는 정수 데이터 저장 / uint8_t*: 변수나 배열의 메모리 주소저장
 //msg_data:PDU/ data:SDU
    msg_data[L3_MSG_OFFSET_TYPE] = L3_MSG_TYPE_START;
    msg_data[L3_MSG_OFFSET_SEQ] = seq;
    memcpy(&msg_data[L3_MSG_OFFSET_DATA], data, len*sizeof(uint8_t));

    return len+L3_MSG_OFFSET_DATA;
}
//encode COLLECT
uint8_t L3_MSG_encode_COLLECT(uint8_t* msg_data, uint8_t* data, int seq, int len)
{
    msg_data[L3_MSG_OFFSET_TYPE] = L3_MSG_TYPE_COLLECT;
    msg_data[L3_MSG_OFFSET_SEQ] = seq;
    memcpy(&msg_data[L3_MSG_OFFSET_DATA], data, len*sizeof(uint8_t));

    return len+L3_MSG_OFFSET_DATA;
}
//encode QUESTION
uint8_t L3_MSG_encode_QUESTION(uint8_t* msg_data, int answer_id, int len, uint8_t* question)
{
    msg_data[L3_MSG_OFFSET_TYPE] = L3_MSG_TYPE_QUESTION;
    msg_data[L3_MSGQ_OFFSET_AID] = answer_id;
    msg_data[L3_MSGQ_OFFSET_LEN] = len;
    memcpy(&msg_data[L3_MSGQ_OFFSET_VAL], question, len*sizeof(uint8_t));

    return len+L3_MSGQ_OFFSET_VAL;
}
//encode TOTAL
uint8_t L3_MSG_encode_TOTAL(uint8_t* msg_data, uint8_t* data, int seq, int len)
{
    msg_data[L3_MSG_OFFSET_TYPE] = L3_MSG_TYPE_TOTAL;
    msg_data[L3_MSG_OFFSET_SEQ] = seq;
    memcpy(&msg_data[L3_MSG_OFFSET_DATA], data, len*sizeof(uint8_t));

    return len+L3_MSG_OFFSET_DATA;
}
/*
uint8_t L3_MSG_encode_NoAnswer(uint8_t* msg_data, uint8_t* data, int seq, int len)
{
    msg_data[L3_MSG_OFFSET_TYPE] = L3_MSG_TYPE_NoAnswer;
    msg_data[L3_MSG_OFFSET_SEQ] = seq;
    memcpy(&msg_data[L3_MSG_OFFSET_DATA], data, len*sizeof(uint8_t));

    return len+L3_MSG_OFFSET_DATA;
}
*/

//
uint8_t L3_msg_getSeq(uint8_t* msg)
{
    return msg[L3_MSG_OFFSET_SEQ];
}

uint8_t* L3_msg_getWord(uint8_t* msg)
{
    return &msg[L3_MSG_OFFSET_DATA];
}