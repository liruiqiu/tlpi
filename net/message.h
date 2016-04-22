#ifndef _MESSAGE_H
#define _MESSAGE_H
#include "common/base.h"

//#pragma pack(push, 1)
/*
 * =====================================================================================
 *	原始数据包协议头
 * =====================================================================================
 */
struct packet_head
{
    uint16_t idx;
    uint32_t len;
} __packed packet_head_t;

/*  消息头大小 */
#define		PACKET_HEAD_SIZE				(sizeof(packet_head_t))
#define		CONNECTOR_UP 	 				(0)	/* 上行 */
#define    	CONNECTOR_DOWN  				(1)  /* 下行 */
#define    	CONNECTOR_UNKNOWN 				(2)  /* 下行 */

enum OPT_RESULT
{
    OR_OK						= 0, /* 操作成功 */
    OR_ERROR					= 1, /* 未知错误 */
    OR_NO_SERVER				= 2, /* 未找到服务器*/
    OR_OVERTIME_CLOSE			= 3, /* 连接超时断开 */
    OR_EXCEPTION_CLOSE			= 4, /* 连接异常断开 */
};

//#pragma pack(pop)





#endif
