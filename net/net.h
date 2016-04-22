#ifndef _NET_H
#define _NET_H
#include "message.h"

enum
{
    ConnecterConnect, // 建立连接
    ConnecterClose,		// 关闭连接
    ConnecterSendPacket, 		// 发送数据
    ConnecterRecievePacket 	// 接收数据
};
/**
 * @brief	需要参数个数:1 初始化
 * @param	uint32_t	uMaxSize	最大连接数  心跳毫秒数
 * @return	BOOL		成功返回1，否则0
 */
/*
bool Init(uint32_t uMaxSize);
bool Tick(uint32_t uTime);
void Quit();

bool Acceptor(uint32_t port,const char *ip,  uint32_t backlog);
bool ConnectToServer(const char* ip,  uint16_t port,  int16_t ConnectorID);
bool CloseConnect(int16_t ConnectorID);
bool SendPacket(int16_t ConnectorID, const char *buf, uint32_t len);
//call back
bool ConnecterState(int16_t ConnectorID, int32_t state, const char *buf, uint32_t len);
//bool RecievePacket(int16_t ConnectorID, const char *buf, uint32_t len);
// extern struct connector_pool g_connector_pool;
*/
#endif
