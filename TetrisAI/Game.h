#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <sstream>
#include <fstream>
#include "Tetris.h"
using namespace std;
using namespace sf;

#define Window_width		1350
#define Window_height		1000

#define B_START_CORNER_X	621
#define B_START_CORNER_Y	763
#define B_HOLD_CORNER_X		621
#define B_HOLD_CORNER_Y		822
#define B_LEFT_CORNER_X		70
#define B_LEFT_CORNER_Y		460
#define B_RIGHT_CORNER_X	1295
#define B_RIGHT_CORNER_Y	460

#define P1_SCORE_CORNER_X	366
#define P1_SCORE_CORNER_Y	110
#define P2_SCORE_CORNER_X	1195
#define P2_SCORE_CORNER_Y	110
#define INFO_CORNER_X		565
#define INFO_CORNER_Y		442

typedef enum ButtonState
{
	Continue_Dark,	// ������
	Continue_Light, // ������
	Hold_Dark,		// ��ͣ��
	Hold_Light,		// ��ͣ��
	Close_Dark,		// ������
	Close_Light,	// ������
	Start_Dark,		// ��ʼ��
	Start_Light,	// ��ʼ��
};

class Game
{
public:
	Game();
	~Game();


	RenderWindow window;
	Tetris player1, player2;
	bool gameOver, gameQuit;
	Clock clock;

	int imgBGno, imgSkinNo;
	Texture tTiles, tBackground, tButtons, tSwitcher, tFrame,
		tCover, tScore, tGameOver;
	Sprite sTiles, sBackground, sButtons, sSwitcher, sFrame,
		sCover, sScore, sGameOver;
	Text text;
	Font font;

	//��ť����
	bool isGameBegin, isGameHold;
	int imgSetNo;
	IntRect ButtonRectStart, ButtonRectHold, ButtonRectLeft, ButtonRectRight;
	int ButtonState_Start, ButtonState_Hold;

	void GameRun();

	void GameInitial();
	void LoadMediaData();

	void DrawButton();
	void DrawResults();
	void DrawText();
	void GameDraw();

	void GameInput();

	void GameLogic();

	int Paritition1(scoreBlock A[], int low, int high);
	void QuickSort(scoreBlock A[], int low, int high);
};