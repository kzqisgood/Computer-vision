
// MFCDemo2Dlg.h: 头文件
//

#pragma once

#include "Image.h"


// CMFCDemo2Dlg 对话框
class CMFCDemo2Dlg : public CDialogEx
{
// 构造
public:
	CMFCDemo2Dlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCDEMO2_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnOpenFile();

	HH::CImage m_image_org;
	HH::CImage m_image_Obj;
	HH::CImage m_grayImage;
	afx_msg void OnStnClickedStaticOrgBmp();
	afx_msg void OnStnClickedStaticObjBmp();
	CComboBox m_combFunc;
	CComboBox m_comboImageSegment;
	afx_msg void OnCbnSelchangeComboFunc();
	afx_msg void OnCbnSelchangeComboImageSegment();
	afx_msg void OnBnClickedBtnClear();
};
