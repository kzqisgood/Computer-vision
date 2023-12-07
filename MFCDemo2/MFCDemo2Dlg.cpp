
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
	DDX_Control(pDX, IDC_COMBO_IMAGE_SEGMENT, m_comboImageSegment);
	DDX_Control(pDX, IDC_COMBO_FUNC2, m_smooth_sharp);
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
	ON_CBN_SELCHANGE(IDC_COMBO_IMAGE_SEGMENT, &CMFCDemo2Dlg::OnCbnSelchangeComboImageSegment)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CMFCDemo2Dlg::OnBnClickedBtnClear)
	ON_CBN_SELCHANGE(IDC_COMBO_FUNC2, &CMFCDemo2Dlg::OnCbnSelchangeComboFunc2)
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
	m_combFunc.AddString("Canny算子");

	m_comboImageSegment.AddString("Robert算子");
	m_comboImageSegment.AddString("Sobel算子");
	m_comboImageSegment.AddString("Canny算子");
	m_comboImageSegment.AddString("Prewitt算子");
	m_comboImageSegment.AddString("区域生长");
	m_comboImageSegment.AddString("分水岭算法");
	m_comboImageSegment.AddString("K 均值聚类");

	m_smooth_sharp.AddString("中值滤波");
	m_smooth_sharp.AddString("高通滤波器");
	m_smooth_sharp.AddString("低通滤波器");

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
	else if (0 == str.Compare("Canny算子"))
	{
		CTMatrix< BYTE > Canny_edge_operator = CImageProcess::Canny_edge_operator(m_image_org.Get_gray_image());
		m_image_Obj.ImportFrom(Canny_edge_operator);
		m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
	}
}


void CMFCDemo2Dlg::OnCbnSelchangeComboImageSegment()
{
	int nIndex = m_comboImageSegment.GetCurSel();

	CString str;

	CPoint ptLeftTop;
	CRect rectOrcBmp;
	GetDlgItem(IDC_STATIC_OBJ_BMP)->GetClientRect(&rectOrcBmp);
	ptLeftTop.x = rectOrcBmp.left;
	ptLeftTop.y = rectOrcBmp.top;


	m_comboImageSegment.GetLBText(nIndex, str);

	if (0 == str.Compare("Robert算子"))
	{
		if (m_image_org.IsColorImage() || m_image_org.IsGrayImage())
		{
			if (!m_image_org.IsGrayImage())
			m_image_org.ColorToGray();
			CTMatrix< BYTE > Robert_edge_operator = CImageProcess::Robert_edge_operator(m_image_org.Get_gray_image());
			m_image_Obj.ImportFrom(Robert_edge_operator);
			m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
		}
	}
	else if (0 == str.Compare("Sobel算子"))
	{
		if (m_image_org.IsColorImage()||m_image_org.IsGrayImage())
		{
			if(!m_image_org.IsGrayImage())
			m_image_org.ColorToGray();
			CTMatrix< BYTE > Sobel_edge_horizontal = CImageProcess::Sobel_edge_horizontal(m_image_org.Get_gray_image());
			m_image_Obj.ImportFrom(Sobel_edge_horizontal);
			m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
		}
	}
	else if (0 == str.Compare("Canny算子"))//计算量大
	{
		if (m_image_org.IsColorImage() || m_image_org.IsGrayImage())
		{
			if (!m_image_org.IsGrayImage())
			m_image_org.ColorToGray();
			CTMatrix< BYTE > Canny_edge_operator = CImageProcess::Canny_edge_operator(m_image_org.Get_gray_image());
			m_image_Obj.ImportFrom(Canny_edge_operator);
			m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
		}
	}
	else if (0 == str.Compare("Prewitt算子"))
	{
		if (m_image_org.IsColorImage() || m_image_org.IsGrayImage())
		{
			if (!m_image_org.IsGrayImage())
				m_image_org.ColorToGray();
			CTMatrix< BYTE > Prewitt_edge_operator = CImageProcess::Prewitt_edge_operator(m_image_org.Get_gray_image());
			m_image_Obj.ImportFrom(Prewitt_edge_operator);
			m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
		}
	}
	else if (0 == str.Compare("区域生长"))
	{
		if (m_image_org.IsColorImage() || m_image_org.IsGrayImage())
		{
			if (!m_image_org.IsGrayImage())
				m_image_org.ColorToGray();
			CTMatrix< BlackWhite > Region_growing =CImageProcess::Region_growing(m_image_org.Get_gray_image(),245, 6);
			m_image_Obj.ImportFrom(Region_growing);
			m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
		}
	}
	else if (0 == str.Compare("分水岭算法"))
	{
		if (m_image_org.IsColorImage() || m_image_org.IsGrayImage())
		{
			if (!m_image_org.IsGrayImage())
				m_image_org.ColorToGray();
			CTMatrix< int > Watershed = CImageProcess::Watershed(m_image_org.Get_gray_image(), 52);
			int image_height = m_image_org.Get_gray_image().Get_height();
			int image_width = m_image_org.Get_gray_image().Get_width();

			CTMatrix< RGB_TRIPLE > display_image(image_height, image_width);

			for (int row = 0; row < image_height; row++)
				for (int column = 0; column < image_width; column++)
					if (Watershed[row][column] != 0)
					{
						display_image[row][column] = RGB_TRIPLE(255, 0, 0);
					}
					else
					{
						display_image[row][column].m_Blue = m_image_org.Get_gray_image()[row][column];
						display_image[row][column].m_Green = m_image_org.Get_gray_image()[row][column];
						display_image[row][column].m_Red = m_image_org.Get_gray_image()[row][column];
					}

			m_image_Obj.ImportFrom(display_image);
			m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
		}
	}
	else if (0 == str.Compare("K 均值聚类"))
	{
		if (m_image_org.IsColorImage())
		{
			long number_of_clusters = 4;

			CTArray< RGB_TRIPLE > template_of_display(number_of_clusters);
			template_of_display[0] = RGB_TRIPLE(255, 255, 0);
			template_of_display[1] = RGB_TRIPLE(0, 175, 175);
			template_of_display[2] = RGB_TRIPLE(100, 0, 100);
			template_of_display[3] = RGB_TRIPLE(0, 0, 0);


			CTMatrix< int > cluster_results = CImageProcess::K_means_clustering(m_image_org.Get_color_image(), number_of_clusters);

			int image_height = m_image_org.Get_color_image().Get_height();
			int image_width = m_image_org.Get_color_image().Get_width();

			CTMatrix< RGB_TRIPLE > display_image(image_height, image_width);

			for (int row = 0; row < image_height; row++)
				for (int column = 0; column < image_width; column++)
					display_image[row][column] = template_of_display[cluster_results[row][column]];

			m_image_Obj.ImportFrom(display_image);
			m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
		}
		}
}


void CMFCDemo2Dlg::OnBnClickedBtnClear()
{
	CRect rectObjWnd;
	GetDlgItem(IDC_STATIC_OBJ_BMP)->GetClientRect(rectObjWnd);
	GetDlgItem(IDC_STATIC_OBJ_BMP)->InvalidateRect(rectObjWnd);
}


void CMFCDemo2Dlg::OnCbnSelchangeComboFunc2()
{
	// TODO: 在此添加控件通知处理程序代码
	int nIndex = m_smooth_sharp.GetCurSel();

	CString str;

	CPoint ptLeftTop;
	CRect rectOrcBmp;
	GetDlgItem(IDC_STATIC_OBJ_BMP)->GetClientRect(&rectOrcBmp);
	ptLeftTop.x = rectOrcBmp.left;
	ptLeftTop.y = rectOrcBmp.top;


	m_smooth_sharp.GetLBText(nIndex, str);

	if (0 == str.Compare("中值滤波"))
	{
		CTMatrix< BYTE > filter_image = CImageProcess::Median_filter(m_image_org.Get_gray_image(), 3);
		m_image_Obj.ImportFrom(filter_image);
		m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
	}
	else if (0 == str.Compare("低通滤波器"))
	{
		long new_height, new_width;
		CTArray< complex > array_of_complex = CImageProcess::Image_to_complex(m_image_org.Get_gray_image(), new_height, new_width);

		complex* pointer_of_complex = new complex[array_of_complex.GetDimension()];

		array_of_complex.ExportTo(pointer_of_complex);
		CImageProcess::forward_fft2d(pointer_of_complex, new_height, new_width); // [ 傅里叶正变换 ]
		array_of_complex.ImportFrom(pointer_of_complex);

		array_of_complex = CImageProcess::Low_pass_filter(array_of_complex); // [ 低通滤波（巴特沃斯滤波器） ]
//		array_of_complex = CImageProcess::High_pass_filter(array_of_complex, new_height, new_width); // [ 高通滤波（巴特沃斯滤波器） ]
		array_of_complex.ExportTo(pointer_of_complex);
		CImageProcess::inverse_fft2d(pointer_of_complex, new_height, new_width); // [ 傅里叶反变换 ]
		array_of_complex.ImportFrom(pointer_of_complex);

		delete[] pointer_of_complex;

		CTMatrix< BYTE > filter_image = CImageProcess::Complex_to_image(array_of_complex, new_height, new_width);

		m_image_Obj.ImportFrom(filter_image);

		m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
	}
	else if (0 == str.Compare("高通滤波器"))
	{
		long new_height, new_width;
		CTArray< complex > array_of_complex = CImageProcess::Image_to_complex(m_image_org.Get_gray_image(), new_height, new_width);

		complex* pointer_of_complex = new complex[array_of_complex.GetDimension()];

		array_of_complex.ExportTo(pointer_of_complex);
		CImageProcess::forward_fft2d(pointer_of_complex, new_height, new_width); // [ 傅里叶正变换 ]
		array_of_complex.ImportFrom(pointer_of_complex);

//		array_of_complex = CImageProcess::Low_pass_filter(array_of_complex); // [ 低通滤波（巴特沃斯滤波器） ]
		array_of_complex = CImageProcess::High_pass_filter(array_of_complex, new_height, new_width); // [ 高通滤波（巴特沃斯滤波器） ]
		array_of_complex.ExportTo(pointer_of_complex);
		CImageProcess::inverse_fft2d(pointer_of_complex, new_height, new_width); // [ 傅里叶反变换 ]
		array_of_complex.ImportFrom(pointer_of_complex);

		delete[] pointer_of_complex;

		CTMatrix< BYTE > filter_image = CImageProcess::Complex_to_image(array_of_complex, new_height, new_width);

		m_image_Obj.ImportFrom(filter_image);

		m_image_Obj.ShowImage(GetDlgItem(IDC_STATIC_OBJ_BMP)->GetDC(), ptLeftTop, CSize(rectOrcBmp.Width(), rectOrcBmp.Height()));
	}
}
