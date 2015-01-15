// iTetrisDlg.cpp : implementation file
//

#include "stdafx.h"
#include "iTetris.h"
#include "iTetrisDlg.h"
#include "afxdialogex.h"
#include <sstream>
#include <cstdarg>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef _DEBUG
string g_strLog;
HANDLE g_hConsole;
#endif

//方便调试时统一换产生的形状
SHAPE g_shape = SHAPE_INVALID;


#ifdef _DEBUG
void PrintOnConsole(char* pszFormat, ...)
{
    if (NULL == pszFormat)
    {
        return;
    }

    char buf[1024] = {0};
    va_list args;

    va_start(args, pszFormat);
    vsprintf_s(buf, pszFormat, args); //怪！3个参数居然也行！
    va_end(args);

    TCHAR szBuf[1024] = {0};
    int count = wsprintf(szBuf, _T("%S"), buf);
    WriteConsole(g_hConsole, (const VOID*)szBuf, count, NULL, NULL);
}
#endif

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:

};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CiTetrisDlg dialog

CiTetrisDlg::CiTetrisDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CiTetrisDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CiTetrisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CiTetrisDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//ON_WM_KEYDOWN()
	//ON_COMMAND(IDB_START, &CiTetrisDlg::OnGameStart)
	//ON_COMMAND(IDB_PAUSE, &CiTetrisDlg::OnGamePause)
	//ON_COMMAND(IDB_CLOSE, &CiTetrisDlg::OnGameClose)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CiTetrisDlg message handlers

BOOL CiTetrisDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

    //Added by guiwanli below
#ifdef _DEBUG
    AllocConsole();
    SetConsoleTitle(_T("iTetris debug"));
    g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

	m_rectGameRegionFrame.SetRect(g_ptTopLeft, g_ptBottomRight);
    InitGameRegion();

    m_stTetrisBase.Init();
    m_stTetrisBase.GenerateNext(g_shape);
    m_stTetrisElements[0] = m_stTetrisBase.GetTetrisElement(); //保存下一个形状
    m_stTetrisBase.GenerateNext(g_shape);

    m_bDisplayGrid = false;
	m_bPause = false;
    m_bStart = false;

    m_bKeydownOfA = false;
    m_bKeydownOfD = false;

    m_uiScore = 0;
    m_uiSpeed = 350;
    m_uiLevel = 1;

    m_bGameOver = false;
    m_bAIMode = false;

    //Added by guiwanli above

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CiTetrisDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CiTetrisDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}

	//Added by guiwanli below 2014/12/19
    DrawGameRegionFrame();
    DrawTetris();
    DrawGameRegion();
    DrawGrid();
    DrawHelpInfo();
    DrawNext();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CiTetrisDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//void CiTetrisDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: Add your message handler code here and/or call default
//
//  //该函数废弃不用，改为用 PreTranslateMessage()
//	switch(nChar)
//	{
//	case VK_LEFT:
//	case 'A':
//		{
////#ifdef _DEBUG
////            g_strLog = "A is pushed!";
////            PrintOnConsole_Macro(g_strLog);
////#endif
//		}break;
//	case VK_UP:
//	case 'W': break;
//	case VK_RIGHT:
//	case 'D': break;
//	case VK_DOWN:
//	case 'S': break;
//    case 'V':
//    case VK_F3:
//        {
//
//        } break;
//	default: break;
//	}
//
//	CDialogEx::OnKeyDown(nChar, nRepCnt, nFlags);
//}

void CiTetrisDlg::OnGameStart()
{
	// TODO: Add your command handler code here
    if (m_bAIMode) //AI模式屏蔽其它开始按键
    {
        return;
    }

    if (m_bStart) //已经处于开始状态，则直接返回
    {
        return;
    }

	SetTimer(0, m_uiSpeed, NULL);
    m_bStart = true;
}

void CiTetrisDlg::OnGamePause()
{
	// TODO: Add your command handler code here
    if (m_bAIMode) //AI模式屏蔽其它暂停按键
    {
        return;
    }

    if (!m_bStart) //还没开始，则按暂停算无效
    {
        return;
    }

	if (!m_bPause)
	{
		KillTimer(0);
		m_bPause = true;
	}
	else
	{
		SetTimer(0, m_uiSpeed, NULL);
		m_bPause = false;
	}
}

void CiTetrisDlg::OnGameClose()
{
	// TODO: Add your command handler code here
	AfxGetMainWnd()->SendMessage(WM_CLOSE);

#ifdef _DEBUG
    CloseHandle(g_hConsole);
    FreeConsole();
#endif  
}

void CiTetrisDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
    UnDrawGameRegionFrame();
    UnDrawTetris();
    UnDrawHelpInfo();

    TetrisElement te;
    te = m_stTetrisBase.GetTetrisElement();

#ifdef _DEBUG
    //PrintOnConsole("before move, te.rects[0].top = [%d]\n", te.rects[0].top);
#endif
    
    int iAllowInterval = WIDTH; //由于类型不一致，不能直接把 WIDTH 放参数列表
    if (CanTetrisMoveDown(te, iAllowInterval)) //可以下移才允许移动
    {
        m_stTetrisBase.Move(); //默认由定时向下移
    }
    
    AddTetrisToGameRegion();

#ifdef _DEBUG
    //te = m_stTetrisBase.GetTetrisElement();
    //PrintOnConsole("after move, te.rects[0].top = [%d] ret=[%d]\n", te.rects[0].top, ret);
#endif

	CDialogEx::OnTimer(nIDEvent);
}

void CiTetrisDlg::DrawGameRegion()
{
    CDC* pDC = GetDC();

    if (NULL == pDC)
    {
        return;
    }

    //CBrush brush(RGB(0,255,0));

    CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(GRAY_BRUSH));
    CBrush* pOldBrush = pDC->SelectObject(pBrush);
    for (int i = 0; i < COLUMN; ++i)
    {
        for (int j = 0; j < LINE; ++j)
        {
            if (m_stGameRegion[i][j].bIsFilled)
            {
                pDC->Rectangle(m_stGameRegion[i][j].rectData);
                //pDC->FillRect(m_stGameRegion[i][j].rectData, &brush);
            }
        }
    }
    pDC->SelectObject(pOldBrush);

    ReleaseDC(pDC);
}

void CiTetrisDlg::UnDrawGameRegion()
{
    for (int i = 0; i < COLUMN; ++i)
    {
        for (int j = 0; j < LINE; ++j)
        {
            if (m_stGameRegion[i][j].bIsFilled)
            {
                InvalidateRect(m_stGameRegion[i][j].rectData);
                m_stGameRegion[i][j].bIsFilled = false;
            }
        }
    }
}

void CiTetrisDlg::DrawGameRegionFrame()
{
    CDC* pDC = GetDC();

    if (NULL == pDC)
    {
        return;
    }

    pDC->Rectangle(m_rectGameRegionFrame);

    ReleaseDC(pDC);
}

void CiTetrisDlg::UnDrawGameRegionFrame()
{
    InvalidateRect(m_rectGameRegionFrame);
}

void CiTetrisDlg::DrawGrid()
{
    if (!m_bDisplayGrid)
    {
        return;
    }

#ifdef _DEBUG
    g_strLog = "Grid is displaying...";
    //PrintOnConsole_Macro(g_strLog);
#endif

    CPoint ptTmp;
    CPoint ptStart;
    CDC* pDC = GetDC();

    if (NULL == pDC)
    {
        return;
    }

    //先画横线
    ptStart = g_ptTopLeft;
    ptStart.y += WIDTH;

    ptTmp = ptStart;
    ptTmp.x += WIDTH * COLUMN;

    for (CPoint i = ptStart; i.y < g_ptBottomRight.y; i.y += WIDTH)
    {
        pDC->MoveTo(i);
        pDC->LineTo(ptTmp);

        ptTmp.y += WIDTH;
    }

    //再画纵线
    ptStart = g_ptTopLeft;
    ptStart.x += WIDTH;

    ptTmp = ptStart;
    ptTmp.y += WIDTH * LINE;

    for (CPoint i = ptStart; i.x < g_ptBottomRight.x; i.x += WIDTH)
    {
        pDC->MoveTo(i);
        pDC->LineTo(ptTmp);

        ptTmp.x += WIDTH;
    }

    ReleaseDC(pDC);
}

void CiTetrisDlg::DrawTetris()
{
    CDC* pDC = GetDC();

    if (NULL == pDC)
    {
        return;
    }

    CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(DKGRAY_BRUSH));
    CBrush* pOldBrush = pDC->SelectObject(pBrush);

    TetrisElement te;

    te = m_stTetrisBase.GetTetrisElement();

    //pDC->Rectangle(te.rects[0]);
    //pDC->Rectangle(te.rects[1]);
    //pDC->Rectangle(te.rects[2]);
    //pDC->Rectangle(te.rects[3]);
    for (int i = 0; i < RECT_COUNT; ++i)
    {
        pDC->Rectangle(te.rects[i]);
    }

    pDC->SelectObject(pOldBrush);

    ReleaseDC(pDC);
}

void CiTetrisDlg::UnDrawTetris()
{
    TetrisElement te;

    te = m_stTetrisBase.GetTetrisElement();
    for (int i = 0; i < RECT_COUNT; ++i)
    {
        InvalidateRect(te.rects[i]);
    }
}

void CiTetrisDlg::DrawHelpInfo()
{
    CDC* pDC = GetDC();

    if (NULL == pDC)
    {
        return;
    }

    CString info;

    info = "开始: F3或Enter";
    pDC->TextOut(310, 15, info);

    info = "暂停: F4或空格";
    pDC->TextOut(310, 35, info);

    info = "显示网格: F5";
    pDC->TextOut(310, 55, info);

    info = "立即降落: E";
    pDC->TextOut(310, 75, info);

    info = "反变形: Q";
    pDC->TextOut(310, 95, info);

    info = "正变形: W或上箭头";
    pDC->TextOut(310, 115, info);

    info = "向左移: A或左箭头";
    pDC->TextOut(310, 135, info);

    info = "向下移: S或下箭头";
    pDC->TextOut(310, 155, info);

    info = "向右移: D或右箭头";
    pDC->TextOut(310, 175, info);

    info.Empty();
    info.Format(_T("得分：%d"), m_uiScore);
    pDC->TextOut(310, 200, info);

    info.Empty();
    info.Format(_T("关数：%d"), m_uiLevel);
    pDC->TextOut(310, 220, info);

    info = "下一个：";
    pDC->TextOut(310, 240, info);

    ReleaseDC(pDC);
}

void CiTetrisDlg::UnDrawHelpInfo()
{
    CRect rectTmp;

    GetClientRect(&rectTmp);
    InvalidateRect(rectTmp); //触发得分区域的重绘
}

BOOL CiTetrisDlg::PreTranslateMessage(MSG* pMsg)
{
    if(WM_KEYDOWN == pMsg->message)
    {
        if (VK_ESCAPE == pMsg->wParam)
        {
            if (!m_bStart) //游戏还未开始才允许进入AI模式，否则不允许
            {
                AfxBeginThread(&CiTetrisDlg::BeginAI, (LPVOID)this);
            }

            if (m_bAIMode) //如果已经是AI模式，那么再次按了ESC就游戏结束
            {
                m_bGameOver = true;
                HandleGameOver();
            }

            if ((!m_bAIMode) && m_bStart) //普通模式，按了ESC也可以结束游戏
            {
                m_bGameOver = true;
                HandleGameOver();
            }
            
            return TRUE; //屏蔽系统响应 ESC 按键
        }
        else if (VK_F1 == pMsg->wParam)
        {
            return TRUE; //屏蔽系统响应 F1 按键（屏蔽不了，很奇怪）
        }
        else if ((VK_F3 == pMsg->wParam) || (VK_RETURN == pMsg->wParam))
        {
#ifdef _DEBUG
            g_strLog = "Start button is pushed!";
            PrintOnConsole_Macro(g_strLog);
#endif
            OnGameStart();
            return TRUE; //屏蔽系统响应 Enter 按键
        }
        else if ((VK_F4 == pMsg->wParam) || (VK_SPACE == pMsg->wParam))
        {
#ifdef _DEBUG
            g_strLog = "Pause button is pushed!";
            PrintOnConsole_Macro(g_strLog);
#endif
            OnGamePause();
        }
        else if(VK_F5 == pMsg->wParam)
        {
#ifdef _DEBUG
            g_strLog = "Grid button is pushed!";
            PrintOnConsole_Macro(g_strLog);
#endif
            if (!m_bDisplayGrid)
            {
                m_bDisplayGrid = true;
            }
            else
            {
                m_bDisplayGrid = false;
            }

            InvalidateRect(m_rectGameRegionFrame); //触发网格的重绘
        }
        else if ('E' == pMsg->wParam) //立即降落
        {
#ifdef _DEBUG
            g_strLog = "E is pushed!";
            //PrintOnConsole_Macro(g_strLog);
#endif
            if (!m_bAIMode) //不是AI模式，才允许响应该按键
            {
                OnTetrisMoveDownSuddenly();
            }
        }
        else if ('Q' == pMsg->wParam) //控制方块旋转，逆时针
        {    
#ifdef _DEBUG
            g_strLog = "Q is pushed!";
            //PrintOnConsole_Macro(g_strLog);
#endif
            if (!m_bAIMode) //不是AI模式，才允许响应该按键
            {
                OnTetrisRotate(ROTATION_2); //ROTATION_2 代表逆时针旋转
            }
        }
        else if (('W' == pMsg->wParam) || (VK_UP == pMsg->wParam)) //控制方块旋转，顺时针
        {     
#ifdef _DEBUG
            g_strLog = "W is pushed!";
            //PrintOnConsole_Macro(g_strLog);
#endif
            if (!m_bAIMode) //不是AI模式，才允许响应该按键
            {
                OnTetrisRotate(); //参数默认取值 ROTATION_1 代表顺时针旋转
            }
        }
        else if (('A' == pMsg->wParam) || (VK_LEFT == pMsg->wParam)) //控制方块左移
        {      
#ifdef _DEBUG
            g_strLog = "A is pushed!";
            //PrintOnConsole_Macro(g_strLog);
#endif
            if (!m_bAIMode) //不是AI模式，才允许响应该按键
            {
                OnTetrisMove(DIRECTION_LEFT);
            }
        }
        else if (('S' == pMsg->wParam) || (VK_DOWN == pMsg->wParam)) //控制方块下移
        {       
#ifdef _DEBUG
            g_strLog = "S is pushed!";
            //PrintOnConsole_Macro(g_strLog);
#endif
            if (!m_bAIMode) //不是AI模式，才允许响应该按键
            {
                OnTetrisMove(DIRECTION_DOWN);
            }
        }
        else if (('D' == pMsg->wParam) || (VK_RIGHT == pMsg->wParam)) //控制方块右移
        {     
#ifdef _DEBUG
            g_strLog = "D is pushed!";
            //PrintOnConsole_Macro(g_strLog);
#endif
            if (!m_bAIMode) //不是AI模式，才允许响应该按键
            {
                OnTetrisMove(DIRECTION_RIGHT);
            }
        }
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void CiTetrisDlg::InitGameRegion()
{
    CRect rectTmp;
    CPoint ptUpLeft = g_ptTopLeft;
    CPoint ptBottomRight = g_ptTopLeft;

    for (int i = 0; i < COLUMN; ++i)
    {
        ptBottomRight.x += WIDTH;
        for (int j = 0; j < LINE; ++j)
        {
            ptBottomRight.y += WIDTH;
            rectTmp.SetRect(ptUpLeft, ptBottomRight);

            m_stGameRegion[i][j].rectData = rectTmp;
            m_stGameRegion[i][j].bIsFilled = false;

            ptUpLeft.y += WIDTH;
        }

        ptUpLeft.x = ptBottomRight.x;
        ptUpLeft.y = g_ptTopLeft.y;
        ptBottomRight.y = g_ptTopLeft.y;
    }
}

void CiTetrisDlg::AddTetrisToGameRegion()
{
    if (!CanTetrisMove())
    {
        TetrisElement te;
        CPoint ptTmp;

        //1. 加入新的方块到区域中
        te = m_stTetrisBase.GetTetrisElement();
        for (int i = 0; i < RECT_COUNT; ++i)
        {
            ptTmp = GetCoordinateOfRect(te.rects[i]);
            m_stGameRegion[ptTmp.x][ptTmp.y].bIsFilled = true;
        }

        //2. 判断消行和得分
        char cFullLineFlag = RemoveFullLine();
        int lines = 0;
        while(cFullLineFlag) //计算一个10序列中1的个数
        {
            ++lines;
            cFullLineFlag &= (cFullLineFlag - 1);
        }
        ComputeScore(lines);

        //3. 产生下一个
        m_stTetrisBase.GenerateNext(g_shape); //产生下一个方块
        te = m_stTetrisBase.GetTetrisElement();
        m_stTetrisBase.SetTetrisElement(m_stTetrisElements[0]); //把先前保存的作为下一个
        m_stTetrisElements[0] = te; //把刚刚生成的作为下一个保存

#ifdef _DEBUG
        PrintOnConsole("[%s][%d]:shape=[%d],rotation=[%d]\n", 
            __FUNCTION__, __LINE__, 
            m_stTetrisBase.GetTetrisElement().shape, m_stTetrisBase.GetTetrisElement().rotation);
#endif      
        //4. 游戏结束判断
        HandleGameOver();
    }
}

CPoint CiTetrisDlg::GetCoordinateOfRect(const CRect& rect)
{
    CPoint ptTmp;

    ptTmp.x = (rect.TopLeft().x - g_ptTopLeft.x) / WIDTH;
    ptTmp.y = (rect.TopLeft().y - g_ptTopLeft.y) / WIDTH;

    return ptTmp;
}

void CiTetrisDlg::OnTetrisRotate(ROTATION rotation)
{
#ifdef _DEBUG
    //DEBUG模式，暂停时可以旋转，以方便测试
    if ((!m_bStart) /*|| m_bPause*/)
    {
        return;
    }
#else
    if ((!m_bStart) || m_bPause)
    {
        return;
    }
#endif 

    m_stTetrisBase.Rotate(m_stGameRegion, rotation);

    UnDrawTetris();
}

void CiTetrisDlg::OnTetrisMove(DIRECTION direction)
{
    if ((!m_bStart) || m_bPause)
    {
        return;
    }

    TetrisElement te;
    te = m_stTetrisBase.GetTetrisElement();

#ifdef _DEBUG
    CPoint ptTmp;
    ptTmp = m_stTetrisBase.GetMostDownPoint(te);
    PrintOnConsole("[%s][%d]: [%d][%d]\n", __FUNCTION__, __LINE__, ptTmp.x, ptTmp.y);
#endif

    //按键向左时，若不能再向左移，则直接返回
    if (DIRECTION_LEFT == direction)
    {
        m_bKeydownOfA = true;
        if (!CanTetrisMoveLeft(te))
        {
            return;
        }
    }

    //按键向右时，若不能再向右移，则直接返回
    if (DIRECTION_RIGHT == direction)
    {
        m_bKeydownOfD = true;
        if (!CanTetrisMoveRight(te))
        {
            return;
        }
    }

    int iAllowInterval = WIDTH;

    //按键向下时，若不能再向下移，则直接返回
    if (DIRECTION_DOWN == direction)
    {
        if (!CanTetrisMoveDown(te, iAllowInterval))
        {
            AddTetrisToGameRegion();
            return;
        }
    }

    //向下Move时，由于按键Move和自由下落Move时的移动距离不一致，导致当按键下移的距离不够时，出现下移重叠了
    //Move()函数下移距离最小值只能取到（当前游戏区域底部（包括最底部和占有方块的底部） - 当前方块的最底部点）
    //因此此处不能直接传入interval = WIDTH，考虑下移的情况
    
    m_stTetrisBase.Move(direction, iAllowInterval);

    AddTetrisToGameRegion();

    UnDrawTetris();
}

bool CiTetrisDlg::CanTetrisMove()
{
    TetrisElement te;
    bool bCanMoveLeft = true;
    bool bCanMoveRight = true;
    bool bCanMoveDown = true;
    int iAllowInterval = WIDTH;

    te = m_stTetrisBase.GetTetrisElement();
    bCanMoveLeft = CanTetrisMoveLeft(te);
    bCanMoveRight = CanTetrisMoveRight(te);
    bCanMoveDown = CanTetrisMoveDown(te, iAllowInterval);

    if (!bCanMoveDown)
    {
        //不能下移时，若可以向左移且按键了A，则允许移动
        if (bCanMoveLeft && m_bKeydownOfA)
        {
            m_bKeydownOfA = false;
            return true;
        }

        //不能下移时，若可以向右移且按键了D，则允许移动
        if (bCanMoveRight && m_bKeydownOfD)
        {
            m_bKeydownOfD = false;
            return true;
        }

        return false;
    }
    /*
    //可以左移或右移的时候，再判断是否可以下移
    if (bCanMoveLeft || bCanMoveRight)
    {
        if (!bCanMoveDown)
        {
            //不能下移时，若可以向左移且按键了A，则允许移动
            if (bCanMoveLeft && m_bKeydownOfA)
            {
                m_bKeydownOfA = false;
                return true;
            }

            //不能下移时，若可以向右移且按键了D，则允许移动
            if (bCanMoveRight && m_bKeydownOfD)
            {
                m_bKeydownOfD = false;
                return true;
            }

            return false;
        }
        else
        {
            return true;
        }
    }
    //同时不可以左移或右移，还需要判断是否可以下移
    else
    {
        if (!bCanMoveDown)
        {
            return false;
        }
        else
        {
            return true;
        }
    }*/

#ifdef _DEBUG
    CPoint ptTmp;
    ptTmp = m_stTetrisBase.GetMostDownPoint(te);
    //PrintOnConsole("[%s][%d]: [%d][%d]\n", __FUNCTION__, __LINE__, ptTmp.x, ptTmp.y);
#endif

    return true;
}

bool CiTetrisDlg::CanTetrisMoveDown(const TetrisElement& te, int& allowInterval)
{
    CRect rectTmp;
    CPoint ptTmp;

    /*
    //先来判断是否只剩半格就到底部或重叠了
    rectTmp = TetrisBase::GetMostDownRect(te);
    rectTmp = TetrisBase::MoveRect(rectTmp, DIRECTION_DOWN, WIDTH / 2); //测试性向下移动半格
    ptTmp = GetCoordinateOfRect(rectTmp);
    if (m_stGameRegion[ptTmp.x][ptTmp.y].bIsFilled || (g_ptBottomRight.y == rectTmp.bottom))
    {
        allowInterval = WIDTH / 2;
        return true;
    }*/
    /*for (int i = 0; i < RECT_COUNT; ++i)
    {
        rectTmp = TetrisBase::MoveRect(te.rects[i], DIRECTION_DOWN, WIDTH / 2); //测试性向下移动半格
        ptTmp = GetCoordinateOfRect(rectTmp);
        if (m_stGameRegion[ptTmp.x][ptTmp.y].bIsFilled || (g_ptBottomRight.y == te.rects[i].bottom))
        {
            allowInterval = WIDTH / 2;
            return true;
        }
    }*/
    for (int i = 0; i < RECT_COUNT; ++i)
    {
        //te.rects[i]和rectTmp位于同一列
        ptTmp = GetCoordinateOfRect(te.rects[i]);
        rectTmp = GetTheTopRectOfTheColumn(ptTmp.x);
        if (0 != rectTmp.top)
        {
            if ((WIDTH / 2) == (rectTmp.top - te.rects[i].bottom))
            {
                allowInterval = WIDTH / 2; //匹配到这种条件说明允许下移，并且只够下移半格了
                return true;
            }
        }
    }

    //上边情况排除后，然后正常来判断
    for (int i = 0; i < RECT_COUNT; ++i)
    {
        rectTmp = TetrisBase::MoveRect(te.rects[i], DIRECTION_DOWN, WIDTH); //测试性向下移动一格
        ptTmp = GetCoordinateOfRect(rectTmp);
        if (m_stGameRegion[ptTmp.x][ptTmp.y].bIsFilled || (g_ptBottomRight.y <= te.rects[i].bottom))
        {
            return false;
        }
    }

    return true;
}

bool CiTetrisDlg::CanTetrisMoveLeft(const TetrisElement& te)
{
    CRect rectTmp;
    CPoint ptTmp;

    for (int i = 0; i < RECT_COUNT; ++i)
    {
        rectTmp = TetrisBase::MoveRect(te.rects[i], DIRECTION_LEFT, WIDTH); //测试性向左移动一格
        ptTmp = GetCoordinateOfRect(rectTmp);
        if (m_stGameRegion[ptTmp.x][ptTmp.y].bIsFilled || (g_ptTopLeft.x >= te.rects[i].left))
        {
            return false;
        }
    }

    return true;
}

bool CiTetrisDlg::CanTetrisMoveRight(const TetrisElement& te)
{
    CRect rectTmp;
    CPoint ptTmp;

    for (int i = 0; i < RECT_COUNT; ++i)
    {
        rectTmp = TetrisBase::MoveRect(te.rects[i], DIRECTION_RIGHT, WIDTH); //测试性向左移动一格
        ptTmp = GetCoordinateOfRect(rectTmp);
        if (m_stGameRegion[ptTmp.x][ptTmp.y].bIsFilled || (g_ptBottomRight.x <= te.rects[i].right))
        {
            return false;
        }
    }

    return true;
}

CRect CiTetrisDlg::GetTheTopRectOfTheColumn(const int col)
{
    CRect rectTmp;

    rectTmp.SetRectEmpty();

    //从最顶部开始判断 
    for (int i = 0; i < LINE; ++i)
    {
        if (m_stGameRegion[col][i].bIsFilled)
        {
            rectTmp = m_stGameRegion[col][i].rectData;
            break;
        }
    }

    return rectTmp;
}

void CiTetrisDlg::HandleGameOver()
{
    TetrisElement te;
    CPoint ptTmp;

    //此时te是刚刚生成的
    te = m_stTetrisBase.GetTetrisElement();
    for (int i = 0; i < RECT_COUNT; ++i)
    {
        ptTmp = GetCoordinateOfRect(te.rects[i]);
        //PrintOnConsole("[%s][%d]: [%d][%d]\n", __FUNCTION__, __LINE__, te.rects[i].left, te.rects[i].top);
        //PrintOnConsole("[%s][%d]: [%d][%d]\n", __FUNCTION__, __LINE__, ptTmp.x, ptTmp.y);
        if (m_stGameRegion[ptTmp.x][ptTmp.y].bIsFilled)
        {
            m_bGameOver = true;
            break;
        }
    }

    //当新生成的方块在游戏区域已被占有则判定游戏已结束
    if (m_bGameOver)
    {
#ifdef _DEBUG
        PrintOnConsole("[%s][%d]: Game over~\n", __FUNCTION__, __LINE__);
#endif

        KillTimer(0);

        CString info("游戏结束了！");
        if (IDOK == AfxMessageBox(info))
        {
            //PrintOnConsole("[%s][%d]: Clicked MB_OK!", __FUNCTION__, __LINE__);
            m_stTetrisBase.Init();
            m_stTetrisBase.GenerateNext(g_shape);

            m_bPause = false;
            m_bStart = false;

            m_uiScore = 0;
            m_uiSpeed = 350;
            m_uiLevel = 1;

            m_bGameOver = false;
            m_bAIMode = false;

            UnDrawGameRegionFrame();
            UnDrawGameRegion();
            UnDrawHelpInfo();
        }
    }
}

char CiTetrisDlg::RemoveFullLine()
{
    int iFullLine1;
    char cFullLineFlag = 0;

    iFullLine1 = GetTheMostBottomFullLine();

    //存在满行，才继续，并且该满行是最靠底部的满行
    if (LINE != iFullLine1)
    {
        bool bFullLine2, bFullLine3, bFullLine4;
        //第4行
        //第3行
        //第2行
        //第1行
        //将相邻4行是否满行转化为半个字节的数字，各比特位来表示是否满行，0x01代表最靠近底部的那行是满的
        cFullLineFlag = 0x01;

        bFullLine2 = IsLineFull(iFullLine1 - 1);
        bFullLine3 = IsLineFull(iFullLine1 - 2);
        bFullLine4 = IsLineFull(iFullLine1 - 3);

        if (bFullLine2)
        {
            cFullLineFlag |= 0x02;
        }

        if (bFullLine3)
        {
            cFullLineFlag |= 0x04;
        }

        if (bFullLine4)
        {
            cFullLineFlag |= 0x08;
        }

        switch(cFullLineFlag)
        {
        //1、2行是满的
        case 0x0F:
            {
                RemoveTheChosenLine(iFullLine1 - 3, iFullLine1);
            } break;
        case 0x0B:
            {
                RemoveTheChosenLine(iFullLine1 - 3, iFullLine1 - 3);
                RemoveTheChosenLine(iFullLine1 - 1, iFullLine1);
            } break;
        case 0x07:
            {
                RemoveTheChosenLine(iFullLine1 - 2, iFullLine1);
            } break;
        case 0x03:
            {
                RemoveTheChosenLine(iFullLine1 - 1, iFullLine1);
            } break;
        //2行不是满的
        case 0x0D:
            {
                RemoveTheChosenLine(iFullLine1 - 3, iFullLine1 - 2);
                RemoveTheChosenLine(iFullLine1, iFullLine1);
            } break;
        case 0x09:
            {
                RemoveTheChosenLine(iFullLine1 - 3, iFullLine1 - 3);
                RemoveTheChosenLine(iFullLine1, iFullLine1);
            } break;
        case 0x05:
            {
                RemoveTheChosenLine(iFullLine1 - 2, iFullLine1 - 2);
                RemoveTheChosenLine(iFullLine1, iFullLine1);
            } break;
        case 0x01:
            {
                RemoveTheChosenLine(iFullLine1, iFullLine1);
            } break;
        default: break;
        }

        /*
        if (bFullLine2 && bFullLine3 && bFullLine4)
        {
            //连消4行
            return;
        }

        if (bFullLine2 && bFullLine3 && !bFullLine4)
        {
            //连消3行，此时bFullLine4必为false
            return;
        }

        if (bFullLine2 && !bFullLine3)
        {
            //连消2行，此时bFullLine3必为false，bFullLine4不确定
            if (bFullLine4)
            {
            }
            return;
        }

        if (!bFullLine2 && bFullLine3 && bFullLine4)
        {
            return;
        }

        if (!bFullLine2 && bFullLine3 && !bFullLine4)
        {
            return;
        }

        if (!bFullLine2 && !bFullLine3 && bFullLine4)
        {
            return;
        }

        //以上都不成立，则只有一开始的最靠近底部的那行是满行
        //...
        */
    }

    //根据返回的cFullLineFlag来判断总共消了多少行
    return cFullLineFlag;
}

int CiTetrisDlg::GetTheMostBottomFullLine()
{
    int line = LINE;
    int i, j;

    for (i = LINE - 1; i >= 0; --i)
    {
        for (j = 0; j < COLUMN; ++j)
        {
            if (!m_stGameRegion[j][i].bIsFilled)
            {
                break;
            }
        }

        //说明该行全满
        if (COLUMN == j)
        {
            line = i;
            break;
        }
    }

    return line;
}

bool CiTetrisDlg::IsLineFull(const int line)
{
    for (int i = 0; i < COLUMN; ++i)
    {
        if (!m_stGameRegion[i][line].bIsFilled)
        {
            return false;
        }
    }

    return true;
}

void CiTetrisDlg::RemoveTheChosenLine(const int line1, const int line2)
{
    CRect rectTmp;
    CPoint ptTmp;

    //TODO 该处可能有问题
    for (int i = 0; i < COLUMN; ++i)
    {
        rectTmp = GetTheTopRectOfTheColumn(i);
        ptTmp = GetCoordinateOfRect(rectTmp);
        if (line2 >= ptTmp.y)
        {
            for (int j = line1; j <= line2; ++j)
            {
                m_stGameRegion[ptTmp.x][ptTmp.y + j - line1].bIsFilled = false;
            }
        }
    }
}

void CiTetrisDlg::ComputeScore(int lines)
{
    if (0 < lines)
    {
        unsigned int uiOldScore = m_uiScore;
        m_uiScore += (lines - 1) * 20 + lines * 10;

        //每满1000分就进入下一关，并加快速度
        if (1 == ((m_uiScore / 1000) - (uiOldScore / 1000)))
        {
            m_uiSpeed = (m_uiSpeed < 100) ? 100 : (m_uiSpeed - 50);
            m_uiLevel++;

            SetTimer(0, m_uiSpeed, NULL); //重新设定降落速度
        }

#ifdef _DEBUG
        PrintOnConsole("[%s][%d]:m_uiScore=[%d],m_uiSpeed=[%d],m_uiLevel=[%d]\n", 
            __FUNCTION__, __LINE__, m_uiScore, m_uiSpeed, m_uiLevel);
#endif
    }
}

void CiTetrisDlg::OnTetrisMoveDownSuddenly()
{
    if ((!m_bStart) || m_bPause)
    {
        return;
    }

    int iAllowInterval = 0, iTmp;
    int col;
    CRect rectTop; //记录某一列堆积的最高的一个矩形
    TetrisElement te;
    
    te = m_stTetrisBase.GetTetrisElement();
    col = (te.rects[0].left - g_ptTopLeft.x) / WIDTH;
    rectTop = GetTheTopRectOfTheColumn(col);
    if (rectTop.IsRectNull())
    {
        iAllowInterval = g_ptBottomRight.y - te.rects[0].bottom;
    }
    else
    {
        iAllowInterval = rectTop.top - te.rects[0].bottom;
    }
    
    for (int i = 1; i < RECT_COUNT; ++i)
    {
        col = (te.rects[i].left - g_ptTopLeft.x) / WIDTH;
        rectTop = GetTheTopRectOfTheColumn(col);
        if (rectTop.IsRectNull())
        {
            iTmp = g_ptBottomRight.y - te.rects[i].bottom;
        }
        else
        {
            iTmp = rectTop.top - te.rects[i].bottom;
        }
        if (iTmp < iAllowInterval)
        {
            iAllowInterval = iTmp;
        }
    }

    //PrintOnConsole("[%s][%d]:iAllowInterval=[%d]\n", __FUNCTION__, __LINE__, iAllowInterval);
    m_stTetrisBase.Move(DIRECTION_DOWN, iAllowInterval);

    AddTetrisToGameRegion();
    UnDrawTetris();
}

void CiTetrisDlg::DrawNext()
{
    TetrisElement te = m_stTetrisElements[0];
    CRect rect;

    CDC* pDC = GetDC();

    if (NULL == pDC)
    {
        return;
    }

    CBrush* pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(GRAY_BRUSH));
    CBrush* pOldBrush = pDC->SelectObject(pBrush);

    for (int i = 0; i < RECT_COUNT; ++i)
    {
        rect = TetrisBase::MoveRect(te.rects[i], DIRECTION_DOWN, WIDTH * 13);
        rect = TetrisBase::MoveRect(rect, DIRECTION_RIGHT, WIDTH * 10);
        pDC->Rectangle(rect);
    }

    pDC->SelectObject(pOldBrush);

    ReleaseDC(pDC);
}

//****************************************************************
// Implement of AI below
//****************************************************************
UINT CiTetrisDlg::BeginAI(LPVOID pParam)
{
    if (NULL == pParam)
    {
        return 0;
    }

    CiTetrisDlg* pDialog = (CiTetrisDlg*)pParam;

    //1. 游戏开始
    pDialog->OnGameStart();
    pDialog->m_bAIMode = true;

    for (; ;)
    {
        //2. 计算当前方块的目标降落位置，按照特定的ROTATION进行匹配，并变形到目标形状
        pDialog->MoveToProperColumn();

        Sleep(500);

        if (pDialog->m_bGameOver)
        {
#ifdef _DEBUG
            PrintOnConsole("[%s][%d]:Game over~\n", __FUNCTION__, __LINE__);
#endif
            break;
        }

        //3. 将方块移动到目标位置
        pDialog->OnTetrisMoveDownSuddenly();

        //::SendMessage(this->m_hWnd, WM_KEYDOWN, MAKEWPARAM(VK_ESCAPE, 0), NULL);
        Sleep(800);
#ifdef _DEBUG
        PrintOnConsole("[%s][%d]:after sleep 800 ms.\n", __FUNCTION__, __LINE__);
#endif
        
        if (pDialog->m_bGameOver)
        {
#ifdef _DEBUG
            PrintOnConsole("[%s][%d]:Game over~\n", __FUNCTION__, __LINE__);
#endif
            break;
        }
    }

    return 0;
}

void CiTetrisDlg::MoveToProperColumn()
{
    //1. 首先找目标位置和目标变形形状
    TetrisElement te = m_stTetrisBase.GetTetrisElement();
    ROTATION targetRotation = ROTATION_INVALID;
    int iTargetCol = 0;
    
    iTargetCol = FindTheProperColumn(te.shape, targetRotation);

    //2. 变形到目标形状
    while (1)
    {
        if (m_bGameOver)
        {
#ifdef _DEBUG
            PrintOnConsole("[%s][%d]:Game over~\n", __FUNCTION__, __LINE__);
#endif
            return;
        }

        //直到变形到目标形状
        if ((ROTATION_INVALID == targetRotation) || (te.rotation == targetRotation))
        {
            break;
        }

        OnTetrisRotate();
        te = m_stTetrisBase.GetTetrisElement();
    }

    //3. 移动到目标位置所在列
    CPoint ptTmp = TetrisBase::GetMostLeftPoint(te);
    int iCurrCol = (ptTmp.x - g_ptTopLeft.x) / WIDTH;

    if (iCurrCol > iTargetCol) //此时需向左移
    {
        while (1)
        {
            if (m_bGameOver)
            {
#ifdef _DEBUG
                PrintOnConsole("[%s][%d]:Game over~\n", __FUNCTION__, __LINE__);
#endif
                return;
            }

            if (iCurrCol == iTargetCol)
            {
                break;
            }

            OnTetrisMove(DIRECTION_LEFT);
            te = m_stTetrisBase.GetTetrisElement();
            ptTmp = TetrisBase::GetMostLeftPoint(te);
            iCurrCol = (ptTmp.x - g_ptTopLeft.x) / WIDTH;
        }
    }
    else if (iCurrCol < iTargetCol) //此时需向右移
    {
        while (1)
        {
            if (m_bGameOver)
            {
#ifdef _DEBUG
                PrintOnConsole("[%s][%d]:Game over~\n", __FUNCTION__, __LINE__);
#endif
                return;
            }

            if (iCurrCol == iTargetCol)
            {
                break;
            }

            OnTetrisMove(DIRECTION_RIGHT);
            te = m_stTetrisBase.GetTetrisElement();
            ptTmp = TetrisBase::GetMostLeftPoint(te);
            iCurrCol = (ptTmp.x - g_ptTopLeft.x) / WIDTH;
        }
    }
    else
    {
        ;
    }
}

int CiTetrisDlg::FindTheProperColumn(SHAPE s, ROTATION& r)
{
    switch (s)
    {
    case SHAPE_SQUARE: return FindTheProperColumnForSquareShape(r); break;
    case SHAPE_STRIP: return FindTheProperColumnForStripShape(r); break;
    case SHAPE_T: return FindTheProperColumnForTShape(r); break;
    case SHAPE_STEP: return FindTheProperColumnForStepShape(r); break;
    case SHAPE_RSTEP: return FindTheProperColumnForRStepShape(r); break;
    case SHAPE_L: return FindTheProperColumnForLShape(r); break;
    case SHAPE_RL: return FindTheProperColumnForRLShape(r); break;
    default: break;
    }

    return 0;
}

int CiTetrisDlg::FindTheProperColumnForSquareShape(ROTATION& r)
{
    CRect rectTmp;

    for (int i = LINE - 1; i >= 0; --i)
    {  
        for (int j = 0; j < COLUMN - 1; ++j)
        {
            rectTmp = m_stGameRegion[j][i].rectData;

            //判断一行中连续相邻2个是否是空的
            if ((!IsRectOccupied(rectTmp))
                && (!IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH))))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i))
                {
                    r = ROTATION_1; //SHAPE_SQUARE只有一种变形为ROTATION_1
                    return j; //返回符合条件的位置的最靠左边的列的值
                }
            }
        }
    }

    return 0;
}

int CiTetrisDlg::FindTheProperColumnForStripShape(ROTATION& r)
{
    //对于SHAPE_STRIP，优先级为 ROTATION_2 -> ROTATION_1
    //即  *  ->   * * * *
    //    *
    //    *
    //    *

    CRect rectTmp;

    //使用ROTATION_2来匹配
    for (int i = LINE - 1; i >= 0; --i)
    {
        for (int j = 0; j < COLUMN; ++j)
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i))
                {
                    r = ROTATION_2; //此时目标变形形状是ROTATION_2
                    return j;
                }
            }
        }
    }

    //使用ROTATION_1来匹配
    //TODO

    return 0;
}

int CiTetrisDlg::FindTheProperColumnForTShape(ROTATION& r)
{
    //对于SHAPE_T，优先级为 ROTATION_4 -> ROTATION_2 -> ROTATION_1 -> ROTATION_3
    //即  *     ->    *  ->  * * *  ->   *
    //    * *       * *        *       * * *
    //    *           *

    CRect rectTmp;

    for (int i = LINE - 1; i >= 0; --i)
    {
        //使用ROTATION_4来匹配
        for (int j = 0; j < COLUMN - 1; ++j) // 注：j最多到COLUMN - 1 - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i - 1))
                {
                    r = ROTATION_4; //此时目标变形形状是ROTATION_4
                    return j;
                }
            }
        }

        //使用ROTATION_2来匹配
        for (int j = 1; j < COLUMN; ++j) // 注：j从1开始，最多到COLUMN - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_LEFT, WIDTH)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j - 1, i - 1))
                {
                    r = ROTATION_2; //此时目标变形形状是ROTATION_2
                    return j - 1; //始终返回最靠左边的列的值
                }
            }
        }

        //使用ROTATION_1来匹配
        for (int j = 1; j < COLUMN - 1; ++j) // 注：j从1开始，最多到COLUMN - 1 - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_LEFT, WIDTH))
                && IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j - 1, i - 1)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i - 1))
                {
                    r = ROTATION_1; //此时目标变形形状是ROTATION_1
                    return j - 1; //始终返回最靠左边的列的值
                }
            }
        }

        //使用ROTATION_3来匹配
        for (int j = 0; j < COLUMN - 2; ++j) // 注：j最多到COLUMN - 2 - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && (!IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH)))
                && (!IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH * 2))))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i)
                    && IsTheColumnNotFullAboveTheLine(j + 2, i))
                {
                    r = ROTATION_3; //此时目标变形形状是ROTATION_3
                    return j;
                }
            }
        }
    }

    return 0;
}

int CiTetrisDlg::FindTheProperColumnForStepShape(ROTATION& r)
{
    //对于SHAPE_STEP，优先级为 ROTATION_1 -> ROTATION_2
    //即 *     ->    * *
    //   * *       * *
    //     *
    
    CRect rectTmp;

    for (int i = LINE - 1; i >= 0; --i)
    {
        //使用ROTATION_1来匹配
        for (int j = 1; j < COLUMN; ++j) // 注：注：j从1开始，最多到COLUMN - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_LEFT, WIDTH)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j - 1, i - 1))
                {
                    r = ROTATION_1; //此时目标变形形状是ROTATION_1
                    return j - 1; //始终返回最靠左边的列的值
                }
            }
        }

        //使用ROTATION_2来匹配
        for (int j = 0; j < COLUMN - 2; ++j) // 注：注：j从0开始，最多到COLUMN - 2 - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && !IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH))
                && !IsRectOccupied(TetrisBase::MoveRect(
                TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH * 2), DIRECTION_UP, WIDTH)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i)
                    && IsTheColumnNotFullAboveTheLine(j + 2, i - 1))
                {
                    r = ROTATION_2; //此时目标变形形状是ROTATION_2
                    return j; //始终返回最靠左边的列的值
                }
            }
        }
    }

    return 0;
}

int CiTetrisDlg::FindTheProperColumnForRStepShape(ROTATION& r)
{
    //对于SHAPE_RSTEP，优先级为 ROTATION_1 -> ROTATION_2
    //即   *   ->  * *
    //   * *         * *
    //   *

    CRect rectTmp;

    for (int i = LINE - 1; i >= 0; --i)
    {
        //使用ROTATION_1来匹配
        for (int j = 0; j < COLUMN - 1; ++j) // 注：注：j从0开始，最多到COLUMN - 1 - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i - 1))
                {
                    r = ROTATION_1; //此时目标变形形状是ROTATION_1
                    return j; //始终返回最靠左边的列的值
                }
            }
        }

        //使用ROTATION_2来匹配
        for (int j = 1; j < COLUMN - 1; ++j) // 注：注：j从1开始，最多到COLUMN - 1 - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && !IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH))
                && !IsRectOccupied(TetrisBase::MoveRect(
                TetrisBase::MoveRect(rectTmp, DIRECTION_LEFT, WIDTH), DIRECTION_UP, WIDTH)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i)
                    && IsTheColumnNotFullAboveTheLine(j - 1, i - 1))
                {
                    r = ROTATION_2; //此时目标变形形状是ROTATION_2
                    return j - 1; //始终返回最靠左边的列的值
                }
            }
        }
    }

    return 0;
}

int CiTetrisDlg::FindTheProperColumnForLShape(ROTATION& r)
{
    //对于SHAPE_L，优先级为 ROTATION_3 -> ROTATION_2 -> ROTATION_4 -> ROTATION_1
    //即  * *  ->  * * *  ->      *   ->  *
    //      *      *          * * *       *
    //      *                             * *

    CRect rectTmp;

    for (int i = LINE - 1; i >= 0; --i)
    {
        //使用ROTATION_3来匹配
        for (int j = 1; j < COLUMN; ++j) // 注：注：j从1开始，最多到COLUMN - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_LEFT, WIDTH))
                && IsRectOccupied(TetrisBase::MoveRect(
                TetrisBase::MoveRect(rectTmp, DIRECTION_LEFT, WIDTH), DIRECTION_UP, WIDTH)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j - 1, i - 2))
                {
                    r = ROTATION_3; //此时目标变形形状是ROTATION_3
                    return j - 1; //始终返回最靠左边的列的值
                }
            }
        }

        //使用ROTATION_2来匹配
        for (int j = 0; j < COLUMN - 2; ++j) // 注：注：j从0开始，最多到COLUMN - 2 - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH))
                && IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH * 2)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i - 1)
                    && IsTheColumnNotFullAboveTheLine(j + 2, i - 1))
                {
                    r = ROTATION_2; //此时目标变形形状是ROTATION_2
                    return j; //始终返回最靠左边的列的值
                }
            }
        }

        //使用ROTATION_4来匹配
        for (int j = 0; j < COLUMN - 2; ++j) // 注：注：j从0开始，最多到COLUMN - 2 - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && !IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH))
                && !IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH * 2)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i)
                    && IsTheColumnNotFullAboveTheLine(j + 2, i))
                {
                    r = ROTATION_4; //此时目标变形形状是ROTATION_4
                    return j; //始终返回最靠左边的列的值
                }
            }
        }

        //使用ROTATION_1来匹配
        for (int j = 0; j < COLUMN - 1; ++j) // 注：注：j从0开始，最多到COLUMN - 1 - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && !IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i))
                {
                    r = ROTATION_1; //此时目标变形形状是ROTATION_1
                    return j; //始终返回最靠左边的列的值
                }
            }
        }
    }

    return 0;
}

int CiTetrisDlg::FindTheProperColumnForRLShape(ROTATION& r)
{
    //对于SHAPE_RL，优先级为 ROTATION_3 -> ROTATION_4 -> ROTATION_2 -> ROTATION_1
    //即  * *   ->   * * *   ->   *      ->    *
    //    *              *        * * *        *
    //    *                                  * *

    CRect rectTmp;

    for (int i = LINE - 1; i >= 0; --i)
    {
        //使用ROTATION_3来匹配
        for (int j = 0; j < COLUMN - 1; ++j) // 注：注：j从0开始，最多到COLUMN - 1 - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH))
                && IsRectOccupied(TetrisBase::MoveRect(
                TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH), DIRECTION_UP, WIDTH)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i - 2))
                {
                    r = ROTATION_3; //此时目标变形形状是ROTATION_3
                    return j; //始终返回最靠左边的列的值
                }
            }
        }

        //使用ROTATION_4来匹配
        for (int j = 2; j < COLUMN; ++j) // 注：注：j从2开始，最多到COLUMN - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_LEFT, WIDTH))
                && IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_LEFT, WIDTH * 2)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j - 1, i - 1)
                    && IsTheColumnNotFullAboveTheLine(j - 2, i - 1))
                {
                    r = ROTATION_4; //此时目标变形形状是ROTATION_4
                    return j - 2; //始终返回最靠左边的列的值
                }
            }
        }

        //使用ROTATION_2来匹配
        for (int j = 0; j < COLUMN - 2; ++j) // 注：注：j从0开始，最多到COLUMN - 2 - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && !IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH))
                && !IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH * 2)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i)
                    && IsTheColumnNotFullAboveTheLine(j + 2, i))
                {
                    r = ROTATION_2; //此时目标变形形状是ROTATION_2
                    return j; //始终返回最靠左边的列的值
                }
            }
        }

        //使用ROTATION_1来匹配
        for (int j = 0; j < COLUMN - 1; ++j) // 注：注：j从0开始，最多到COLUMN - 1 - 1
        {
            rectTmp = m_stGameRegion[j][i].rectData;
            if (!IsRectOccupied(rectTmp)
                && !IsRectOccupied(TetrisBase::MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH)))
            {
                if (IsTheColumnNotFullAboveTheLine(j, i)
                    && IsTheColumnNotFullAboveTheLine(j + 1, i))
                {
                    r = ROTATION_1; //此时目标变形形状是ROTATION_1
                    return j; //始终返回最靠左边的列的值
                }
            }
        }
    }

    return 0;
}

bool CiTetrisDlg::IsRectOccupied(const CRect& rect)
{
    CPoint ptTmp;

    ptTmp.x = (rect.TopLeft().x - g_ptTopLeft.x) / WIDTH;
    ptTmp.y = (rect.TopLeft().y - g_ptTopLeft.y) / WIDTH;

    if (m_stGameRegion[ptTmp.x][ptTmp.y].bIsFilled)
    {
        return true;
    }

    return false;
}

bool CiTetrisDlg::IsTheColumnNotFullAboveTheLine(int col, int line)
{
    CRect rectTmp;

    for (int i = line; i >= 0; --i)
    {
        rectTmp = m_stGameRegion[col][i].rectData;
        if (IsRectOccupied(rectTmp))
        {
            return false;
        }
    }

    return true;
}

//****************************************************************
// Implement of AI above
//****************************************************************





