#include <stdio.h>

#define MAX_NUM_STR 10

typedef struct ChatSysMsg {
	int		Connfd;
	char	Version;
	char	MsgType;
	char	SerialNumber;
	int		MsgLen;
	char	Msg[MAX_NUM_STR]; 
}tChatSysMsg;


typedef struct QueueNode {
	struct QueueNode * pNext;
}tQueueNode;

typedef struct QMSG {
	tQueueNode Node;
	tChatSysMsg data;

}tQMSG;

int main()
{
	// 它们间的强制转换:
	tQMSG * pTempMsg;
	(tQueueNode **)&pTempMsg;		// 此语句的作用是什么?

	&pTempMsg->data;
	// 也就是 
	(tChatSysMsg *)&pTempMsg->data;
	return 0;
}
