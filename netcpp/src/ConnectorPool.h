#ifndef _CONNECTOR_POOL_H
#define _CONNECTOR_POOL_H

#include "Util.h"
#include "Connector.h"

class	ConnectorPool
{
public:
	ConnectorPool();
	~ConnectorPool();

	bool				Init(uint32_t uMaxCount);
	bool				InitPoolData(int32_t nIndex,Connector* pConnector);

	uint32_t				GetMaxCount() { return m_uMaxCount; }
	uint32_t				GetConnectorCount() { return m_uMaxCount - m_uConnectorCount; }

	Connector*			NewConnector();
	Connector*			GetConnector(ConnectorID_t ConnectorID);
	bool					DelConnector(ConnectorID_t ConnectorID);

private:
	Connector**			m_apConnectors;
	uint32_t				m_uConnectorCount;
	uint32_t				m_uMaxCount;
	uint32_t				m_Position;
	FastLock				m_Lock;
};

#endif
