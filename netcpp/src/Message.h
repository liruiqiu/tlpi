#ifndef _MESSAGE_H
#define _MESSAGE_H
#include <stdint.h>

#pragma pack(push, 1)
/*
 * =====================================================================================
 *	原始数据包协议头
 * =====================================================================================
 */
struct NET_MSG_HEAD
{
	char byte_head[5];
	uint32_t body_len;				/* 包体数据长度 */
};
/*  消息头大小 */
#define		NET_MSG_HEAD_SIZE				(sizeof(NET_MSG_HEAD))
#define		CONNECTOR_UP 	 				(0)	/* 上行 */
#define    	CONNECTOR_DOWN  				(1)  /* 下行 */
#define    	CONNECTOR_UNKNOWN 				(2)  /* 下行 */

/*
 * =====================================================================================
 *	CSPipe   SCPipe  通道协议头
 *	CSUpPipe  SCUpPipe   CSDownPipe  SCDownPipe 通道协议头
 * =====================================================================================
 */
struct MQ_MSG_HEAD
{
	uint16_t 		MsgID; 				/* short 消息类型ID */
	uint32_t 		Flag;
	int32_t 		ConnectorID;		/* 4字节 链接ID */
	char 			ConnectorType;		/* 0:上行 1：下行 */
	int32_t 		Port;				/* 参加server.conf <Type>0</Type> */
	int32_t 		ErrCode;			/* 错误码 */
	int32_t			size;				/* 数据大小 */
};
#define  MQ_MSG_HEAD_SIZE				(sizeof(MQ_MSG_HEAD))

struct MQ_UDP_MSG_HEAD
{
	uint16_t 	MsgID; 			/* short 消息类型ID */
	uint32_t 	Flag;
	uint32_t		IP;
	int32_t		Port;
	int32_t 		ErrCode;			/* 错误码 */
	int32_t		size;				/* 数据大小 */
};
#define  MQ_UDP_MSG_HEAD_SIZE				(sizeof(MQ_UDP_MSG_HEAD))
/*
 * =====================================================================================
 * UDPPipe 通道专用协议 直接传 实际的协议包
 * =====================================================================================
 */

enum OPT_RESULT
{
	OR_OK						= 0, /* 操作成功 */
	OR_ERROR					= 1, /* 未知错误 */
	OR_NO_SERVER				= 2, /* 未找到服务器*/
	OR_OVERTIME_CLOSE			= 3, /* 连接超时断开 */
	OR_EXCEPTION_CLOSE			= 4, /* 连接异常断开 */
};

#pragma pack(pop)
#endif
