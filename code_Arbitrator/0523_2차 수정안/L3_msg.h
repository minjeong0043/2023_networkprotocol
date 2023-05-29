#include "mbed.h"

#define L3_MSG_TYPE_START       0
#define L3_MSG_TYPE_COLLECT     1
#define L3_MSG_TYPE_QUESTION    2
#define L3_MSG_TYPE_TOTAL       3
//#define L3_MSG_TYPE_NoAnswer    4

#define L3_MSG_TYPE_DEMAND      0
#define L3_MSG_TYPE_ANS         1

#define L3_MSG_OFFSET_TYPE      0
#define L3_MSG_OFFSET_SEQ       1
#define L3_MSG_OFFSET_DATA      2

#define L3_MSGQ_OFFSET_AID      1
#define L3_MSGQ_OFFSET_LEN      2
#define L3_MSGQ_OFFSET_VAL      3



#define L3_MSG_MAX_SEQNUM  1024

uint8_t L3_msg_encodeData(uint8_t* msg_data, uint8_t* data, int seq, int len, uint8_t);
uint8_t L3_msg_getSeq(uint8_t* msg);
uint8_t* L3_msg_getWord(uint8_t* msg);

/////
int L3_checkIfStart(uint8_t* msg);
int L3_checkIfCollect(uint8_t* msg);
int L3_checkIfQuestion(uint8_t* msg);
int L3_checkIfTotal(uint8_t* msg);

uint8_t qnaMsg_encodeDemand(uint8_t* msg_ack, uint8_t seq);
uint8_t qnaMsg_encodeAnswer(uint8_t* msg_data, uint8_t* data, int seq, int len);
uint8_t qnaMsg_getSeq(uint8_t* msg);
uint8_t* qnaMsg_getWord(uint8_t* msg);
uint8_t L3_MSG_encode_QUESTION(uint8_t* msg_data, int answer_id, int len, uint8_t* question);