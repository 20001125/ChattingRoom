
// ChatClientDlg.h : ͷ�ļ�
//

#pragma once
#include "common.h"
#include "TcpSocket.h"

// CChatClientDlg �Ի���
class CChatClientDlg : public CDialogEx
{
// ����
public:
	CChatClientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CHATCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	CTcpSocket m_sockClient;
	sockaddr_in m_siServer;//��������ַ
	CString m_csToUsername;
	CString m_Name;

	static DWORD WINAPI RecvThread(LPVOID pParam);
	void EnableSendUI(BOOL bEnable);

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLogin();
	afx_msg void OnBnClickedSend();
	afx_msg void OnClose();
	afx_msg void OnBnClickedLogout();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedSendto();
	CListCtrl m_NameListBox;
	afx_msg void OnNMClickName(NMHDR* pNMHDR, LRESULT* pResult);
};
