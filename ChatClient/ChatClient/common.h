#pragma once

enum DataType
{
	DT_LOGIN,	//��¼��
	DT_LOGIN_OK,//��¼�ɹ�
	DT_LOGOUT,  //����
	DT_MSG,		//�������ݰ�
	DT_HEART,   //������
	DT_FLUSH_NAME, //ˢ������
	DT_ADD_NAME,  //�������
	DT_DEL_NAME,  //ɾ��
};

//���ݵİ�ͷ
typedef struct  tagPackageHeader
{
	int m_nDataLen;  //���ݵĳ���
	int m_nDataType; //���ݵ�����
}PACKAGEHEADER, *PPACKAGEHEADER;
