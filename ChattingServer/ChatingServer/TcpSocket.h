#pragma once
#include <windows.h>
#include "common.h"

typedef struct tagDataPackage
{
	PACKAGEHEADER m_hdr;	//���ݰ��İ�ͷ
	char* m_pBuff;			//���ݰ��İ���
}DATAPACKAGE, *PDATAPACKAGE;


class CTcpSocket
{
public:
	CTcpSocket();
	~CTcpSocket();

	BOOL CreateSocket();
	BOOL BindListen(char* szIp, u_short nPort);
	BOOL Accept(CTcpSocket* pTcpSocket );
	BOOL Connect(char* szIp, u_short nPort);
	BOOL Recv(char* pBuff, int* pLen);//������
	BOOL Send(char* pBuff, int* pLen);//������


	BOOL RecvPackage(DATAPACKAGE* pPackage);
	BOOL SendPackage(DATAPACKAGE* pPackage);


	void CloseSocket();

	const sockaddr_in& GetSockaddrIn()const;
	SOCKET m_socket;

private:

	sockaddr_in m_si;
};

