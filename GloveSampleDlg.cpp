// GloveSampleDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "3DSLoader.h"
#include "GloveSampleDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"


// GloveSampleDlg �Ի���

IMPLEMENT_DYNAMIC(GloveSampleDlg, CDialogEx)

GloveSampleDlg::GloveSampleDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_RobotControl, pParent)
{

}

GloveSampleDlg::~GloveSampleDlg()
{
}

void GloveSampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST2, m_ComList);
	DDX_Control(pDX, IDC_LIST1, m_ComList_gesture);
}


BEGIN_MESSAGE_MAP(GloveSampleDlg, CDialogEx)
	ON_LBN_SELCHANGE(IDC_LIST2, &GloveSampleDlg::OnLbnSelchangeList2)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &GloveSampleDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &GloveSampleDlg::OnClickedSendGesture)
	ON_BN_CLICKED(IDC_CHECK1, &GloveSampleDlg::OnClickedSynchronize)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CGloveSampleDlg message handlers

BOOL GloveSampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add comport choices to the comport listbox.
	m_ComList.AddString("USB 0");
	m_ComList.AddString("USB 1");
	m_ComList.AddString("USB 2");
	m_ComList.AddString("USB 3");

	m_ComList_gesture.AddString("���� 0:  ��");
	m_ComList_gesture.AddString("���� 1:  ��");
	m_ComList_gesture.AddString("���� 2:  ��");
	m_ComList_gesture.AddString("���� 3:  ƽ");
	m_ComList_gesture.AddString("���� 4:  ˺");
	m_ComList_gesture.AddString("���� 5:  ��");
	m_ComList_gesture.AddString("���� 6:  ��");
	m_ComList_gesture.AddString("���� 7:  ��");
	m_ComList_gesture.AddString("���� 8:  ץ");


	m_pGlove = NULL;

	UpdateInputs();
	// Create a timer which pulses 10 times a second.
	SetTimer(1, 10, NULL);

	return TRUE;
}
// Read sensor information, and update the dialog box's static text.
bool GloveSampleDlg::UpdateInputs()
{
	// Check whether a glove is currently connected.
	if (m_pGlove) {
		unsigned short acGloveData[18];
		float acGloveData_f[18]; //��׼������
		char acString[256];
		char acString_f[256];
		char acString_a[256];//�Ƕ�ֵ
		int iGesture;
		strResult = "";
		strTumb = "";
		const char * filename = "./5DT/jc.cal";
		fdLoadCalibration(m_pGlove, filename);
		// Read raw data
		fdGetSensorRawAll(m_pGlove, acGloveData);
		fdGetSensorScaledAll(m_pGlove, acGloveData_f);
		iGesture = fdGetGesture(m_pGlove);
		// Update text
		sprintf(acString, "%d", acGloveData[0]);
		sprintf(acString_f, "%f", acGloveData_f[0]);
		angle[0] = acGloveData_f[0] * 80;
		sprintf(acString_a, "%.2f", angle[0]);
		SetDlgItemText(IDC_S0, acString);
		SetDlgItemText(IDC_A0, acString_a);
		strResult += acString_f;
		strResult += ",";
		strTumb += acString_f;
		strTumb += ",";


		sprintf(acString, "%d", acGloveData[1]);
		sprintf(acString_f, "%f", acGloveData_f[1]);
		angle[1] = acGloveData_f[1] * 90;
		sprintf(acString_a, "%.2f", angle[1]);
		SetDlgItemText(IDC_S1, acString);
		SetDlgItemText(IDC_A1, acString_a);
		strResult += acString_f;
		strResult += ",";
		strTumb += acString_f;
		strTumb += ",";

		sprintf(acString, "%d", acGloveData[2]);
		sprintf(acString_f, "%f", acGloveData_f[2]);
		angle[2] = acGloveData_f[2] * 60;
		sprintf(acString_a, "%.2f", angle[2]);
		SetDlgItemText(IDC_S2, acString);
		SetDlgItemText(IDC_A2, acString_a);
		strResult += acString_f;
		strResult += ",";
		strTumb += acString_f;

		sprintf(acString, "%d", acGloveData[3]);
		sprintf(acString_f, "%f", acGloveData_f[3]);
		angle[3] = acGloveData_f[3] * 90;
		sprintf(acString_a, "%.2f", angle[3]);
		SetDlgItemText(IDC_S3, acString);
		SetDlgItemText(IDC_A3, acString_a);
		strResult += acString_f;
		strResult += ",";

		sprintf(acString, "%d", acGloveData[4]);
		sprintf(acString_f, "%f", acGloveData_f[4]);
		angle[4] = acGloveData_f[4] * 108;
		sprintf(acString_a, "%.2f", angle[4]);
		SetDlgItemText(IDC_S4, acString);
		SetDlgItemText(IDC_A4, acString_a);
		strResult += acString_f;
		strResult += ",";

		sprintf(acString, "%d", acGloveData[5]);
		sprintf(acString_f, "%f", acGloveData_f[5]);
		angle[5] = acGloveData_f[5] * 30-22;
		sprintf(acString_a, "%.2f", angle[5]);
		SetDlgItemText(IDC_S5, acString);
		SetDlgItemText(IDC_A5, acString_a);
		strResult += acString_f;
		strResult += ",";

		sprintf(acString, "%d", acGloveData[6]);
		sprintf(acString_f, "%f", acGloveData_f[6]);
		angle[6] = acGloveData_f[6] * 90;
		sprintf(acString_a, "%.2f", angle[6]);
		SetDlgItemText(IDC_S6, acString);
		SetDlgItemText(IDC_A6, acString_a);
		strResult += acString_f;
		strResult += ",";

		sprintf(acString, "%d", acGloveData[7]);
		sprintf(acString_f, "%f", acGloveData_f[7]);
		angle[7] = acGloveData_f[7] * 108;
		sprintf(acString_a, "%.2f", angle[7]);
		SetDlgItemText(IDC_S7, acString);
		SetDlgItemText(IDC_A7, acString_a);
		strResult += acString_f;
		strResult += ",";

		sprintf(acString, "%d", acGloveData[8]);
		sprintf(acString_f, "%f", acGloveData_f[8]);
		angle[8] = acGloveData_f[8] * 20-16;
		sprintf(acString_a, "%.2f", angle[8]);
		SetDlgItemText(IDC_S8, acString);
		SetDlgItemText(IDC_A8, acString_a);
		strResult += acString_f;
		strResult += ",";

		sprintf(acString, "%d", acGloveData[9]);
		sprintf(acString_f, "%f", acGloveData_f[9]);
		angle[9] = acGloveData_f[9] * 90;
		sprintf(acString_a, "%.2f", angle[9]);
		SetDlgItemText(IDC_S9, acString);
		SetDlgItemText(IDC_A9, acString_a);
		strResult += acString_f;
		strResult += ",";

		sprintf(acString, "%d", acGloveData[10]);
		sprintf(acString_f, "%f", acGloveData_f[10]);
		angle[10] = acGloveData_f[10] * 108;
		sprintf(acString_a, "%.2f", angle[10]);
		SetDlgItemText(IDC_S10, acString);
		SetDlgItemText(IDC_A10, acString_a);
		strResult += acString_f;
		strResult += ",";

		sprintf(acString, "%d", acGloveData[11]);
		sprintf(acString_f, "%f", acGloveData_f[11]);
		angle[11] = acGloveData_f[11] * 20-13;
		sprintf(acString_a, "%.2f", angle[11]);
		SetDlgItemText(IDC_S11, acString);
		SetDlgItemText(IDC_A11, acString_a);
		strResult += acString_f;
		strResult += ",";

		sprintf(acString, "%d", acGloveData[12]);
		sprintf(acString_f, "%f", acGloveData_f[12]);
		angle[12] = acGloveData_f[12] * 90;
		sprintf(acString_a, "%.2f", angle[12]);
		SetDlgItemText(IDC_S12, acString);
		SetDlgItemText(IDC_A12, acString_a);
		strResult += acString_f;
		strResult += ",";

		sprintf(acString, "%d", acGloveData[13]);
		sprintf(acString_f, "%f", acGloveData_f[13]);
		angle[13] = acGloveData_f[13] * 108;
		sprintf(acString_a, "%.2f", angle[13]);
		SetDlgItemText(IDC_S13, acString);
		SetDlgItemText(IDC_A13, acString_a);
		strResult += acString_f;
		// Gesture
		CString str;
		gesture = gpd.getGloveGesture(strResult);
		switch (gesture)
		{
		case 0:
			str = "��";
			break;
		case 1:
			str = "��";
			break;
		case 3:
			str = "ƽ";
			break;
		case 4:
			str = "˺";
			break;
		case 5:
			str = "��";
			break;
		case 6:
			str = "��";
			break;
		case 7:
			str = "��";
			break;
		case 8:
			str = "ץ";
			break;
		default:
			str = " ";
			break;
		}
		SetDlgItemText(IDC_GESTURE, str);
		angle[14] = gpd.getTumbData(strTumb);
		
	}
	else {
		// With no glove, we just zero out all text.
		SetDlgItemText(IDC_S0, "");
		SetDlgItemText(IDC_S1, "");
		SetDlgItemText(IDC_S2, "");
		SetDlgItemText(IDC_S3, "");
		SetDlgItemText(IDC_S4, "");
		SetDlgItemText(IDC_S5, "");
		SetDlgItemText(IDC_S6, "");
		SetDlgItemText(IDC_S7, "");
		SetDlgItemText(IDC_S8, "");
		SetDlgItemText(IDC_S9, "");
		SetDlgItemText(IDC_S10, "");
		SetDlgItemText(IDC_S11, "");
		SetDlgItemText(IDC_S12, "");
		SetDlgItemText(IDC_S13, "");
		SetDlgItemText(IDC_GESTURE, "");
		SetDlgItemText(IDC_A0, "");
		SetDlgItemText(IDC_A1, "");
		SetDlgItemText(IDC_A2, "");
		SetDlgItemText(IDC_A3, "");
		SetDlgItemText(IDC_A4, "");
		SetDlgItemText(IDC_A5, "");
		SetDlgItemText(IDC_A6, "");
		SetDlgItemText(IDC_A7, "");
		SetDlgItemText(IDC_A8, "");
		SetDlgItemText(IDC_A9, "");
		SetDlgItemText(IDC_A10, "");
		SetDlgItemText(IDC_A11, "");
		SetDlgItemText(IDC_A12, "");
		SetDlgItemText(IDC_A13, "");
	}
	return true;
}

// GloveSampleDlg ��Ϣ�������
void GloveSampleDlg::OnLbnSelchangeList2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char acComString[256];
	int iPort;
	// Index into list box indicates com number.
	iPort = m_ComList.GetCurSel();
	gpd.initPython();
	// Close current connection.
	if (m_pGlove) fdClose(m_pGlove);
	m_pGlove = NULL;

	// Create new connection.
	sprintf(acComString, "USB%d", iPort);
	m_pGlove = fdOpen(acComString);
	if (m_pGlove == NULL) {
		MessageBox("Failed to find a glove.", "5dt Glove Sample");
		return;
	}
}
void GloveSampleDlg::OnTimer(UINT_PTR nIDEvent)
{
	// Update inputs on timer interrupt.
	UpdateInputs();
	if (isSynchronize) { //�ж��Ƿ���ͬ��
		OnSendAngle();
	}
	CDialog::OnTimer(nIDEvent);
}

void GloveSampleDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	GetDlgItemText(IDC_GESTURE, iGesture); 
	isSynchronize = false;
	if (m_pGlove) {
		gesture = gpd.getGloveGesture(strResult);
		//tumb = gpd.getTumbData(strTumb);
		//CString str;
		//str.Format("%f", tumb);
		//AfxMessageBox(str);
		switch (gesture)
		{ 
		case 0:
			AfxMessageBox("��ǰ����:��");
			break;
		case 1:
			AfxMessageBox("��ǰ����:��");
			break;
		case 3:
			AfxMessageBox("��ǰ����:ƽ");
			break;
		case 4:
			AfxMessageBox("��ǰ����:˺");
			break;
		case 5:
			AfxMessageBox("��ǰ����:��");
			break;
		case 6:
			AfxMessageBox("��ǰ����:��");
			break;
		case 7:
			AfxMessageBox("��ǰ����:��");
			break;
		case 8:
			AfxMessageBox("��ǰ����:ץ");
			break;
		default:
			break;
		}
	}
//	CDialogEx::OnOK();
}


void GloveSampleDlg::OnClickedSendGesture()
{
	// ���Ͷ�������
	isSynchronize = false;
	if (m_pGlove) {
		CMainFrame *pFrame;
		pFrame = (CMainFrame*)AfxGetMainWnd();
		PostMessageW(pFrame->m_hWnd, WM_MYMSG, NULL, NULL);
	}
}


void GloveSampleDlg::OnSendAngle()
{
	if (m_pGlove) {
		// ���ͽǶ�����
		CMainFrame *pFrame;
		pFrame = (CMainFrame*)AfxGetMainWnd();
		PostMessageW(pFrame->m_hWnd, WM_MYMSG2, NULL, NULL);
	}
}


void GloveSampleDlg::OnClickedSynchronize()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (BST_CHECKED == IsDlgButtonChecked(IDC_CHECK1)) {
		isSynchronize = true;
	}
	else
	{
		isSynchronize = false;
	}
}
