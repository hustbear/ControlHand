#pragma once
#include "fglove.h"
#include "getPythonData.h"
#include "LineChart.h"


// GloveSampleDlg �Ի���

class GloveSampleDlg : public CDialogEx
{
	DECLARE_DYNAMIC(GloveSampleDlg)

public:
	GloveSampleDlg(CWnd* pParent = NULL);   // ��׼���캯��
	bool UpdateInputs();
	fdGlove *m_pGlove;	//�������״���ָ��
	CLineChart *lineChart;	//����ͼ����ָ��
	CListBox	m_ComList;
	CListBox	m_ComList_gesture;
	CCheckListBox m_check;
	CString iGesture;
	int gesture=-1;
	float tumb = 0;
	string strGesture;
	string strTumb;
	string strResult;
	float angle[15];//�洢�Ƕ�ֵ
	getPythonData gpd;
	bool isSynchronize=false;//�Ƕ�ͬ��
	virtual ~GloveSampleDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_RobotControl };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnLbnSelchangeList2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOk();
	afx_msg void OnClickedSendGesture();
	afx_msg void OnSendAngle();
	afx_msg void OnClickedSynchronize();
	afx_msg void OnBnClickedDrawline();
};
