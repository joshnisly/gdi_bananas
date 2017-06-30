// GdiDialogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GdiDialog.h"
#include "GdiDialogDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CGdiDialogDlg::CGdiDialogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGdiDialogDlg::IDD, pParent)
{
}

void CGdiDialogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGdiDialogDlg, CDialog)
	ON_WM_PAINT()
   ON_WM_TIMER()
   ON_BN_CLICKED(IDC_LEFT_THROW_BTN, OnLeftThrow)
   ON_BN_CLICKED(IDC_RIGHT_THROW_BTN, OnRightThrow)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CGdiDialogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

   CRect vRect;
   GetWindowRect(&vRect);
   vRect.bottom = vRect.top + 700;
   vRect.right = vRect.left + 900;
   MoveWindow(vRect);

   GetClientRect(&vRect);
   CRect vGameRect;
   GetDlgItem(IDC_GAME_CANVAS)->GetWindowRect(vGameRect);
   vGameRect.bottom = vRect.bottom;
   vGameRect.right = vRect.right;
   vGameRect.left = vRect.left;
   vGameRect.top = vRect.top + 30;
   GetDlgItem(IDC_GAME_CANVAS)->MoveWindow(vGameRect);


   SetTimer(0, 100, NULL);

   m_vGame.SetHwnd(GetDlgItem(IDC_GAME_CANVAS)->m_hWnd);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGdiDialogDlg::OnPaint()
{
	CDialog::OnPaint();
}

void CGdiDialogDlg::OnTimer(UINT nIDEvent)
{
   m_vGame.Draw();
}

void CGdiDialogDlg::OnLeftThrow()
{
   OnThrowClicked(true, IDC_LEFT_SPEED_EDIT, IDC_LEFT_ANGLE_EDIT);
}

void CGdiDialogDlg::OnRightThrow()
{
   OnThrowClicked(false, IDC_RIGHT_SPEED_EDIT, IDC_RIGHT_ANGLE_EDIT);
}

void CGdiDialogDlg::OnThrowClicked(bool bLeft, int iSpeedEditID, int iAngleEditID)
{
   CString sSpeed;
   GetDlgItem(iSpeedEditID)->GetWindowText(sSpeed);
   int iSpeed = atoi(sSpeed);
   if (iSpeed <= 0 || iSpeed > 50000)
   {
      AfxMessageBox("Please enter a valid speed.");
      return;
   }

   CString sAngle;
   GetDlgItem(iAngleEditID)->GetWindowText(sAngle);
   int iAngle = atoi(sAngle);
   if (iAngle <= 0 || iAngle > 90)
   {
      AfxMessageBox("Please enter a valid angle.");
      return;
   }
   m_vGame.OnThrow(bLeft, iSpeed, iAngle);
}



BOOL CGdiDialogDlg::PreTranslateMessage(MSG* pMsg)
{
   if (pMsg->message == WM_KEYDOWN)
   {
      if (pMsg->wParam == VK_RETURN)
      {
         if (GetFocus() == GetDlgItem(IDC_LEFT_SPEED_EDIT) ||
             GetFocus() == GetDlgItem(IDC_LEFT_ANGLE_EDIT) ||
             GetFocus() == GetDlgItem(IDC_LEFT_THROW_BTN))
         {
            OnLeftThrow();
         }
         else
            OnRightThrow();

         return TRUE; //translated this message
      }
      if (pMsg->wParam == 'P' && m_vGame.IsGameOver())
      {
         m_vGame.Reset();
         return TRUE;
      }
   }

   return CDialog::PreTranslateMessage(pMsg);
}

void CGdiDialogDlg::OnCancel()
{
   if (AfxMessageBox("Are you sure you want to quit?", MB_YESNO) == IDYES)
      EndDialog(IDCANCEL);
}

void CGdiDialogDlg::OnOK()
{
}