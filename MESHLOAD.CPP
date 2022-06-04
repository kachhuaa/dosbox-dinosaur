/*if file not found for import, show error, don't quit*/

#include <iostream.h>
#include <conio.h>
#include <graphics.h>
#include <stdlib.h>
#include <dos.h>
#include <fstream.h>

enum State {EMPTY, FILL};
enum Bool {FALSE, TRUE};

struct Point
{
	float x;
	float y;
};

class Pixel
{
private:
	State state;
	Bool selected;
	float x;
	float y;
	float size;
public:
	Pixel(float x, float y, float size);
	Pixel(Pixel &);
	void show();
	void cycleState();
	void changeState(State state);
	void highlight();
	void unhighlight();
	State retState();
	float retX();
	float retY();
	float retSize();
	Pixel *prev;
	Pixel *next;
};

void deletePixels(Pixel *&sheetEnd);
void initGraphMod(int &maxX, int &maxY);
void createSheet(int cols, int rows, float pixelSize, Pixel *&pixelSheet, Pixel *&sheetEnd, Pixel *&selectedPixel, int &quit);
void select(char key, Pixel *&selectedPixel, Pixel *&pixelSheet, Pixel *&sheetEnd, int rows);
void displayScreen(Pixel *pixelSheet, Pixel *selectedPixel);
void exportCoord(Pixel *pixelSheet, int cols, int rows, int &quit, int &maxX, int &maxY);
void importCoord(Pixel *pixelSheet, int cols, int rows, int &maxX, int &maxY, int &quit);
//void sortCoord(char fileName[], int &quit);
//void quickSort(char fileNmae[], int start, int end, int &quit);
//int partition(char fileName[], int start, int end, int &quit);
//void swap(char fileName[], int a, int b, int &quit);
void updatePixels(Pixel *pixelSheet);
void display(char fileName[], int &quit);
void clearSheet(Pixel *pixelSheet);

int main()
{
	clrscr();
	int maxX, maxY;
	int quit = FALSE;
	float pixelSize = 10;
	Pixel *sheetEnd = NULL;
	Pixel *pixelSheet = NULL;
	Pixel *selectedPixel = NULL;
	const int cols = 20, rows = 10;

	initGraphMod(maxX, maxY);
	cleardevice();

	createSheet(cols, rows, pixelSize, pixelSheet, sheetEnd, selectedPixel, quit);
	displayScreen(pixelSheet, selectedPixel);

	while(quit == FALSE)
	{
		if (kbhit())
		{
			char key = getch();
			if (key == 27)
			{
				quit = TRUE;
				continue;

			}
			else if (key == 'd' || key == 'D' || key == 'a' || key == 'A' || key == 'w' || key == 'W' || key == 's' || key == 'S')
			{
				select(key, selectedPixel, pixelSheet, sheetEnd, rows);
			}
			else if (key == ' ')
			{
				selectedPixel->cycleState();
			}
			else if (key == 'e' || key == 'E')
			{
				exportCoord(pixelSheet, cols, rows, quit, maxX, maxY);
			}
			else if (key == 'q' || key == 'Q')
			{
				importCoord(pixelSheet, cols, rows, maxX, maxY, quit);

			}
			else if (key == 'f' || key == 'F')
			{
				clearSheet(pixelSheet);
			}
			displayScreen(pixelSheet, selectedPixel);
		}
		delay(20);
	}

	cleardevice();
	closegraph();
	clrscr();
	deletePixels(sheetEnd);
	cout << "Program Ended..." << endl;
	cout << "Press any key to exit..." << endl;
	getch();
	return 0;
}

void initGraphMod(int &maxX, int &maxY)
{
	int gDriver = DETECT, gMode, errCode;
	initgraph(&gDriver, &gMode, "C:\\TURBOC3\\BGI");
	errCode = graphresult();
	if (errCode != grOk)
	{
		cout << "Graphics Initialization Failed..." << endl;
		cout << grapherrormsg(errCode) << endl;
		cout << "Press any key to exit..." << endl;
		getch();
		exit(0);
	}
	else
	{
		maxX = getmaxx();
		maxY = getmaxy();
	}

}

void createSheet(int cols, int rows, float pixelSize, Pixel *&pixelSheet, Pixel *&sheetEnd, Pixel *&selectedPixel, int &quit)
{
	Pixel *newPixel = NULL;
	for (int i=0; i<cols; i++)
	{
		for (int j=0; j<rows; j++)
		{
			newPixel = new Pixel(40 + i*pixelSize, 30 + j*pixelSize, pixelSize);
			if (newPixel == NULL)
			{
				quit = TRUE;
			}
			else
			{
				if (pixelSheet == NULL)
				{
					pixelSheet = newPixel;
					sheetEnd = newPixel;
					selectedPixel = newPixel;
					selectedPixel->highlight();
				}
				else
				{
					sheetEnd->next = newPixel;
					sheetEnd->next->prev = sheetEnd;
					sheetEnd = newPixel;
				}
			}
		}
	}
}

void clearSheet(Pixel *pixelSheet)
{
	while (pixelSheet != NULL)
	{
		pixelSheet->changeState(EMPTY);
		pixelSheet = pixelSheet->next;
	}
}

void select(char key, Pixel *&selectedPixel, Pixel *&pixelSheet, Pixel *&sheetEnd, int rows)
{
	selectedPixel->unhighlight();
	if (key == 's' || key == 'S')
	{
		if (!(selectedPixel == sheetEnd))
			selectedPixel = selectedPixel->next;
	}
	else if (key == 'w' || key == 'W')
	{
		if (!(selectedPixel == pixelSheet))
			selectedPixel = selectedPixel->prev;
	}
	else if (key == 'a' || key == 'A')
	{
		while (rows>0 && !(selectedPixel == pixelSheet))
		{
			selectedPixel = selectedPixel->prev;
			rows--;
		}
	}
	else if (key == 'd' || key == 'D')
	{
		while (rows>0 && !(selectedPixel == sheetEnd))
		{
			selectedPixel = selectedPixel->next;
			rows--;
		}
	}
	selectedPixel->highlight();
}

void deletePixels(Pixel *&sheetEnd)
{
	Pixel *temp = sheetEnd;
	do
	{
		temp = temp->prev;
		delete sheetEnd;
		sheetEnd = temp;
	} while (temp != NULL);
}

void displayScreen(Pixel *pixelSheet, Pixel *selectedPixel)
{
	cleardevice();
	setcolor (WHITE);
	char *temp;

	Pixel *p = pixelSheet;
	while (p != NULL)
	{
		p->show();
		p = p->next;
	}
	selectedPixel->show();
}

void exportCoord(Pixel *pixelSheet, int cols, int rows, int &quit, int &maxX, int &maxY)
{
	cleardevice();
	closegraph();
	clrscr();
	char choice = '\0';
	do
	{
		cout << "Are you sure you want to save?(Y/N) - ";
		cin>>choice;
		if (choice == 'n' || choice == 'N')
		{
			clrscr();
			initGraphMod(maxX, maxY);
			cleardevice();
			return;
		}
		else if (choice != 'y' && choice != 'Y')
		{
			cin.get(choice);
			cout << "Invalid Response, Please Try Again..." << endl;
			choice = '\0';
		}
	} while (choice != 'y' && choice != 'Y');
	cout << "Save as - ";
	char fileName[20];
	cin.get(choice);
	cin.getline(fileName, 20, '\n');
	//getch();
	ofstream pxFile;
	Point point = {0, 0};
	pxFile.open(fileName, ios::binary|ios::out);
	if (pxFile == NULL)
	{
		quit = TRUE;
	}
	else
	{
		while (pixelSheet != NULL)
		{
			if (pixelSheet->retState() == FILL)
			{
				point.x = (pixelSheet->retX() - 40 - (cols-1)/2.0*pixelSheet->retSize())/pixelSheet->retSize();
				point.y = (pixelSheet->retY() - 30 - (rows-1)/2.0*pixelSheet->retSize())/pixelSheet->retSize();
				pxFile.write((char *)&point, sizeof(point));
			}
			pixelSheet = pixelSheet->next;
		}
		pxFile.close();
		display(fileName, quit);
	}
	clrscr();
	initGraphMod(maxX, maxY);
	cleardevice();
}

void importCoord(Pixel *pixelSheet, int cols, int rows, int &maxX, int &maxY, int &quit)
{
	cleardevice();
	closegraph();
	clrscr();
	char choice = '\0';
	do
	{
		cout << "Are you sure you want to import?(Y/N) - ";
		cin>>choice;
		if (choice == 'n' || choice == 'N')
		{
			clrscr();
			initGraphMod(maxX, maxY);
			cleardevice();
			return;
		}
		else if (choice != 'y' && choice != 'Y')
		{
			cin.get(choice);
			cout << "Invalid Response, Please Try Again..." << endl;
			choice = '\0';
		}
	} while (choice != 'y' && choice != 'Y');
	cout << "Import from File - ";
	char fileName[20];
	cin.get(choice);
	ifstream pxFile;
	Point point = {0, 0};
	cin.getline(fileName, 20, '\n');
	pxFile.open(fileName, ios::binary|ios::in);
	choice = '\0';
	while (pxFile.fail() || pxFile.bad())
	{
		cout << "Invalid File Name..." << endl;

		while (choice != 'y' && choice != 'Y')
		{
			cout << "Continue(Y/N)? - " << endl;
			cin >> choice;
			if (choice == 'n' || choice == 'N')
			{
				clrscr();
				initGraphMod(maxX, maxY);
				cleardevice();
				return;
			}
			else if (choice != 'y' && choice != 'Y')
			{
				cin.get(choice);
				cout << "Invalid Response...";
				choice = '\0';
			}
		}
		cout << "Import from File - ";
		cin.get(choice);
		cin.getline(fileName, 20, '\n');
		pxFile.open(fileName, ios::binary|ios::in);
	}

	if (pxFile == NULL)
	{
		quit = TRUE;
	}
	else
	{
		clearSheet(pixelSheet);
		while (!pxFile.eof())
		{
			Pixel *curPixel = pixelSheet;
			pxFile.read((char *)&point, sizeof(point));
			if (pxFile.eof())
			{
				break;
			}
			float coordX = (point.x + (cols-1)/2.0f)*pixelSheet->retSize() + 40;
			float coordY = (point.y + (rows-1)/2.0f)*pixelSheet->retSize() + 30;
			while (curPixel != NULL)
			{
				if (curPixel->retX() == coordX && curPixel->retY() == coordY)
				{
					curPixel->changeState(FILL);
				}
				curPixel = curPixel->next;
			}
		}
		pxFile.close();
	}
	clrscr();
	initGraphMod(maxX, maxY);
	cleardevice();
}

void display(char fileName[], int &quit)
{
	cout << "Texture Details - " << endl;
	cout << "X Coordinate" << "\t\t      " << "Y Coordinate" << endl;
	ifstream pxFile;
	pxFile.open(fileName, ios::binary|ios::in);
	if (pxFile == NULL)
	{
		quit = TRUE;
	}
	Point point;
	while(!pxFile.eof())
	{
		pxFile.read((char *)&point, sizeof(point));
		if (pxFile.eof())
			break;
		cout << "    " << point.x << "  \t\t\t" << point.y << endl;
	}
	pxFile.close();
	getch();
}

/*void sortCoord(char fileName[], int &quit)
{
	fstream pxFile;
	int numPoints = 0;
	pxFile.open(fileName, ios::binary|ios::in|ios::out|ios::ate);
	if (pxFile == NULL)
	{
		quit = TRUE;
	}
	else
	{
		pxFile.seekg(0*sizeof(Point), ios::end);
		numPoints = pxFile.tellg()/sizeof(Point);
		pxFile.close();
		quickSort(fileName, 0, numPoints-1, quit);
	}
}

void quickSort(char fileName[], int start, int end, int &quit)
{
	if (start >= end)
		return;
	int index = partition(fileName, start, end, quit);
	if (index == -1)
	{
		quit = TRUE;
		return;
	}
	quickSort(fileName, start, index-1, quit);
	quickSort(fileName, index+1, end, quit);
}

int partition(char fileName[], int start, int end, int &quit)
{
	int index = start;
	Point pivot, curPoint;
	fstream pxFile;
	int open = FALSE;
	pxFile.open(fileName, ios::in|ios::out|ios::binary|ios::ate);
	if (pxFile == NULL)
	{
		quit = TRUE;
	}
	else
	{
		open = TRUE;
		pxFile.seekg(end*sizeof(Point), ios::beg);
		pxFile.read((char *)&pivot, sizeof(pivot));

		for (int i=start; i<end; i++)
		{
			if (open == FALSE)
			{
				pxFile.open(fileName, ios::in|ios::out|ios::binary|ios::ate);
				if (pxFile == NULL)
				{
					quit = TRUE;
					return -1;
				}
				else
				{
					open = TRUE;
				}
			}
			pxFile.seekg(i*sizeof(Point), ios::beg);
			pxFile.read((char *)&curPoint, sizeof(curPoint));
			if (curPoint.pos <= pivot.pos)
			{
				pxFile.close();
				open = FALSE;
				swap(fileName, i, index, quit);
				index++;
			}
		}
		if (open == TRUE)
			pxFile.close();
		swap(fileName, end, index, quit);
		return index;
	}
	return -1;
}

void swap(char fileName[], int a, int b, int &quit)
{
	Point first, second;
	fstream pxFile;
	pxFile.open(fileName, ios::in|ios::out|ios::binary|ios::ate);
	if (pxFile == NULL)
	{
		quit = TRUE;
	}
	else
	{
		pxFile.seekg(a*sizeof(Point), ios::beg);
		pxFile.read((char *)&first, sizeof(first));
		pxFile.seekg(b*sizeof(Point), ios::beg);
		pxFile.read((char *)&second, sizeof(second));
		pxFile.seekp(a*sizeof(Point), ios::beg);
		pxFile.write((char *)&second, sizeof(second));
		pxFile.seekp(b*sizeof(Point), ios::beg);
		pxFile.write((char *)&first, sizeof(first));
		pxFile.close();
	}
}

*/

Pixel::Pixel(float x, float y, float size)
{
	this->x = x;
	this->y = y;
	this->selected = FALSE;
	this->size = size;
	this->state = EMPTY;
	this->next = NULL;
	this->prev = NULL;
}

Pixel::Pixel(Pixel &p)
{
	x = p.x;
	y = p.y;
	selected = p.selected;
	size = p.size;
	state = p.state;
	next = p.next;
	prev = p.prev;
}

void Pixel::changeState(State state)
{
	this->state = state;
}

void Pixel::show()
{
	if (state == FILL)
	{
		setfillstyle(SOLID_FILL, WHITE);
	}
	else if (state == EMPTY)
	{
		setfillstyle(SOLID_FILL, BLACK);
	}
	else
	{
		setfillstyle(SOLID_FILL, GREEN);
	}

	(selected) ? setcolor(RED) : setcolor(WHITE);

	bar(x-size/2, y-size/2, x+size/2, y+size/2);
	rectangle(x-size/2, y-size/2, x+size/2, y+size/2);
}

void Pixel::cycleState()
{
	if (this->state == EMPTY)
	{
		changeState(FILL);
	}
	else if (this->state == FILL)
	{
		changeState(EMPTY);
	}
}

void Pixel::highlight()
{
	selected = TRUE;
}

void Pixel::unhighlight()
{
	selected = FALSE;
}

float Pixel::retY()
{
	return y;
}

float Pixel::retX()
{
	return x;
}

State Pixel::retState()
{
	return state;
}

float Pixel::retSize()
{
	return size;
}