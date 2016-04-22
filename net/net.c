//#include "linux/base.h"
#include "net.h"
//#include "message.h"
//#include "connector.h"
#include "connector_manager.h"


/*
bool connector_init() {

    return true;
}

bool connector_tick(uint32_t uTime)
{
    //g_ConnectorManager_Client.RunTick((void*)uTime);
    pthread_t id;
    //	pthread_create(&id, NULL, g_ConnectorManager_Client.RunTick, (void*)uTime);
}


void connector_quit() {
}

bool connector_acceptor(uint32_t port, const char* ip, uint32_t backlog) {
    return true;//return g_ConnectorManager_Client.Acceptor(port, ip, backlog);
}

bool connector_server(const char* ip, uint16_t port, int16_t ConnectorID) {
    return true;//return g_ConnectorManager_Client.ConnectToServer(ip, port, ConnectorID);
}

bool close_connector(int16_t ConnectorID) {
    return true;//return g_ConnectorManager_Client.RemoveConnector(g_ConnectorManager_Client.GetConnector(ConnectorID));
}

bool send_packet(int16_t ConnectorID, const char* buf, uint32_t len) {
    return true;//return g_ConnectorManager_Client.SendPacket(ConnectorID, buf, len);
}
bool ConnecterState(int16_t ConnectorID, int32_t state, const char* buf, uint32_t len) {
    printf("SendPacketSucc connID=%d,state=%d,len=%d\n", ConnectorID, state, len);
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
*/
void test1()
{
    pr_info("---------");
    struct connector *pc;
    struct packet_head head;
    char a[256] = {0};
    //connector_manager_init(1024);
    pool_init(&g_cm.cp, 1024, sizeof(struct connector));
    pc = connect_server("127.0.0.1", 3333);
    head.idx = 1;
    head.len = 16;

    for (;;)
    {
	   memset(a, 0, 256);
	    memcpy(a, &head, PACKET_HEAD_SIZE); 
	    memcpy(a+PACKET_HEAD_SIZE, "0123456789abcdef", 16); 
	    //pr_info("%s",a);
	    print_hex_dump_bytes("data: ", DUMP_PREFIX_OFFSET, a, 22);
	    connector_manager_send_packet(pc->fd, a, PACKET_HEAD_SIZE+16);
	    sleep(1);
    }

    pr_info("------end---");
}

int main(int argc, char *argv[]) {

	printf("!!!Hello World!!!\n");
	if (argc > 1) {
		if (strcmp(argv[1], "-c") == 0) {
			test1();
		}
		return 1;
	}

	connector_manager_init(1024);
	connector_manager_listen("127.0.0.1", 3333);

	while(1)
	{
		connector_manager_tick(); 
		sleep(1);
	}

	connector_manager_clear();

	/*
	*/

	//connector_init();


	//ConnectToServer("172.16.2.200", 3900, ConnectorID);

	/*
	   NET_MSG_HEAD head;
	   memset(&head,0,NET_MSG_HEAD_SIZE);
	   head.body_len =1;
	   char str[10] = {0};
	   str[9] = 1;
	   memcpy(&str[0], &head, NET_MSG_HEAD_SIZE);

	   SendPacket(ConnectorID, str, 10);

	   sleep(999999);
	   */
	printf("!!!end!!!\n");
	return 0;
}

