#include "ConnectorPool.h"


ConnectorPool::ConnectorPool()
{
	m_apConnectors		= NULL;
	m_uConnectorCount	= 0;
	m_Position			= 0;
	m_uMaxCount			= 0;
}

ConnectorPool::~ConnectorPool()
{
	if( m_apConnectors )
	{
		for( int32_t i=0; i<m_uMaxCount; i++ )
		{
			SAFE_DELETE(m_apConnectors[i]);
		}
	}

	SAFE_DELETE_ARRAY(m_apConnectors);
}

bool ConnectorPool::Init(uint32_t uMaxCount)
{

	m_Position = 0;
	m_uMaxCount = uMaxCount;
	m_uConnectorCount = uMaxCount;
	
	m_apConnectors = new Connector*[uMaxCount];

	Check(m_apConnectors);

	for(int32_t i = 0; i < uMaxCount; i++)
	{
		m_apConnectors[i] = NULL;
	}

	return true;
}

bool ConnectorPool::InitPoolData(int32_t nIndex,Connector* pConnector)
{

	Check(pConnector);
	Check(nIndex>=0 && nIndex<m_uMaxCount);

	m_apConnectors[nIndex] = pConnector;
	m_apConnectors[nIndex]->SetConnectorID((ConnectorID_t)nIndex);
	m_apConnectors[nIndex]->SetEmpty(true);
	m_apConnectors[nIndex]->InitSocket();		

	return true;
}


Connector* ConnectorPool::GetConnector(ConnectorID_t ConnectorID)
{
	if(ConnectorID >= m_uMaxCount || ConnectorID < 0) 
		return NULL;
	if( m_apConnectors[ConnectorID]->IsEmpty() )
		return NULL;
	return m_apConnectors[ConnectorID];
}


Connector* ConnectorPool::NewConnector()
{

	lock_guard<FastLock>	autolock(m_Lock);
	int32_t					iRet = 0;

	for(int32_t i = 0; i < m_uMaxCount; i++)
	{
		if(m_apConnectors[m_Position]->IsEmpty())
		{
			iRet = m_Position;
			//if( bClear )
				m_apConnectors[iRet]->Clear();
			m_apConnectors[m_Position]->SetEmpty(false);

			m_Position++;
			if(m_Position >= m_uMaxCount)
				m_Position = 0;

			m_uConnectorCount--;

			return m_apConnectors[iRet];
		}

		m_Position++;
		if(m_Position >= m_uMaxCount) 
			m_Position = 0;
	}

	return NULL;
}


bool ConnectorPool::DelConnector(ConnectorID_t ConnectorID)
{
	
	lock_guard<FastLock>	autolock(m_Lock);

	if( ConnectorID<0 || ConnectorID>=m_uMaxCount )
	{
		WriteLog(KERN_ERR "ConnectorPool::DelConnector(): Error ! ConnectorID=%d", ConnectorID );
		return false;
	}

	m_apConnectors[ConnectorID]->Clear();

	m_uConnectorCount++;

	return true;
}
