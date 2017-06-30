
class CBuilding
{
public:
   CBuilding(); // For arrays only
   CBuilding(CRect vRect, COLORREF crColor);

   CBuilding& operator = (const CBuilding& rOther);

   void Draw(HWND hWnd, HDC hDC) const;

   CRect GetRect() const { return m_vBuildingRect; }

private:
   CRect m_vBuildingRect;
   COLORREF m_crColor;
   COLORREF m_crDoorColor;

   CArray<CRect> m_aWindows;
};

class CBanana
{
public:
   CBanana(CPoint vPoint, double dInitialSpeed, double dInitialAngle, bool bGoLeft);

   void Move();

   CPoint GetCurrentPosition();

private:
   double m_dX;
   double m_dY;

   double m_dHorizontalSpeed;
   double m_dCurrentVerticalSpeed;
};

class CGorillaGame
{
public:
   CGorillaGame();
   ~CGorillaGame();

   void Reset();

   void SetHwnd(HWND hMainWnd);

   void Draw();

   void OnThrow(bool bLeftPlayer, int iSpeed, int iAngle);

   bool IsGameOver();

protected:
   // Setup functions
   void GenerateBuildings();
   void PlaceGorillas();

   // Misc functions
   void CheckBananaPosition(CPoint vBananaPos, CRect vWindowRect);

   enum EGameState
   {
      EGameState_Starting,
      EGameState_LeftPlayerEnteringValues,
      EGameState_LeftBananaFlying,
      EGameState_RightPlayerEnteringValues,
      EGameState_RightBananaFlying,
      EGameState_BananaExploding,
      EGameState_GameOver
   };

   enum EGameResult
   {
      EGameResult_InProgress,
      EGameResult_LeftWon,
      EGameResult_RightWon,
   };

   HWND m_hWnd;
   EGameState m_eGameState;
   EGameResult m_eGameResult;
   CBanana* m_pBanana;
   CRect m_vExplosionRect;

   CArray<CBuilding, CBuilding&> m_aBuildings;

   CRect m_vLeftGorillaRect;
   CRect m_vRightGorillaRect;

};