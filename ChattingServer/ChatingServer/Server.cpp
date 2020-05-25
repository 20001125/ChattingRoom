#include "Server.h"

Server::Server()
{
}

Server::~Server()
{
}

BOOL Server::CreateServer(const char* szIp, u_short nPort)
{
	//1.����tcp�ͻ���
	BOOL bRet = m_tcpSocket.CreateSocket();
	if (!bRet)
	{
		cout << "tcp�ͻ��˴���ʧ��" << endl;
		return FALSE;
	}

	//2.�󶨶˿�
	bRet = m_tcpSocket.BindListen((char*)"127.0.0.1", 9527);
	if (!bRet)
	{
		cout << "�󶨶˿ڼ���ʧ��" << endl;
		return FALSE;
	}

	//3.����һ���̣߳��������տͻ�������
	HANDLE hThread = CreateThread(NULL, 0, HandleClientsThread, (LPVOID)this, 0, NULL);
	CloseHandle(hThread);

	return TRUE;
}

BOOL Server::RunServer()
{
	//�������Կͻ��˵�����
	while (TRUE)
	{
		ClientInfo* pCI = new ClientInfo(clock(), &m_lstClients);
		BOOL bRet = m_tcpSocket.Accept(&pCI->m_tcpsocketClient);
		if (!bRet)
		{
			break;
		}
		printf("IP:%s port:%d ���ӵ�������. \r\n",
			inet_ntoa(pCI->m_tcpsocketClient.GetSockaddrIn().sin_addr),
			ntohs(pCI->m_tcpsocketClient.GetSockaddrIn().sin_port));

		m_lstClients.push_back(pCI);

	}
	return 0;
}

DWORD WINAPI Server::HandleClientsThread(LPVOID pParam)
{
	Server* pThis = (Server*)pParam;

	while (TRUE)
	{
		//�������ݰ�
		fd_set fdRead;
		FD_ZERO(&fdRead); //��ʼ��

		//�����е�socket��������
		pThis->g_lock.Lock();
		for (auto client : pThis->m_lstClients)
		{
			FD_SET(client->m_tcpsocketClient.m_socket, &fdRead);
		}

		pThis->g_lock.UnLock();

		//���ָ����socket
		timeval tv = { 1, };
		int nRet = select(fdRead.fd_count, &fdRead, NULL, NULL, &tv);
		if (nRet == 0 || nRet == SOCKET_ERROR)
		{
			continue;
		}

		pThis->g_lock.Lock();
		for (auto itr = pThis->m_lstClients.begin(); itr != pThis->m_lstClients.end(); itr++)
		{
			if (FD_ISSET((*itr)->m_tcpsocketClient.m_socket, &fdRead))
			{
				if (!pThis->HandleData(*itr))
				{
					pThis->m_lstClients.erase(itr);
					break;
				}
			}
		}
		pThis->g_lock.UnLock();
	}
	

	return 0;
}

bool Server::HandleData(ClientInfo* pCL)
{
	DATAPACKAGE pkgRecv;
	BOOL bRet = pCL->m_tcpsocketClient.RecvPackage(&pkgRecv);

	if (!bRet)
	{
		return 0;
	}

	//�ж���������
	switch (pkgRecv.m_hdr.m_nDataType)
	{
	case DT_LOGIN:
	{
		//�����¿ͻ��˵�����
		pCL->m_clockHeartTime = clock();

		//��ͻ��˷��͵�¼���
		DATAPACKAGE package;
		package.m_hdr.m_nDataType = DT_FLUSH_NAME;
		package.m_hdr.m_nDataLen = 0;
		package.m_pBuff = NULL;
		for (auto ci : *(pCL->m_pLstClients))
		{
			BOOL bRet = ci->m_tcpsocketClient.SendPackage(&package);
		}
		break;
	}
	case DT_ADD_NAME:
	{
		for (auto ci : *(pCL->m_pLstClients))
		{
			BOOL bRet = ci->m_tcpsocketClient.SendPackage(&pkgRecv);
		}
		break;
	}
	case DT_DEL_NAME:
	{
		for (auto ci : *(pCL->m_pLstClients))
		{
			BOOL bRet = ci->m_tcpsocketClient.SendPackage(&pkgRecv);
		}
		break;
	}
	case DT_MSG:
	{
		//�ж���Ⱥ������˽��
		neb::CJsonObject json;
		json.Parse(pkgRecv.m_pBuff);

		string strType;
		json.Get("type", strType);

		if (strType == "public")//Ⱥ��
		{
			//����Ϣת���������ͻ���
			for (auto ci : *(pCL->m_pLstClients))
			{
				BOOL bRet = ci->m_tcpsocketClient.SendPackage(&pkgRecv);
			}
		}
		else if (strType == "private") //˽�� 
		{
			//����Ϣת���������ͻ���
			for (auto ci : *(pCL->m_pLstClients))
			{
				BOOL bRet = ci->m_tcpsocketClient.SendPackage(&pkgRecv);
			}
		}
		break;
	}
	case DT_LOGOUT:
	{
		for (auto itr = pCL->m_pLstClients->begin(); itr != pCL->m_pLstClients-> end(); itr++)
		{
			if ((*itr) == pCL)
			{
				pCL->m_pLstClients->erase(itr);
				delete pCL;

				printf("ip:%s �˿ں�: %d ������ \r\n",
					inet_ntoa(pCL->m_tcpsocketClient.GetSockaddrIn().sin_addr),
					ntohs(pCL->m_tcpsocketClient.GetSockaddrIn().sin_port));
				return false;
			}
		}
		break;
	}
	}

	if (pkgRecv.m_pBuff != NULL)
	{
		delete[] pkgRecv.m_pBuff;
	}
	return TRUE;
}
