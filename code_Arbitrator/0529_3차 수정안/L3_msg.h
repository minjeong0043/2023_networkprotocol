#include "mbed.h"

// msg offset-> TYPE|AID|LEN|DATA (PDU format)
#define L3_MSG_OFFSET_TYPE      0
#define L3_MSG_OFFSET_AID       1
#define L3_MSG_OFFSET_AID       2
#define L3_MSG_OFFSET_LEN       3
#define L3_MSG_OFFSET_DATA      4

// 보내는 거 -> *TYPE*|AID|LEN|DATA
#define L3_MSG_TYPE_QUIZSTART           0
#define L3_MSG_TYPE_PARTICONFIRMED      1
#define L3_MSG_TYPE_COLLECTCOMPLETE     2
#define L3_MSG_TYPE_QUESTION            3
#define L3_MSG_TYPE_TOTAL               4
#define L3_MSG_TYPE_NOANSWER            5

// 받는 거
#define L3_RXMSG_TYPE_DEMAND            6
#define L3_RXMSG_TYPE_ANSWER            7      

#define L3_MSG_MAXDATASIZE      26
#define L3_MSG_MAX_SEQNUM       1024


