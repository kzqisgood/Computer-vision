﻿
// MFCDemo2Dlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCDemo2.h"
#include "MFCDemo2Dlg.h"
#include "afxdialogex.h"

#include "ImageProcess.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMFCDemo2Dlg 对话框



CMFCDemo2Dlg::CMFCDemo2Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCDEMO2_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCDemo2Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_FUNC, m_combFunc);
}

BEGIN_MESSAGE_MAP(CMFCDemo2Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_OPEN_FILE, &CMFCDemo2Dlg::OnBnClickedBtnOpenFile)

	ON_STN_CLICKED(IDC_STATIC_ORG_BMP, &CMFCDemo2Dlg::OnStnClickedStaticOrgBmp)

	ON_STN_CLICKED(IDC_STATIC_ORG_BMP, &CMFCDemo2Dlg::OnStnClickedStaticOrgBmp)
	ON_STN_CLICKED(IDC_STATIC_ORG_BMP, &CMFCDemo2Dlg::OnStnClickedStaticOrgBmp)
	ON_STN_CLICKED(IDC_STATIC_OBJ_BMP, &CMFCDemo2Dlg::OnStnClickedStaticObjBmp)
	ON_CBN_SELCHANGE(IDC_COMBO_FUNC, &CMFCDemo2Dlg::OnCbnSelchangeComboFunc)
END_MESSAGE_MAP()


// CMFCDemo2Dlg 消息处理程序

BOOL CMFCDemo2Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_combFunc.AddString("图像平移");
	m_combFunc.AddString("图像镜像翻转");
	m_combFunc.AddString("图像仿射变换");
	m_combFunc.AddString("图像转置");
	m_combFunc.AddString("图像旋转");
	m_combFunc.AddString("图像缩放");
	m_combFunc.AddString("边缘算子");
	m_combFunc.AddString("Sobel 边缘算子（水平方向)");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCDemo2Dlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCDemo2Dlg::OnPaint()
{
	CPoint ptLeftTop;
	CRect rectOrcBmp;
	GetDlgItem(IDC_STATIC_ORG_BMP)->GetClientRect(&rectOrcBmp);
	ptLeftTop.x = rectOrcBmp.left;
	ptLeftTop.y = rectOrcBmp.top;
	m_image_org.ShowImage(GetDlgItem(IDC_STATIC_ORG_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	CPaintDC dc(this);
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCDemo2Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFCDemo2Dlg::OnBnClickedBtnOpenFile()
{
//	AfxMessageBox(_T("测试"));
	TCHAR szFilter[] = _T("位图文件(*.bmp)|*.bmp|所有文件(*.*)|*.*||");	//设置过滤器
	CFileDialog fileDlg(TRUE, _T("bmp"), NULL, 0, szFilter, this);	//构造打开文件对话框
	CString strFilePath;//	文件路径
	CPoint ptLeftTop;
	CRect rectOrcBmp;
	GetDlgItem(IDC_STATIC_ORG_BMP)->GetClientRect(&rectOrcBmp);
	ptLeftTop.x = rectOrcBmp.left;
	ptLeftTop.y = rectOrcBmp.top;
	if (IDOK == fileDlg.DoModal())	// 显示打开文件对话框
	{
		strFilePath = fileDlg.GetPathName();

		m_image_org.ImportFrom(strFilePath);//显示图像

		m_image_org.ShowImage(GetDlgItem(IDC_STATIC_ORG_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
	}
}


void CMFCDemo2Dlg::OnStnClickedStaticOrgBmp()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CMFCDemo2Dlg::OnStnClickedStaticObjBmp()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CMFCDemo2Dlg::OnCbnSelchangeComboFunc()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_combFunc.GetCurSel();

	CString str;

	CPoint ptLeftTop;
	CRect rectOrcBmp;
	GetDlgItem(IDC_STATIC_OBJ_BMP)->GetClientRect(&rectOrcBmp);
	ptLeftTop.x = rectOrcBmp.left;
	ptLeftTop.y = rectOrcBmp.top;


	m_combFunc.GetLBText(nIndex, str);

	if (0 == str.Compare("图像平移"))
	{
		if (m_image_org.IsColorImage())
		{
			long delta_x = 60;
			long delta_y = 30;
			CTMatrix< RGB_TRIPLE > translated_image = CImageProcess::Image_translation(m_image_org.Get_color_image(), delta_x, delta_y);
			m_image_Obj.ImportFrom(translated_image);

			m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
		}

	}
	else if (0 == str.Compare("图像镜像翻转"))
	{
		CTMatrix< RGB_TRIPLE > Image_mirror = CImageProcess::Image_mirror(m_image_org.Get_color_image(), true);
		m_image_Obj.ImportFrom(Image_mirror);

		m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
	}
	else if (0 == str.Compare("图像仿射变换"))
	{
		CTMatrix< RGB_TRIPLE > Image_affine = CImageProcess::Image_affine(m_image_org.Get_color_image(), 0.1, 0.1, 0.3, 0.3, 0.4, 0.2);
		m_image_Obj.ImportFrom(Image_affine);

		m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
	}
	else if (0 == str.Compare("图像转置"))
	{
		CTMatrix< RGB_TRIPLE > Image_transpose =CImageProcess::Image_transpose(m_image_org.Get_color_image());
		m_image_Obj.ImportFrom(Image_transpose);

		m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
	}
	else if (0 == str.Compare("图像旋转"))
	{
		CTMatrix< RGB_TRIPLE > Image_rotation = CImageProcess::Image_rotation(m_image_org.Get_color_image(), 90);
		m_image_Obj.ImportFrom(Image_rotation);

		m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
	}
	else if (0 == str.Compare("图像缩放"))
	{
		double h1, w1;
		h1 = 0.5;
		w1 = 0.5;
		CTMatrix< RGB_TRIPLE > Image_zoom = CImageProcess::Image_zoom(m_image_org.Get_color_image(), w1, h1);
		m_image_Obj.ImportFrom(Image_zoom);
		m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width()*w1, rectOrcBmp.Height()*h1));
//		m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(m_image_org.Get_image_width(), m_image_org.Get_image_height()));
	}
	else if (0 == str.Compare("边缘算子"))
	{
		CTMatrix< BYTE > Robert_edge_operator = CImageProcess::Robert_edge_operator(m_image_org.Get_gray_image());
		m_image_Obj.ImportFrom(Robert_edge_operator);
		m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
	}
	else if (0 == str.Compare("Sobel 边缘算子（水平方向)"))
	{
		CTMatrix< BYTE > Sobel_edge_horizontal = CImageProcess::Sobel_edge_horizontal(m_image_org.Get_gray_image());
		m_image_Obj.ImportFrom(Sobel_edge_horizontal);
		m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
	}
}
