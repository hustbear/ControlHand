// KinectWin.cpp : kinect����ͷ��ʵ���ļ�
//

#include "stdafx.h"
#include "3DSLoader.h"
#include "KinectWin.h"
#include "afxdialogex.h"
#include "MainFrm.h"

SOCKET listen_sock;
SOCKET sock;

SOCKET sock1;
SOCKET sock2;

std::string colorImagePath2 = "./images/pcd0055r.png"; //��ʼ����ͼ��ַ
std::vector<std::string> result;
bool analyFlag=false;
bool imageFlag = false;
bool methodFlag = true;//������������ʾ�Ƿ���fast-rcnn����
bool isRunning = true;

bool remoteAnaly = false;
bool remoteAngle = false;

// KinectWin �Ի���
IMPLEMENT_DYNAMIC(KinectWin, CDialogEx)

KinectWin::KinectWin(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_Kinect, pParent)
{

}


void KinectWin::OnTimer(UINT_PTR nIDEvent)
{
	if (1 == nIDEvent) {
		CaptureVideo(); //��׽�Դ�����ͷ
	}
	else if (2 == nIDEvent) {
		CaptureKinectVideo(); //��׽��ɫ����ͷ
	}
	else if (3 == nIDEvent) {
		CaptureKinectDepthVideo(); //��׽�������ͷ
	}
	CDialog::OnTimer(nIDEvent);
}

void KinectWin::CaptureVideo()
{
	cv::Mat frame;
	capture >> frame;
	cv::Mat dst;
	cv::resize(frame, dst, cv::Size(x, y), 0, 0, 1);
	cv::imshow("view", dst);
}

void KinectWin::CaptureKinectVideo()
{
	const NUI_IMAGE_FRAME * pImageFrame = NULL;
	//4.1�����޵ȴ��µ����ݣ��ȵ��󷵻�  
	if (WaitForSingleObject(nextColorFrameEvent, INFINITE) == 0)
	{
		//4.2���ӸղŴ���������������еõ���֡���ݣ���ȡ�������ݵ�ַ����pImageFrame  
		hr = NuiImageStreamGetNextFrame(colorStreamHandle, 0, &pImageFrame);
		if (FAILED(hr))
		{
			AfxMessageBox(_T("���ܻ�ȡ��ɫͼ��"));
			NuiShutdown();
			return;
		}

		INuiFrameTexture * pTexture = pImageFrame->pFrameTexture;
		NUI_LOCKED_RECT LockedRect;

		//4.3����ȡ����֡��LockedRect���������������ݶ���pitchÿ���ֽ�����pBits��һ���ֽڵ�ַ  
		//���������ݣ����������Ƕ����ݵ�ʱ��kinect�Ͳ���ȥ�޸���  
		pTexture->LockRect(0, &LockedRect, NULL, 0);
		//4.4��ȷ�ϻ�õ������Ƿ���Ч  
		if (LockedRect.Pitch != 0)
		{
			//4.5��������ת��ΪOpenCV��Mat��ʽ  
			for (int i = 0; i < image.rows; i++)
			{
				uchar *ptr = image.ptr<uchar>(i);  //��i�е�ָ��  

												   //ÿ���ֽڴ���һ����ɫ��Ϣ��ֱ��ʹ��uchar  
				uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
				for (int j = 0; j < image.cols; j++)
				{
					ptr[3 * j] = pBuffer[4 * j];  //�ڲ�������4���ֽڣ�0-1-2��BGR����4������δʹ��   
					ptr[3 * j + 1] = pBuffer[4 * j + 1];
					ptr[3 * j + 2] = pBuffer[4 * j + 2];
				}
			}
			cv::Mat dst;
			cv::resize(image, dst, cv::Size(x, y), 0, 0, 1);
			cv::imshow("view", dst);
		}
		else
		{
			AfxMessageBox(_T("��ȡ������Ч"));
		}
		//5����֡�Ѿ��������ˣ����Խ������  
		pTexture->UnlockRect(0);
		//6���ͷű�֡���ݣ�׼��ӭ����һ֡   
		NuiImageStreamReleaseFrame(colorStreamHandle, pImageFrame);
	}
}

void KinectWin::CaptureKinectDepthVideo()
{
	const NUI_IMAGE_FRAME *pImageFrame_depth = NULL;
	//���ͼ��Ĵ���
	if (WaitForSingleObject(nextDepthFrameEvent, INFINITE) == 0)
	{
		hr = NuiImageStreamGetNextFrame(depthStreamHandle, 0, &pImageFrame_depth);
		if (FAILED(hr))
		{
			AfxMessageBox(_T("���ܶ�ȡ���ͼ��"));
			NuiShutdown();
			return;
		}
		INuiFrameTexture * pTexture = pImageFrame_depth->pFrameTexture;
		NUI_LOCKED_RECT LockedRect;
		//7.3����ȡ����֡��LockedRect���������������ݶ���pitchÿ���ֽ�����pBits��һ���ֽڵ�ַ  
		//���������ݣ����������Ƕ����ݵ�ʱ��kinect�Ͳ���ȥ�޸���  
		pTexture->LockRect(0, &LockedRect, NULL, 0);
		//7.4��ȷ�ϻ�õ������Ƿ���Ч  
		if (LockedRect.Pitch != 0)
		{
			//7.5��������ת��ΪOpenCV��Mat��ʽ  
			for (int i = 0; i<depthImage.rows; i++)
			{
				uchar *ptr = depthImage.ptr(i);  //��i�е�ָ��  
												 //ÿ���ֽڴ���һ����ɫ��Ϣ��ֱ��ʹ��uchar  
				uchar *pBuffer = (uchar*)(LockedRect.pBits) + i * LockedRect.Pitch;
				USHORT *pBufferRun = (USHORT *)pBuffer;//������Ҫת������Ϊÿ�����������2���ֽڣ�Ӧ��BYTEת��USHORT  
				for (int j = 0; j<depthImage.cols; j++)
				{
					ptr[j] = 255 - (BYTE)(256 * pBufferRun[j] / 0x1fff); //�����ݹ�һ������*  
				}
			}
			cv::Mat dst;
			cv::resize(depthImage, dst, cv::Size(xd, yd), 0, 0, 1);
			cv::imshow("depth", dst);
		}
		else
		{
			AfxMessageBox(_T("��ȡ������Ч"));
		}
		//8����֡�Ѿ��������ˣ����Խ������  
		pTexture->UnlockRect(0);
		//9���ͷű�֡���ݣ�׼��ӭ����һ֡  
		NuiImageStreamReleaseFrame(depthStreamHandle, pImageFrame_depth);
	}
}

void KinectWin::Update(CString str)
{
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	int nLength =pEdit->GetWindowTextLength();
	//ѡ����ǰ�ı���ĩ��  
	pEdit->SetSel(nLength, nLength);
	pEdit->ReplaceSel(str + _T("\r\n"));
}
void KinectWin::UpdateCls(CString cls)
{
	//��������
	CEdit* pCls = (CEdit*)GetDlgItem(IDC_CLS);
	int nLength = pCls->GetWindowTextLength();
	//ѡ����ǰ�ı���ĩ��  
	pCls->SetSel(nLength, nLength);
	pCls->ReplaceSel(cls + _T("\r\n"));
}

KinectWin::~KinectWin()
{
}

void KinectWin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_method);
	DDX_Control(pDX, IDC_COMBOCLIENT, clientType);
}



BEGIN_MESSAGE_MAP(KinectWin, CDialogEx)
	ON_WM_TIMER()
	//ON_BN_CLICKED(IDC_SELF, &KinectWin::OnBnClickedSelf)
	ON_BN_CLICKED(IDC_BUTTON4, &KinectWin::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON1, &KinectWin::OnBnClickedButton1)
	ON_BN_CLICKED(IDCANCEL2, &KinectWin::OnBnClickedCancel2)
	ON_BN_CLICKED(IDC_BUTTON5, &KinectWin::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_ScreenShot, &KinectWin::OnBnClickedScreenshot)
	ON_BN_CLICKED(IDC_REMOTE, &KinectWin::OnBnClickedRemote)
	ON_BN_CLICKED(IDC_ANALY, &KinectWin::OnBnClickedAnaly)
	ON_BN_CLICKED(IDC_RESET, &KinectWin::OnBnClickedReset)
	ON_CBN_SELCHANGE(IDC_COMBO1, &KinectWin::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_CONNECT, &KinectWin::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_SENDFILE, &KinectWin::OnBnClickedSendfile)
	ON_BN_CLICKED(IDC_WRITEANGLE, &KinectWin::OnBnClickedWriteangle)
	ON_BN_CLICKED(IDCANCEL, &KinectWin::OnBnClickedCancel)
END_MESSAGE_MAP()


// KinectWin ��Ϣ�������
BOOL KinectWin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_method.AddString("Faster RCNN");
	m_method.AddString("SSD");
	m_method.SetCurSel(0);

	clientType.AddString("client1");
	clientType.AddString("client2");
	clientType.SetCurSel(0);

	count = 0;
	cvNamedWindow("view", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("depth", CV_WINDOW_AUTOSIZE);

	HWND hWnd = (HWND)cvGetWindowHandle("view");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STATIC)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);//���ظ�����
	
	HWND hWndDept = (HWND)cvGetWindowHandle("depth");
	HWND hParentDept = ::GetParent(hWndDept);
	::SetParent(hWndDept, GetDlgItem(IDC_Dept)->m_hWnd);
	::ShowWindow(hParentDept, SW_HIDE);//���ظ�����
	

	/*��ʼ�����б༭��*/
	pEdit = (CEdit*)GetDlgItem(IDC_EDIT1);
	imageCut = NULL; //��ʼ��ԭʼͼ��

	image.create(480, 640, CV_8UC3);
	depthImage.create(480, 640, CV_8UC1);
	//1����ʼ��NUI   
	hr = NuiInitialize(NUI_INITIALIZE_FLAG_USES_COLOR | NUI_INITIALIZE_FLAG_USES_DEPTH); //��ʼ����ɫ�����ͼ��
	if (FAILED(hr))
	{
		//׷���ı�  
		Update("��kinectʧ��,����ʹ��kinect��ȡͼ��");
	}
	else
	{
		Update("��kinect�ɹ�");
	}

	/*socket����*/
	AfxBeginThread(&Server_Th, 0); //��ʼ��socket�߳�
	send_edit = (CEdit *)GetDlgItem(IDC_ESEND);
	send_edit->SetFocus();

	//��ť��ɫ
	GetDlgItem(IDC_SENDFILE)->EnableWindow(false);
	GetDlgItem(IDC_REMOTE)->EnableWindow(false);

	char name[80];
	CString IP;
	hostent * pHost;
	WSADATA wsData;
	::WSAStartup(MAKEWORD(2, 2), &wsData);
	//���������  
	if (gethostname(name, sizeof(name)))
	{
		Update("�޷���ȡ������ַ");
		return TRUE;
	}

	pHost = gethostbyname(name);//��������ṹ  
	IP = inet_ntoa(*(in_addr *)pHost->h_addr);
	Update("������ַ" + IP);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}
/*
void KinectWin::OnBnClickedSelf()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	Update("���Դ�����ͷ");
	CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_STATIC);
	pWnd->GetClientRect(&rect); //��ȡ�ؼ���С
	x = rect.Width();
	y = rect.Height();
	capture = 0;
	SetTimer(1, 25, NULL); //��ʱ������ʱʱ���֡��һ��
}
*/
void KinectWin::OnBnClickedButton4()
{
	//������ȡ��һ֡���ź��¼����������KINECT�Ƿ���Կ�ʼ��ȡ��һ֡����  
	nextColorFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	colorStreamHandle = NULL; //����ͼ���������ľ����������ȡ����   
							  //3����KINECT�豸�Ĳ�ɫͼ��Ϣͨ��������colorStreamHandle��������ľ�����Ա����Ժ��ȡ  
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_COLOR, NUI_IMAGE_RESOLUTION_640x480,
		0, 2, nextColorFrameEvent, &colorStreamHandle);
	if (FAILED(hr))//�ж��Ƿ���ȡ��ȷ   
	{
		AfxMessageBox(_T("��ȡ����"));
		NuiShutdown();
		return;
	}
	CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_STATIC);
	pWnd->GetClientRect(&rect); //��ȡ�ؼ���С
	x = rect.Width();
	y = rect.Height();
	//4����ʼ��ȡ��ɫͼ����   
	//׷���ı�  
	Update("��ʼ��ȡkinect��ɫ����ͷ����");
	SetTimer(2, 25, NULL); //��ʱ������ʱʱ���֡��һ��
}

void KinectWin::DrawPicToHDC(IplImage * img, UINT ID)
{
	CDC *pDC = GetDlgItem(ID)->GetDC();
	HDC hDC = pDC->GetSafeHdc();
	CRect rect;
	GetDlgItem(ID)->GetClientRect(&rect);
	CvvImage cimg;
	cimg.CopyOf(img); // ����ͼƬ
	cimg.DrawToHDC(hDC, &rect); // ��ͼƬ���Ƶ���ʾ�ؼ���ָ��������
	ReleaseDC(pDC);
}

void KinectWin::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������	
	CString filter;
	filter = "����ͼƬ(*.png;*.jpg)||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	CString result;
	CString strPath = "F:\\3DSLoader\\images\\";
	dlg.m_ofn.lpstrInitialDir = strPath;//��ʼ��·����
	if (dlg.DoModal() == IDOK)
	{
		result = dlg.GetPathName();
		colorImagePath2 = result;
	}

	if (imageCut) cvReleaseImage(&imageCut);
	imageCut = cvLoadImage(colorImagePath2.c_str(), 1); //��ʾͼƬ
	DrawPicToHDC(imageCut, IDC_Cut);
	Update("�Ѵ�ѡ��ͼƬ");
}

void KinectWin::OnBnClickedCancel2()
{
	KillTimer(1);
	KillTimer(2);
	KillTimer(3);
	/*if (SUCCEEDED(hr)) {
	NuiShutdown();
	}*/
	if (analyFlag) {
		close(sock);
		close(listen_sock);
	}
	else {
		closesocket(sock);
		closesocket(listen_sock);
		this->PostMessage(WM_CLOSE, 0, 0);
	}
}


void KinectWin::OnBnClickedButton5()
{
	// ���������ͷ
	//2�������¼����   
	//������ȡ��һ֡���ź��¼����������KINECT�Ƿ���Կ�ʼ��ȡ��һ֡����  
	nextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	//���ͼ���¼����
	depthStreamHandle = NULL;
	//ʵ����������������NUI_IMAGE_TYPE_DEPTH��ʾ���ͼ��
	hr = NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480, 0, 2, nextDepthFrameEvent, &depthStreamHandle);
	if (FAILED(hr))//�ж��Ƿ���ȡ��ȷ   
	{
		AfxMessageBox(_T("��ȡ��ȴ���"));
		NuiShutdown();
		return;
	}
	Update("��ʼ��ȡkinect�������ͷ����");
	
	CRect rect;
	CWnd *pWnd = GetDlgItem(IDC_Dept);
	
	pWnd->GetClientRect(&rect); //��ȡ�ؼ���С
	xd = rect.Width();
	yd = rect.Height();
	SetTimer(3, 25, NULL); //��ʱ������ʱʱ���֡��һ��
}


void KinectWin::OnBnClickedScreenshot()
{
	if (FAILED(hr))
	{
		AfxMessageBox(_T("δ��kinect�����ܽ�ͼ!"));
		return;
	}
	std::string basePath = "./images/";
	std::string colorPathPrefix = "Color";
	std::string depthPathPrefix = "Depth";
	std::string commonPathSuffix = ".png";
	std::stringstream ss;
	ss << basePath << "pcd00" << count << "r" << commonPathSuffix;
	colorImagePath2 = ss.str();
	imwrite(colorImagePath2, image);
	/*
	std::stringstream ssd;
	ssd << basePath<<count<<"-" << depthPathPrefix << commonPathSuffix;
	std::string depthImagePath = ssd.str();
	imwrite(depthImagePath, depthImage);*/
	count++;
	Update("��ͼ�ɹ�,�ѱ��浽images�ļ�����");
}


UINT Server_Th(LPVOID p)
{
	//kinect�߳�
	WSADATA wsaData;

	WORD wVersion;

	wVersion = MAKEWORD(2, 2);

	WSAStartup(wVersion, &wsaData);

	SOCKADDR_IN local_addr;
	SOCKADDR_IN client_addr;
	int iaddrSize = sizeof(SOCKADDR_IN);
	int res;
	char msg[1024];
	HWND hWnd = ::FindWindow(NULL, _T("��ȡ����ͷ"));      //�õ��Ի���ľ��
	KinectWin * dlg = (KinectWin *)KinectWin::FromHandle(hWnd); //�ɾ���õ��Ի���Ķ���ָ��
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(5150);
	local_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if ((listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		dlg->Update("��������ʧ��");
	}
	if (bind(listen_sock, (struct sockaddr*) &local_addr, sizeof(SOCKADDR_IN)))
	{
		dlg->Update("�󶨴���");
	}
	listen(listen_sock, 1);

	while (isRunning)
	{
		if ((sock=accept(listen_sock, (struct sockaddr *)&client_addr, &iaddrSize)) == INVALID_SOCKET)
		{
			dlg->Update("accept ʧ��");
		}
		else
		{
			CString port;
			int temp = ntohs(client_addr.sin_port);
			port.Format(_T("%d"), temp);
			CString ipClient = CString(inet_ntoa(client_addr.sin_addr));
			dlg->Update("���������ԣ�" + ipClient + "  �˿ڣ�" + port);
			if (strcmp(ipClient, "125.216.243.10") == 0)
			{
				sock1 = sock;
				AfxBeginThread(&Client_Th1, 0); //��ʼ��socket�߳�
				remoteAngle = true;
				dlg->GetDlgItem(IDC_SENDFILE)->EnableWindow(true);
				dlg->Update("�����ӽǶȷ����ͻ���");
			}
			else if (strcmp(ipClient, "125.216.243.115") == 0)
			{
				sock2 = sock;
				remoteAnaly = true;
				AfxBeginThread(&Client_Th2, 0); //��ʼ��socket�߳�

				dlg->GetDlgItem(IDC_REMOTE)->EnableWindow(true);
				dlg->Update("������ͼƬ�����ͻ���");
			}
		}
	}
	return 0;
}
UINT Client_Th2(LPVOID p)
{
	int res;
	char msg[1024];
	HWND hWnd = ::FindWindow(NULL, _T("��ȡ����ͷ"));      //�õ��Ի���ľ��
	KinectWin * dlg = (KinectWin *)KinectWin::FromHandle(hWnd); //�ɾ���õ��Ի���Ķ���ָ��
																//��������  
	while (1)
	{
		if ((res = recv(sock2, msg, 1024, 0)) == -1)
		{
			dlg->Update("ʧȥͼƬ�����ͻ�������");
			break;
		}
		else
		{
			int temp = strlen(msg);
			msg[temp] = '\0';
			if (strcmp(msg, "leave") == 0) {
				dlg->Update("ʧȥͼƬ�����ͻ�������");
				break;
			}
			else if (strcmp(msg, "sendResult") == 0) {
				analyFlag = true;
				dlg->Update("׼����÷������");
				memset(msg, 0, 1024);
				res = recv(sock, msg, 1024, 0);
				dlg->Update("���:" + CString(msg));

				// �Զ���Ϊ�ָ�������ַ���
				std::string temp = msg;
				int count = 0;
				int head = 0;
				for (int i = 0; i <= temp.length(); i++) {
					if (i == temp.length() || temp[i] == ',') {
						result.push_back(temp.substr(head, count));
						head = i + 1;
						count = 0;
					}
					else
					{
						count++;
					}
				}
				//�����д���������
				dlg->UpdateCls("���:" + CString(msg));
				dlg->OnBnClickedAnaly();
				result.clear();
			}
			else {
				dlg->Update("client1:" + CString(msg));
				memset(msg, 0, 1024);
			}
		}
	}
	dlg->GetDlgItem(IDC_REMOTE)->EnableWindow(false);
	return 0;
}

UINT Client_Th1(LPVOID p)
{
	int res;
	char msg[1024];
	HWND hWnd = ::FindWindow(NULL, _T("��ȡ����ͷ"));      //�õ��Ի���ľ��
	KinectWin * dlg = (KinectWin *)KinectWin::FromHandle(hWnd); //�ɾ���õ��Ի���Ķ���ָ��
	while (1)
	{
		if ((res = recv(sock1, msg, 1024, 0)) == -1)
		{
			dlg->Update("ʧȥ�Ƕȷ����ͻ�������");
			break;
		}
		else
		{
			int temp = strlen(msg);
			msg[temp] = '\0';
			if (strcmp(msg, "leave") == 0) {
				dlg->Update("ʧȥ�Ƕȷ����ͻ�������");
				break;
			}
			else {
				dlg->Update("client2:" + CString(msg));
				memset(msg, 0, 1024);
			}
		}
	}
	dlg->GetDlgItem(IDC_SENDFILE)->EnableWindow(false);
	return 0;
}

void KinectWin::OnBnClickedRemote()
{
	HANDLE hFile;
	imageFlag = false; //ͼƬδ����
	GetDlgItem(IDC_CLS)->SetWindowText(_T("")); //��ս����ʾ��

	if (imageCut) cvReleaseImage(&imageCut);
	imageCut = cvLoadImage(colorImagePath2.c_str(), 1); //��ʾͼƬ
	DrawPicToHDC(imageCut, IDC_Cut);
	Update("�Ѵ�ѡ��ͼƬ");

	hFile = CreateFile(CString(colorImagePath2.c_str()), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	unsigned long long file_size = 0;
	file_size = GetFileSize(hFile, NULL);
	char buffer[BUFFER_SIZE];
	const char * str;
	if (methodFlag) {
		str = "sendImage_f";
	}
	else
	{
		str = "sendImage_s";
	}
	//����׼����������
	memset(buffer, 0, BUFFER_SIZE);
	strncpy(buffer, str, strlen(str));
	if (send(sock2, buffer, strlen(str), 0) == SOCKET_ERROR)
	{
		Update("����ʧ��");
		return;
	}
	else
	{
		Update("server:׼������ͼƬ");
	}

	//����ͼƬ����
	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, &file_size, sizeof(file_size) + 1);
	if (send(sock2, buffer, sizeof(file_size) + 1, 0) == SOCKET_ERROR)
	{
		Update("����ʧ��");
		return;
	}
	else
	{
		Update("��ʼ����ͼƬ");
	}
	memset(buffer, 0, sizeof(buffer));
	DWORD dwNumberOfBytesRead;
	do
	{
		::ReadFile(hFile, buffer, sizeof(buffer), &dwNumberOfBytesRead, NULL);
		::send(sock2, buffer, dwNumberOfBytesRead, 0);
	} while (dwNumberOfBytesRead);
	CloseHandle(hFile);
	Update("�ɹ�����ͼƬ");
}

void KinectWin::OnBnClickedAnaly()
{
	CString x, y, angle, width;
	float xRaw[4], yRaw[4];
	float xNew[4], yNew[4];
	float height = 33;
	if (!imageFlag) {
		imageCut = cvLoadImage(colorImagePath2.c_str(), 1); //��ʾͼƬ
		imageFlag = true;
	}
	if (result.size() < 4) {
		AfxMessageBox("��δ�õ����������");
		return;
	}
	float xf = atof(result[1].c_str());
	float yf = atof(result[2].c_str());
	float anglef = atof(result[3].c_str());
	float widthf = atof(result[4].c_str());


	xRaw[1] = xf - 0.5*widthf;
	yRaw[1] = yf - 0.5*height;

	xRaw[0] = xf + 0.5*widthf;
	yRaw[0] = yRaw[1];

	xRaw[2] = xRaw[1];
	yRaw[2] = yf + 0.5*height;

	xRaw[3] = xRaw[0];
	yRaw[3] = yRaw[2];



	float anglePi = -anglef*pi / 180.0;
	float cosA = cos(anglePi);
	float sinA = sin(anglePi);

	xNew[0] = (xRaw[0] - xf)*cosA - (yRaw[0] - yf)*sinA + xf;
	yNew[0] = (xRaw[0] - xf)*sinA + (yRaw[0] - yf)*cosA + yf;

	xNew[1] = (xRaw[1] - xf)*cosA - (yRaw[1] - yf)*sinA + xf;
	yNew[1] = (xRaw[1] - xf)*sinA + (yRaw[1] - yf)*cosA + yf;

	xNew[2] = (xRaw[2] - xf)*cosA - (yRaw[2] - yf)*sinA + xf;
	yNew[2] = (xRaw[2] - xf)*sinA + (yRaw[2] - yf)*cosA + yf;

	xNew[3] = (xRaw[3] - xf)*cosA - (yRaw[3] - yf)*sinA + xf;
	yNew[3] = (xRaw[3] - xf)*sinA + (yRaw[3] - yf)*cosA + yf;

	cvLine(imageCut, cvPoint(xNew[0], yNew[0]), cvPoint(xNew[1], yNew[1]), cvScalar(0, 0, 255), 3);
	cvLine(imageCut, cvPoint(xNew[1], yNew[1]), cvPoint(xNew[2], yNew[2]), cvScalar(255, 0, 0), 3);
	cvLine(imageCut, cvPoint(xNew[2], yNew[2]), cvPoint(xNew[3], yNew[3]), cvScalar(0, 0, 255), 3);
	cvLine(imageCut, cvPoint(xNew[0], yNew[0]), cvPoint(xNew[3], yNew[3]), cvScalar(255, 0, 0), 3);

	DrawPicToHDC(imageCut, IDC_Cut);
	Update("ͼƬ���޸�");
}

void KinectWin::OnBnClickedReset()
{
	//��λͼƬ
	if (imageCut) cvReleaseImage(&imageCut);
	imageCut = cvLoadImage(colorImagePath2.c_str(), 1); //��ʾͼƬ
	DrawPicToHDC(imageCut, IDC_Cut);
	Update("ͼƬ�Ѹ�λ");
}


void KinectWin::OnCbnSelchangeCombo1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int nIndex = m_method.GetCurSel();
	if (nIndex == 0) {
		methodFlag = true;
		Update("ѡ��Faster RCNN��������");
	}
	else
	{
		Update("ѡ��SSD��������");
		methodFlag = false;
	}
}


void KinectWin::OnBnClickedConnect()
{
	send_edit = (CEdit *)GetDlgItem(IDC_ESEND);
	send_edit->SetFocus();
	CString str;
	send_edit->GetWindowText(str);
	char buffer[BUFFER_SIZE];
	//����׼����������
	memset(buffer, 0, BUFFER_SIZE);
	strncpy(buffer, str, strlen(str));
	int nIndex = clientType.GetCurSel();
	if (nIndex == 0) 
	{
		sock = sock1;
	}
	else 
	{
		sock = sock2;
	}

	if (send(sock, buffer, strlen(str), 0) == SOCKET_ERROR)
	{
		Update("����ʧ��");
	}
	else if (str == "")
	{
		AfxMessageBox(_T("��������Ϣ"));
	}
	else
	{
		Update("server:"+str);
		send_edit->SetWindowText("");
	}
}


void KinectWin::OnBnClickedSendfile()
{
	if (!remoteAngle) {
		Update("δ���ӵ��Ƕȷ����ͻ��ˣ������Ӻ����");
		return;
	}
	HANDLE hFile;
	string txtPath = "angle.txt";
	hFile = CreateFile(CString(txtPath.c_str()), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	unsigned long long file_size = 0;
	file_size = GetFileSize(hFile, NULL);
	char buffer[BUFFER_SIZE];
	const char * str;
	str = "sendAngle";
	//����׼����������
	memset(buffer, 0, BUFFER_SIZE);
	strncpy(buffer, str, strlen(str));
	if (send(sock1, buffer, strlen(str), 0) == SOCKET_ERROR)
	{
		Update("����ʧ��");
		return;
	}
	else
	{
		Update("server:׼�����ͽǶ��ļ�");
	}
	//����ͼƬ����
	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, &file_size, sizeof(file_size) + 1);
	if (send(sock1, buffer, sizeof(file_size) + 1, 0) == SOCKET_ERROR)
	{
		Update("����ʧ��");
		return;
	}
	else
	{
		Update("��ʼ���ͽǶ��ļ�");
	}
	memset(buffer, 0, sizeof(buffer));
	DWORD dwNumberOfBytesRead;
	do
	{
		::ReadFile(hFile, buffer, sizeof(buffer), &dwNumberOfBytesRead, NULL);
		::send(sock1, buffer, dwNumberOfBytesRead, 0);
	} while (dwNumberOfBytesRead);
	CloseHandle(hFile);
	Update("�ɹ����ͽǶ��ļ�");
}


void KinectWin::OnBnClickedWriteangle()
{
	CMainFrame *pFrame;
	pFrame = (CMainFrame*)AfxGetMainWnd();
	PostMessageW(pFrame->m_hWnd, WM_MYMSG3, NULL, NULL);
	Update("��д��Ƕ��ļ�angle.txt");
}


void KinectWin::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	closesocket(sock);
	closesocket(sock1);
	closesocket(sock2);
	closesocket(listen_sock);
	CDialogEx::OnCancel();
}
