#include "mbed.h"
#include "L3_msg.h"

//아래 부분 수정필요 복붙한 거

int L3_Msg_checkIfData(uint8_t* msg)
{
    return (msg[L3_MSG_OFFSET_TYPE] == L3_MSG_TYPE_DATA);
}

int L3_Msg_checkIfAck(uint8_t* msg)
{
    return (msg[L3_MSG_OFFSET_TYPE] == L3_MSG_TYPE_ACK);
}

uint8_t L3_Msg_encodeData(uint8_t* msg_data, uint8_t* data, int seq, int len)
{
    msg_data[L3_MSG_OFFSET_TYPE] = L3_MSG_TYPE_DATA;
    msg_data[L3_MSG_OFFSET_SEQ] = seq;
    memcpy(&msg_data[L3_MSG_OFFSET_DATA], data, len*sizeof(uint8_t));

    return len+L3_MSG_OFFSET_DATA;
}
                    

uint8_t L3_Msg_getSeq(uint8_t* msg)
{
    return msg[L3_MSG_OFFSET_SEQ];
}

uint8_t* L3_Msg_getWord(uint8_t* msg)
{
    return &msg[L3_MSG_OFFSET_DATA];
}