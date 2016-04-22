#include "Net.h"
#include "Message.h"
#include "ConnectorManager_Client.h"
#include <stdio.h>
#include <string.h>

bool NET::Init(uint32_t uMaxSize) {
	bool ret = g_ConnectorManager_Client.Init(uMaxSize);

}

bool NET::Tick(uint32_t uTime)
{
	//g_ConnectorManager_Client.RunTick((void*)uTime);
	pthread_t id;
	pthread_create(&id, NULL, g_ConnectorManager_Client.RunTick, (void*)uTime);
}


void NET::Quit() {
	g_ConnectorManager_Client.stop();
}

bool NET::Acceptor(uint32_t port, const char* ip, uint32_t backlog) {
	return g_ConnectorManager_Client.Acceptor(port, ip, backlog);
}

bool NET::ConnectToServer(const char* ip, uint16_t port, int16_t& ConnectorID) {
	return g_ConnectorManager_Client.ConnectToServer(ip, port, ConnectorID);
}

bool NET::CloseConnect(int16_t ConnectorID) {
	return g_ConnectorManager_Client.RemoveConnector(g_ConnectorManager_Client.GetConnector(ConnectorID));
}

bool NET::SendPacket(int16_t ConnectorID, const char* buf, uint32_t len) {
	return g_ConnectorManager_Client.SendPacket(ConnectorID, buf, len);
}
bool NET::ConnecterState(int16_t ConnectorID, int32_t state, const char* buf, uint32_t len) {
	printf("NET::SendPacketSucc connID=%d,state=%d,len=%d\n", ConnectorID, state, len);
	switch( state ) {
	    case ConnecterConnect:
	      break;
	    case ConnecterClose:
	      break;
	    case ConnecterSendPacket:
	      break;
	    case ConnecterRecievePacket:
	      break;
	    default:
	      break;
	  }
	return true;
}
//bool NET::RecievePacket(int16_t ConnectorID, const char* buf, uint32_t len) {
//	printf("NET::RecievePacket connID=%d,len=%d\n", ConnectorID, len);
//	return true;
//}

int main() {
	printf("!!!Hello World!!!\n");

	NET::Init(256);
	NET::Acceptor(3900);
	NET::Tick(30);



	int16_t ConnectorID;
	NET::ConnectToServer("172.16.2.200", 3900, ConnectorID);

//*******************test data
	NET_MSG_HEAD head;
	memset(&head,0,NET_MSG_HEAD_SIZE);
	head.body_len =1;
	char str[10] = {0};
	str[9] = 1;
	memcpy(&str[0], &head, NET_MSG_HEAD_SIZE);
//*******************test data

	NET::SendPacket(ConnectorID, str, 10);

	sleep(999999);
	printf("!!!end!!!\n");
	return 0;
}
