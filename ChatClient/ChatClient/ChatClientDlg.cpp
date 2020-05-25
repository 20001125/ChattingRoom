
// ChatClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ChatClient.h"
#include "ChatClientDlg.h"
#include "afxdialogex.h"
#include "CJsonObject.hpp"
#include <string>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CChatClientDlg �Ի���



CChatClientDlg::CChatClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CHATCLIENT_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChatClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//DDX_Control(pDX, LSTB_CLIENTS, m_NameListBox);
	DDX_Control(pDX, LST_NAME, m_NameListBox);
}

BEGIN_MESSAGE_MAP(CChatClientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(BTN_LOGIN, &CChatClientDlg::OnBnClickedLogin)
	ON_BN_CLICKED(BTN_SEND, &CChatClientDlg::OnBnClickedSend)
	ON_WM_CLOSE()
	ON_BN_CLICKED(BTN_LOGOUT, &CChatClientDlg::OnBnClickedLogout)
	ON_WM_TIMER()
	ON_BN_CLICKED(BTN_SENDTO, &CChatClientDlg::OnBnClickedSendto)
	ON_NOTIFY(NM_CLICK, LST_NAME, &CChatClientDlg::OnNMClickName)
END_MESSAGE_MAP()


// CChatClientDlg ��Ϣ�������


BOOL CChatClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	EnableSendUI(FALSE);
	SetDlgItemInt(EDT_PORT, 9527);

	m_NameListBox.ModifyStyle(LVS_TYPEMASK, //�����ǰ�ķ�� 
		LVS_REPORT | LVS_SINGLESEL | LVS_SORTASCENDING); //����Ϊ������

	m_NameListBox.SetExtendedStyle(m_NameListBox.GetExtendedStyle() | LVS_EX_FULLROWSELECT);

	int nItemIdx = 0;
	m_NameListBox.InsertColumn(nItemIdx++, "����", LVCFMT_LEFT, 70);

	HANDLE hTread = CreateThread(NULL, 0, RecvThread, this, 0, NULL);
	CloseHandle(hTread);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CChatClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CChatClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CChatClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

DWORD WINAPI CChatClientDlg::RecvThread(LPVOID pParam)
{
	CChatClientDlg* pThis = (CChatClientDlg*)pParam;

	while (TRUE)
	{
		//�������ݰ�
		DATAPACKAGE package = { 0 };
		if (!pThis->m_sockClient.RecvPackage(&package))
		{
			//����ʧ��, ����������, ���¼�����������
			continue;
		}

		switch (package.m_hdr.m_nDataType)
		{
		case DT_MSG:
		{
			//����json�ַ���
			neb::CJsonObject json;
			json.Parse(package.m_pBuff);

			std::string strType;
			json.Get("type", strType);

			//��Ϣ
			std::string strMSg;
			json.Get("msg", strMSg);

			//�ǳ�
			std::string strUsername;
			json.Get("name", strUsername);

			//��ʾ����
			CString csText;
			pThis->GetDlgItemText(EDT_SHOW, csText);

			if (strType == "public")//Ⱥ��
			{
				//����� XXX ˵: XXX�ĸ�ʽ
				std::string strToShow = strUsername + " ˵: " + strMSg;
				csText += "\r\n";
				csText += strToShow.c_str();	
			}
			else if (strType == "private") //˽�� 
			{
				std::string strToUsername;
				json.Get("to", strToUsername);
			
				if (!strcmp(strToUsername.c_str(), pThis->m_Name))
				{
					std::string strToShow = strToUsername + " ����˵: " + strMSg;
					csText += "\r\n";
					csText += strToShow.c_str();
				}
				
			}
			pThis->SetDlgItemText(EDT_SHOW, csText);

			break;
		}
		case DT_FLUSH_NAME:
		{
			pThis->m_NameListBox.DeleteAllItems();
			DATAPACKAGE package;
			package.m_hdr.m_nDataLen = pThis->m_Name.GetLength() + 1;
			package.m_hdr.m_nDataType = DT_ADD_NAME;
			package.m_pBuff = pThis->m_Name.GetBuffer(0);
			pThis->m_sockClient.SendPackage(&package);
			break;
		}
		case DT_ADD_NAME:
		{
			pThis->m_NameListBox.InsertItem(0, package.m_pBuff);
			break;
		}
		case DT_DEL_NAME:
		{
			CString szName;
			int nCount = pThis->m_NameListBox.GetItemCount();
			for (int i = 0; i < nCount; i++)
			{
				szName = pThis->m_NameListBox.GetItemText(i, 0);
				if (szName == package.m_pBuff)
				{
					pThis->m_NameListBox.DeleteItem(i);
					break;
				}
			}
			break;
		}

		}
		//�ͷŻ�����
		if (package.m_pBuff != NULL)
		{
			delete[] package.m_pBuff;
		}
	}
}


void CChatClientDlg::EnableSendUI(BOOL bEnable)
{
	//����¼���
	GetDlgItem(EDT_USERNAME)->EnableWindow(!bEnable);
	GetDlgItem(EDT_PORT)->EnableWindow(!bEnable);
	GetDlgItem(BTN_LOGIN)->EnableWindow(!bEnable);

	//��������ص�
	GetDlgItem(EDT_SHOW)->EnableWindow(bEnable);
	GetDlgItem(EDT_SEND)->EnableWindow(bEnable);
	GetDlgItem(BTN_LOGOUT)->EnableWindow(bEnable);
	GetDlgItem(BTN_SEND)->EnableWindow(bEnable);
}

void CChatClientDlg::OnBnClickedLogin()
{
	if (!m_sockClient.CreateSocket())
	{
		AfxMessageBox("����socketʧ��, ������!!");
	}

	//���ӷ�����
	CString csUsername;
	GetDlgItemText(EDT_USERNAME, csUsername);
	int nPort = GetDlgItemInt(EDT_PORT);
	m_Name = csUsername;

	BOOL bRet = m_sockClient.Connect("127.0.0.1", nPort);
	////����������͵�¼��
	DATAPACKAGE package;
	package.m_hdr.m_nDataLen = csUsername.GetLength() + 1;
	package.m_hdr.m_nDataType = DT_LOGIN;
	package.m_pBuff = csUsername.GetBuffer(0);
	m_sockClient.SendPackage(&package);
	EnableSendUI(TRUE);

	

}

void CChatClientDlg::OnBnClickedSend()
{
	//���������������
	CString csText;
	GetDlgItemText(EDT_SEND, csText);


	CString csUsername;
	GetDlgItemText(EDT_USERNAME, csUsername);
	neb::CJsonObject jsonSend;
	jsonSend.Add("type", "public");
	jsonSend.Add("name", csUsername.GetBuffer(0));
	jsonSend.Add("msg", csText.GetBuffer(0));
	std::string strText = jsonSend.ToString();

	//����JSON
	DATAPACKAGE package;
	package.m_hdr.m_nDataLen = strText.size() + 1;
	package.m_hdr.m_nDataType = DT_MSG;
	package.m_pBuff = (char*)strText.c_str();
	m_sockClient.SendPackage(&package);

}


void CChatClientDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	DATAPACKAGE package;

	package.m_hdr.m_nDataLen = m_Name.GetLength() + 1;
	package.m_hdr.m_nDataType = DT_DEL_NAME;
	package.m_pBuff = m_Name.GetBuffer(0);
	m_sockClient.SendPackage(&package);

	package.m_hdr.m_nDataLen = 0;
	package.m_hdr.m_nDataType = DT_LOGOUT;
	package.m_pBuff = NULL;
	m_sockClient.SendPackage(&package);

	
	m_sockClient.CloseSocket();

	CDialogEx::OnClose();
}


void CChatClientDlg::OnBnClickedLogout()
{
	// ��������
	DATAPACKAGE package;

	package.m_hdr.m_nDataLen = m_Name.GetLength() + 1;
	package.m_hdr.m_nDataType = DT_DEL_NAME;
	package.m_pBuff = m_Name.GetBuffer(0);
	m_sockClient.SendPackage(&package);

	package.m_hdr.m_nDataLen = 0;
	package.m_hdr.m_nDataType = DT_LOGOUT;
	package.m_pBuff = NULL;
	m_sockClient.SendPackage(&package);


	m_sockClient.CloseSocket();
	EnableSendUI(FALSE);

}


void CChatClientDlg::OnTimer(UINT_PTR nIDEvent)
{
	//�����������������, ÿ��1��
	DATAPACKAGE package;
	package.m_hdr.m_nDataLen = 0;
	package.m_hdr.m_nDataType = DT_HEART;
	package.m_pBuff = NULL;
	m_sockClient.SendPackage(&package);

	CDialogEx::OnTimer(nIDEvent);
}


void CChatClientDlg::OnBnClickedSendto()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString csText;
	GetDlgItemText(EDT_SEND, csText);

	CString csUsername;
	
	GetDlgItemText(EDT_USERNAME, csUsername);
	m_Name = csUsername;
	
	neb::CJsonObject jsonSend;
	jsonSend.Add("type", "private");
	jsonSend.Add("name", csUsername.GetBuffer(0));
	jsonSend.Add("msg", csText.GetBuffer(0));
	jsonSend.Add("to", m_csToUsername.GetBuffer(0));
	std::string strText = jsonSend.ToString();

	//����
	DATAPACKAGE package;
	package.m_hdr.m_nDataLen = strText.size() + 1;
	package.m_hdr.m_nDataType = DT_MSG;
	package.m_pBuff = (char*)strText.c_str();
	m_sockClient.SendPackage(&package);
}


void CChatClientDlg::OnNMClickName(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
	int nItem = pNMItemActivate->iItem;
	if (nItem != -1)
	{
		m_csToUsername = m_NameListBox.GetItemText(nItem, 0);
	}
	
}
