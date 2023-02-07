#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
using namespace std;
using namespace sf;

#define GRIDSIZE			35
#define STAGE_WIDTH			10
#define STAGE_HEIGHT		20

#define P1_STAGE_CORNER_X	156
#define P1_STAGE_CORNER_Y	174
#define P2_STAGE_CORNER_X	844
#define P2_STAGE_CORNER_Y	174

#define P1_NEXT_CORNER_X	587
#define P1_NEXT_CORNER_Y	165
#define P2_NEXT_CORNER_X	702
#define P2_NEXT_CORNER_Y	165

#define HOLD_CORNER_X		640
#define HOLD_CORNER_Y		375

#define DELAYVALUE			0.5

// 单组权值重复实验次数
#define ITERLIM				1

// 种群个体数量
#define POPULATION			100

// 变异概率
#define MUTATEPROB			0.05

// 总共选育迭代次数
#define BREEDLIM			50



typedef enum PLAYROLE
{
	roleNONE,
	rolePLAYER1,
	rolePLAYER2
};

typedef enum gridShape
{
	shapeI,
	shapeS,
	shapeZ,
	shapeT,
	shapeL,
	shapej,
	shapeO
};

struct scoreBlock
{
	double score;
	int index;
};

class Tetris
{
public:
	Tetris();
	~Tetris();


	RenderWindow* window;

	// 绘制区域
	Vector2i mCornPoint,
		nextSquareCornPoint,
		holdSquareCornPoint;

	// hold区域
	static int holdColorNum, holdShapeNum;
	static Vector2i holdSquare[4];	// hold区域的方块

	int gridsize;
	int imgBGno, imgSkinNo;
	// 方块
	Texture* tTiles;
	Sprite sTiles;
	Texture tBackground, tButtons, tNum, tTimer, tCounter, tGameover;
	Sprite sBackground, sButtons, sNum, sTimer, sCounter, sGameover;

	int Field[STAGE_HEIGHT][STAGE_WIDTH] = { 0 };

	Vector2i currentSquare[4],	// 当前四个方块
		nextSquare[4],			// next区域的方块
		shadowSquare[4],		// 阴影
		tempSquare[4];			// 临时保存四个方块

	// 形状
	int Figures[7][4] =
	{
		1,3,5,7,	// I
		2,4,5,7,	// S
		3,4,5,6,	// Z
		3,4,5,7,	// T
		2,3,5,7,	// L
		3,5,6,7,	// J
		2,3,4,5,	// O
	};

	int dx;
	bool rotate, hold, hardDrop, newShapeFlag, animationFlag, gameover, dropNoSlowdown, shapeChanged;
	int colorNum, nextColorNum, tempColorNum;
	int currentShapeNum, nextShapeNum, tempShapeNum;

	int b7array[7], b7Int;
	int Bag7();

	int animationRow[4];
	float animationCtrlValue;

	float timer, delay;
	int role;
	unsigned long int score;


	// 方格纹理
	void Initial(Texture* tex);
	void Input(Event event);
	void Logic();
	void basicLogic();
	void Draw(RenderWindow* window);

	// 方块碰撞，撞为0，没撞为1；
	bool hitTest(Vector2i* obj);
	void isWin();
	// 遍历检查、消行
	void checkLine();
	void clearLine();

	// hold区交换
	int holdFunc();
	// 重新生成方块
	void newShapeFunc();
	void animationFunc(int i);

	void xMove();
	void yMove();
	void Rotate();
	// 速降
	void hardDropFunc();

	// 把currentSquare恢复到backup
	void backupRecovery();

	// 下落缓冲
	void slowLanding();
	// 画影子
	void updateShadow();



	/*AI算法部分*/

	// 获取特征值
	int getLandingHeight();
	int getRowEliminated();
	int getRowTransitions();
	int getColumnTransitions();
	int getNumofHoles();
	int getWellSums();

	int testDrop(int col, int rotate);
	int testRotate(Vector2i* obj);
	int testHold(int col, int rotate);

	// ai算法
	void AI();
	// ai移动
	void AImove(int dstCol, int dstRotate);
	// 决策
	int bestCol, bestRotate;
	// 移动变量
	bool AImovedone, AIrotatedone;
	int drotate;
	// ai算法用field
	int testField[STAGE_HEIGHT][STAGE_WIDTH] = { 0 };

	// 死亡记录
	int overTime;
	int scoreRec[ITERLIM];
	int highestScore;
	int lowestScore;
	int skipScoreIndex[2]; // 0位最小值 1位最大值，抛弃
	int thisTimeRowEli;
	// 当前特征值总和
	long double nowCharc;
	int chooseCurrentOrHold;

	long double HeightWeight;
	long double RowEliWeight;
	long double RowTransWeight;
	long double ColTransWeight;
	long double HolesWeight;
	long double WellWeight;

	long double HeightWeightPool[POPULATION];
	long double RowEliWeightPool[POPULATION];
	long double RowTransWeightPool[POPULATION];
	long double ColTransWeightPool[POPULATION];
	long double HolesWeightPool[POPULATION];
	long double WellWeightPool[POPULATION];

	// GA遗传算法
	void generateInitialWeights();

	int nowDataIndex;
	struct scoreBlock scoreOrderIndex[POPULATION];
	bool breedDone;

	// 读取种群
	void readCrowd();
	// 交叉杂交
	void crossBreed();
	// 概率变异
	void mutate(int i);
	int breedTime;



	// 是否开启消行动画
	bool noClrAnimation = true;

	// 是否在cmd输出权值列表
	bool ifCoutWeights = false;

	// 是否开启AI
	bool ifAI = false;
	
	// AI是否速降
	bool ifAIHardDrop = true;
};