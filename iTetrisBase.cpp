#include "stdafx.h"
#include "iTetrisBase.h"
#include "iTetris.h"
#include "iTetrisDlg.h"

int TetrisBase::m_iWidth = 0;
int TetrisBase::m_iGap = 0;
CPoint TetrisBase::m_ptUpLeft;
TetrisElement TetrisBase::m_stTetrisElement;

TetrisBase::TetrisBase()
{
    
}

TetrisBase::~TetrisBase()
{

}

void TetrisBase::Init()
{
    //设定随机数种子
    srand((unsigned int)time(NULL));

    m_iWidth = WIDTH;
    m_iGap = GAP;

    m_ptUpLeft = g_ptCenterOfFirstLine;

    m_mapRectDataHandler.insert(make_pair(SHAPE_INVALID, (pfSetRectData)NULL));
    m_mapRectDataHandler.insert(make_pair(SHAPE_SQUARE, TetrisBase::SetSquareData));
    m_mapRectDataHandler.insert(make_pair(SHAPE_STRIP, TetrisBase::SetStripData));
    m_mapRectDataHandler.insert(make_pair(SHAPE_T, TetrisBase::SetTData));
    m_mapRectDataHandler.insert(make_pair(SHAPE_STEP, TetrisBase::SetStepData));
    m_mapRectDataHandler.insert(make_pair(SHAPE_RSTEP, TetrisBase::SetRStepData));
    m_mapRectDataHandler.insert(make_pair(SHAPE_L, TetrisBase::SetLData));
    m_mapRectDataHandler.insert(make_pair(SHAPE_RL, TetrisBase::SetRLData));
}

void TetrisBase::GenerateNext(SHAPE s)
{
    //1. 确定形状
    if (SHAPE_INVALID == s) //表示随机生成形状
    {
        m_stTetrisElement.shape = (SHAPE)(rand() % 7 + 1);
    }
    else //指定形状
    {
        m_stTetrisElement.shape = s;
    }
    
    //2. 确定变形状态
    if (SHAPE_SQUARE == m_stTetrisElement.shape)
    {
        m_stTetrisElement.rotation = ROTATION_1;
    }
    else if ((SHAPE_STRIP == m_stTetrisElement.shape)
        || (SHAPE_STEP == m_stTetrisElement.shape)
        || (SHAPE_RSTEP == m_stTetrisElement.shape))
    {
        m_stTetrisElement.rotation = (ROTATION)(rand() % 2 + 1);
    }
    else
    {
        m_stTetrisElement.rotation = (ROTATION)(rand() % 4 + 1);
    }

    //3. 确定方块坐标数据
    SetRectsData(m_stTetrisElement.shape, m_stTetrisElement.rotation);
}

void TetrisBase::SetRectsData(SHAPE s, ROTATION r)
{
    CPoint ptTmp;
    CRect rectTmp;

    ptTmp.x = m_ptUpLeft.x + m_iWidth;
    ptTmp.y = m_ptUpLeft.y + m_iWidth;

    rectTmp.SetRect(m_ptUpLeft, ptTmp);

    m_pfSetRectData = m_mapRectDataHandler[s];

    if (NULL != m_pfSetRectData)
    {
        (*m_pfSetRectData)(rectTmp, r);
    }
}

void TetrisBase::SetSquareData(const CRect& rectRefer, ROTATION r)
{
    if (ROTATION_1 != r)
    {
        return;
    }

    // * *
    // * *
    m_stTetrisElement.rects[0] = rectRefer;
    m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_RIGHT, m_iWidth + m_iGap);
    m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_DOWN, m_iWidth + m_iGap);
    m_stTetrisElement.rects[3] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
}

void TetrisBase::SetStripData(const CRect& rectRefer, ROTATION r)
{
    // * * * * 
    if (ROTATION_1 == r)
    {
        m_stTetrisElement.rects[1] = rectRefer;
        m_stTetrisElement.rects[0] = MoveRect(rectRefer, DIRECTION_LEFT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(rectRefer, DIRECTION_RIGHT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[2], DIRECTION_RIGHT, m_iWidth + m_iGap);
    }

    // *
    // *
    // *
    // *
    if (ROTATION_2 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[2], DIRECTION_DOWN, m_iWidth + m_iGap);
    }
}

void TetrisBase::SetTData(const CRect& rectRefer, ROTATION r)
{
    // * * * 
    //   *
    if (ROTATION_1 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_LEFT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(rectRefer, DIRECTION_RIGHT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
    }
    //   *
    // * *
    //   *
    else if (ROTATION_2 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_LEFT, m_iWidth + m_iGap);
    }

    //   *
    // * * * 
    else if (ROTATION_3 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_LEFT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_RIGHT, m_iWidth + m_iGap);
    }

    // *
    // * *
    // *
    else if (ROTATION_4 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_RIGHT, m_iWidth + m_iGap);
    }
}

void TetrisBase::SetStepData(const CRect& rectRefer, ROTATION r)
{
    // *
    // * *
    //   *
    if (ROTATION_1 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_RIGHT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[2], DIRECTION_DOWN, m_iWidth + m_iGap);
    }

    //   * *
    // * *
    if (ROTATION_2 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_RIGHT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[2], DIRECTION_LEFT, m_iWidth + m_iGap);
    }
}

void TetrisBase::SetRStepData(const CRect& rectRefer, ROTATION r)
{
    //   *
    // * *
    // *
    if (ROTATION_1 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_LEFT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[2], DIRECTION_DOWN, m_iWidth + m_iGap);
    }

    // * *
    //   * *
    if (ROTATION_2 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_LEFT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[2], DIRECTION_RIGHT, m_iWidth + m_iGap);
    }
}

void TetrisBase::SetLData(const CRect& rectRefer, ROTATION r)
{
    // *
    // *
    // * *
    if (ROTATION_1 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[2], DIRECTION_RIGHT, m_iWidth + m_iGap);
    }
    // * * *
    // *
    else if (ROTATION_2 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_RIGHT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_RIGHT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
    }
    // * *
    //   *
    //   *
    else if (ROTATION_3 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_LEFT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[2], DIRECTION_DOWN, m_iWidth + m_iGap);
    }
    //     *
    // * * *
    else if (ROTATION_4 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_LEFT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[2], DIRECTION_LEFT, m_iWidth + m_iGap);
    }
}

void TetrisBase::SetRLData(const CRect& rectRefer, ROTATION r)
{
    //   *
    //   *
    // * *
    if (ROTATION_1 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[2], DIRECTION_LEFT, m_iWidth + m_iGap);
    }
    // *
    // * * *
    else if (ROTATION_2 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_RIGHT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[2], DIRECTION_RIGHT, m_iWidth + m_iGap);
    }
    // * *
    // *
    // *
    else if (ROTATION_3 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_RIGHT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(m_stTetrisElement.rects[2], DIRECTION_DOWN, m_iWidth + m_iGap);
    }
    // * * *
    //     *
    else if (ROTATION_4 == r)
    {
        m_stTetrisElement.rects[0] = rectRefer;
        m_stTetrisElement.rects[1] = MoveRect(rectRefer, DIRECTION_LEFT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[2] = MoveRect(m_stTetrisElement.rects[1], DIRECTION_LEFT, m_iWidth + m_iGap);
        m_stTetrisElement.rects[3] = MoveRect(rectRefer, DIRECTION_DOWN, m_iWidth + m_iGap);
    }
}

CRect TetrisBase::MoveRect(const CRect& rect, DIRECTION direction, int interval)
{
    CRect rectTmp;
    CPoint ptNewTopLeft = rect.TopLeft();
    CPoint ptNewBottomRight = rect.BottomRight();

    MovePoint(ptNewTopLeft, direction, interval);
    MovePoint(ptNewBottomRight, direction, interval);

    rectTmp.SetRect(ptNewTopLeft, ptNewBottomRight);

    return rectTmp;
}

void TetrisBase::MovePoint(CPoint& point, DIRECTION direction, int interval)
{
    switch(direction)
    {
    case DIRECTION_LEFT:
        {
            point.x -= interval;
        } break;
    case DIRECTION_UP:
        {
            point.y -= interval;
        } break;
    case DIRECTION_RIGHT:
        {
            point.x += interval;
        } break;
    case DIRECTION_DOWN:
        {
            point.y += interval;
        } break;
    default: break;
    }
}

TetrisElement TetrisBase::GetTetrisElement()
{
    return m_stTetrisElement;
}

bool TetrisBase::Move(DIRECTION direction, int interval)
{
    //移动方向不合法，则直接返回
    if ((DIRECTION_LEFT != direction)
        && (DIRECTION_RIGHT != direction)
        && (DIRECTION_DOWN != direction))
    {
        return false;
    }

    if (0 >= interval)
    {
        return false;
    }

    int iTmp = 0;

    if (DIRECTION_LEFT == direction)
    {
        CPoint ptTmp = GetMostLeftPoint(m_stTetrisElement);
        if (g_ptTopLeft.x >= ptTmp.x)
        {
            return false;
        }

        iTmp = ((ptTmp.x - g_ptTopLeft.x) < interval) ? (ptTmp.x - g_ptTopLeft.x) : interval;
    }

    if (DIRECTION_RIGHT == direction)
    {
        CPoint ptTmp = GetMostRightPoint(m_stTetrisElement);
        if (g_ptBottomRight.x <= ptTmp.x)
        {
            return false;
        }

        iTmp = ((g_ptBottomRight.x - ptTmp.x) < interval) ? (g_ptBottomRight.x - ptTmp.x) : interval;
    }

    if (DIRECTION_DOWN == direction)
    {
        CPoint ptTmp = GetMostDownPoint(m_stTetrisElement);
        if (g_ptBottomRight.y <= ptTmp.y)
        {
            return false;
        }

        iTmp = ((g_ptBottomRight.y - ptTmp.y) < interval) ? (g_ptBottomRight.y - ptTmp.y) : interval;
    }

    for (int i = 0; i < RECT_COUNT; ++i)
    {
        m_stTetrisElement.rects[i] = MoveRect(m_stTetrisElement.rects[i], direction, iTmp);
    }

    return true;
}

CPoint TetrisBase::GetMostLeftPoint(const TetrisElement& te)
{
    CPoint ptTmp = te.rects[0].TopLeft();

    for (int i = 1; i < RECT_COUNT; ++i)
    {
        if (te.rects[i].TopLeft().x < ptTmp.x)
        {
            ptTmp = te.rects[i].TopLeft();
        }
    }

    return ptTmp;
}

CPoint TetrisBase::GetMostRightPoint(const TetrisElement& te)
{
    CPoint ptTmp = te.rects[0].BottomRight();

    for (int i = 1; i < RECT_COUNT; ++i)
    {
        if (te.rects[i].BottomRight().x > ptTmp.x)
        {
            ptTmp = te.rects[i].BottomRight();
        }
    }

    return ptTmp;
}

CPoint TetrisBase::GetMostDownPoint(const TetrisElement& te)
{
    CPoint ptTmp = te.rects[0].BottomRight();

    for (int i = 1; i < RECT_COUNT; ++i)
    {
        if (te.rects[i].BottomRight().y > ptTmp.y)
        {
            ptTmp = te.rects[i].BottomRight();
        }
    }

    return ptTmp;
}

CRect TetrisBase::GetMostLeftRect(const TetrisElement& te)
{
    CPoint ptTmp;
    CPoint ptLeftMost = GetMostLeftPoint(te);

    ptTmp = ptLeftMost;
    ptTmp.x += WIDTH;
    ptTmp.y += WIDTH;

    return CRect(ptLeftMost, ptTmp);
}

CRect TetrisBase::GetMostRightRect(const TetrisElement& te)
{
    CPoint ptTmp;
    CPoint ptRightMost = GetMostRightPoint(te);

    ptTmp = ptRightMost;
    ptTmp.x -= WIDTH;
    ptTmp.y -= WIDTH;

    return CRect(ptTmp, ptRightMost);
}

CRect TetrisBase::GetMostDownRect(const TetrisElement& te)
{
    CPoint ptTmp;
    CPoint ptDownMost = GetMostDownPoint(te);

    ptTmp = ptDownMost;
    ptTmp.x -= WIDTH;
    ptTmp.y -= WIDTH;

    return CRect(ptTmp, ptDownMost);
}

void TetrisBase::Rotate(pGameRegion region, ROTATION rotation)
{
    switch(m_stTetrisElement.shape)
    {
    case SHAPE_SQUARE: RotateSquare(region, rotation); break;
    case SHAPE_STRIP: RotateStrip(region, rotation); break;
    case SHAPE_T: RotateT(region, rotation); break;
    case SHAPE_STEP: RotateStep(region, rotation); break;
    case SHAPE_RSTEP: RotateRStep(region, rotation); break;
    case SHAPE_L: RotateL(region, rotation); break;
    case SHAPE_RL: RotateRL(region, rotation); break;
    default: break;
    }
}

void TetrisBase::RotateSquare(pGameRegion region, ROTATION rotation)
{
    //正方形方块无旋转
    return;
}

void TetrisBase::RotateStrip(pGameRegion region, ROTATION rotation)
{
    CRect rectRefer, rectTmp, rectTheTopOfTheColumn;
    int col;

    //顺逆旋转是一样的，rotation不起作用
    rotation = rotation;

    switch(m_stTetrisElement.rotation)
    {
    // 0 1 2 3
    // * * * *  ->  * 0
    //              * 1
    //              * 2
    //              * 3
    case ROTATION_1:
        {
            m_stTetrisElement.rotation = ROTATION_2;

            rectRefer = m_stTetrisElement.rects[1]; //以rects[1]作为变形后的rects[1]
            rectRefer = MoveRect(rectRefer, DIRECTION_UP, WIDTH); //调整到ROTATION_2的基准点

            col = (rectRefer.left - g_ptTopLeft.x) / WIDTH;

            ClearTetrisData(); //清空原有数据
            
            rectTheTopOfTheColumn = GetTheTopRectOfTheColumn(region, col); 
            
            //旋转后与顶部重叠
            if (rectRefer.top < g_ptTopLeft.y)
            {
                rectTmp = MoveRect(rectRefer, DIRECTION_DOWN, WIDTH * 3);
                if (IsRectOccupied(region, rectTmp))
                {
                    rectRefer = MoveRect(rectRefer, DIRECTION_DOWN, WIDTH); //恢复成ROTATION_1的基准点
                    SetStripData(rectRefer, ROTATION_1); //这时无法变形为ROTATION_2
                    return;
                }
                rectRefer = MoveRect(rectRefer, DIRECTION_DOWN, WIDTH);
                SetStripData(rectRefer, ROTATION_2);
            }
            //旋转后与底部重叠
            else if ((g_ptBottomRight.y - rectRefer.bottom) / WIDTH < 3)
            {
                rectTmp = MoveRect(rectRefer, DIRECTION_UP, 
                    WIDTH * (3 - (g_ptBottomRight.y - rectRefer.bottom) / WIDTH)); //移到ROTATION_2的基准点
                if (IsRectOccupied(region, rectTmp))
                {
                    rectRefer = MoveRect(rectRefer, DIRECTION_DOWN, WIDTH); //恢复成ROTATION_1的基准点
                    SetStripData(rectRefer, ROTATION_1); //这时无法变形为ROTATION_2
                    return;
                }
                
                SetStripData(rectTmp, ROTATION_2);
            }
            //旋转后与某一层重叠
            else if (!rectTheTopOfTheColumn.IsRectNull() 
                && ((rectTheTopOfTheColumn.top - rectTmp.bottom) / WIDTH < 3))
            {
                rectTmp = MoveRect(rectRefer, DIRECTION_UP, 
                    WIDTH * (3 - (rectTheTopOfTheColumn.top - rectRefer.bottom) / WIDTH));
                if (IsRectOccupied(region, rectTmp))
                {
                    rectRefer = MoveRect(rectRefer, DIRECTION_DOWN, WIDTH); //恢复成ROTATION_1的基准点
                    SetStripData(rectRefer, ROTATION_1); //这时无法变形为ROTATION_2
                    return;
                }
                SetStripData(rectTmp, ROTATION_2);
            }
            //正常旋转
            else
            {
                SetStripData(rectRefer, ROTATION_2);
            }
        } break;
    //          0 1 2 3
    // 0 *  ->  * * * *
    // 1 *
    // 2 *
    // 3 *
    case ROTATION_2:
        {
            rectRefer = m_stTetrisElement.rects[1]; //以rects[0]作为变形后的rects[1]

            if (g_ptTopLeft.x == rectRefer.left) //当贴着左边缘时
            {
                //防止旋转后重叠
                rectTmp = rectRefer;
                for (int i = 0; i < RECT_COUNT; ++i)
                {
                    if (IsRectOccupied(region, rectTmp))
                    {
                        return;
                    }
                    rectTmp = MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH);
                }

                m_stTetrisElement.rotation = ROTATION_1;
                ClearTetrisData(); //清空原有数据
                rectRefer = MoveRect(rectRefer, DIRECTION_RIGHT, WIDTH);
                SetStripData(rectRefer, ROTATION_1);
            }
            else if (g_ptBottomRight.x == rectRefer.right) //当贴着右边缘
            {
                //防止旋转后重叠
                rectTmp = rectRefer;
                for (int i = 0; i < RECT_COUNT; ++i)
                {
                    if (IsRectOccupied(region, rectTmp))
                    {
                        return;
                    }
                    rectTmp = MoveRect(rectTmp, DIRECTION_LEFT, WIDTH);
                }

                m_stTetrisElement.rotation = ROTATION_1;
                ClearTetrisData(); //清空原有数据
                rectRefer = MoveRect(rectRefer, DIRECTION_LEFT, WIDTH * 2);
                SetStripData(rectRefer, ROTATION_1);
            }
            else if ((g_ptBottomRight.x - WIDTH) == rectRefer.right) //当与右边缘相差一格
            {
                //防止旋转后重叠
                rectTmp = rectRefer;
                for (int i = 0; i < RECT_COUNT; ++i)
                {
                    if (IsRectOccupied(region, rectTmp))
                    {
                        return;
                    }
                    rectTmp = MoveRect(rectTmp, DIRECTION_LEFT, WIDTH);
                }

                m_stTetrisElement.rotation = ROTATION_1;
                ClearTetrisData(); //清空原有数据
                rectRefer = MoveRect(rectRefer, DIRECTION_LEFT, WIDTH);
                SetStripData(rectRefer, ROTATION_1);
            }
            else
            {
                //防止旋转后重叠
                rectTmp = MoveRect(rectRefer, DIRECTION_LEFT, WIDTH);;
                for (int i = 0; i < RECT_COUNT; ++i)
                {
                    if (IsRectOccupied(region, rectTmp))
                    {
                        return;
                    }
                    rectTmp = MoveRect(rectTmp, DIRECTION_RIGHT, WIDTH);
                }

                m_stTetrisElement.rotation = ROTATION_1;
                ClearTetrisData(); //清空原有数据
                SetStripData(rectRefer, ROTATION_1);
            }
        } break;
    default: break;
    }
}

void TetrisBase::RotateT(pGameRegion region, ROTATION rotation)
{
    CRect rectRefer, rectTmp;
    ROTATION targetRotation;

    switch(m_stTetrisElement.rotation)
    {
    //
    // * * *  ->   *
    //   *       * * 
    //             *
    case ROTATION_1: 
        {
            targetRotation = ROTATION_2;
            if (ROTATION_2 == rotation)
            {
                targetRotation = ROTATION_4;
            }

            //把ROTATION_1的rects[0]向上移一格作为ROTATION_2的基准点rects[0]
            rectRefer = m_stTetrisElement.rects[0];
            rectRefer = MoveRect(rectRefer, DIRECTION_UP, WIDTH);

            //此时需要下移rects[0]，但要确保最底部没有被占有 
            if ((g_ptTopLeft.y > rectRefer.top) || (IsRectOccupied(region, rectRefer)))
            {
                rectTmp = MoveRect(rectRefer, DIRECTION_UP, WIDTH * 3);
                if (IsRectOccupied(region, rectTmp))
                {
                    //这时无法变形为ROTATION_2
                    return;
                }

                //否则调整rects[0]后可以变形为ROTATION_2
                rectRefer = MoveRect(rectRefer, DIRECTION_DOWN, WIDTH);
                m_stTetrisElement.rotation = targetRotation;
                ClearTetrisData(); //清空原有数据
                SetTData(rectRefer, m_stTetrisElement.rotation);
                return;
            }

            m_stTetrisElement.rotation = targetRotation;
            ClearTetrisData(); //清空原有数据
            SetTData(rectRefer, m_stTetrisElement.rotation);
        } break;
    //
    //   *  ->    *
    // * *      * * *
    //   *
    case ROTATION_2: 
        {
            targetRotation = ROTATION_3;
            if (ROTATION_2 == rotation)
            {
                targetRotation = ROTATION_1;
            }

            //把ROTATION_2的rects[0]作为ROTATION_3的基准点rects[0]
            rectRefer = m_stTetrisElement.rects[0];
            rectTmp = MoveRect(m_stTetrisElement.rects[1], DIRECTION_RIGHT, WIDTH);

            //当贴着右边缘或旋转后跟已占有的重叠
            if ((g_ptBottomRight.x == rectRefer.right) || (IsRectOccupied(region, rectTmp))) 
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_LEFT, WIDTH);
                m_stTetrisElement.rotation = targetRotation;
                ClearTetrisData(); //清空原有数据
                SetTData(rectRefer, m_stTetrisElement.rotation);
                return;
            }

            m_stTetrisElement.rotation = targetRotation;
            ClearTetrisData(); //清空原有数据
            SetTData(rectRefer, m_stTetrisElement.rotation);
        } break;
    //
    //   *   ->  *
    // * * *     * *
    //           *
    case ROTATION_3: 
        {
            targetRotation = ROTATION_4;
            if (ROTATION_2 == rotation)
            {
                targetRotation = ROTATION_2;
            }

            CRect rectTheTopOfTheColumn;
            int col;

            //把ROTATION_3的rects[0]作为ROTATION_4的基准点rects[0]
            rectRefer = m_stTetrisElement.rects[0];
            rectTmp = m_stTetrisElement.rects[1];

            col = (rectRefer.left - g_ptTopLeft.x) / WIDTH;
            rectTheTopOfTheColumn = GetTheTopRectOfTheColumn(region, col);

            //当与底部接触或与底部只剩半格，或与已有的接触或只剩半格就与已有的重叠时，需调整
            if ((g_ptBottomRight.y == rectTmp.bottom) 
                || (g_ptBottomRight.y - rectTmp.bottom == WIDTH / 2)
                || (rectTheTopOfTheColumn.top == rectTmp.bottom) 
                || (rectTheTopOfTheColumn.top - rectTmp.bottom == WIDTH / 2))
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_UP, WIDTH);
                m_stTetrisElement.rotation = targetRotation;
                ClearTetrisData(); //清空原有数据
                SetTData(rectRefer, m_stTetrisElement.rotation);
                return;
            }

            m_stTetrisElement.rotation = targetRotation;
            ClearTetrisData(); //清空原有数据
            SetTData(rectRefer, m_stTetrisElement.rotation);
        } break;
    //
    // *    ->  * * *
    // * *        *
    // *
    case ROTATION_4: 
        {
            targetRotation = ROTATION_1;
            if (ROTATION_2 == rotation)
            {
                targetRotation = ROTATION_3;
            }

            //把ROTATION_4的rects[1]作为ROTATION_1的基准点rects[0]
            rectRefer = m_stTetrisElement.rects[1];
            rectTmp = MoveRect(m_stTetrisElement.rects[1], DIRECTION_LEFT, WIDTH);

            //当贴着左边缘或旋转后跟已占有的重叠
            if ((g_ptTopLeft.x == rectRefer.left) || (IsRectOccupied(region, rectTmp))) 
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_RIGHT, WIDTH);
                m_stTetrisElement.rotation = targetRotation;
                ClearTetrisData(); //清空原有数据
                SetTData(rectRefer, m_stTetrisElement.rotation);
                return;
            }

            m_stTetrisElement.rotation = targetRotation;
            ClearTetrisData(); //清空原有数据
            SetTData(rectRefer, m_stTetrisElement.rotation);
        } break;
    default: break;
    }
}

void TetrisBase::RotateStep(pGameRegion region, ROTATION rotation)
{
    CRect rectRefer, rectTmp;

    //顺逆旋转是一样的，rotation不起作用
    rotation = rotation;

    switch(m_stTetrisElement.rotation)
    {
    //              0 1
    // 0 *          * *
    // 1 * *  ->  * *
    //     *
    case ROTATION_1: 
        {
            rectRefer = m_stTetrisElement.rects[0];
            rectRefer = MoveRect(rectRefer, DIRECTION_RIGHT, WIDTH);
            rectTmp = MoveRect(m_stTetrisElement.rects[2], DIRECTION_RIGHT, WIDTH);

            if ((g_ptBottomRight.x < rectTmp.right) || (IsRectOccupied(region, rectTmp)))
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_LEFT, WIDTH);
                m_stTetrisElement.rotation = ROTATION_2;
                ClearTetrisData(); //清空原有数据
                SetStepData(rectRefer, ROTATION_2);
                return;
            }

            m_stTetrisElement.rotation = ROTATION_2;
            ClearTetrisData(); //清空原有数据
            SetStepData(rectRefer, ROTATION_2);
        } break;
    //
    //   * *  ->  *
    // * *        * *
    //              *
    case ROTATION_2: 
        {
            CRect rectTheTopOfTheColumn;
            int col;

            rectRefer = m_stTetrisElement.rects[0];
            rectRefer = MoveRect(rectRefer, DIRECTION_LEFT, WIDTH);
            rectTmp = m_stTetrisElement.rects[2];

            col = (m_stTetrisElement.rects[0].left - g_ptTopLeft.x) / WIDTH;
            rectTheTopOfTheColumn = GetTheTopRectOfTheColumn(region, col);

            if ((g_ptBottomRight.y == rectTmp.bottom) 
                || (g_ptBottomRight.y - rectTmp.bottom == WIDTH / 2)
                || (rectTheTopOfTheColumn.top == rectTmp.bottom) 
                || (rectTheTopOfTheColumn.top - rectTmp.bottom == WIDTH / 2))
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_UP, WIDTH);
                m_stTetrisElement.rotation = ROTATION_1;
                ClearTetrisData(); //清空原有数据
                SetStepData(rectRefer, ROTATION_1);
                return;
            }

            m_stTetrisElement.rotation = ROTATION_1;
            ClearTetrisData(); //清空原有数据
            SetStepData(rectRefer, ROTATION_1);
        } break;
    default: break;
    }
}

void TetrisBase::RotateRStep(pGameRegion region, ROTATION rotation)
{
    CRect rectRefer, rectTmp;

    //顺逆旋转是一样的，rotation不起作用
    rotation = rotation;

    switch(m_stTetrisElement.rotation)
    {
    //   *      * *
    // * *  ->    * *
    // *
    case ROTATION_1: 
        {
            rectRefer = m_stTetrisElement.rects[0];
            rectRefer = MoveRect(rectRefer, DIRECTION_LEFT, WIDTH);
            rectTmp = MoveRect(m_stTetrisElement.rects[2], DIRECTION_LEFT, WIDTH);

            if ((g_ptTopLeft.x > rectTmp.left) || (IsRectOccupied(region, rectTmp)))
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_RIGHT, WIDTH);
                m_stTetrisElement.rotation = ROTATION_2;
                ClearTetrisData(); //清空原有数据
                SetRStepData(rectRefer, ROTATION_2);
                return;
            }

            m_stTetrisElement.rotation = ROTATION_2;
            ClearTetrisData(); //清空原有数据
            SetRStepData(rectRefer, ROTATION_2);
        } break;
    //              *
    // * *    ->  * *
    //   * *      *  
    case ROTATION_2: 
        {
            CRect rectTheTopOfTheColumn;
            int col;

            rectRefer = m_stTetrisElement.rects[0];
            rectRefer = MoveRect(rectRefer, DIRECTION_RIGHT, WIDTH);
            rectTmp = m_stTetrisElement.rects[2];

            col = (m_stTetrisElement.rects[0].left - g_ptTopLeft.x) / WIDTH;
            rectTheTopOfTheColumn = GetTheTopRectOfTheColumn(region, col);

            if ((g_ptBottomRight.y == rectTmp.bottom)
                || (g_ptBottomRight.y - rectTmp.bottom == WIDTH / 2)
                || (rectTheTopOfTheColumn.top == rectTmp.bottom)
                || (rectTheTopOfTheColumn.top - rectTmp.bottom == WIDTH / 2))
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_UP, WIDTH);
                m_stTetrisElement.rotation = ROTATION_1;
                ClearTetrisData(); //清空原有数据
                SetRStepData(rectRefer, ROTATION_1);
                return;
            }

            m_stTetrisElement.rotation = ROTATION_1;
            ClearTetrisData(); //清空原有数据
            SetRStepData(rectRefer, ROTATION_1);
        } break;
    default: break;
    }
}

void TetrisBase::RotateL(pGameRegion region, ROTATION rotation)
{
    CRect rectRefer, rectTmp;
    ROTATION targetRotation;

    switch(m_stTetrisElement.rotation)
    {
    // *        * * *
    // *    ->  *  
    // * *
    //
    case ROTATION_1: 
        {
            targetRotation = ROTATION_2;
            if (ROTATION_2 == rotation)
            {
                targetRotation = ROTATION_4;
            }

            rectRefer = m_stTetrisElement.rects[1];
            rectTmp = MoveRect(m_stTetrisElement.rects[3], DIRECTION_RIGHT, WIDTH);

            //与右边缘接触
            if ((g_ptBottomRight.x < rectTmp.right) || IsRectOccupied(region, rectTmp))
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_LEFT, WIDTH);
                m_stTetrisElement.rotation = targetRotation;
                ClearTetrisData(); //清空原有数据
                SetLData(rectRefer, m_stTetrisElement.rotation);
                return;
            }

            m_stTetrisElement.rotation = targetRotation;
            ClearTetrisData(); //清空原有数据
            SetLData(rectRefer, m_stTetrisElement.rotation);
        } break;
    //            * *
    // * * *  ->    *
    // *            *
    case ROTATION_2: 
        {
            targetRotation = ROTATION_3;
            if (ROTATION_2 == rotation)
            {
                targetRotation = ROTATION_1;
            }

            CRect rectTheTopOfTheColumn;
            int col;

            rectRefer = m_stTetrisElement.rects[1];
            rectTmp = m_stTetrisElement.rects[3];

            col = (rectRefer.left - g_ptTopLeft.x) / WIDTH;
            rectTheTopOfTheColumn = GetTheTopRectOfTheColumn(region, col);
            if ((g_ptBottomRight.y == rectTmp.bottom) 
                || (g_ptBottomRight.y - rectTmp.bottom == WIDTH / 2)
                || (rectTheTopOfTheColumn.top == rectTmp.bottom)
                || (rectTheTopOfTheColumn.top - rectTmp.bottom == WIDTH / 2))
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_UP, WIDTH);
                m_stTetrisElement.rotation = targetRotation;
                ClearTetrisData(); //清空原有数据
                SetLData(rectRefer, m_stTetrisElement.rotation);
                return;
            }

            m_stTetrisElement.rotation = targetRotation;
            ClearTetrisData(); //清空原有数据
            SetLData(rectRefer, m_stTetrisElement.rotation);
        } break;
    // * *          *
    //   *  ->  * * *
    //   *
    case ROTATION_3: 
        {
            targetRotation = ROTATION_4;
            if (ROTATION_2 == rotation)
            {
                targetRotation = ROTATION_2;
            }

            rectRefer = m_stTetrisElement.rects[0];
            rectTmp = MoveRect(m_stTetrisElement.rects[1], DIRECTION_LEFT, WIDTH);

            if ((g_ptTopLeft.x == rectTmp.left) || IsRectOccupied(region, rectTmp))
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_RIGHT, WIDTH);
                m_stTetrisElement.rotation = targetRotation;
                ClearTetrisData(); //清空原有数据
                SetLData(rectRefer, m_stTetrisElement.rotation);
                return;
            }

            m_stTetrisElement.rotation = targetRotation;
            ClearTetrisData(); //清空原有数据
            SetLData(rectRefer, m_stTetrisElement.rotation);
        } break;
    //     *      *
    // * * *  ->  *
    //            * *
    case ROTATION_4: 
        {
            targetRotation = ROTATION_1;
            if (ROTATION_2 == rotation)
            {
                targetRotation = ROTATION_3;
            }

            rectRefer = m_stTetrisElement.rects[2];
            rectRefer = MoveRect(rectRefer, DIRECTION_UP, WIDTH * 2);
            rectTmp = MoveRect(m_stTetrisElement.rects[0], DIRECTION_UP, WIDTH);

            if (g_ptTopLeft.y > rectTmp.bottom)
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_DOWN, WIDTH);
                m_stTetrisElement.rotation = targetRotation;
                ClearTetrisData(); //清空原有数据
                SetLData(rectRefer, m_stTetrisElement.rotation);
                return;
            }

            m_stTetrisElement.rotation = targetRotation;
            ClearTetrisData(); //清空原有数据
            SetLData(rectRefer, m_stTetrisElement.rotation);
        } break;
    default: break;
    }
}

void TetrisBase::RotateRL(pGameRegion region, ROTATION rotation)
{
    CRect rectRefer, rectTmp;
    ROTATION targetRotation;

    switch(m_stTetrisElement.rotation)
    {
    //   *       *
    //   *   ->  * * *
    // * *
    case ROTATION_1: 
        {
            targetRotation = ROTATION_2;
            if (ROTATION_2 == rotation)
            {
                targetRotation = ROTATION_4;
            }

            rectRefer = m_stTetrisElement.rects[1];
            rectRefer = MoveRect(rectRefer, DIRECTION_LEFT, WIDTH);
            rectTmp = MoveRect(m_stTetrisElement.rects[2], DIRECTION_RIGHT, WIDTH);

            if ((g_ptBottomRight.x < rectTmp.right) || IsRectOccupied(region, rectTmp))
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_LEFT, WIDTH);
                m_stTetrisElement.rotation = targetRotation;
                ClearTetrisData(); //清空原有数据
                SetRLData(rectRefer, m_stTetrisElement.rotation);
                return;
            }

            m_stTetrisElement.rotation = targetRotation;
            ClearTetrisData(); //清空原有数据
            SetRLData(rectRefer, m_stTetrisElement.rotation);
        } break;
    // *          * *
    // * * *  ->  *
    //            *
    case ROTATION_2: 
        {
            targetRotation = ROTATION_3;
            if (ROTATION_2 == rotation)
            {
                targetRotation = ROTATION_1;
            }

            CRect rectTheTopOfTheColumn;
            int col;

            rectRefer = m_stTetrisElement.rects[0];
            rectTmp = m_stTetrisElement.rects[1];

            col = (rectRefer.left - g_ptTopLeft.x) / WIDTH;
            rectTheTopOfTheColumn = GetTheTopRectOfTheColumn(region, col);
            if ((g_ptBottomRight.y == rectTmp.bottom) 
                || (g_ptBottomRight.y - rectTmp.bottom == WIDTH / 2)
                || (rectTheTopOfTheColumn.top == rectTmp.bottom)
                || (rectTheTopOfTheColumn.top - rectTmp.bottom == WIDTH / 2))
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_UP, WIDTH);
                m_stTetrisElement.rotation = targetRotation;
                ClearTetrisData(); //清空原有数据
                SetRLData(rectRefer, m_stTetrisElement.rotation);
                return;
            }

            m_stTetrisElement.rotation = targetRotation;
            ClearTetrisData(); //清空原有数据
            SetRLData(rectRefer, m_stTetrisElement.rotation);
        } break;
    // * *      * * *
    // *    ->      *
    // *
    case ROTATION_3: 
        {
            targetRotation = ROTATION_4;
            if (ROTATION_2 == rotation)
            {
                targetRotation = ROTATION_2;
            }

            rectRefer = m_stTetrisElement.rects[1];
            rectTmp = MoveRect(m_stTetrisElement.rects[0], DIRECTION_LEFT, WIDTH);

            if ((g_ptTopLeft.x > rectTmp.left) || IsRectOccupied(region, rectTmp))
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_RIGHT, WIDTH);
                m_stTetrisElement.rotation = targetRotation;
                ClearTetrisData(); //清空原有数据
                SetRLData(rectRefer, m_stTetrisElement.rotation);
                return;
            }

            m_stTetrisElement.rotation = targetRotation;
            ClearTetrisData(); //清空原有数据
            SetRLData(rectRefer, m_stTetrisElement.rotation);
        } break;
    // * * *        *
    //     *  ->    *
    //            * *
    case ROTATION_4: 
        {
            targetRotation = ROTATION_1;
            if (ROTATION_2 == rotation)
            {
                targetRotation = ROTATION_3;
            }

            rectRefer = m_stTetrisElement.rects[0];
            rectRefer = MoveRect(rectRefer, DIRECTION_UP, WIDTH);
            rectTmp = rectRefer;

            if (g_ptTopLeft.y > rectTmp.bottom)
            {
                rectRefer = MoveRect(rectRefer, DIRECTION_DOWN, WIDTH);
                m_stTetrisElement.rotation = targetRotation;
                ClearTetrisData(); //清空原有数据
                SetRLData(rectRefer, m_stTetrisElement.rotation);
                return;
            }

            m_stTetrisElement.rotation = targetRotation;
            ClearTetrisData(); //清空原有数据
            SetRLData(rectRefer, m_stTetrisElement.rotation);
        } break;
    default: break;
    }
}

CRect TetrisBase::GetTheTopRectOfTheColumn(pGameRegion region, const int col)
{
    CRect rectTmp;

    rectTmp.SetRectEmpty();

    //从最顶部开始判断 
    for (int i = 0; i < LINE; ++i)
    {
        if (region[col][i].bIsFilled)
        {
            rectTmp = region[col][i].rectData;
            break;
        }
    }

    return rectTmp;
}

void TetrisBase::ClearTetrisData()
{
    for (int i = 0; i < RECT_COUNT; ++i)
    {
        m_stTetrisElement.rects[i].SetRectEmpty();
    }
}

bool TetrisBase::IsRectOccupied(pGameRegion region, const CRect& rect)
{
    CPoint ptTmp;

    ptTmp.x = (rect.TopLeft().x - g_ptTopLeft.x) / WIDTH;
    ptTmp.y = (rect.TopLeft().y - g_ptTopLeft.y) / WIDTH;

    if (region[ptTmp.x][ptTmp.y].bIsFilled)
    {
        return true;
    }

    return false;
}

void TetrisBase::SetTetrisElement(const TetrisElement& te)
{
    m_stTetrisElement = te;
}








