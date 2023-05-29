#include "mbed.h"
#include "L3_msg.h"

int L3_checkIfanswerPdu(uint8_t* msg)
{
    return (msg[L3_MSG_OFFSET_TYPE] == L3_RXMSG_TYPE_ANSWER);
}

int L3_checkIfdemandPdu(uint8_t* msg)
{
    return (msg[L3_MSG_OFFSET_TYPE] == L3_RXMSG_TYPE_DEMAND);
}

// msg offset-> TYPE|AID|LEN|DATA
//encode QUIZ START SDU
uint8_t L3_MSG_encode_QUIZSTART(uint8_t* msg_data, uint8_t* data, int len)
{
    msg_data[L3_MSG_OFFSET_TYPE] = L3_MSG_TYPE_QUIZSTART;
    msg_data[L3_MSG_OFFSET_LEN] = len;
    //L3_MSG_OFFSET_AID 비워둬도 괜찮은지
    memcpy(&msg_data[L3_MSG_OFFSET_DATA], data, len*sizeof(uint8_t));
    return len+L3_MSG_OFFSET_DATA;
}

//encode pariconfirmed SDU/ 너가 참여자이다~
uint8_t L3_MSG_encode_PARTICONFIRMED(uint8_t* msg_data, uint8_t* data, int len)
{
    msg_data[L3_MSG_OFFSET_TYPE] = L3_MSG_TYPE_PARTICONFIRMED;
    msg_data[L3_MSG_OFFSET_LEN] = len;
    //L3_MSG_OFFSET_AID 비워둬도 괜찮은지
    memcpy(&msg_data[L3_MSG_OFFSET_DATA], data, len*sizeof(uint8_t));
    return len+L3_MSG_OFFSET_DATA;
}

//encode COLLECT완료 SDU
uint8_t L3_MSG_encode_COLLECTCOMPLETE(uint8_t* msg_data, uint8_t* data, int len)
{
    msg_data[L3_MSG_OFFSET_TYPE] = L3_MSG_TYPE_COLLECTCOMPLETE;
    msg_data[L3_MSG_OFFSET_LEN] = len;
    //L3_MSG_OFFSET_AID 비워둬도 괜찮은지
    memcpy(&msg_data[L3_MSG_OFFSET_DATA], data, len*sizeof(uint8_t));
    return len+L3_MSG_OFFSET_DATA;
}

//encode Question SDU
uint8_t L3_MSG_encode_QUESTION(uint8_t* msg_data, int answer_id, int len, uint8_t* question)
{
    msg_data[L3_MSG_OFFSET_TYPE] = L3_MSG_TYPE_QUESTION;
    msg_data[L3_MSG_OFFSET_AID] = answer_id;
    msg_data[L3_MSG_OFFSET_LEN] = len;
    memcpy(&msg_data[L3_MSG_OFFSET_DATA], question, len*sizeof(uint8_t));
    return len+L3_MSG_OFFSET_DATA;
}


//edcode TOTAL SDU