#ifndef ITETRISBASE_H
#define ITETRISBASE_H

#include <map>
using namespace std;

//#define _DEBUG

//俄罗斯方块的7种形状
enum SHAPE
{
    SHAPE_INVALID = 0,
    //正方形
    // * *
    // * *
    SHAPE_SQUARE = 1,

    //长条形
    // * * * *
    SHAPE_STRIP = 2,

    //T形
    // * * *
    //   *
    SHAPE_T = 3,

    //阶梯形
    // *
    // * *
    //   *
    SHAPE_STEP = 4,
    
    //反阶梯形
    //   *
    // * *
    // *
    SHAPE_RSTEP = 5,

    //L形
    // *
    // *
    // * *
    SHAPE_L = 6,

    //反L形
    //   *
    //   *
    // * *
    SHAPE_RL = 7
};

//形状的旋转状态，最多4种状态，按顺时针旋转
enum ROTATION
{
    ROTATION_INVALID = 0,
    //ROTATION_1包括
    // * *             * * *   *       *    *       *
    // * *   * * * *     *     * *   * *    *       *
    //                           *   *      * *   * *
    ROTATION_1 = 1,

    //ROTATION_2包括
    // *     *       * *   * *      * * *    *
    // *   * *     * *       * *    *        * * *
    // *     *
    // *
    ROTATION_2 = 2,

    //ROTATION_3包括
    //   *     * *     * *
    // * * *     *     *
    //           *     *
    ROTATION_3 = 3,

    //ROTATION_4包括
    // *          *   * * *
    // * *    * * *       *
    // *
    ROTATION_4 = 4
};

//矩形移动的方向
enum DIRECTION
{
    DIRECTION_INVALID = 0,
    DIRECTION_LEFT = 1,
    DIRECTION_UP = 2,
    DIRECTION_RIGHT = 3,
    DIRECTION_DOWN = 4
};


const int WIDTH = 20; //组成俄罗斯方块的小矩形的宽
const int GAP = 0; //每个小矩形的间隔
const CPoint g_ptCenterOfFirstLine(150, 15); //第一行中心点
const CPoint g_ptTopLeft(10, 15); //游戏区域边框左上角点
const CPoint g_ptBottomRight(310, 415); //游戏区域边框右下角点
const int RECT_COUNT = 4; //每种俄罗斯方块形状由4个小矩形组成

//7种形状的属性集
struct TetrisElement
{
    SHAPE shape; //形状标识
    ROTATION rotation; //所处的旋转状态
    CRect rects[4]; //每种形状4个小正方形的数据
};

typedef void (*pfSetRectData)(const CRect& rectRefer, ROTATION r);

struct GameRegion;
typedef GameRegion (*pGameRegion)[20];

class TetrisBase
{
public:
    TetrisBase();
    ~TetrisBase();

public:
    //各种初始化
    void Init();

    //产生下一种形状，不指定参数时默认情况随机生成形状
    void GenerateNext(SHAPE s = SHAPE_INVALID);

    TetrisElement GetTetrisElement();

    //使俄罗斯方块移动，默认向下移，默认一次移动半个宽度
    bool Move(DIRECTION direction = DIRECTION_DOWN, int interval = WIDTH / 2);

    //将一个矩形往指定方向移动
    static CRect MoveRect(const CRect& rect, DIRECTION direction, int interval);
    //将一个点往指定方向移动
    static void MovePoint(CPoint& point, DIRECTION direction, int interval);

    //方块的旋转，传入游戏区域信息,ROTATION_1表示顺时针，ROTATION_2表示逆时针
    void Rotate(pGameRegion region, ROTATION rotation = ROTATION_1);

    void SetTetrisElement(const TetrisElement& te);

    static CPoint GetMostLeftPoint(const TetrisElement& te);
    static CPoint GetMostRightPoint(const TetrisElement& te);
    static CPoint GetMostDownPoint(const TetrisElement& te);

    static CRect GetMostLeftRect(const TetrisElement& te);
    static CRect GetMostRightRect(const TetrisElement& te);
    static CRect GetMostDownRect(const TetrisElement& te);

private:
    void SetRectsData(SHAPE s, ROTATION r);

    //设定7种形状坐标数据的一组函数
    static void SetSquareData(const CRect& rectRefer, ROTATION r);
    static void SetStripData(const CRect& rectRefer, ROTATION r);
    static void SetTData(const CRect& rectRefer, ROTATION r);
    static void SetStepData(const CRect& rectRefer, ROTATION r);
    static void SetRStepData(const CRect& rectRefer, ROTATION r);
    static void SetLData(const CRect& rectRefer, ROTATION r);
    static void SetRLData(const CRect& rectRefer, ROTATION r);

    //7种形状方块的旋转的一组函数
    void RotateSquare(pGameRegion region, ROTATION rotation = ROTATION_1);
    void RotateStrip(pGameRegion region, ROTATION rotation = ROTATION_1);
    void RotateT(pGameRegion region, ROTATION rotation = ROTATION_1);
    void RotateStep(pGameRegion region, ROTATION rotation = ROTATION_1);
    void RotateRStep(pGameRegion region, ROTATION rotation = ROTATION_1);
    void RotateL(pGameRegion region, ROTATION rotation = ROTATION_1);
    void RotateRL(pGameRegion region, ROTATION rotation = ROTATION_1);

    void ClearTetrisData();

    //获得某一列堆积的最高点的矩形
    CRect GetTheTopRectOfTheColumn(pGameRegion region, const int col);
    //判断任意一个最小单位矩形是否已在游戏区域中被占有
    bool IsRectOccupied(pGameRegion region, const CRect& rect);

private:
    static int m_iWidth; //每种形状由4个小正方形拼成，其中小正方形的宽度
    static int m_iGap; //每种形状4个小正方形之间的间隔
    static CPoint m_ptUpLeft; //每种形状中最靠上方，同时最靠左方的小正方形的左上角点，或不一定是形状中的点，只是用于确定形状的4个小矩形的坐标的参照点

    static TetrisElement m_stTetrisElement;
    map<SHAPE, pfSetRectData> m_mapRectDataHandler;

    pfSetRectData m_pfSetRectData;
};












#endif
