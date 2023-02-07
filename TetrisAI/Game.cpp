#include "Game.h"

int FPS = 15;

Game::Game()
{
	srand(time(NULL));
	imgBGno = 2;
	imgSkinNo = 3;
	imgSetNo = 1;
	window.create(VideoMode(Window_width, Window_height), L"AI俄罗斯方块 by PJJ");
	window.setFramerateLimit(FPS);
	window.setKeyRepeatEnabled(0);	//按键按下只响应一次
}

Game::~Game() {}


void Game::GameRun()
{
	while (!gameQuit && window.isOpen())
	{
		GameInitial();

		while (!gameOver && window.isOpen())
		{
			GameInput();

			if (isGameBegin == true)
				GameLogic();
			else
				GameInitial();

			GameDraw();
		}
	}
}

void Game::GameInitial()
{
	LoadMediaData();

	isGameBegin = false;
	isGameHold = false;
	ButtonState_Start = Start_Dark;
	ButtonState_Hold = Hold_Dark;

	player1.role = rolePLAYER1;
	player2.role = rolePLAYER2;
	player1.Initial(&tTiles);
	player2.Initial(&tTiles);
}

void Game::LoadMediaData()
{
	stringstream ss;
	ss << "data/images/bg" << imgSetNo << ".jpg";

	if (!tBackground.loadFromFile(ss.str()))
		cout << "BKimg未找到" << endl;

	ss.str("");
	ss << "data/images/tiles" << imgSetNo << ".jpg";
	if (!tTiles.loadFromFile(ss.str()))
		cout << "tiles.jpg未找到" << endl;

	if (!tFrame.loadFromFile("data/images/frame.png"))
		cout << "frame.png未找到" << endl;
	if (!tCover.loadFromFile("data/images/cover.png"))
		cout << "cover.png未找到" << endl;
	if (!tGameOver.loadFromFile("data/images/end.png"))
		cout << "end.png未找到" << endl;
	if (!tButtons.loadFromFile("data/images/button.png"))
		cout << "button.png 没有找到" << endl;
	if (!tSwitcher.loadFromFile("data/images/bgSwitch.png"))
		cout << "bgSwap.png 没有找到" << endl;

	sBackground.setTexture(tBackground);
	sFrame.setTexture(tFrame);
	sCover.setTexture(tCover);
	sGameOver.setTexture(tGameOver);
	sButtons.setTexture(tButtons);
	sSwitcher.setTexture(tSwitcher);
	sSwitcher.setOrigin(sSwitcher.getLocalBounds().width / 2.0, sSwitcher.getLocalBounds().height / 2.0);

	sGameOver.setScale(0.6, 0.6);

	if (!font.loadFromFile("data/Fonts/simsun.ttc"))
		std::cout << "字体没有找到" << std::endl;
	text.setFont(font);
}

void Game::GameInput()
{
	Event event;
	while (window.pollEvent(event))
	{
		if (event.type == Event::Closed)
		{
			window.close();
			gameQuit = 1;
		}

		if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::Escape && !isGameBegin)
		{
			window.close();
			gameQuit = true;
		}
		if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::F1)
		{
			if (player1.ifAI)
				player1.ifAI = false;
			else
				player1.ifAI = true;

			if (player2.ifAI)
				player2.ifAI = false;
			else
				player2.ifAI = true;
		}
		if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::F2)
		{
			if (player1.noClrAnimation)
				player1.noClrAnimation = false;
			else
				player1.noClrAnimation = true;

			if (player2.noClrAnimation)
				player2.noClrAnimation = false;
			else
				player2.noClrAnimation = true;
		}
		if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::F3)
		{
			if (player1.ifAIHardDrop)
				player1.ifAIHardDrop = false;
			else
				player1.ifAIHardDrop = true;

			if (player2.ifAIHardDrop)
				player2.ifAIHardDrop = false;
			else
				player2.ifAIHardDrop = true;
		}
		if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::J)
		{
			if (FPS <= 500)
			{
				FPS += 15;
				window.setFramerateLimit(FPS);
			}
		}
		if (event.type == Event::EventType::KeyReleased && event.key.code == Keyboard::K)
		{
			if (FPS > 0)
			{
				FPS -= 15;
				window.setFramerateLimit(FPS);
			}
		}
		if (event.type == Event::MouseButtonReleased && event.mouseButton.button == Mouse::Left)
		{
			if (ButtonRectStart.contains(event.mouseButton.x, event.mouseButton.y))
			{
				if (isGameBegin == false)
				{
					isGameBegin = true;
					// 初始化动画的状态
					player1.animationFlag = false;
					player2.animationFlag = false;
					ButtonState_Start = Close_Light;
				}
				else
				{
					isGameBegin = false;
					ButtonState_Start = Start_Light;

					player1.score = 0;
					player2.score = 0;
					player1.gameover = false;
					player2.gameover = false;
				}
			}

			if (ButtonRectHold.contains(event.mouseButton.x, event.mouseButton.y))
			{
				if (isGameHold == false)
				{
					isGameHold = true;
					ButtonState_Hold = Continue_Light;
				}
				else
				{
					isGameHold = false;
					ButtonState_Hold = Hold_Light;
				}
			}

			if (ButtonRectLeft.contains(event.mouseButton.x, event.mouseButton.y))
			{
				imgSetNo--;
				if (imgSetNo < 1)
					imgSetNo = 4;
				LoadMediaData();
			}

			if (ButtonRectRight.contains(event.mouseButton.x, event.mouseButton.y))
			{
				imgSetNo++;
				if (imgSetNo > 4)
					imgSetNo = 1;
				LoadMediaData();
			}
		}
		if (event.type == Event::MouseMoved)
		{
			if (ButtonRectStart.contains(event.mouseMove.x, event.mouseMove.y))
			{
				if (isGameBegin == false)
					ButtonState_Start = Start_Light;
				else
					ButtonState_Start = Close_Light;
			}
			else
			{
				if (isGameBegin == false)
					ButtonState_Start = Start_Dark;
				else
					ButtonState_Start = Close_Dark;
			}


			if (ButtonRectHold.contains(event.mouseMove.x, event.mouseMove.y))
			{
				if (isGameHold == false)
					ButtonState_Hold = Hold_Light;
				else
					ButtonState_Hold = Continue_Light;
			}
			else
			{
				if (isGameHold == false)
					ButtonState_Hold = Hold_Dark;
				else
					ButtonState_Hold = Continue_Dark;
			}

		}

		player1.Input(event);
		player2.Input(event);
	}
}

void Game::GameLogic()
{
	if (isGameHold == 1)
		return;
	float time = clock.getElapsedTime().asSeconds();
	clock.restart();
	player1.timer += time;
	player2.timer += time;

	if (player1.gameover == false)
	{
		// 游戏开始时进行 旧种群杂交
		//if (player1.breedDone == false)
		//{
		//	player1.readCrowd();
		//	player1.crossBreed();
		//	player1.breedDone = true;
		//}
		player1.Logic();
	}
	/*
	// 当前个体当前重复试验次数
	if (player1.gameover)
	{
		// 死亡后记录分数
		if (player1.overTime == 0)
		{
			player1.highestScore = player1.score;
			player1.lowestScore = player1.score;
			player1.skipScoreIndex[0] = player1.overTime;
			player1.skipScoreIndex[1] = player1.overTime;
		}
		else
		{
			if (player1.score > player1.highestScore)
			{
				player1.highestScore = player1.score;
				player1.skipScoreIndex[1] = player1.overTime;
			}
			if (player1.score < player1.lowestScore)
			{
				player1.lowestScore = player1.score;
				player1.skipScoreIndex[0] = player1.overTime;
			}
		}
		player1.scoreRec[player1.overTime] = player1.score;
		player1.overTime++;


		// 结束
		isGameBegin = false;
		player1.score = 0;
		player1.gameover = false;


		// 重新开始
		GameInitial();
		isGameBegin = true;
		player1.animationFlag = false;
	}
	// 当前个体结束重复实验
	if (player1.overTime == ITERLIM)
	{
		// 游戏停止
		isGameBegin = false;
		player1.score = 0;
		player1.gameover = false;

		// 记录个体结果
		
		//double Ave = 0;
		//for (int i = 0; i < ITERLIM; i++)
		//{
		//	if (i != player1.skipScoreIndex[0] && i != player1.skipScoreIndex[1])
		//		Ave += player1.scoreRec[i];
		//}
		//Ave = Ave / (ITERLIM - 2);
		//Ave = Ave / (ITERLIM - 2);
		//player1.scoreOrderIndex[player1.nowDataIndex].score = Ave;
		player1.scoreOrderIndex[player1.nowDataIndex].score = player1.highestScore;
		player1.scoreOrderIndex[player1.nowDataIndex].index = player1.nowDataIndex;


		// 赋值下一组权重
		player1.nowDataIndex++;
		player1.HeightWeight = player1.HeightWeightPool[player1.nowDataIndex];
		player1.RowEliWeight = player1.RowEliWeightPool[player1.nowDataIndex];
		player1.RowTransWeight = player1.RowTransWeightPool[player1.nowDataIndex];
		player1.ColTransWeight = player1.ColTransWeightPool[player1.nowDataIndex];
		player1.HolesWeight = player1.HolesWeightPool[player1.nowDataIndex];
		player1.WellWeight = player1.WellWeightPool[player1.nowDataIndex];


		// 下一个体重新开始
		player1.overTime = 0;
		GameInitial();
		isGameBegin = true;
		player1.animationFlag = false;
	}
	// 种群全部结束
	if (player1.nowDataIndex == POPULATION)
	{
		// 游戏停止
		isGameBegin = false;
		player1.score = 0;
		player1.gameover = false;
		player1.nowDataIndex = 0;

		QuickSort(player1.scoreOrderIndex, 0, POPULATION - 1);
		
		// 清空popu和score
		fstream clr;
		clr.open("data/rec/popu.txt", ios::out | ios::trunc);
		clr << "";
		clr.close();
		clr.open("data/rec/score.txt", ios::out | ios::trunc);
		clr << "";
		clr.close();

		// 导出种群结果
		fstream score;
		score.open("data/rec/score.txt", ios::out | ios::app);
		fstream file;
		file.open("data/rec/popu.txt", ios::out | ios::app);
		
		for (int i = 0; i < POPULATION; i++)
		{
			score << player1.scoreOrderIndex[i].score << endl;
			file << player1.HeightWeightPool[player1.scoreOrderIndex[i].index] << ","
				<< player1.RowEliWeightPool[player1.scoreOrderIndex[i].index] << ","
				<< player1.RowTransWeightPool[player1.scoreOrderIndex[i].index] << ","
				<< player1.ColTransWeightPool[player1.scoreOrderIndex[i].index] << ","
				<< player1.HolesWeightPool[player1.scoreOrderIndex[i].index] << ","
				<< player1.WellWeightPool[player1.scoreOrderIndex[i].index] << "," << endl;
		}
		score << endl;
		score.close();
		file << endl;
		file.close();

		player1.breedTime++;
		if (player1.breedTime < BREEDLIM)
		{
			// 下一次选育重新开始
			player1.breedDone = false;
			player1.overTime = 0;
			player1.nowDataIndex = 0;
			GameInitial();
			isGameBegin = true;
			player1.animationFlag = false;
		}
	}
	*/

	if (player2.gameover == false)
	{
		player2.Logic();
	}
}

void Game::GameDraw()
{
	window.clear();

	window.draw(sBackground);
	window.draw(sFrame);
	player1.Draw(&window);
	player2.Draw(&window);

	sCover.setPosition(P1_STAGE_CORNER_X, P1_STAGE_CORNER_Y);
	window.draw(sCover);
	sCover.setPosition(P2_STAGE_CORNER_X, P2_STAGE_CORNER_Y);
	window.draw(sCover);

	DrawButton();

	DrawText();

	DrawResults();

	window.display();
}

void Game::DrawButton()
{
	int ButtonWidth, ButtonHeight;
	ButtonWidth = 110;
	ButtonHeight = sButtons.getLocalBounds().height;

	//ButtonRectStart
	sButtons.setTextureRect(IntRect(ButtonState_Start * ButtonWidth, 0, ButtonWidth, ButtonHeight));//读取按钮的纹理区域
	sButtons.setPosition(B_START_CORNER_X, B_START_CORNER_Y);
	ButtonRectStart.left = B_START_CORNER_X;
	ButtonRectStart.top = B_START_CORNER_Y;
	ButtonRectStart.width = ButtonWidth;
	ButtonRectStart.height = ButtonHeight;
	window.draw(sButtons);

	//ButtonRectHold
	sButtons.setTextureRect(IntRect(ButtonState_Hold * ButtonWidth, 0, ButtonWidth, ButtonHeight));//读取按钮的纹理区域
	sButtons.setPosition(B_HOLD_CORNER_X, B_HOLD_CORNER_Y);
	ButtonRectHold.left = B_HOLD_CORNER_X;
	ButtonRectHold.top = B_HOLD_CORNER_Y;
	ButtonRectHold.width = ButtonWidth;
	ButtonRectHold.height = ButtonHeight;
	window.draw(sButtons);

	//背景素材切换
	ButtonWidth = sSwitcher.getLocalBounds().width;
	ButtonHeight = sSwitcher.getLocalBounds().height;

	//ButtonRectLeft
	sSwitcher.setPosition(B_LEFT_CORNER_X, B_LEFT_CORNER_Y);
	ButtonRectLeft.left = B_LEFT_CORNER_X - ButtonWidth / 2;
	ButtonRectLeft.top = B_LEFT_CORNER_Y - ButtonHeight / 2;
	ButtonRectLeft.width = ButtonWidth;
	ButtonRectLeft.height = ButtonHeight;
	window.draw(sSwitcher);

	//ButtonRectRight
	sSwitcher.setPosition(B_RIGHT_CORNER_X, B_RIGHT_CORNER_Y);
	ButtonRectRight.left = B_RIGHT_CORNER_X - ButtonWidth / 2;
	ButtonRectRight.top = B_RIGHT_CORNER_Y - ButtonHeight / 2;
	ButtonRectRight.width = ButtonWidth;
	ButtonRectRight.height = ButtonHeight;
	sSwitcher.rotate(180);//只有一个箭头素材，需代码生成另一个
	window.draw(sSwitcher);
	sSwitcher.rotate(180);//还原角度
}

void Game::DrawResults()
{
	int ButtonWidth, ButtonHeight;
	ButtonWidth = 250;
	ButtonHeight = sGameOver.getLocalBounds().height;

	if (player1.gameover || player2.gameover)
	{
		sGameOver.setTextureRect(IntRect(player1.gameover * ButtonWidth, 0, ButtonWidth, ButtonHeight));
		sGameOver.setPosition(P1_STAGE_CORNER_X + GRIDSIZE * 3, GRIDSIZE / 2);
		window.draw(sGameOver);

		sGameOver.setTextureRect(IntRect(player2.gameover * ButtonWidth, 0, ButtonWidth, ButtonHeight));
		sGameOver.setPosition(P2_STAGE_CORNER_X + GRIDSIZE * 3, GRIDSIZE / 2);
		window.draw(sGameOver);
	}
}

void Game::DrawText()
{
	int initialX, initialY;
	int CharacterSize = 48;
	text.setCharacterSize(CharacterSize - 10);
	text.setFillColor(Color(255, 0, 0, 255));
	text.setStyle(Text::Bold);
	text.setPosition(P1_SCORE_CORNER_X, P1_SCORE_CORNER_Y + 10);
	std::stringstream ss;

	// 遗传算法进度
	//text.setPosition(200,0);
	//ss << player1.overTime + 1 << " / " << ITERLIM;
	//text.setString(ss.str()); window.draw(text);
	//ss.str("");
	//text.setPosition(200,50);
	//ss << player1.nowDataIndex + 1 << " / " << POPULATION;
	//text.setString(ss.str()); window.draw(text);
	//ss.str("");
	//text.setPosition(400,0);
	//ss << player1.breedTime + 1 << " / " << BREEDLIM;
	//text.setString(ss.str()); window.draw(text);
	//ss.str("");

	// 显示FPS
	text.setCharacterSize(20);
	text.setPosition(10,10);
	ss << "fps: " << FPS;
	text.setString(ss.str()); window.draw(text);
	ss.str("");



	text.setCharacterSize(CharacterSize - 10);
	text.setPosition(P1_SCORE_CORNER_X, P1_SCORE_CORNER_Y + 10);
	ss << player1.score;
	text.setString(ss.str()); window.draw(text);

	text.setPosition(P2_SCORE_CORNER_X - CharacterSize * 3, P2_SCORE_CORNER_Y + 10);
	ss.str("");
	ss << player2.score;
	text.setString(ss.str()); window.draw(text);
	CharacterSize = 18;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setStyle(Text::Regular || Text::Italic);
	initialY = P1_STAGE_CORNER_Y + STAGE_HEIGHT * GRIDSIZE;

	/*
	CharacterSize = 24;
	text.setCharacterSize(CharacterSize);
	text.setFillColor(Color(255, 255, 255, 255));
	text.setStyle(Text::Bold);
	initialY = INFO_CORNER_Y;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"玩家 1:"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 方向键：WASD"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 速降： Space 键"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 交换： 左 Ctrl 键"); window.draw(text);
	initialY += CharacterSize * 1.5;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"玩家 2:"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 方向键：↑←↓→"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 速降： Enter 键"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L" 交换： 右 Ctrl 键"); window.draw(text);
	initialY += 2 * CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"自带底部缓冲功能"); window.draw(text);
	initialY += CharacterSize;
	text.setPosition(INFO_CORNER_X, initialY);
	text.setString(L"退出： Esc 键"); window.draw(text);
	*/
}

//严蔚敏《数据结构》标准分割函数
int Game::Paritition1(scoreBlock A[], int low, int high)
{
	scoreBlock pivot = A[low];
	while (low < high)
	{
		while (low < high && A[high].score >= pivot.score) 
		{
			--high;
		}
		A[low] = A[high];
		while (low < high && A[low].score <= pivot.score)
		{
			++low;
		}
		A[high] = A[low];
	}
	A[low] = pivot;
	return low;
}

//快排母函数
void Game::QuickSort(scoreBlock A[], int low, int high) 
{
	if (low < high) 
	{
		int pivot = Paritition1(A, low, high);
		QuickSort(A, low, pivot - 1);
		QuickSort(A, pivot + 1, high);
	}
}