#pragma once
#include "GloveSampleDlg.h"
#include "tchart1.h"
#include "CSeries.h"

// CLineChart �Ի���

class CLineChart : public CDialogEx
{
	DECLARE_DYNAMIC(CLineChart)

public:
	CLineChart(CWnd* pParent = NULL);   // ��׼���캯��

	afx_msg void OnBnClickedDrawline();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void drawMoving();
	double m_ThumbArray[3][32];	//��Ĵָ�Ƕ�����
	CSeries LineThumb[3];	//��Ĵָ����

	double m_IndexArray[3][32];	//ʳָ�Ƕ�����
	CSeries LineIndex[3];	//ʳָ����

	double m_MiddleArray[2][32];	//��ָ�Ƕ�����
	CSeries LineMiddle[2];	//��ָ����

	double m_RingArray[3][32];	//����ָ�Ƕ�����
	CSeries LineRing[3];	//����ָ����

	double m_LittleArray[3][32];	//Сָ�Ƕ�����
	CSeries LineLittle[3];	//Сָ����

	double m_BaseArray[32];	//����ؽڽǶ�����
	CSeries LineBase;	//����ؽ�����

	double m_X[32];
	unsigned int m_count;
	const size_t m_Length = 32;
	virtual ~CLineChart();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DRAW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	
public:
	CTchart1 CThumb;
	CTchart1 CIndex;
	CTchart1 CMiddle;
	CTchart1 CRing;
	CTchart1 CLittle;
	CTchart1 CBase;
};
