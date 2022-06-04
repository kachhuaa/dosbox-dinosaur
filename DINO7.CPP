/************************************************************************

DosBox Dinosaur version 0.7

By Vrishabh Bansal and Debabhiram Manik

Changes:
-Refactored Program Code
-Introduced Scores
-Introduced Highscore System
-Added Main Menu
-Added Help Screen
-Added Environment
-Tweaked Gameplay Difficulty

Current State - Stable
Build Type - Debug

*************************************************************************/

/************************************************************************
HEADER FILES
*************************************************************************/
#include <iostream.h>
#include <conio.h>
#include <graphics.h>
#include <stdlib.h>
#include <dos.h>
#include <fstream.h>

enum Bool {FALSE, TRUE};
enum HUD_Type {HUD_INVALID, HUD_TEXT, HUD_BUTTON};
enum HUD_State {HUD_DEFAULT, HUD_SELECTED};

/*************************************************************************
CLASS DECLARATIONS
**************************************************************************/
class Vector;
class Model;
class Entity;
class Dino;
class Spike;
class SpikeNode;
class HUD_Element;
class Player;

class Vector
{
private:
	float x;
	float y;
public:
	Vector();
	Vector(float x, float y);
	Vector(Vector &v);
	void restrict(Vector minXY, Vector maxXY);
	void operator=(Vector v);
	Vector operator+(Vector v);
	Vector operator-(Vector v);
	static Vector positive(Vector v);
	static Vector scale(Vector v1, Vector v2);
	float retX();
	float retY();
};

class Model
{
private:
	struct Anim
	{
		int numPts;
		Vector *points;
		const char *fileName;
		int animNo;
	}*anims;
	Bool copy;
	int numAnim;
	int curAnim;
	void loadModel();
	void setModel(const char *const *modelFiles);
public:
	Model();
	Model(const char *const *modelFiles, int numAnim, int curAnim);
	~Model();
	Model(Model &model);
	void show(Vector position);
	void setAnim(int anim);
	int retAnim();
};

class Entity
{
protected:
	Vector position;
	Vector velocity;
	Vector acceleration;
	Vector size;
	Model model;
public:
	Entity();
	Entity(Vector position, Vector velocity, Vector size, const char *const *modelFiles, int numAnim, int curAnim);
	Entity(Vector position, Vector velocity, Vector size, Model model);
	void operator=(Entity e);
	Entity(Entity &e);
	void applyForce(Vector force);
	void showCollPol();
	void show();
	Bool outOfBounds(Vector minXY, Vector maxXY);
	void move(Vector minXY, Vector maxXY);
	static Bool collision(Entity e1, Entity e2);
};

class Spike
{
private:
	int packSize;
	Entity *cacti;
public:
	Spike();
	Spike(int packSize, Vector position, Vector velocity, Vector size, Model model, int numAnim);
	Spike(Spike &s);
	~Spike();
	void show();
	void showCollPol();
	Bool outOfBounds(Vector minXY, Vector maxXY);
	Bool collide(Dino &dino);
	void move();
};

class Dino : public Entity
{
private:
	enum AnimNames {DEAD_ANIM, RUN_ANIM_A, RUN_ANIM_B, JUMP_ANIM};
	enum State {DEAD, RUNNING, JUMPING};

	Vector jumpForce;
	State state;
public:
	Dino();
	Dino(Vector position, Vector velocity, Vector size, const char *const *modelFiles, int numAnim, int curAnim, Vector jumpForce);
	Dino(Dino &d);
	Bool isDead();
	void die();
	void jump();
	void show(long int tick);
	void move(Vector minXY, Vector maxXY, Vector gravity);
};

class SpikeNode
{
private:
	Spike spike;
public:
	SpikeNode(int packSize, Vector position, Vector velocity, Vector size, Model model, int numAnim = 5);
	SpikeNode(Spike s);
	Spike *retSpike();
	SpikeNode *next;
};

class HUD_Element
{
private:
	Vector position;
	const char *label;
	HUD_Type type;
	HUD_State state;
	int font;
	float fontSize;
public:
	HUD_Element();
	HUD_Element(Vector position, const char *label, HUD_Type type, HUD_State state, int font, float fontSize);
	HUD_Element(HUD_Element &e);
	void operator=(HUD_Element e);
	void show(Bool labelOnly = FALSE);
	void setPos(Vector &position);
	void setFontSize(float fontSize);
	void select();
	void deselect();
	Bool isSelected();
};

class Player
{
private:
	char name[12];
	long int score;
	friend void displayScore(char *fileName, Vector windowXY);
	friend void highScore(char *fileName, long int score, Vector windowXY);
public:
	Player();
	Player(char *name, long int score);
	Player(Player &p);
	void operator = (Player &p);
};

/*************************************************************************
GLOBAL FUNCTION DECLARATIONS
*************************************************************************/
void initGraphMod(Vector &windowXY);
void game(Vector windowXY);
void addSpikes(SpikeNode *&first, SpikeNode *&last, Vector windowXY, float difficulty, Model model);
void updateSpikes(SpikeNode *&first, Dino &dino);
void showSpikes(SpikeNode *first);
void deleteSpikes(SpikeNode *&first);
void mainMenu(Vector windowXY);
void waitMenu(Vector windowXY, Bool &gameOver);
void helpScreen(Vector windowXY);
void addClouds(Entity *&clouds, int size, Vector windowXY, Model model);
void showClouds(Entity *clouds, int size);
void updateClouds(Entity *clouds, int size, Vector windowXY, Model model);
void deleteClouds(Entity *&clouds);
void displayScore(char *fileName, Vector windowXY);
void highScore(char *fileName, long int score, Vector windowXY);

/*************************************************************************
GLOBAL FUNCTION DEFINITIONS
*************************************************************************/

int main()
{
	clrscr();
	randomize();

	Vector windowXY;

	initGraphMod(windowXY);

	mainMenu(windowXY);

	closegraph();
	return 0;
}

void helpScreen(Vector windowXY)
{
	cleardevice();
	closegraph();
	initGraphMod(windowXY);
	cleardevice();

	HUD_Element title = HUD_Element(Vector(windowXY.retX()/2, 50), "HELP", HUD_TEXT, HUD_DEFAULT, BOLD_FONT, 1.0f);
	HUD_Element menuTitle = HUD_Element(Vector(160, 150), "Menu Controls - ", HUD_TEXT, HUD_DEFAULT, COMPLEX_FONT, 1.0f);
	HUD_Element menuButton1 = HUD_Element(Vector(95, 200), "Spacebar", HUD_BUTTON, HUD_DEFAULT, BOLD_FONT, 0.7f);
	HUD_Element menuText1 = HUD_Element(Vector(230, 200), "to cycle,", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
	HUD_Element menuButton2 = HUD_Element(Vector(320, 200), "Enter", HUD_BUTTON, HUD_DEFAULT, BOLD_FONT, 0.7f);
	HUD_Element menuText2 = HUD_Element(Vector(420, 200), "to select", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
	HUD_Element gameTitle = HUD_Element(Vector(160, 300), "Game Controls - ", HUD_TEXT, HUD_DEFAULT, COMPLEX_FONT, 1.0f);
	HUD_Element gameButton1 = HUD_Element(Vector(95, 350), "Spacebar", HUD_BUTTON, HUD_DEFAULT, BOLD_FONT, 0.7f);
	HUD_Element gameText1 = HUD_Element(Vector(235, 350), "to jump,", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
	HUD_Element gameButton2 = HUD_Element(Vector(310, 350), "Esc", HUD_BUTTON, HUD_DEFAULT, BOLD_FONT, 0.7f);
	HUD_Element gameText2 = HUD_Element(Vector(395, 350), "to pause", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
	title.show();
	menuTitle.show(TRUE);
	menuButton1.show();
	menuText1.show(TRUE);
	menuButton2.show();
	menuText2.show(TRUE);
	gameTitle.show(TRUE);
	gameButton1.show();
	gameText1.show(TRUE);
	gameButton2.show();
	gameText2.show(TRUE);
	getch();
}

void mainMenu(Vector windowXY)
{
	Bool quit = FALSE;

	setbkcolor(BLACK);
	cleardevice();

	settextstyle(BOLD_FONT, HORIZ_DIR, 0);
	setusercharsize(2, 1, 2, 1);

	HUD_Element titleA = HUD_Element(Vector(windowXY.retX()/2, (windowXY.retY()-textheight("DosBox"))/2), "DosBox", HUD_TEXT, HUD_DEFAULT, BOLD_FONT, 2.0f);
	HUD_Element titleB = HUD_Element(Vector(windowXY.retX()/2, (windowXY.retY()+textheight("Dinosaur"))/2), "Dinosaur", HUD_TEXT, HUD_DEFAULT, BOLD_FONT, 2.0f);
	titleA.show(TRUE);
	titleB.show(TRUE);
	delay(1000);

	for (float i=1; i>=0; i-=0.01)
	{
		cleardevice();
		settextstyle(BOLD_FONT, HORIZ_DIR, 0);
		setusercharsize((i+1)*1000, 1000, (i+1)*1000, 1000);
		titleA.setFontSize(i+1);
		titleB.setFontSize(i+1);
		titleA.setPos(Vector(windowXY.retX()/2*i+(windowXY.retX()-textwidth("DosBox Dinosaur")/2)/2*(1-i), (windowXY.retY()-textheight("DosBox"))/2*i+50*(1-i)));
		titleB.setPos(Vector(windowXY.retX()/2*i+(windowXY.retX()+textwidth("DosBox Dinosaur")/2)/2*(1-i), (windowXY.retY()+textheight("Dinosaur"))/2*i+50*(1-i)));
		titleA.show(TRUE);
		titleB.show(TRUE);
		delay(20);
	}

	HUD_Element title = HUD_Element(Vector(windowXY.retX()/2, 50), "DosBox Dinosaur", HUD_TEXT, HUD_DEFAULT, BOLD_FONT, 1.0f);
	HUD_Element playGame = HUD_Element(Vector(windowXY.retX()/2, 170), "           Play        ", HUD_BUTTON, HUD_SELECTED, SIMPLEX_FONT, 0.7f);
	HUD_Element showHelp = HUD_Element(Vector(windowXY.retX()/2, 270), "           Help        ", HUD_BUTTON, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
	HUD_Element exitProgram = HUD_Element(Vector(windowXY.retX()/2, 320), "        Exit Game    ", HUD_BUTTON, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
	HUD_Element showScore = HUD_Element(Vector(windowXY.retX()/2, 220), "     View Highscore  ", HUD_BUTTON, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);

	char key = '\0';

	while (quit == FALSE)
	{
		switch (key)
		{
			case ' ':
				if (exitProgram.isSelected() == TRUE)
				{
					playGame.select();
					showHelp.deselect();
					exitProgram.deselect();
					showScore.deselect();
				}
				else if (playGame.isSelected() == TRUE)
				{
					showScore.select();
					showHelp.deselect();
					playGame.deselect();
					exitProgram.deselect();
				}
				else if (showHelp.isSelected() == TRUE)
				{
					showScore.deselect();
					exitProgram.select();
					playGame.deselect();
					showHelp.deselect();
				}
				else if (showScore.isSelected() == TRUE)
				{
					showScore.deselect();
					showHelp.select();
					playGame.deselect();
					exitProgram.deselect();
				}
				else
				{
					cleardevice();
					closegraph();
					clrscr();
					cout << "No Button Selected..." << endl;
					cout << "Press any key to exit..." << endl;
					getch();
					exit(1);
				}
				break;
			case 13:
				if (playGame.isSelected() == TRUE)
				{
					game(windowXY);
				}
				else if (showHelp.isSelected() == TRUE)
				{
					helpScreen(windowXY);
				}
				else if (exitProgram.isSelected() == TRUE)
				{
					quit = TRUE;
				}
				else if (showScore.isSelected() == TRUE)
				{
					displayScore("SCORE.DAT", windowXY);
				}
				else
				{
					cleardevice();
					closegraph();
					clrscr();
					cout << "No Button Selected..." << endl;
					cout << "Press any key to exit..." << endl;
					getch();
					exit(1);
				}
				break;
		}

		setbkcolor(BLACK);
		cleardevice();

		title.show();
		playGame.show();
		showHelp.show();
		showScore.show();
		exitProgram.show();

		if (quit == FALSE)
			key = getch();
	}
}

void game(Vector windowXY)
{
	Bool loop = TRUE;
	do
	{
		cleardevice();
		closegraph();
		initGraphMod(windowXY);
		setbkcolor(BLACK);
		cleardevice();

		Vector gravity(0.0f, 4.0f);
		long int tick = 0;
		Bool gameOver = FALSE;
		float difficulty = 10.0f;
		long int score = 0;

		const char *dinoMdlFiles[] = {"DINODEAD.DAT", "DINORUNA.DAT", "DINORUNB.DAT", "DINOJUMP.DAT"};
		const char *cactiMdlFiles[] = {"CACTUS1.DAT", "CACTUS2.DAT", "CACTUS3.DAT", "CACTUS4.DAT", "CACTUS5.DAT"};
		const char *cloudMdlFiles[] = {"CLOUD1.DAT", "CLOUD2.DAT", "CLOUD3.DAT"};

		Model cactiModel(cactiMdlFiles, 5, -1);
		Model cloudModel(cloudMdlFiles, 3, -1);

		Dino *dino = new Dino(Vector(50, 300), Vector(0, 0), Vector(30, 30), dinoMdlFiles, 4, 3, Vector(0.0f, -37.0f));

		SpikeNode *first = NULL, *last = NULL;

		int numCloud = 3;
		Entity *clouds;
		addClouds(clouds, numCloud, windowXY, cloudModel);

		Vector randPoints[20];
		for (int i=0; i<20; i++)
		{
			randPoints[i] = Vector(windowXY.retX()/20*i, windowXY.retY()-random(30));
		}

		setbkcolor(BLACK);
		cleardevice();

		while(gameOver == FALSE)
		{
			if (kbhit())
			{
				char key = getch();
				if (key == 27)
				{
					waitMenu(windowXY, gameOver);
					if (gameOver == TRUE)
					{
						delete dino;
						deleteSpikes(first);
						deleteClouds(clouds);
						return;
					}
				}
				else if (key == ' ')
				{
					dino->jump();
				}
			}

			setbkcolor(BLACK);
			cleardevice();

			char scoreStr[33];
			ltoa(score, scoreStr, 10);
			HUD_Element scoreText = HUD_Element(Vector(windowXY.retX()-150, 30), "Score - ", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 1.0f);
			HUD_Element scoreVal = HUD_Element(Vector(windowXY.retX()-50, 30), scoreStr, HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 1.0f);
			scoreText.show(TRUE);
			scoreVal.show(TRUE);

			dino->move(Vector(50, 15), Vector(50, windowXY.retY()-45), gravity);

			addSpikes(first, last, windowXY, difficulty, cactiModel);
			updateSpikes(first, *dino);

			updateClouds(clouds, numCloud, windowXY, cloudModel);

			setcolor(WHITE);
			line(0, windowXY.retY()-30, windowXY.retX(), windowXY.retY()-30);

			//dino.showCollPol();
			dino->show(tick);
			showSpikes(first);
			showClouds(clouds, numCloud);

			for (int i=0; i<20; i++)
			{
				randPoints[i] = Vector(randPoints[i].retX()-difficulty, randPoints[i].retY());
				if (randPoints[i].retX() <= 0)
				{
					randPoints[i] = Vector(windowXY.retX()*(1+i/20) + random(11) - 5, windowXY.retY()-random(30));
				}
				putpixel(randPoints[i].retX(), randPoints[i].retY(), WHITE);
			}

			delay(20);
			if (difficulty < 39.995f)
				difficulty += 0.005f;
			tick++;
			score += difficulty/10.0f;
			gameOver = dino->isDead();
		}
		delay(500);
		highScore("SCORE.DAT", score, windowXY);
		delete dino;
		deleteSpikes(first);
		deleteClouds(clouds);
		clouds = NULL;
		first = last = NULL;
		dino = NULL;
		waitMenu(windowXY, loop);
	} while (loop == TRUE);
	return;
}

void waitMenu(Vector windowXY, Bool &msgToggle)
{
	HUD_Element message, play;
	if (msgToggle == FALSE)
	{
		message = HUD_Element(Vector(windowXY.retX()/2, 150), "------GAME PAUSED------", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 1.0f);
		play = HUD_Element(Vector(windowXY.retX()/2, 220), "      Resume      ", HUD_BUTTON, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
	}
	else
	{
		message = HUD_Element(Vector(windowXY.retX()/2, 150), "------GAME OVER------", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 1.0f);
		play = HUD_Element(Vector(windowXY.retX()/2, 220), "      Restart      ", HUD_BUTTON, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
	}
	HUD_Element quit = HUD_Element(Vector(windowXY.retX()/2, 270), "Exit to Main Menu", HUD_BUTTON, HUD_SELECTED, SIMPLEX_FONT, 0.7f);

	char key = ' ';

	do
	{
		switch (key)
		{
			case ' ':
				if (play.isSelected() == TRUE)
				{
					quit.select();
					play.deselect();
				}
				else if (quit.isSelected() == TRUE)
				{
					play.select();
					quit.deselect();
				}
				break;
			case 13:
				if (quit.isSelected() == TRUE)
				{
					msgToggle = msgToggle == FALSE ? TRUE : FALSE;
				}
				else if (play.isSelected() == FALSE)
				{
					cleardevice();
					closegraph();
					clrscr();
					cout << "No Button Selected..." << endl;
					cout << "Press any key to exit..." << endl;
					getch();
					exit(1);
				}
				key = '\0';
				break;
		}

		message.show(TRUE);
		play.show();
		quit.show();

		if (key != '\0')
		{
			key = getch();
		}
	} while (key != '\0');
}

void addClouds(Entity *&clouds, int size, Vector windowXY, Model model)
{
	clouds = new Entity[size];
	for (int i=0; i<size && clouds != NULL; i++)
	{
		clouds[i] = Entity(Vector(windowXY.retX() + random(windowXY.retX()), windowXY.retY()-random(20)-150), Vector(-3, 0), Vector(20, 10), model);
	}
}

void showClouds(Entity *clouds, int size)
{
	for (int i=0; i<size && clouds != NULL; i++)
	{
		clouds[i].show();
	}
}

void updateClouds(Entity *clouds, int size, Vector windowXY, Model model)
{
	for (int i=0; i<size && clouds != NULL; i++)
	{
		clouds[i].move(Vector(-100, -100), windowXY+Vector(1000, 1000));
		if (clouds[i].outOfBounds(Vector(-50, -50), windowXY+Vector(1000, 1000)))
			clouds[i] = Entity(Vector(windowXY.retX() + random(windowXY.retX()), windowXY.retY()-random(20)-150), Vector(-3, 0), Vector(20, 10), model);
	}
}

void deleteClouds(Entity *&clouds)
{
	if (clouds != NULL)
		delete []clouds;
}

void initGraphMod(Vector &windowXY)
{
	int gDriver = DETECT, gMode, errCode;
	initgraph(&gDriver, &gMode, "C:/TURBOC3/BGI");
	errCode = graphresult();
	if (errCode != grOk)
	{
		cout << "Graphics Initialization Failed..." << endl;
		cout << grapherrormsg(errCode) << endl;
		cout << "Press any key to exit..." << endl;
		getch();
		exit(1);
	}
	else
	{
		windowXY = Vector(getmaxx(), getmaxy());
	}
}

void addSpikes(SpikeNode *&first, SpikeNode *&last, Vector windowXY, float difficulty, Model model)
{
	static int spawn = -5;
	SpikeNode *newPtr = NULL;
	int packSize = random(3) + 1;
	float prob = 4.0f/packSize * (spawn-15)/16.0f * difficulty*difficulty/100.0f;
	float r2 = random(10001)/100.0;
	Vector velocity = Vector(-difficulty, 0);
	if (r2<prob)
	{
		newPtr = new SpikeNode(packSize, Vector(windowXY.retX()+45, windowXY.retY()-50), velocity, Vector(20.0f, 40.0f), model, 5);
		if (newPtr == NULL)
		{
			getch();
			closegraph();
			exit(1);
		}
		else
		{
			spawn = 0;
			if (first == NULL)
			{
				first = newPtr;
				last = newPtr;
			}
			else
			{
				last->next = newPtr;
				last = newPtr;
			}
		}
	}
	else
	{
		spawn++;
	}
}

void updateSpikes(SpikeNode *&first, Dino &dino)
{
	SpikeNode *current = first;
	while ((current != NULL) && (current->next != NULL) && (current->retSpike()->outOfBounds(Vector(-50, 0), Vector (1000, 1000)) == TRUE))
	{
		current = current->next;
		delete first;
		first = current;
	}
	current = first;
	while (current != NULL)
	{
		if (current->retSpike()->collide(dino) == TRUE)
		{
			dino.die();
			return;
		}
		current = current->next;
	}
	current = first;
	while (current != NULL)
	{
		current->retSpike()->move();
		current = current->next;
	}
}

void showSpikes(SpikeNode *first)
{
	while (first != NULL)
	{
		//first->retSpike()->showCollPol();
		first->retSpike()->show();
		first = first->next;
	}
}

void deleteSpikes(SpikeNode *&first)
{
	SpikeNode *current = first;
	while (first != NULL)
	{
		first = first->next;
		delete current;
		current = first;
	}
}

void displayScore(char *fileName, Vector windowXY)
{
	char randomNames[20][12] = {"Josh", "Mark", "Michael", "Alex", "Simon", "Tony", "Richard", "Ken", "Martha", "Vanessa", "Kate", "John", "Joanne", "Sarah", "Rita", "Ryan", "Diana", "Vladimir", "Sergei", "Jason"};

	Player player;
	HUD_Element title(Vector(windowXY.retX()/2, 50), "High Score", HUD_TEXT, HUD_DEFAULT, BOLD_FONT, 1.0f);
	HUD_Element playerNo(Vector(150, 140), "S.No.", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.8f);
	HUD_Element playerName(Vector(300, 140), "Name", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.8f);
	HUD_Element playerScore(Vector(500, 140), "Score", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.8f);

	setbkcolor(BLACK);
	cleardevice();

	title.show(FALSE);
	playerNo.show(TRUE);
	playerName.show(TRUE);
	playerScore.show(TRUE);


	ifstream scoreFile;
	scoreFile.open(fileName, ios::in | ios::binary | ios::nocreate | ios::noreplace);
	while (!scoreFile)
	{
		ofstream temp;
		temp.open(fileName, ios::out | ios::binary | ios::noreplace | ios::trunc);
		for (int i=0; i<10; i++)
		{
			temp.write((char *)&player, sizeof(player));
		}

		temp.close();

		scoreFile.open(fileName, ios::in | ios::binary);
	}
	scoreFile.seekg(0, ios::beg);

	int count = 0;
	char temp1[17] = "";
	char temp2[33] = "";
	while(!scoreFile.eof() && count < 10)
	{
		scoreFile.read((char *)&player, sizeof(player));
		if (scoreFile.eof())
			break;
		itoa(count+1, temp1, 10);
		playerNo = HUD_Element(Vector(150, 180+count*30), temp1, HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.5f);
		if (player.name[0] == '\0')
			playerName = HUD_Element(Vector(300, 180+count*30), randomNames[random(20)], HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.5f);
		else
			playerName = HUD_Element(Vector(300, 180+count*30), player.name, HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.5f);
		ltoa(player.score, temp2, 10);
		playerScore = HUD_Element(Vector(500, 180+count*30), temp2, HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.5f);

		playerNo.show(TRUE);
		playerName.show(TRUE);
		playerScore.show(TRUE);
		count++;
	}

	while(getch() != 27);
	scoreFile.close();
	return;
}

void highScore(char *fileName, long int score, Vector windowXY)
{
	closegraph();
	initGraphMod(windowXY);
	Player player, p;

	fstream scoreFile;
	scoreFile.open(fileName, ios::in | ios::binary | ios::out | ios::noreplace | ios::ate | ios::nocreate);
	while (!scoreFile)
	{
		ofstream temp;
		temp.open(fileName, ios::out | ios::binary | ios::trunc);
		for (int i=0; i<10; i++)
		{
			temp.write((char *)&player, sizeof(player));
		}

		temp.close();

		scoreFile.open(fileName, ios::in | ios::binary | ios::out | ios::noreplace | ios::ate | ios::nocreate);
	}
	scoreFile.seekg(0, ios::beg);

	int count = 0;
	while (!scoreFile.eof() && count < 10)
	{
		scoreFile.seekg(count * sizeof(player), ios::beg);
		scoreFile.read((char *)&player, sizeof(player));
		if (scoreFile.eof())
			break;

		if (player.score < score)
		{
			setfillstyle(SOLID_FILL, RED);
			bar(windowXY.retX()/2-200, windowXY.retY()/2-100, windowXY.retX()/2+200, windowXY.retY()/2+100);
			setfillstyle(SOLID_FILL, BLUE);
			bar(windowXY.retX()/2-190, windowXY.retY()/2-90, windowXY.retX()/2+190, windowXY.retY()/2+90);
			HUD_Element title = HUD_Element(Vector(windowXY.retX()/2, windowXY.retY()/2-50), "HIGH SCORE!", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 1.0f);
			title.show(FALSE);
			HUD_Element nameText = HUD_Element(Vector(windowXY.retX()/2-40, windowXY.retY()/2), "Name :", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
			nameText.show(TRUE);
			HUD_Element nameVal = HUD_Element(Vector(windowXY.retX()/2+60, windowXY.retY()/2), "", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
			nameVal.show(TRUE);
			HUD_Element scoreText = HUD_Element(Vector(windowXY.retX()/2-40, windowXY.retY()/2+30), "Score :", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
			scoreText.show(TRUE);
			char temp[37];
			itoa(score, temp, 10);
			HUD_Element scoreVal = HUD_Element(Vector(windowXY.retX()/2-20+textwidth(temp), windowXY.retY()/2+30), temp, HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
			scoreVal.show(TRUE);

			char text[12] = {""};
			int noOfChar = 0;
			char key = '0';
			while (noOfChar < 11)
			{
				key = getch();
				if ((key >= '0' && key <= '9') || (key >= 'a' && key <= 'z') || (key >= 'A' && key <= 'Z'))
				{
					text[noOfChar] = key;
					text[noOfChar+1] = '\0';
					nameVal = HUD_Element(Vector(windowXY.retX()/2+5+textwidth(text)/2, windowXY.retY()/2), text, HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 0.7f);
				}
				else
				{
					if (key == '\n')
						key = getch();
						text[noOfChar] = '\0';
					break;
				}
				setfillstyle(SOLID_FILL, RED);
				bar(windowXY.retX()/2-200, windowXY.retY()/2-100, windowXY.retX()/2+200, windowXY.retY()/2+100);
				setfillstyle(SOLID_FILL, BLUE);
				bar(windowXY.retX()/2-190, windowXY.retY()/2-90, windowXY.retX()/2+190, windowXY.retY()/2+90);
				title.show(FALSE);
				nameText.show(TRUE);
				nameVal.show(TRUE);
				scoreText.show(TRUE);
				scoreVal.show(TRUE);
				noOfChar++;
			}
			Player p = Player(text, score);
			for (int i=8; i>=count; i--)
			{
				scoreFile.seekg(i*sizeof(player), ios::beg);
				scoreFile.read((char *)&player, sizeof(player));
				scoreFile.seekp(scoreFile.tellg(), ios::beg);
				scoreFile.write((char *)&player, sizeof(player));
			}
			scoreFile.seekp(count*sizeof(player), ios::beg);
			scoreFile.write((char *)&p, sizeof(p));
			HUD_Element splash = HUD_Element(Vector(windowXY.retX()/2, windowXY.retY()-100), "High Score Added!", HUD_TEXT, HUD_DEFAULT, SIMPLEX_FONT, 1.0f);
			splash.show();
			getch();
			break;
		}
		count++;
	}
	scoreFile.close();
	cleardevice();
}

/*************************************************************************
CLASS VECTOR
Member Functions
**************************************************************************/

Vector::Vector()
{
	x = 0;
	y = 0;
}

Vector::Vector(float x, float y)
{
	this->x = x;
	this->y = y;
}

Vector::Vector(Vector &v)
{
	x = v.x;
	y = v.y;
}

void Vector::operator=(Vector v)
{
	x = v.x;
	y = v.y;
}

Vector Vector::operator+(Vector v)
{
	return Vector(x+v.x, y+v.y);
}

Vector Vector::operator-(Vector v)
{
	return Vector(x-v.x, y-v.y);
}

float Vector::retX()
{
	return x;
}

float Vector::retY()
{
	return y;
}

void Vector::restrict(Vector minXY, Vector maxXY)
{
	if (x < minXY.x)
		x = minXY.x;
	else if (x > maxXY.x)
		x = maxXY.x;
	if (y < minXY.y)
		y = minXY.y;
	else if (y > maxXY.y)
		y = maxXY.y;
}

Vector Vector::scale(Vector v1, Vector v2)
{
	return Vector(v1.x*v2.x, v1.y*v2.y);
}

Vector Vector::positive(Vector v)
{
	if (v.retX() < 0.0f)
		v = Vector::scale(v, Vector(-1.0f, 1.0f));
	if (v.retY() < 0.0f)
		v = Vector::scale(v, Vector(1.0f, -1.0f));
	return v;
}

/***************************************************************************
CLASS MODEL
Member Functions
****************************************************************************/

Model::Model()
{
	copy = FALSE;
	anims = NULL;
	numAnim = 0;
	curAnim = -1;
	setModel(NULL);
}

Model::Model(const char *const *modelFiles, int numAnim, int curAnim)
{
	this->copy = FALSE;
	this->numAnim = numAnim;
	this->curAnim = curAnim;
	this->anims = new Anim[numAnim];
	for (int i=0; i<numAnim; i++)
	{
		anims[i].numPts = 0;
		anims[i].points = NULL;
		anims[i].fileName = NULL;
		anims[i].animNo = -1;
	}
	setModel(modelFiles);
}

Model::Model(Model &model)
{
	copy = TRUE;
	numAnim = model.numAnim;
	curAnim = model.curAnim;
	anims = model.anims;
}

void Model::show(Vector position)
{
	for (int i=0; i<numAnim; i++)
	{
		if (anims[i].animNo == curAnim)
		{
			for (int j=0; j<anims[i].numPts; j++)
			{
				putpixel(position.retX()+anims[i].points[j].retX(), position.retY()+anims[i].points[j].retY(), WHITE);
			}
			break;
		}
	}
}

void Model::setModel(const char *const *modelFiles)
{
	if (modelFiles != NULL)
	{
		for (int i=0; i<numAnim && anims != NULL; i++)
		{
			anims[i].fileName = modelFiles[i];
			anims[i].animNo = i;
		}
		loadModel();
	}
}

void Model::setAnim(int anim)
{
	if (anim == -1)
		curAnim = random(numAnim);
	else
		curAnim = anim;
}

int Model::retAnim()
{
	return curAnim;
}

void Model::loadModel()
{
	ifstream texture;
	Vector point;

	for (int i=0; i<numAnim && anims != NULL; i++)
	{
		if (anims[i].fileName == NULL)
		{
			getch();
			closegraph();
			exit(1);
		}
		texture.open(anims[i].fileName, ios::binary|ios::in);
		if (texture == NULL)
		{
			getch();
			closegraph();
			exit(1);
		}
		else
		{
			int index = 0;
			texture.read((char *)&point, sizeof(point));
			do
			{
				if (index == anims[i].numPts)
				{
					anims[i].numPts += 50;
					Vector *temp = new Vector[(anims[i].numPts)];
					if (anims[i].points == NULL)
					{
						anims[i].points = temp;
					}
					else
					{
						for (int j=0; j<index; j++)
						{
							temp[j] = anims[i].points[j];
						}
						delete [](anims[i].points);
						anims[i].points = temp;
					}
				}
				anims[i].points[index] = point;

				texture.read((char *)&point, sizeof(point));
				index++;
			} while (!texture.eof());

			while(index < anims[i].numPts)
			{
				anims[i].points[index] = anims[i].points[index-1];
				index++;
			}
			texture.close();
		}
	}
}

Model::~Model()
{
	if (copy == FALSE)
	{
		for (int i=0; i<numAnim; i++)
		{
			delete [](anims[i].points);
		}
		delete []anims;
	}
}

/***************************************************************************
CLASS ENTITY
Member Functions
***************************************************************************/

Entity::Entity() : model()
{
	position = Vector(0, 0);
	velocity = Vector(0, 0);
	acceleration = Vector(0, 0);
	size = Vector(0, 0);
}

Entity::Entity(Vector position, Vector velocity, Vector size, const char *const *modelFiles, int numAnim, int curAnim) : model(modelFiles, numAnim, curAnim)
{
	this->position = position;
	this->velocity = velocity;
	this->acceleration = Vector(0, 0);
	this->size = size;
}

Entity::Entity(Vector position, Vector velocity, Vector size, Model model) : model(model)
{
	this->position = position;
	this->velocity = velocity;
	this->acceleration = Vector(0, 0);
	this->size = size;
}

Entity::Entity(Entity &e) : model(e.model)
{
	position = e.position;
	velocity = e.velocity;
	acceleration = e.acceleration;
	size = e.size;
}

void Entity::applyForce(Vector force)
{
	acceleration = acceleration + force;
}


void Entity::showCollPol()
{
	setcolor(RED);
	rectangle(position.retX()-size.retX()/2, position.retY()-size.retY()/2, position.retX()+size.retX()/2, position.retY()+size.retY()/2);
}

void Entity::move(Vector minXY, Vector maxXY)
{
	position = position + velocity;
	velocity = velocity + acceleration;
	acceleration = Vector(0.0f, 0.0f);
	position.restrict(minXY, maxXY);
}

void Entity::operator=(Entity e)
{
	position = e.position;
	velocity = e.velocity;
	acceleration = e.acceleration;
	size = e.size;
	model = Model(e.model);
}

void Entity::show()
{
	if (model.retAnim() == -1)
	{
		model.setAnim(-1);
	}
	model.show(position);
}

Bool Entity::collision(Entity e1, Entity e2)
{
	Vector diff = e1.position - e2.position;
	diff = Vector::positive(diff);
	Vector maxDiff = e1.size + e2.size;
	maxDiff = Vector::scale(maxDiff, Vector(0.5f, 0.5f));
	if (diff.retX() <= maxDiff.retX() && diff.retY() <= maxDiff.retY())
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

Bool Entity::outOfBounds(Vector minXY, Vector maxXY)
{
	if (position.retX() < minXY.retX() || position.retY() < minXY.retY() || position.retY() > maxXY.retY() || position.retX() > maxXY.retX())
		return TRUE;
	else
		return FALSE;
}

/**************************************************************************
CLASS DINO
Member Functions
**************************************************************************/

Dino::Dino() : Entity()
{
	jumpForce = Vector(0, -37);
	state = JUMPING;
}

Dino::Dino(Vector position, Vector velocity, Vector size, const char *const *modelFiles, int numAnim, int curAnim, Vector jumpForce) : Entity(position, velocity, size, modelFiles, numAnim, curAnim)
{
	this->jumpForce = jumpForce;
	this->state = JUMPING;
}

Dino::Dino(Dino &d) : Entity(d.position, d.velocity, d.size, d.model)
{
	jumpForce = Vector(d.jumpForce);
	state = d.state;
}

void Dino::die()
{
	state = DEAD;
}

Bool Dino::isDead()
{
	return ((state == DEAD)?TRUE:FALSE);
}

void Dino::show(long int tick)
{
	if (state == DEAD)
	{
		model.setAnim(DEAD_ANIM);
	}
	else if (state == JUMPING)
	{
		model.setAnim(JUMP_ANIM);
	}
	else if (state == RUNNING)
	{
		if (!(tick%3))
		{
			if (model.retAnim() == RUN_ANIM_A || model.retAnim() == JUMP_ANIM)
			{
				model.setAnim(RUN_ANIM_B);
			}
			else if (model.retAnim() == RUN_ANIM_B)
			{
				model.setAnim(RUN_ANIM_A);
			}
		}
	}
	Entity::show();
}

void Dino::move(Vector minXY, Vector maxXY, Vector gravity)
{
	if (state != DEAD)
	{
		applyForce(gravity);

		Entity::move(minXY, maxXY);

		if (position.retY() >= maxXY.retY() && velocity.retY() >= 0)
		{
			state = RUNNING;
			velocity = Vector(0.0f, 0.0f);
		}
	}
}

void Dino::jump()
{
	if (state != DEAD && state != JUMPING)
	{
		applyForce(jumpForce);
		state = JUMPING;
	}
}

/***************************************************************************
CLASS CACTUS
Member Functions
***************************************************************************/
Spike::Spike()
{
	packSize = 0;
	cacti = NULL;
}

Spike::Spike(int packSize, Vector position, Vector velocity, Vector size, Model model, int numAnim)
{
	this->packSize = packSize;
	cacti = new Entity[(this->packSize)];
	for (int i=0; i<this->packSize; i++)
	{
		model.setAnim(random(numAnim));
		Vector pos = Vector(position.retX() + (size.retX()/2.0f * (2*i + 1 - this->packSize)), position.retY());
		cacti[i] = Entity(pos, velocity, size, model);
	}
}

Spike::Spike(Spike &s)
{
	packSize = s.packSize;
	cacti = s.cacti;
}

void Spike::show()
{
	for (int i=0; i<packSize; i++)
	{
		cacti[i].show();
	}
}

Spike::~Spike()
{
	delete []cacti;
}

void Spike::showCollPol()
{
	for (int i=0; i<packSize; i++)
	{
		cacti[i].showCollPol();
	}
}

Bool Spike::outOfBounds(Vector minXY, Vector maxXY)
{
	if (cacti[packSize-1].outOfBounds(minXY, maxXY) == TRUE)
		return TRUE;
	else
		return FALSE;
}

void Spike::move()
{
	for (int i=0; i<packSize; i++)
	{
		cacti[i].move(Vector(-1000.0f, -1000.0f), Vector (1000.0f, 1000.0f));
	}
}

Bool Spike::collide(Dino &dino)
{
	for (int i=0; i<packSize; i++)
	{
		if (Entity::collision(cacti[i], dino) == TRUE)
			return TRUE;
	}
	return FALSE;
}

/***************************************************************************
CLASS SPIKENODE
Member Functions
***************************************************************************/

SpikeNode::SpikeNode(int packSize, Vector position, Vector velocity, Vector size, Model model, int curAnim) : spike(packSize, position, velocity, size, model, curAnim)
{
	next = NULL;
}

SpikeNode::SpikeNode(Spike s) : spike(s)
{
	next = NULL;
}

Spike *SpikeNode::retSpike()
{
	return &spike;
}

/**************************************************************************
CLASS HUD_ELEMENT
Member Functions
**************************************************************************/
HUD_Element::HUD_Element()
{
	position = Vector(0, 0);
	label = "\0";
	type = HUD_INVALID;
	state = HUD_DEFAULT;
	font = DEFAULT_FONT;
	fontSize = 1.0f;
}

HUD_Element::HUD_Element(Vector position, const char *label, HUD_Type type, HUD_State state, int font, float fontSize)
{
	this->position = position;
	this->label = label;
	this->type = type;
	this->state = state;
	this->font = font;
	this->fontSize = fontSize;
}

HUD_Element::HUD_Element(HUD_Element &e)
{
	position = e.position;
	label = e.label;
	type = e.type;
	state = e.state;
	font = e.font;
	fontSize = e.fontSize;
}

void HUD_Element::show(Bool labelOnly)
{
	settextjustify(CENTER_TEXT, CENTER_TEXT);
	settextstyle(font, HORIZ_DIR, 0);
	setusercharsize(fontSize*1000, 1000, fontSize*1000, 1000);
	if (type == HUD_TEXT && labelOnly == FALSE)
	{
		setcolor(RED);
		rectangle(position.retX()-textwidth(label)/2-10, position.retY()-textheight(label)/2, position.retX()+textwidth(label)/2+10, position.retY()+textheight(label)/2+20);
	}
	else if (type == HUD_BUTTON && labelOnly == FALSE)
	{
		if (state == HUD_SELECTED)
			setfillstyle(SOLID_FILL, RED);
		else if (state == HUD_DEFAULT)
			setfillstyle(SOLID_FILL, GREEN);
		bar(position.retX()-textwidth(label)/2-10, position.retY()-textheight(label)/2, position.retX()+textwidth(label)/2+10, position.retY()+textheight(label)/2+10);
	}
	setcolor(YELLOW);
	outtextxy(position.retX(), position.retY(), label);
}

void HUD_Element::setPos(Vector &position)
{
	this->position = position;
}

void HUD_Element::select()
{
	state = HUD_SELECTED;
}

void HUD_Element::deselect()
{
	state = HUD_DEFAULT;
}

void HUD_Element::setFontSize(float fontSize)
{
	this->fontSize = fontSize;
}

Bool HUD_Element::isSelected()
{
	return (state == HUD_SELECTED ? TRUE : FALSE);
}

void HUD_Element::operator=(HUD_Element e)
{
	position = e.position;
	label = e.label;
	type = e.type;
	state = e.state;
	font = e.font;
	fontSize = e.fontSize;
}

/**************************************************************************
CLASS PLAYER
MEMBER FUNCTIONS
**************************************************************************/
Player::Player()
{
	for (int i=0; i<12; i++)
	{
		name[i] = '\0';
	}
	score = 0;
}

Player::Player(char *name, long int score)
{
	for (int i=0; i < 11 && name[i] != '\0'; i++)
	{
		this->name[i] = name[i];
	}
	while(i<12)
	{
		this->name[i] = '\0';
		i++;
	}
	this->score = score;
}

Player::Player(Player &p)
{
	for (int i=0; p.name[i] != '\0' && i<11; i++)
	{
		name[i] = p.name[i];
	}
	while(i<12)
	{
		name[i] = '\0';
		i++;
	}
	score = p.score;
}

void Player::operator =(Player &p)
{
	for (int i=0; p.name[i] != '\0' && i<11; i++)
	{
		name[i] = p.name[i];
	}
	while(i<12)
	{
		name[i] = '\0';
		i++;
	}
	score = p.score;
}