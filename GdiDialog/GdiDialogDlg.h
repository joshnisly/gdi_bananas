#pragma once

#include "Game.h"


class CGdiDialogDlg : public CDialog
{
public:
	CGdiDialogDlg(CWnd* pParent = NULL);	// standard constructor

	enum { IDD = IDD_GDIDIALOG_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

   CGorillaGame m_vGame;

protected:
   void OnThrowClicked(bool bLeft, int iSpeedEditID, int iAngleEditID);

   BOOL PreTranslateMessage(MSG* pMsg);

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);

   afx_msg void OnLeftThrow();
   afx_msg void OnRightThrow();

   virtual void OnCancel();
   virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
