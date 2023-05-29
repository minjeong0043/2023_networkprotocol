#include "mbed.h"

#define L3_MSG_TYPE_DATA    0
#define L3_MSG_TYPE_ACK     1

#define L3_MSG_OFFSET_TYPE  0
#define L3_MSG_OFFSET_SEQ   1
#define L3_MSG_OFFSET_DATA  2

#define L3_MSG_ACKSIZE      3 //ACK메시지의 총 길이가 3바이트.

#define L3_MSG_MAXDATASIZE  26
#define L3_MSSG_MAX_SEQNUM  1024


int L3_Msg_checkIfData(uint8_t* msg);
int L3_Msg_checkIfAck(uint8_t* msg);
uint8_t L3_Msg_encodeAck(uint8_t* msg_ack, uint8_t seq); // encodeAck: ACK만드는 거, 메시지의 크기가 리턴이 된다.
uint8_t L3_Msg_encodeData(uint8_t* msg_data, uint8_t* data, int seq, int len); // encodeData: 데이터 만드는 거
uint8_t L3_Msg_getSeq(uint8_t* msg);
uint8_t* L3_Msg_getWord(uint8_t* msg);