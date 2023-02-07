#include "Tetris.h"
using namespace sf;

int Tetris::holdColorNum = 0, Tetris::holdShapeNum = 0;
Vector2i Tetris::holdSquare[4] = { {0,0},{0,0} ,{0,0} ,{0,0} };


Tetris::Tetris()
{
	dx = 0;
	rotate = false;
	colorNum = 1;
	timer = 0;
	delay = DELAYVALUE;
	b7Int = 0;
	dropNoSlowdown = false;
	shapeChanged = false;
	AImovedone = false;
	AIrotatedone = false;
	drotate = 0;
	overTime = 0;
	nowDataIndex = 0;
	breedDone = false;
	breedTime = 0;

	// ����ֵ����
	HeightWeight = (-1.7010739008742570);
	RowEliWeight = 1.6243746982198954;
	RowTransWeight = (-1.3925500991886857);
	ColTransWeight = (-1.3796998007953167);
	HolesWeight = (-3.3242344013333316);
	WellWeight = (-0.57807568958848710);

	//generateInitialWeights();
}

Tetris::~Tetris() {}


void Tetris::Initial(Texture* tex)
{
	tTiles = tex;
	dx = 0;
	drotate = 0;
	rotate = 0;
	colorNum = 1;
	hardDrop = false;
	shapeChanged = false;
	AImovedone = false;
	AIrotatedone = false;
	timer = 0;
	delay = DELAYVALUE;
	hold = false;
	holdSquareCornPoint = { HOLD_CORNER_X,HOLD_CORNER_Y };
	animationFlag = true;
	animationCtrlValue = 1.0;

	if (role == rolePLAYER1)
	{
		mCornPoint = { P1_STAGE_CORNER_X, P1_STAGE_CORNER_Y };
		nextSquareCornPoint = { P1_NEXT_CORNER_X ,P1_NEXT_CORNER_Y };
	}
	if (role == rolePLAYER2)
	{
		mCornPoint = { P2_STAGE_CORNER_X,P2_STAGE_CORNER_Y };
		nextSquareCornPoint = { P2_NEXT_CORNER_X ,P2_NEXT_CORNER_Y };
	}

	sTiles.setTexture(*tTiles);

	colorNum = rand() % 7 + 1;
	currentShapeNum = rand() % 7;
	newShapeFunc();

	holdShapeNum = -1;

	memset(b7array, 0, sizeof(int));

	for (int i = 0; i < 4; i++)
	{
		currentSquare[i].x = Figures[currentShapeNum][i] % 2 + STAGE_WIDTH / 2;
		currentSquare[i].y = Figures[currentShapeNum][i] / 2;
		nextSquare[i].x = Figures[nextShapeNum][i] % 2;
		nextSquare[i].y = Figures[nextShapeNum][i] / 2;

		animationRow[i] = -1;// -1Ϊ�쳣ֵ
	}

	for (int i = 0; i < STAGE_HEIGHT; i++)
	{
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			Field[i][j] = 0;
		}
	}
}

void Tetris::Logic()
{
	if (!animationFlag)
		basicLogic();
	// �·�����������
	if (newShapeFlag)
	{
		// ���ж��Ƿ��ж������ţ��� new �µķ���
		if (animationFlag == false)
		{
			checkLine();
			isWin();
			if (animationFlag == false)
			{
				newShapeFunc();
			}
		}
		else
		{
			// �������ж���
			if (noClrAnimation)
			{
				animationFlag = false;
				for (int i = 0; i < 4; i++)
					animationRow[i] = -1;
				clearLine();
				newShapeFunc();
			}
			// �����ж���
			else
			{
				animationCtrlValue -= 0.1;
				if (animationCtrlValue < 0)
				{
					animationFlag = false;
					animationCtrlValue = 1.0;
					for (int i = 0; i < 4; i++)
						animationRow[i] = -1;
					clearLine();
					newShapeFunc();
				}
			}
		}
	}
}

void Tetris::basicLogic()
{
	if (ifAI)
	{
		if (shapeChanged)
		{
			AI();
			shapeChanged = false;
		}

		AImove(bestCol, bestRotate);
	}
	
	// hold
	if (hold)
	{
		holdFunc();
		hold = 0;
	}

	// ˮƽ
	xMove();

	// ����
	if (timer > delay)
	{
		yMove();
		timer = 0;
	}

	// ��ת
	if (rotate)
	{
		Rotate();
		rotate = 0;
	}

	updateShadow();

	// hardDrop���Ļ������
	if (!dropNoSlowdown)
		slowLanding();
	else
	{
		delay = 0;
		dropNoSlowdown = false;
	}

	if (hardDrop)
	{
		hardDropFunc();
		hardDrop = 0;
		dropNoSlowdown = true;
	}

	// �����ж�
	checkLine();
}

bool Tetris::hitTest(Vector2i* obj)
{
	for (int i = 0; i < 4; i++)
	{
		if (obj[i].x < 0 
			|| obj[i].x >= STAGE_WIDTH
			|| obj[i].y >= STAGE_HEIGHT)
			return false;

		// ײ�Ϲ̶���
		if(obj[i].y >= 0)
			if (Field[obj[i].y][obj[i].x])
				return false;
	}
	return true;
}

void Tetris::Input(Event event)
{
	if (role == rolePLAYER1)
	{
		if (event.type == Event::KeyPressed)
		{
			if (event.key.code == Keyboard::W)
				if (currentShapeNum != shapeO)
					rotate = 1;
			if (event.key.code == Keyboard::A)
				dx = -1;
			if (event.key.code == Keyboard::D)
				dx = 1;
			if (event.key.code == Keyboard::S);
		}
		if (event.type == Event::KeyReleased)
		{
			if (event.key.code == Keyboard::LControl)
				hold = true;
			if (event.key.code == Keyboard::Space)
				hardDrop = true;
			if (event.key.code == Keyboard::A || event.key.code == Keyboard::D)
				dx = 0;
			if (event.key.code == Keyboard::S)
				hardDrop = true;
		}
	}
	else if (role == rolePLAYER2)
	{
		if (event.type == Event::KeyPressed)
		{
			if (event.key.code == Keyboard::Up)
				if (currentShapeNum != shapeO)
					rotate = 1;
			if (event.key.code == Keyboard::Left)
				dx = -1;
			if (event.key.code == Keyboard::Right)
				dx = 1;
			if (event.key.code == Keyboard::Down);
		}
		if (event.type == Event::KeyReleased)
		{
			if (event.key.code == Keyboard::RControl)
				hold = true;
			if (event.key.code == Keyboard::Enter)
				hardDrop = true;
			if (event.key.code == Keyboard::Left || event.key.code == Keyboard::Right)
				dx = 0;
			if (event.key.code == Keyboard::Down)
				hardDrop = true;
		}
	}
}

void Tetris::yMove()
{
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i].y++;
	}

	// ���ײ��
	if (!hitTest(currentSquare))
	{
		for (int i = 0; i < 4; i++)
			if(tempSquare[i].y >= 0 && tempSquare[i].y < STAGE_HEIGHT
				&& tempSquare[i].x >=0 && tempSquare[i].x < STAGE_WIDTH)
			Field[tempSquare[i].y][tempSquare[i].x] = colorNum;
		newShapeFlag = 1;
	}
}

void Tetris::Rotate()
{
	int originalHeight = currentSquare[0].y;
	// wall kick ����ƫ�Ʊ���
	for (int j = 0; j < 4; j++)
	{
		// ��ת���ĵ�
		Vector2i p = currentSquare[j];
		// ˳ʱ�� 90 ��
		for (int i = 0; i < 4; i++)
		{
			int x = currentSquare[i].y - p.y;
			int y = currentSquare[i].x - p.x;
			currentSquare[i].x = p.x - x;
			currentSquare[i].y = p.y + y;
		}
		// ���ûײ��
		if (hitTest(currentSquare))
		{
			// �������ĵĸ߶Ȳ�
			int detaY = currentSquare[j].y - originalHeight;
			// �Ը߶Ȳ��������
			if (detaY != 0)
				for (int i = 0; i < 4; i++)	
					currentSquare[i].y -= detaY;
			// ���ײ����
			if (!hitTest(currentSquare))
				backupRecovery();
			else
				break;
		}
		else
			backupRecovery();
	}
	rotate = 0;
}

int Tetris::testRotate(Vector2i* obj)
{
	// ��ת���ĵ�
	Vector2i p = obj[0];
	// ˳ʱ�� 90 ��
	for (int i = 0; i < 4; i++)
	{
		int x = obj[i].y - p.y;
		int y = obj[i].x - p.x;
		obj[i].x = p.x - x;
		obj[i].y = p.y + y;

		//if (obj[i].x < 0 || obj[i].x >= STAGE_WIDTH
		//	|| obj[i].y < 0 || obj[i].y >= STAGE_HEIGHT)
		//	return 0;
	}
	return 1;
}

void Tetris::checkLine()
{
	int k = STAGE_HEIGHT - 1;
	int yCount = 0;
	for (int i = STAGE_HEIGHT - 1; i > 0; i--)
	{
		int count = 0;
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if (Field[i][j])
				count++;
		}
		if (count < STAGE_WIDTH)
			k--;
		else
		{
			// ���Ŷ���������
			animationRow[yCount] = i;
			yCount++;
			animationFlag = true;
		}
	}
	switch (yCount)
	{
	case 1:
		score += 10;
		break;
	case 2:
		score += 30;
		break;
	case 3:
		score += 60;
		break;
	case 4:
		score += 100;
		break;
	}
}

void Tetris::Draw(RenderWindow* w)
{
	window = w;

	if (animationFlag == false)
	{
		// shadow ����
		for (int i = 0; i < 4; i++)
		{
			sTiles.setTextureRect(IntRect(colorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(shadowSquare[i].x * GRIDSIZE, shadowSquare[i].y * GRIDSIZE);
			sTiles.setColor(Color(50, 50, 50, 255));
			sTiles.move(mCornPoint.x, mCornPoint.y);
			window->draw(sTiles);
			sTiles.setColor(Color(255, 255, 255, 255));
		}
		// ���ƻ����
		for (int i = 0; i < 4; i++)
		{
			sTiles.setTextureRect(IntRect(colorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(currentSquare[i].x * GRIDSIZE, currentSquare[i].y * GRIDSIZE);
			sTiles.move(mCornPoint.x, mCornPoint.y);
			window->draw(sTiles);
		}
	}
	// ���ƹ̶�����
	for (int i = 0; i < STAGE_HEIGHT; i++)
		if (i == animationRow[0] || i == animationRow[1] || i == animationRow[2] || i == animationRow[3])
			animationFunc(i);
		else
			for (int j = 0; j < STAGE_WIDTH; j++)
			{
				if (Field[i][j] == 0)
					continue;
				sTiles.setTextureRect(IntRect(Field[i][j] * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
				sTiles.setPosition(j * GRIDSIZE, i * GRIDSIZE);
				sTiles.move(mCornPoint.x, mCornPoint.y); //offset
				window->draw(sTiles);
			}

	// next���� ����
	for (int i = 0; i < 4; i++)
	{
		sTiles.setTextureRect(IntRect(nextColorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
		sTiles.setPosition(nextSquare[i].x * GRIDSIZE, nextSquare[i].y * GRIDSIZE);
		sTiles.move(nextSquareCornPoint.x, nextSquareCornPoint.y);
		window->draw(sTiles);
	}

	// hold���� ����
	if (holdShapeNum > -1)
		for (int i = 0; i < 4; i++)
		{
			sTiles.setTextureRect(IntRect(holdColorNum * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
			sTiles.setPosition(holdSquare[i].x * GRIDSIZE, holdSquare[i].y * GRIDSIZE);
			sTiles.move(holdSquareCornPoint.x, holdSquareCornPoint.y);
			window->draw(sTiles);
		}
}

int Tetris::Bag7()
{
	int num;
	num = rand() % 7;
	for (int i = 0; i < b7Int; i++)
	{
		if (b7array[i] == num)
		{
			i = -1;
			num = rand() % 7;
		}
	}
	b7array[b7Int] = num;
	b7Int++;
	if (b7Int == 7)
	{
		b7Int = 0;
		for (int i = 0; i < 7; i++)
		{
			b7array[i] = 0;
		}
	}

	return	num;
}

int Tetris::holdFunc()
{
	Vector2i backUpSquare[4];
	tempColorNum = holdColorNum;
	tempShapeNum = holdShapeNum;

	holdColorNum = colorNum;
	holdShapeNum = currentShapeNum;

	for (int i = 0; i < 4; i++)
	{
		holdSquare[i].x = Figures[holdShapeNum][i] % 2;
		holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		tempSquare[i].x = Figures[tempShapeNum][i] % 2;
		tempSquare[i].y = Figures[tempShapeNum][i] / 2;
		backUpSquare[i] = currentSquare[i];
	}
	if (tempShapeNum < 0)
		newShapeFunc();
	// ��������
	else
	{
		colorNum = tempColorNum;
		currentShapeNum = tempShapeNum;
		// �� hold ���û������ķ���ͼ�Σ�����̨������ļ���
		int minCurrentX = currentSquare[0].x,
			minCurrentY = currentSquare[0].y,
			minTempX = tempSquare[0].x,
			minTempY = tempSquare[0].y;
		int dx, dy;
		for (int i = 1; i < 4; i++)
		{
			if (currentSquare[i].x < minCurrentX)
				minCurrentX = currentSquare[i].x;
			if (currentSquare[i].y < minCurrentY)
				minCurrentY = currentSquare[i].y;
			if (tempSquare[i].x < minTempX)
				minTempX = tempSquare[i].x;
			if (tempSquare[i].y < minTempY)
				minTempY = tempSquare[i].y;
		}
		dx = minCurrentX - minTempX;
		dy = minCurrentY - minTempY;
		for (int i = 0; i < 4; i++)
		{
			currentSquare[i].x = tempSquare[i].x + dx;
			currentSquare[i].y = tempSquare[i].y + dy;
			holdSquare[i].x = Figures[holdShapeNum][i] % 2;
			holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		}

	}
	// ���ײ����
	if (!hitTest(currentSquare))
	{
		colorNum = holdColorNum;
		holdColorNum = tempColorNum;
		holdShapeNum = tempShapeNum;
		for (int i = 0; i < 4; i++)
		{
			currentSquare[i] = backUpSquare[i];
			holdSquare[i].x = Figures[holdShapeNum][i] % 2;
			holdSquare[i].y = Figures[holdShapeNum][i] / 2;
		}
		return 0;
	}
	return 1;
}

void Tetris::xMove()
{
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i].x += dx;
	}
	// ײ�� �ָ�����
	if (!hitTest(currentSquare))
		backupRecovery();
}

void Tetris::backupRecovery()
{
	for (int i = 0; i < 4; i++)
		currentSquare[i] = tempSquare[i];
}

void Tetris::slowLanding()
{
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i].y += 1;
	}
	if (!hitTest(currentSquare))
		delay = DELAYVALUE * 2;
	else
		delay = DELAYVALUE;
	backupRecovery();
}

void Tetris::hardDropFunc()
{
	for (int i = 0; i < 4; i++)
		currentSquare[i] = shadowSquare[i];
}

void Tetris::updateShadow()
{
	for (int i = 0; i < 4; i++)
		shadowSquare[i] = currentSquare[i];
	for (int i = 0; i < STAGE_HEIGHT; i++)
	{
		for (int j = 0; j < 4; j++)
			currentSquare[j].y += 1;
		if (!hitTest(currentSquare)) {
			for (int k = 0; k < 4; k++)
				currentSquare[k].y -= 1;
			break;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		tempSquare[i] = currentSquare[i];
		currentSquare[i] = shadowSquare[i];
		shadowSquare[i] = tempSquare[i];
	}
}

void Tetris::clearLine()
{
	int k = STAGE_HEIGHT - 1;
	for (int i = STAGE_HEIGHT - 1; i > 0; i--)
	{
		int xCount = 0;
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if (Field[i][j])
				xCount++;
			Field[k][j] = Field[i][j];
		}
		if (xCount < STAGE_WIDTH)
			k--;
	}
}

void Tetris::newShapeFunc()
{
	// ȡ�¸�����ͼ��
	colorNum = nextColorNum;
	currentShapeNum = nextShapeNum;

	// �����¸�����ͼ��
	nextColorNum = 1 + rand() % 7;
	nextShapeNum = Bag7();

	for (int i = 0; i < 4; i++)
	{
		currentSquare[i] = nextSquare[i];
		currentSquare[i].x = currentSquare[i].x + STAGE_WIDTH / 2 - 1;
		nextSquare[i].x = Figures[nextShapeNum][i] % 2;
		nextSquare[i].y = Figures[nextShapeNum][i] / 2;
	}

	// ������״ʱ������Ӱ�ӣ���������״��Ӱ�ӻ�������λ�ó���
	updateShadow();

	newShapeFlag = false;

	// AI
	shapeChanged = true;
	AImovedone = false;
	AIrotatedone = false;
	drotate = 0;
}

void Tetris::animationFunc(int i)
{
	Vector2f p;
	sTiles.scale(animationCtrlValue, animationCtrlValue);
	p = sTiles.getOrigin();
	sTiles.setOrigin(GRIDSIZE / 2, GRIDSIZE / 2);
	sTiles.rotate(360 * animationCtrlValue);
	for (int j = 0; j < STAGE_WIDTH; j++)
	{
		sTiles.setTextureRect(IntRect(Field[i][j] * GRIDSIZE, 0, GRIDSIZE, GRIDSIZE));
		sTiles.setPosition(j * GRIDSIZE, i * GRIDSIZE);
		sTiles.move(mCornPoint.x + GRIDSIZE / 2, mCornPoint.y + GRIDSIZE / 2); //offset
		window->draw(sTiles);
	}
	sTiles.scale(1.0 / animationCtrlValue, 1.0 / animationCtrlValue);
	sTiles.rotate(-360 * animationCtrlValue);
	sTiles.setOrigin(p);
}

void Tetris::isWin()
{
	if (Field[2][4] || Field[2][5])
	{
		gameover = true;
	}
}



int Tetris::getRowEliminated()
{
	int k = STAGE_HEIGHT - 1;
	// ����������
	thisTimeRowEli = 0;
	for (int i = STAGE_HEIGHT - 1; i > 0; i--)
	{
		int count = 0;
		for (int j = 0; j < STAGE_WIDTH; j++)
		{
			if (testField[i][j])
				count++;
		}
		if (count < STAGE_WIDTH)
			k--;
		else if (count >= STAGE_WIDTH)
			thisTimeRowEli++;
	}

	// ��testField������
	if (thisTimeRowEli != 0)
	{
		k = STAGE_HEIGHT - 1;
		for (int i = STAGE_HEIGHT - 1; i > 0; i--)
		{
			int xCount = 0;
			for (int j = 0; j < STAGE_WIDTH; j++)
			{
				if (testField[i][j])
					xCount++;
				testField[k][j] = testField[i][j];
			}
			if (xCount < STAGE_WIDTH)
				k--;
		}
	}

	return thisTimeRowEli;
}

int Tetris::getLandingHeight()
{
	int fieldH = 0;

	bool done = false;
	for (int row = 0; row < STAGE_HEIGHT; row++)
	{
		for (int col = 0; col < STAGE_WIDTH; col++)
		{
			if (testField[row][col] != 0)
			{
				done = true;
				fieldH = STAGE_HEIGHT - row;
				break;
			}
		}
		if (done)
			break;
	}

	return fieldH;
}

int Tetris::getRowTransitions()
{
	int rtCount = 0;
	for (int i = 0; i < STAGE_HEIGHT; i++)
	{
		for (int j = 1; j < STAGE_WIDTH; j++)
		{
			if (testField[i][j] != 0 && testField[i][j - 1] != 0);
			else if (testField[i][j] == 0 && testField[i][j - 1] == 0);
			else
				rtCount++;
		}
	}

	return rtCount;
}

int Tetris::getColumnTransitions()
{
	int ctCount = 0;
	for (int i = 0; i < STAGE_WIDTH; i++)
	{
		for (int j = 1; j < STAGE_HEIGHT; j++)
		{
			if (testField[j][i] != 0 && testField[j - 1][i] != 0);
			else if (testField[j][i] == 0 && testField[j - 1][i] == 0);
			else
				ctCount++;
		}
	}

	return ctCount;
}

int Tetris::getNumofHoles()
{

	int holes = 0;
	for (int i = 0; i < STAGE_WIDTH; i++)
	{
		bool start = false;
		for (int j = 0; j < STAGE_HEIGHT; j++)
		{
			if (testField[j][i] != 0 && !start)
			{
				start = true;
			}
			else if (start)
			{
				if (testField[j][i] != 0);
				else
					holes++;
			}
		}
	}

	return holes;
}

int Tetris::getWellSums()
{
	// ��������
	int wellSum = 0;
	for (int i = 0; i < STAGE_WIDTH; i++)
	{
		// һ���ڵľ�������
		int colWell = 0;
		// ����һ��ľ�����
		int deepWell = 0;
		for (int j = 0; j < STAGE_HEIGHT; j++)
		{
			if (i == 0)
			{
				// ������
				if (testField[j][i] == 0 && testField[j][i + 1] != 0)
					colWell++;
				// ��������һ������ �ж��
				if (colWell != 0 
					&& testField[j][i] == 0 && testField[j][i + 1] != 0
					&& testField[j - 1][i] == 0 && testField[j - 1][i + 1] != 0)
				{
					deepWell++;
					colWell += deepWell;
				}
				// ��������һ������ �ж������
				if (colWell != 0
					&& !(testField[j][i] == 0 && testField[j][i + 1] != 0))
				{
					deepWell = 0;
				}
			}
			else if (i == STAGE_WIDTH - 1)
			{
				// ������
				if (testField[j][i] == 0 && testField[j][i - 1] != 0)
					colWell++;
				// ��������һ������ �ж��
				if (colWell != 0 
					&& testField[j][i] == 0 && testField[j][i - 1] != 0
					&& testField[j - 1][i] == 0 && testField[j - 1][i - 1] != 0)
				{
					deepWell++;
					colWell += deepWell;
				}
				// ��������һ������ �ж������
				if (colWell != 0
					&& !(testField[j][i] == 0 && testField[j][i - 1] != 0))
				{
					deepWell = 0;
				}
			}
			else
			{
				// ������
				if (testField[j][i] == 0 
					&& testField[j][i + 1] != 0 && testField[j][i - 1] != 0)
					colWell++;
				// ��������һ������ �ж��
				if (colWell != 0 
					&& testField[j][i] == 0
					&& testField[j][i + 1] != 0 && testField[j][i - 1] != 0
					&& testField[j - 1][i] == 0
					&& testField[j - 1][i + 1] != 0 && testField[j - 1][i - 1] != 0)
				{
					deepWell++;
					colWell += deepWell;
				}
				// ��������һ������ �ж������
				if (colWell != 0
					&& !(testField[j][i] == 0 
						&& testField[j][i + 1] != 0 && testField[j][i - 1] != 0))
				{
					deepWell = 0;
				}
			}

		}
		wellSum += colWell;
	}

	return wellSum;
}


// ����drop��ǰ��
int Tetris::testDrop(int col, int rotate)
{
	Vector2i testSquare[4];
	for (int i = 0; i < 4; i++)
		testSquare[i] = currentSquare[i];

	// ƽ����
	int xDelta = testSquare[0].x - col;
	// ��ת
	for (int time = 0; time < rotate; time++)
		testRotate(testSquare);

	// �ƶ�
	for (int i = 0; i < 4; i++)
	{
		testSquare[i].x -= xDelta;
		// �ж��Ƿ�Խ��
		if (testSquare[i].x < 0 || testSquare[i].x >= STAGE_WIDTH)
			return 0;
	}
	
	// �ӵ�ǰλ������
	for (int i = 0; i < STAGE_HEIGHT; i++)
	{
		for (int j = 0; j < 4; j++)
			testSquare[j].y += 1;
		if (!hitTest(testSquare))
		{
			for (int k = 0; k < 4; k++)
				testSquare[k].y -= 1;
			break;
		}
	}
	
	// ����һ����̨
	for (int i = 0; i < STAGE_HEIGHT; i++)
		for (int j = 0; j < STAGE_WIDTH; j++)
			testField[i][j] = Field[i][j];
	
	// ������testSquare
	for (int i = 0; i < 4; i++)
		testField[testSquare[i].y][testSquare[i].x] = 1;

	return 1;
}


// ���Ի�hold�����鲢drop
int Tetris::testHold(int col, int rotate)
{
	bool canHold = false;
	// ������
	if (holdFunc())
		canHold = true;

	if (canHold)
	{
		if (!testDrop(col, rotate))
		{
			// ����ȥ
			holdFunc();
			return 0;
		}
		holdFunc();
		return 1;
	}
	else
	{
		return 0;
	}
}


// AI�ж�
void Tetris::AI()
{
	long double benchMark = -9999;

	for (int rotate = 0; rotate < 4; rotate++)
	{
		for (int col = 0; col < 10; col++)
		{
			nowCharc = 0;
			bool currentSkip = false;
			bool holdSkip = false;
			if (!testDrop(col, rotate))
				currentSkip = true;
			
			if (!currentSkip)
			{
				nowCharc += RowEliWeight * getRowEliminated();
				nowCharc += HeightWeight * getLandingHeight();
				nowCharc += RowTransWeight * getRowTransitions();
				nowCharc += ColTransWeight * getColumnTransitions();
				nowCharc += HolesWeight * getNumofHoles();
				nowCharc += WellWeight * getWellSums();

				if (nowCharc > benchMark)
				{
					benchMark = nowCharc;
					bestCol = col;
					bestRotate = rotate;
					chooseCurrentOrHold = 0; // current��
				}

				if (ifCoutWeights)
				{
					cout << "��ǰ��||";
					cout << "@" << col << ",r" << rotate << "||";
					cout << "��ظߣ�" << getLandingHeight() << "||"
						<< "��������" << thisTimeRowEli << "||"
						<< "�б任��" << getRowTransitions() << "||"
						<< "�б任��" << getColumnTransitions() << "||"
						<< "�ն�����" << getNumofHoles() << "||"
						<< "��������" << getWellSums() << "||";
					cout << nowCharc << "||" << endl;
				}
			}
			
			if (!testHold(col, rotate))
				holdSkip = true;

			if (!holdSkip)
			{
				nowCharc += RowEliWeight * getRowEliminated();
				nowCharc += HeightWeight * getLandingHeight();
				nowCharc += RowTransWeight * getRowTransitions();
				nowCharc += ColTransWeight * getColumnTransitions();
				nowCharc += HolesWeight * getNumofHoles();
				nowCharc += WellWeight * getWellSums();

				if (nowCharc > benchMark)
				{
					benchMark = nowCharc;
					bestCol = col;
					bestRotate = rotate;
					chooseCurrentOrHold = 1; // hold��
				}

				if (ifCoutWeights)
				{
					cout << "hold��||";
					cout << "@" << col << ",r" << rotate << "||";
					cout << "��ظߣ�" << getLandingHeight() << "||"
						<< "��������" << thisTimeRowEli << "||"
						<< "�б任��" << getRowTransitions() << "||"
						<< "�б任��" << getColumnTransitions() << "||"
						<< "�ն�����" << getNumofHoles() << "||"
						<< "��������" << getWellSums() << "||";
					cout << nowCharc << "||" << endl;
				}
			}
		}

	}
	if (ifCoutWeights)
	{
		if (chooseCurrentOrHold == 0)
			cout << "��ǰ�飬";
		else
			cout << "hold�飬";
		cout << "best now: @" << bestCol << ", r" << bestRotate << endl << endl;
	}
}


// AI�˶�
void Tetris::AImove(int dstCol, int dstRotate)
{
	if (chooseCurrentOrHold == 1)
	{
		holdFunc();
		chooseCurrentOrHold = -1;
	}
	if (currentSquare[0].x == dstCol)
		AImovedone = true;
	if (drotate == dstRotate)
		AIrotatedone = true;

	if (AImovedone && AIrotatedone)
	{
		if(ifAIHardDrop)
			hardDrop = true;
		return;
	}

	// ��ת
	while (drotate < dstRotate)
	{
		Rotate();
		drotate++;
	}


	// xMove
	int Xmove;
	if (currentSquare[0].x > dstCol)
		Xmove = -1;
	else if (currentSquare[0].x < dstCol)
		Xmove = 1;

	if (!AImovedone)
	{
		for (int i = 0; i < 4; i++)
			currentSquare[i].x += Xmove;
		if (!hitTest(currentSquare))
		{
			for (int i = 0; i < 4; i++)
				currentSquare[i].x -= Xmove;
			if (ifAIHardDrop)
				hardDrop = true;
		}
	}
}


// �����µ������Ⱥ
void Tetris::generateInitialWeights()
{
	long double temp[6];
	for (int i = 0; i < POPULATION; i++)
	{
		// ��һ��
		temp[0] = (double)(rand() % 100) / 10;
		temp[1] = (double)(rand() % 100) / 10;
		temp[2] = (double)(rand() % 100) / 10;
		temp[3] = (double)(rand() % 100) / 10;
		temp[4] = (double)(rand() % 100) / 10;
		temp[5] = (double)(rand() % 100) / 10;

		long double sum = 0;
		sum = temp[0] + temp[1] + temp[2] + temp[3] + temp[4] + temp[5];
		temp[0] = temp[0] / sum * 10;
		temp[1] = temp[1] / sum * 10;
		temp[2] = temp[2] / sum * 10;
		temp[3] = temp[3] / sum * 10;
		temp[4] = temp[4] / sum * 10;
		temp[5] = temp[5] / sum * 10;

		HeightWeightPool[i] = - temp[0];
		RowEliWeightPool[i] = temp[1];
		RowTransWeightPool[i] = - temp[2];
		ColTransWeightPool[i] = - temp[3];
		HolesWeightPool[i] = - temp[4];
		WellWeightPool[i] = - temp[5];
	}
	HeightWeight = HeightWeightPool[0];
	RowEliWeight = RowEliWeightPool[0];
	RowTransWeight = RowTransWeightPool[0];
	ColTransWeight = ColTransWeightPool[0];
	HolesWeight = HolesWeightPool[0];
	WellWeight = WellWeightPool[0];
}


// ���ļ���ȡ ��һ�ε���Ⱥ
void Tetris::readCrowd()
{
	// ��ȡ70���ʼ����
	fstream file;
	file.open("data/rec/popu.txt", ios::in);

	long double a;
	int i = 0;
	string token;
	while (getline(file, token, ','))
	{
		// ������ʮ��������͵ģ�30�����ݣ�ÿ��6����
		if (++i <= 180)
			continue;

		// ��ȡ��pool��70λ
		a = atof(token.c_str());
		if (i % 6 == 1)
			HeightWeightPool[(i - 1) / 6] = a;
		else if (i % 6 == 2)
			RowEliWeightPool[(i - 1) / 6] = a;
		else if (i % 6 == 3)
			RowTransWeightPool[(i - 1) / 6] = a;
		else if (i % 6 == 4)
			ColTransWeightPool[(i - 1) / 6] = a;
		else if (i % 6 == 5)
			HolesWeightPool[(i - 1) / 6] = a;
		else if (i % 6 == 0)
			WellWeightPool[(i - 1) / 6] = a;
	}

	file.close();
}


// ����ȡ����Ⱥɸѡ�ӽ�
void Tetris::crossBreed()
{
	// ����30���ӽ����
	int ArandIndex[30], BrandIndex[30];
	for (int i = 0; i < 30; i++)
	{
		int a, b;
		// ��5��һ���Ŵ��ϴα�����õ�
		if (i < 5)
		{
			a = 99;
			b = rand() % 70 + 30;
			while (a == b)
				b = rand() % 70 + 30;
		}
		else
		{
			a = rand() % 70 + 30;
			b = rand() % 70 + 30;
			while (a == b)
				b = rand() % 70 + 30;
		}
		ArandIndex[i] = a;
		BrandIndex[i] = b;
	}

	// �ӽ� && �������䱻ɸѡ����Ⱥ
	float aRate, bRate;
	float mutateRate;
	for (int i = 0; i < 30; i++)
	{
		aRate = (double)(rand() % 100) / 100 ;
		bRate = 1 - aRate;

		// �ӽ�����
		HeightWeightPool[i] = aRate * HeightWeightPool[ArandIndex[i]] + bRate * HeightWeightPool[BrandIndex[i]];
		RowEliWeightPool[i] = aRate * RowEliWeightPool[ArandIndex[i]] + bRate * RowEliWeightPool[BrandIndex[i]];
		RowTransWeightPool[i] = aRate * RowTransWeightPool[ArandIndex[i]] + bRate * RowTransWeightPool[BrandIndex[i]];
		ColTransWeightPool[i] = aRate * ColTransWeightPool[ArandIndex[i]] + bRate * ColTransWeightPool[BrandIndex[i]];
		HolesWeightPool[i] = aRate * HolesWeightPool[ArandIndex[i]] + bRate * HolesWeightPool[BrandIndex[i]];
		WellWeightPool[i] = aRate * WellWeightPool[ArandIndex[i]] + bRate * WellWeightPool[BrandIndex[i]];


		// ���ʷ�������
		mutateRate = (double)(rand() % 100) / 100;
		if (mutateRate <= MUTATEPROB)
			mutate(i);
	}

	//�м�40λ����
	for (int i = 30; i < 70; i++)
		mutate(i);

	HeightWeight = HeightWeightPool[0];
	RowEliWeight = RowEliWeightPool[0];
	RowTransWeight = RowTransWeightPool[0];
	ColTransWeight = ColTransWeightPool[0];
	HolesWeight = HolesWeightPool[0];
	WellWeight = WellWeightPool[0];
}


// ����i�Ÿ���
void Tetris::mutate(int i)
{
	// �����ĸ�����
	int mutateTarget = rand() % 6 + 1;

	// �������� ���췽�� && ǿ�Ҷ�
	float mutateDens;
	mutateDens = (float)(rand() % 200) / 100 - 1;

	switch (mutateTarget)
	{
	case 1: HeightWeightPool[i] += mutateDens; break;
	case 2: RowEliWeightPool[i] += mutateDens; break;
	case 3: RowTransWeightPool[i] += mutateDens; break;
	case 4: ColTransWeightPool[i] += mutateDens; break;
	case 5: HolesWeightPool[i] += mutateDens; break;
	case 6: WellWeightPool[i] += mutateDens; break;
	default: break;
	}

	// ��һ��
	long double temp[6];

	temp[0] = - HeightWeightPool[i];
	temp[1] = RowEliWeightPool[i];
	temp[2] = - RowTransWeightPool[i];
	temp[3] = - ColTransWeightPool[i];
	temp[4] = - HolesWeightPool[i];
	temp[5] = - WellWeightPool[i];

	long double sum = 0;
	sum = temp[0] + temp[1] + temp[2] + temp[3] + temp[4] + temp[5];
	temp[0] = temp[0] / sum * 10;
	temp[1] = temp[1] / sum * 10;
	temp[2] = temp[2] / sum * 10;
	temp[3] = temp[3] / sum * 10;
	temp[4] = temp[4] / sum * 10;
	temp[5] = temp[5] / sum * 10;

	HeightWeightPool[i] = -temp[0];
	RowEliWeightPool[i] = temp[1];
	RowTransWeightPool[i] = -temp[2];
	ColTransWeightPool[i] = -temp[3];
	HolesWeightPool[i] = -temp[4];
	WellWeightPool[i] = -temp[5];

}