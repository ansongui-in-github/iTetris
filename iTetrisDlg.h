// iTetrisDlg.h : header file
//

#pragma once

#include "iTetrisBase.h"
#include <string>
using namespace std;

struct GameRegion
{
    CRect rectData; //区域中的一个小矩形
    bool bIsFilled; //该区域是否填充有小矩形
};

const int LINE = 20; //游戏区域行数
const int COLUMN = 15; //游戏区域列数
//const int WIDTH = 20; //游戏区域中小矩形的宽
//const CPoint g_ptUpLeft(10, 15); //游戏区域边框左上角点
//const CPoint g_ptBottomRight(310, 415); //游戏区域边框右下角点
//const CPoint g_ptCenterOfFirstLine(150, 15); //第一行中心点
const int NEXT_NUM = 1;


// CiTetrisDlg dialog
class CiTetrisDlg : public CDialogEx
{
// Construction
public:
	CiTetrisDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_ITETRIS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    virtual BOOL PreTranslateMessage(MSG* pMsg);

public:
    void DrawGameRegion();
    void UnDrawGameRegion();
    void DrawGameRegionFrame();
    void UnDrawGameRegionFrame();
    void DrawGrid();
    void DrawTetris();
    void UnDrawTetris();
    void DrawHelpInfo();
    void UnDrawHelpInfo();
    void DrawNext();

    void OnGameStart();
    void OnGamePause();
    void OnGameClose();

    void OnTetrisRotate(ROTATION rotation = ROTATION_1); //方块旋转
    void OnTetrisMove(DIRECTION direction); //方块移动，包括左移右移下移 
    void OnTetrisMoveDownSuddenly(); //使方块立即降落

    void AddTetrisToGameRegion();
    void HandleGameOver();
    char RemoveFullLine(); //消行
    void ComputeScore(int lines);

    //AI control function
    static UINT BeginAI(LPVOID pParam); //线程函数

private:
    void InitGameRegion();
    CPoint GetCoordinateOfRect(const CRect& rect); //获取一个矩形单位在游戏区域中的坐标
    bool CanTetrisMove(); //判断方块是否还可以移动，不能移动则加入到游戏区域中
    bool CanTetrisMoveDown(const TetrisElement& te, int& allowInterval);
    bool CanTetrisMoveLeft(const TetrisElement& te);
    bool CanTetrisMoveRight(const TetrisElement& te);

    CRect GetTheTopRectOfTheColumn(const int col); //获得某一列堆积的最高点的矩形
    int GetTheMostBottomFullLine(); //返回最靠近底部的满行
    bool IsLineFull(const int line); //判断一行是否是满行
    void RemoveTheChosenLine(const int line1, const int line2); //消除指定的行，从line1到line2之间都是满行，包括line1、line2，且line1 <= line2

    //Functions for AI below
    void MoveToProperColumn(); //把方块移动到目标位置所在列
    int FindTheProperColumn(SHAPE s, ROTATION& r);
    int FindTheProperColumnForSquareShape(ROTATION& r);
    int FindTheProperColumnForStripShape(ROTATION& r);
    int FindTheProperColumnForTShape(ROTATION& r);
    int FindTheProperColumnForStepShape(ROTATION& r);
    int FindTheProperColumnForRStepShape(ROTATION& r);
    int FindTheProperColumnForLShape(ROTATION& r);
    int FindTheProperColumnForRLShape(ROTATION& r);

    bool IsRectOccupied(const CRect& rect); //游戏区域中的一格小矩形是否已被填充
    bool IsTheColumnNotFullAboveTheLine(int col, int line); //判断某一列从某一行（包括该行）往上是否全为空的


// Implementation
protected:
	HICON m_hIcon;

    //Added by guiwanli below 2014/12/17
    CRect m_rectGameRegionFrame; //游戏区域边框矩形
    GameRegion m_stGameRegion[COLUMN][LINE]; //游戏区域总共20行15列，向右为x方向，向下为y方向
    bool m_bDisplayGrid; //是否显示网格
    bool m_bPause;
    bool m_bStart;

    TetrisBase m_stTetrisBase; //处在游戏区域中的俄罗斯方块
    bool m_bKeydownOfA; //是否有按键A
    bool m_bKeydownOfD; //是否有按键D

    unsigned int m_uiScore; //得分
    unsigned int m_uiSpeed; //掉落速度
    unsigned int m_uiLevel;

    TetrisElement m_stTetrisElements[NEXT_NUM];

    bool m_bGameOver;
    bool m_bAIMode;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	
//public:
	//afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//afx_msg void OnGameStart();
	//afx_msg void OnGamePause();
	//afx_msg void OnGameClose();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
    DECLARE_MESSAGE_MAP()
};

#ifdef _DEBUG
void PrintOnConsole(char* pszFormat, ...);
#endif

#ifdef _DEBUG
#define PrintOnConsole_Macro(str)\
{\
    TCHAR szBuf[255] = {0};\
    int count = wsprintf(szBuf, _T("[Tetris debug]: [%S] [%S] [%d]\n"), (str).c_str(), __FUNCTION__, __LINE__);\
    WriteConsole(g_hConsole, (const VOID*)szBuf, count, NULL, NULL);\
}
#endif





