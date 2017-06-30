#include "StdAfx.h"

#include "Game.h"

void DrawRect(HDC hDC, CRect vRect)
{
   Rectangle(hDC, vRect.left, vRect.top, vRect.right, vRect.bottom);
}

void DrawCircleAtPoint(HDC hDC, CPoint vPoint)
{
   Ellipse(hDC, vPoint.x - 2, vPoint.y - 2, vPoint.x + 2, vPoint.y + 2);
}

int RandomInRange(int iLow, int iHigh)
{
   ASSERT(iLow < iHigh);

   int iRange = iHigh - iLow;
   int iRand = rand() % iRange;
   return iRand + iLow;
}

COLORREF RandomColor()
{
   return RGB(rand() % 255, rand() % 255, rand() % 255);
}

CBuilding::CBuilding()
{
   m_vBuildingRect = CRect(0, 0, 0, 0);
   m_crColor = 0;
   m_crDoorColor = 0;
}

CBuilding& CBuilding::operator = (const CBuilding& rOther)
{
   m_vBuildingRect = rOther.m_vBuildingRect;
   m_crColor = rOther.m_crColor;
   m_crDoorColor = rOther.m_crDoorColor;
   m_aWindows.Copy(rOther.m_aWindows);

   return *this;
}

CBuilding::CBuilding(CRect vRect, COLORREF crColor)
{
   m_vBuildingRect = vRect;
   m_crColor = crColor;
   m_crDoorColor = RandomColor();

   // Calculate windows
   int iWindowHeight_c = 15;
   int iWindowWidth_c = 10;

   int iRemainingHeight = m_vBuildingRect.Height() - 50;
   while (iRemainingHeight > 10 + iWindowHeight_c)
   {
      for (int i = 0; i < 3; i++)
      {
         int iWindowCenter = m_vBuildingRect.left + m_vBuildingRect.Width() * (i + 1) / 4;
         int iWindowBottom = m_vBuildingRect.top + iRemainingHeight;

         CRect vWindowRect(iWindowCenter - (iWindowWidth_c / 2),
                           iWindowBottom - iWindowHeight_c,
                           iWindowCenter + (iWindowWidth_c / 2),
                           iWindowBottom);
         m_aWindows.Add(vWindowRect);
      }

      iRemainingHeight -= (iWindowHeight_c * 2);
   }
}

void CBuilding::Draw(HWND hWnd, HDC hDC) const
{
   HBRUSH hBuildingBackground = (HBRUSH)SelectObject(hDC, CreateSolidBrush(m_crColor));
   HBRUSH hBuildingBorder = (HBRUSH)SelectObject(hDC, ::CreatePen(PS_SOLID, 1, RGB(80, 80, 80)));

   // Draw building background
   DrawRect(hDC, m_vBuildingRect);

   // Draw door
   const int iDoorWidth_c = 20;
   const int iDoorHeight_c = 30;
   int iDoorLeft = m_vBuildingRect.left + (m_vBuildingRect.Width() / 2) - (iDoorWidth_c / 2);
   CRect vDoorRect(iDoorLeft,
                   m_vBuildingRect.bottom - iDoorHeight_c,
                   iDoorLeft + iDoorWidth_c,
                   m_vBuildingRect.bottom);
   HBRUSH hDoorBackground = (HBRUSH)SelectObject(hDC, CreateSolidBrush(m_crDoorColor));
   DrawRect(hDC, vDoorRect);

   // Draw windows
   for (int i = 0; i < m_aWindows.GetSize(); i++)
      DrawRect(hDC, m_aWindows[i]);
   DeleteObject(SelectObject(hDC, hDoorBackground));

   // TODO: we could also draw a door handle on the door.

   DeleteObject(SelectObject(hDC, hBuildingBorder));
   DeleteObject(SelectObject(hDC, hBuildingBackground));
}

const double dFactor_c = 7.0;

CBanana::CBanana(CPoint vPoint, double dInitialSpeed, double dInitialAngle, bool bGoLeft)
{
   m_dX = vPoint.x;
   m_dY = vPoint.y;

   ASSERT(dInitialAngle <= 90 && dInitialAngle >= 0);

   double dHorizSpeedRatio = (90 - dInitialAngle) / 90;
   double dVertSpeedRatio = dInitialAngle / 90;

   m_dHorizontalSpeed = dInitialSpeed * dHorizSpeedRatio / dFactor_c;
   m_dCurrentVerticalSpeed = dInitialSpeed * dVertSpeedRatio / dFactor_c;

   if (!bGoLeft)
      m_dHorizontalSpeed *= -1;
}

void CBanana::Move()
{
   m_dX += m_dHorizontalSpeed;
   m_dY -= m_dCurrentVerticalSpeed;

   m_dCurrentVerticalSpeed -= 6.0 / dFactor_c;
}

CPoint CBanana::GetCurrentPosition()
{
   return CPoint((int)m_dX, (int)m_dY);
}


CGorillaGame::CGorillaGame()
{
   m_hWnd = NULL;
   m_pBanana = NULL;

   Reset();

   srand((unsigned int)time(NULL));
}

CGorillaGame::~CGorillaGame()
{
   if (m_pBanana)
      delete m_pBanana;
}

void CGorillaGame::Reset()
{
   if (m_pBanana)
   {
      delete m_pBanana;
      m_pBanana = NULL;
   }

   m_eGameState = EGameState_Starting;
   m_eGameResult = EGameResult_InProgress;
   m_vExplosionRect = CRect(0, 0, 0, 0);
   m_aBuildings.RemoveAll();

   if (m_hWnd)
   {
      GenerateBuildings();

      PlaceGorillas();
   }
}

void CGorillaGame::SetHwnd(HWND hMainWnd)
{
   m_hWnd = hMainWnd;

   Reset();
}

COLORREF crSkyColor = RGB(183, 211, 247);

void CGorillaGame::Draw()
{
   ASSERT(m_hWnd != NULL);
   ASSERT(m_eGameState >= EGameState_Starting);

   HDC hDC = GetDC(m_hWnd);
   if (!hDC)
      return;

   CRect vGameRect;
   GetClientRect(m_hWnd, &vGameRect);

   // Paint sky
   if (m_eGameState == EGameState_Starting ||
       m_eGameState == EGameState_LeftBananaFlying ||
       m_eGameState == EGameState_RightBananaFlying)
   {
      HBRUSH hSkyFill = (HBRUSH)SelectObject(hDC, CreateSolidBrush(crSkyColor));
      HBRUSH hSkyBorder = (HBRUSH)SelectObject(hDC, ::CreatePen(PS_SOLID, 1, crSkyColor));

      for (int i = 0; i < m_aBuildings.GetSize(); i++)
      {
         CRect vBuilding = m_aBuildings[i].GetRect();
         CRect vSkyAboveBuilding(vBuilding.left, vGameRect.top, vBuilding.right, vBuilding.top);
         DrawRect(hDC, vSkyAboveBuilding);
      }
      DeleteObject(SelectObject(hDC, hSkyFill));
      DeleteObject(SelectObject(hDC, hSkyBorder));
   }

   // Draw buildings
   if (m_eGameState == EGameState_Starting)
   {
      for (int i = 0; i < m_aBuildings.GetSize(); i++)
         m_aBuildings[i].Draw(m_hWnd, hDC);
   }

   // Draw gorillas
   HBRUSH hGorilla = (HBRUSH)SelectObject(hDC, CreateSolidBrush(RGB(128, 107, 89)));
   DrawRect(hDC, m_vLeftGorillaRect);
   DrawRect(hDC, m_vRightGorillaRect);
   DeleteObject(SelectObject(hDC, hGorilla));

   // Draw text at the top
   if (m_eGameState == EGameState_LeftPlayerEnteringValues || m_eGameState == EGameState_RightPlayerEnteringValues)
   {
      CString sPlayer = m_eGameState == EGameState_LeftPlayerEnteringValues ? "Left" : "Right";
      CString sText = sPlayer + " player, enter speed/angle and fling your banana!!";

      CFont vFont;
      vFont.CreatePointFont(120, "Arial");
      HFONT hFont = (HFONT)SelectObject(hDC, vFont);
      CRect vTextRect(50, 20, vGameRect.Width() - 200, 50);
      SetBkColor(hDC, crSkyColor);
      SetBkMode(hDC, OPAQUE);
      ::DrawText(hDC, sText, sText.GetLength(), vTextRect, 0);
   }
   if (m_eGameState == EGameState_GameOver)
   {
      CString sPlayer = m_eGameResult == EGameResult_LeftWon ? "Left" : "Right";
      CString sText = sPlayer + " player won!! Congratulations!!1\nPress 'p' to play again.";

      CFont vFont;
      vFont.CreateFont(30, 0, 0, 0, FW_NORMAL, false, false, false, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, VARIABLE_PITCH, "Arial");
      HFONT hFont = (HFONT)SelectObject(hDC, vFont);
      CRect vTextRect(200, 20, vGameRect.Width() - 200, 120);
      SetBkColor(hDC, crSkyColor);
      SetBkMode(hDC, OPAQUE);
      ::DrawText(hDC, sText, sText.GetLength(), vTextRect, 0);
   }

   if (m_pBanana)
   {
      m_pBanana->Move();

      CPoint vBananaPos = m_pBanana->GetCurrentPosition();

      CheckBananaPosition(vBananaPos, vGameRect);

      if ((m_eGameState == EGameState_LeftBananaFlying || m_eGameState == EGameState_RightBananaFlying) &&
          vGameRect.PtInRect(vBananaPos))
      {
         DrawCircleAtPoint(hDC, vBananaPos);
      }
   }

   if (m_vExplosionRect.Width() > 0)
   {
      HBRUSH hExplosionFill = (HBRUSH)SelectObject(hDC, CreateSolidBrush(RGB(255, 250, 22)));
      HBRUSH hExplosionBorder = (HBRUSH)SelectObject(hDC, ::CreatePen(PS_SOLID, 1, RGB(254, 69, 22)));

      Ellipse(hDC, m_vExplosionRect.left, m_vExplosionRect.top, m_vExplosionRect.right, m_vExplosionRect.bottom);
      m_vExplosionRect.left -= 10;
      m_vExplosionRect.right += 10;
      m_vExplosionRect.top -= 4;
      m_vExplosionRect.bottom += 4;

      if (m_vExplosionRect.Width() > 100)
      {
         m_eGameState = EGameState_GameOver;
         m_vExplosionRect = CRect(0, 0, 0, 0);
      }
      DeleteObject(SelectObject(hDC, hExplosionFill));
      DeleteObject(SelectObject(hDC, hExplosionBorder));
   }

   ReleaseDC(m_hWnd, hDC);

   m_eGameState = max(m_eGameState, EGameState_LeftPlayerEnteringValues);
}

void CGorillaGame::OnThrow(bool bLeftPlayer, int iSpeed, int iAngle)
{
   // Don't allow the player to throw until we're ready.
   if (bLeftPlayer && m_eGameState != EGameState_LeftPlayerEnteringValues)
      return;
   if (!bLeftPlayer && m_eGameState != EGameState_RightPlayerEnteringValues)
      return;

   CRect vGorillaRect = bLeftPlayer ? m_vLeftGorillaRect : m_vRightGorillaRect;

   ASSERT(!m_pBanana); // Shouldn't already be drawing a banana!

   CPoint vBananaStart(vGorillaRect.left + vGorillaRect.Width() / 2, vGorillaRect.top);
   m_pBanana = new CBanana(vBananaStart, iSpeed, iAngle, bLeftPlayer);
   
   if (m_eGameState == EGameState_LeftPlayerEnteringValues)
      m_eGameState = EGameState_LeftBananaFlying;

   if (m_eGameState == EGameState_RightPlayerEnteringValues)
      m_eGameState = EGameState_RightBananaFlying;
}

bool CGorillaGame::IsGameOver()
{
   return m_eGameState == EGameState_GameOver;
}

void CGorillaGame::GenerateBuildings()
{
   CRect vRect;
   GetClientRect(m_hWnd, &vRect);

   int iNextBuildingLeft = 0;
   while (iNextBuildingLeft < vRect.Width())
   {
      int iBuildingWidth = RandomInRange(80, 120);
      int iBuildingHeight = RandomInRange(vRect.Height() / 6, vRect.Height() / 2);

      CRect vBuildingRect;
      vBuildingRect.bottom = vRect.Height();
      vBuildingRect.top = vRect.Height() - iBuildingHeight;
      vBuildingRect.left = iNextBuildingLeft;
      vBuildingRect.right = iNextBuildingLeft + iBuildingWidth;

      iNextBuildingLeft += iBuildingWidth;

      CBuilding vBuilding(vBuildingRect, RandomColor());
      m_aBuildings.Add(vBuilding);
   }
}

CRect GorillaRectFromBuildingRect(CRect vBuildingRect)
{
   int iGorillaCenter = vBuildingRect.left + vBuildingRect.Width() / 2;

   const int iGorillaWidth_c = 30;
   const int iGorillaHeight_c = 40;
   return CRect(iGorillaCenter - iGorillaWidth_c / 2, vBuildingRect.top - iGorillaHeight_c,
                iGorillaCenter + iGorillaWidth_c / 2, vBuildingRect.top);
}

void CGorillaGame::PlaceGorillas()
{
   while (true)
   {
      int iLeftGorillaBuilding = rand() % m_aBuildings.GetSize() - 1;
      int iRightGorillaBuilding = rand() % m_aBuildings.GetSize() - 1;

      if (iLeftGorillaBuilding >= 0 && iLeftGorillaBuilding < iRightGorillaBuilding - 1)
      {
         m_vLeftGorillaRect = GorillaRectFromBuildingRect(m_aBuildings[iLeftGorillaBuilding].GetRect());
         m_vRightGorillaRect = GorillaRectFromBuildingRect(m_aBuildings[iRightGorillaBuilding].GetRect());

         break;
      }

   }
}


void CGorillaGame::CheckBananaPosition(CPoint vBananaPos, CRect vWindowRect)
{
   ASSERT(m_eGameState == EGameState_LeftBananaFlying || m_eGameState == EGameState_RightBananaFlying);

   bool bMissed = false;
   bool bHit = false;

   // Check to see if the banana is off screen, never to come back.
   if (vBananaPos.x < 0 || vBananaPos.x > vWindowRect.Width() || vBananaPos.y < -10000)
   {
      bMissed = true;
   }

   for (int i = 0; i < m_aBuildings.GetSize(); i++)
   {
      if (m_aBuildings[i].GetRect().PtInRect(vBananaPos))
         bMissed = true;
   }

   if (m_vLeftGorillaRect.PtInRect(vBananaPos) || m_vRightGorillaRect.PtInRect(vBananaPos))
   {
      bHit = true;
      ASSERT(m_eGameResult == EGameResult_InProgress);
      if (m_vRightGorillaRect.PtInRect(vBananaPos))
         m_eGameResult = EGameResult_LeftWon;
      else
         m_eGameResult = EGameResult_RightWon;

      m_eGameState = EGameState_BananaExploding;
      m_vExplosionRect = CRect(vBananaPos.x - 5, vBananaPos.y - 2, vBananaPos.x + 5, vBananaPos.y + 2);
   }

   if (bMissed)
   {
      if (m_eGameState == EGameState_LeftBananaFlying)
         m_eGameState = EGameState_RightPlayerEnteringValues;

      if (m_eGameState == EGameState_RightBananaFlying)
         m_eGameState = EGameState_LeftPlayerEnteringValues;
   }

   if (bMissed || bHit)
   {
      delete m_pBanana;
      m_pBanana = NULL;

      Draw();
   }
}

