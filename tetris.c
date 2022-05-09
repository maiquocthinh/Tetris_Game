#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <sys/time.h>
#include <windows.h>

#define TRUE 1
#define FALSE 0

#define LEFT_KEY 75 // ma ASCII tuong ung voi cac phim bam
#define RIGHT_KEY 77
#define UP_KEY 72
#define DOWN_KEY 80
#define ESCAPE_KEY 27
#define PAUSE_KEY 112
#define ROTATE_KEY UP_KEY

// Toa do cua Game Board, Next Shape, Score
#define BOARD_COORD_X 0
#define BOARD_COORD_Y 0
#define NEXT_COORD_X 30
#define NEXT_COORD_Y 0
#define SCORE_COORD_X 30
#define SCORE_COORD_Y 10

// Chieu rong va chieu cao cua Game Board
#define TETRIS_BOARD_WIDTH 10
#define TETRIS_BOARD_HEIGHT 20

#define DEFAULT_TIMER 500000 // 500000 microseconds = 1/2 second

// Dinh nghia struct Shape (Tetromino)
typedef struct
{
	char **shape_matrix;  // stores the shape of the tetromino in a char matrix
	unsigned short width; // width of the tetromino
	short x, y;			  // x and y coordinates of the location of tetromino
} Shape;

// Dinh nghia struct Tetris
typedef struct
{
	char board[TETRIS_BOARD_HEIGHT][TETRIS_BOARD_WIDTH]; // Ma tran Game Board
	unsigned int score, timer;							 // Diem va thoi gian (Game Tick)
} Tetris;

// 7 khoi tetromino cua game Tetris
const Shape Tetrominos[7] = {
	{.shape_matrix = (char *[]){(char[]){0, 0, 0, 0},
								(char[]){1, 1, 1, 1},
								(char[]){0, 0, 0, 0},
								(char[]){0, 0, 0, 0}},
	 .width = 4,
	 .x = 0,
	 .y = 0}, // I tetromino
	{.shape_matrix = (char *[]){(char[]){1, 1},
								(char[]){1, 1}},
	 .width = 2,
	 .x = 0,
	 .y = 0}, // O tetromino
	{.shape_matrix = (char *[]){(char[]){0, 1, 0},
								(char[]){1, 1, 1},
								(char[]){0, 0, 0}},
	 .width = 3,
	 .x = 0,
	 .y = 0}, // T tetromino
	{.shape_matrix = (char *[]){(char[]){0, 1, 1},
								(char[]){1, 1, 0},
								(char[]){0, 0, 0}},
	 .width = 3,
	 .x = 0,
	 .y = 0}, // S tetromino
	{.shape_matrix = (char *[]){(char[]){1, 1, 0},
								(char[]){0, 1, 1},
								(char[]){0, 0, 0}},
	 .width = 3,
	 .x = 0,
	 .y = 0}, // Z tetromino
	{.shape_matrix = (char *[]){(char[]){0, 0, 1},
								(char[]){1, 1, 1},
								(char[]){0, 0, 0}},
	 .width = 3,
	 .x = 0,
	 .y = 0}, // J tetromino
	{.shape_matrix = (char *[]){(char[]){1, 0, 0},
								(char[]){1, 1, 1},
								(char[]){0, 0, 0}},
	 .width = 3,
	 .x = 0,
	 .y = 0} // L tetromino
};

Shape *current, *next;
Tetris tetris;
unsigned short game_on_flag = TRUE;

void MoveCursorToXY(unsigned short x, unsigned short y);
void OpeningScreen();
void InstructionsScreen();
void LoadingScreen();
void UpdateFrame();
void PrintGamePlayUI();
void PrintShapeToConsole();
void EraseShapeFromConsole();
void WriteShapeToBoard();
void DeleteShapeFromBoard();
void GetNewShape();
Shape *CopyShape(const Shape shape);
void DeleteShape(Shape *shape);
unsigned short CheckPosition(Shape *shape);
void CheckRows();
void RotateShape(Shape *shape);
void ControlCurrentShape(char key);
void RecordScore();
void ViewRecordScore();
void StartGame();
void PlayTetris();

// di chuyen con tro chuot den toa do x,y tren man hinh console
void MoveCursorToXY(unsigned short x, unsigned short y)
{
	COORD coord = (COORD){x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// set mau cho chu
void SetColor(int a)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(handle, a);
}

// man hinh mo dau Game
void OpeningScreen()
{
	system("cls");
	int i;
	int key_input;
	SetColor(9);
	MoveCursorToXY(1, 2);
	printf("%c", 218);
	for (i = 0; i < 50; i++)
		printf("%c", 196);
	printf("%c", 191);
	MoveCursorToXY(1, 3);
	printf("%c", 179);
	SetColor(4);
	printf("                   T E T R I S                    ");
	SetColor(9);
	printf("%c", 179);
	MoveCursorToXY(1, 4);
	printf("%c", 192);
	for (i = 0; i < 50; i++)
		printf("%c", 196);
	printf("%c", 217);

	SetColor(14);
	MoveCursorToXY(6, 6);
	printf("[*] Press 's' to Start Game");
	MoveCursorToXY(6, 7);
	printf("[*] Press 'v' to View Recorded Scores");
	MoveCursorToXY(6, 8);
	printf("[*] Press 'i' to Instructions");
	MoveCursorToXY(6, 9);
	printf("[*] Press 'Esc' to Exit Game");
	MoveCursorToXY(0, 11);
	SetColor(7);
	while (1)
	{
		switch (getch())
		{
		case ESCAPE_KEY:
			exit(0);
		case 's':
		case 'S':
			StartGame();
			break;
		case 'v':
		case 'V':
			ViewRecordScore();
			OpeningScreen();
			break;
		case 'i':
		case 'I':
			InstructionsScreen();
			OpeningScreen();
			break;
		}
	}

	system("cls");
}

// huong dan game
void InstructionsScreen()
{
	system("cls");
	printf("Welcome to the Tetris game!"
		   "\n\n\nGame instructions:"
		   "\n\n<-> Use left and right arrow keys to move blocks across the screen, down arrow key to bring them down faster, and the up arrow key to rotate them."
		   "\n\n<-> Your objective is to get all the blocks to fill all the empty space in a row at the bottom of the screen. Thus, the filled row will vanish and you get awarded 100 points."
		   "\n\n<-> Your game is over if a block reaches the top of the screen."
		   "\n\n<-> You can pause the game in its middle by pressing the P key. To continue the paused game press P once again."
		   "\n\n<-> If you want to exit the game at any point press Esc (you will lose all progress)."
		   "\n\n\nPress any key to continue...");
	if (getch() == ESCAPE_KEY)
		exit(0);
	system("cls");
}

// in ra man hinh hieu ung loading
void LoadingScreen()
{
	SetColor(11);
	MoveCursorToXY(16, 14);
	printf("Loading...");
	MoveCursorToXY(10, 15);
	unsigned short i;
	for (i = 1; i <= 20; i++)
	{
		Sleep(100);
		printf("%c", 178);
	}
	system("cls");
	SetColor(7);
}

// cap nhat frame (khung hinh) cua game Tetris
void UpdateFrame()
{
	struct timeval before, after;
	gettimeofday(&before, NULL);
	do
	{
		if (kbhit()) // neu nguoi dung bam bat ky phim nao
		{
			ControlCurrentShape(getch()); // FPS tang dan, sau khi 1 hang full
		}
		gettimeofday(&after, NULL);
		if ((unsigned int)(after.tv_sec * 1000000 + after.tv_usec - before.tv_sec * 1000000 - before.tv_usec) > tetris.timer) // neu khoang cach giua before và after lon hon tetris.timer ?
		{
			before = after;
			ControlCurrentShape(DOWN_KEY); // khoi tetromino roi xuong 1 don vi
		}
	} while (game_on_flag);
	system("cls");
	printf("Game over!" // game over
		   "\nYour Score: %u"
		   "\n(Press C to continue...)",
		   tetris.score);
	char key;
	do
	{
		key = getch();
		if (key == ESCAPE_KEY) // neu bam Esc, thoat chuong trinh
			exit(0);
	} while (key != 'c' && key != 'C'); // lap cho den khi bam 'c' -> ket thuc ham nay
	system("cls");						// xoa man hinh Console
}

// in ra khung Game Board, o chua hinh dang tiep theo va diem cua nguoi choi
void PrintGamePlayUI()
{
	unsigned short i;
	// prints score box
	MoveCursorToXY(SCORE_COORD_X, SCORE_COORD_Y);
	printf("%c", 201);
	for (i = 0; i < 12; i++)
		printf("%c", 196);
	printf("%c", 187);
	MoveCursorToXY(SCORE_COORD_X, SCORE_COORD_Y + 1);
	printf("%c", 179);
	printf("SCORE: %u", tetris.score);
	MoveCursorToXY(SCORE_COORD_X + 13, SCORE_COORD_Y + 1);
	printf("%c", 179);
	MoveCursorToXY(SCORE_COORD_X, SCORE_COORD_Y + 2);
	printf("%c", 200);
	for (i = 0; i < 12; i++)
		printf("%c", 196);
	printf("%c", 188);

	// prints board of Tetris game
	MoveCursorToXY(BOARD_COORD_X, BOARD_COORD_Y);
	printf("%c", 201);
	for (i = 0; i < 2 * TETRIS_BOARD_WIDTH; i++)
		printf("%c", 205);
	printf("%c", 187);
	for (i = 0; i < TETRIS_BOARD_HEIGHT; i++)
	{
		MoveCursorToXY(BOARD_COORD_X, BOARD_COORD_Y + 1 + i);
		printf("%c", 186);
		MoveCursorToXY(BOARD_COORD_X + 1 + 2 * TETRIS_BOARD_WIDTH, BOARD_COORD_Y + 1 + i);
		printf("%c", 186);
	}
	MoveCursorToXY(BOARD_COORD_X, BOARD_COORD_Y + 1 + TETRIS_BOARD_HEIGHT);
	printf("%c", 200);
	for (i = 0; i < 2 * TETRIS_BOARD_WIDTH; i++)
		printf("%c", 205);
	printf("%c", 188);

	// prints next shape board
	MoveCursorToXY(NEXT_COORD_X, NEXT_COORD_Y);
	printf("%c", 201);
	for (i = 0; i < 2 * 4; i++)
		printf("%c", 196);
	printf("%c", 187);
	MoveCursorToXY(NEXT_COORD_X, NEXT_COORD_Y + 1);
	printf("%c  NEXT  %c", 186, 186);
	MoveCursorToXY(NEXT_COORD_X, NEXT_COORD_Y + 2);
	printf("%c", 204);
	for (i = 0; i < 2 * 4; i++)
		printf("%c", 205);
	printf("%c", 185);
	for (i = 0; i < 3; i++)
	{
		MoveCursorToXY(NEXT_COORD_X, NEXT_COORD_Y + 2 + 1 + i);
		printf("%c", 186);
		MoveCursorToXY(NEXT_COORD_X + 1 + 2 * 4, NEXT_COORD_Y + 2 + 1 + i);
		printf("%c", 186);
	}
	MoveCursorToXY(NEXT_COORD_X, NEXT_COORD_Y + 2 + 4);
	printf("%c", 200);
	for (i = 0; i < 2 * 4; i++)
		printf("%c", 205);
	printf("%c", 188);
}

// in hinh dang ke tiep
void PrintNextShapeToConsole()
{
	unsigned short i, j;
	// clear box next shape before print next shape
	for (i = 0; i < 3; i++)
		for (j = 0; j < 4; j++)
		{
			MoveCursorToXY(NEXT_COORD_X + 1 + 2 * j, NEXT_COORD_Y + 2 + 1 + i);
			printf(" ");
		}

	// print next shape
	for (i = 0; i < next->width; i++)
		for (j = 0; j < next->width; j++)
			if (next->shape_matrix[i][j])
			{
				MoveCursorToXY(NEXT_COORD_X + 1 + 2 * j, NEXT_COORD_Y + 2 + 1 + i);
				printf("%c", 254);
			}
}

// in mang Game Board duoc luu tru ra man hinh Console
void PrintShapeToConsole()
{
	unsigned short i, j;
	for (i = 0; i < TETRIS_BOARD_HEIGHT; i++)
		for (j = 0; j < TETRIS_BOARD_WIDTH; j++)
			if (tetris.board[i][j])
			{
				MoveCursorToXY(BOARD_COORD_X + 1 + 2 * j, BOARD_COORD_Y + 1 + i);
				printf("%c", 254);
			}
}

// xoa Shape khoi man hinh Console
void EraseShapeFromConsole()
{
	unsigned short i, j;
	for (i = 0; i < current->width; i++)
		for (j = 0; j < current->width; j++)
			if (current->shape_matrix[i][j])
			{
				MoveCursorToXY(BOARD_COORD_X + 1 + 2 * (j + current->x), BOARD_COORD_Y + 1 + i + current->y);
				printf(" ");
			}
}

// ghi mang Shape vao mang Game Board bang so 1
void WriteShapeToBoard()
{
	unsigned short i, j;
	for (i = 0; i < current->width; i++)
		for (j = 0; j < current->width; j++)
			if (current->shape_matrix[i][j])
				tetris.board[current->y + i][current->x + j] = 1;
}

// xoa mang Shape khoi mang Game Board bang so 0
void DeleteShapeFromBoard()
{
	unsigned short i, j;
	for (i = 0; i < current->width; i++)
		for (j = 0; j < current->width; j++)
			if (current->shape_matrix[i][j])
				tetris.board[current->y + i][current->x + j] = 0;
}

// selects a random tetromino
void GetNewShape()
{
	if (current == NULL)
	{
		current = CopyShape(Tetrominos[rand() % 7]);
		next = CopyShape(Tetrominos[rand() % 7]);
	}
	else
	{
		current = next;
		next = CopyShape(Tetrominos[rand() % 7]);
	}
	current->x = (TETRIS_BOARD_WIDTH - current->width + 1) / 2; // set coordinate x of shape is center when it first appeared
	// kiem tra vi tri cua khoi Tetromino moi lay random
	if (!CheckPosition(current)) // neu vi tri cham thanh phia tren cua Game Board
		game_on_flag = FALSE;	 // game over
}

// nhan ban mot Shape rieng biet
Shape *CopyShape(const Shape shape)
{
	Shape *copy = (Shape *)malloc(sizeof(Shape)); // cap phat bo nho cho bien copy
	copy->width = shape.width;					  // gian cac gia tri cua bien shape cho bien copy
	copy->y = shape.y;
	copy->x = shape.x;
	copy->shape_matrix = (char **)malloc(copy->width * sizeof(char *));
	unsigned short i, j;
	for (i = 0; i < copy->width; i++)
	{
		copy->shape_matrix[i] = (char *)malloc(copy->width * sizeof(char));
		for (j = 0; j < copy->width; j++)
			copy->shape_matrix[i][j] = shape.shape_matrix[i][j];
	}
	return copy;
}

// xoa bien struct Shape va giai phong bo nho
void DeleteShape(Shape *shape)
{
	unsigned short i;
	for (i = 0; i < shape->width; i++)
		free(shape->shape_matrix[i]);
	free(shape->shape_matrix);
	free(shape);
}

// kiem tra vi tri cua khoi tetromino (xem co ra ngoai Game Board hay trung voi cac khoi khac?)
unsigned short CheckPosition(Shape *shape)
{
	unsigned short i, j;
	for (i = 0; i < shape->width; i++)
		for (j = 0; j < shape->width; j++)
			if (shape->shape_matrix[i][j])
			{
				if (shape->x + j < 0 || shape->x + j >= TETRIS_BOARD_WIDTH || shape->y + i >= TETRIS_BOARD_HEIGHT) // neu ra khoi ranh gio cua Game Board
					return FALSE;
				else if (tetris.board[shape->y + i][shape->x + j]) // neu mot khoi khac chiem vi tri nhat dinh
					return FALSE;
			}
	return TRUE;
}

// kiem tra xem hang full chua?
// neu co thi cong 100 diem cho moi hang full
void CheckRows()
{
	unsigned short i, j, counter = 0; // bien counter se dem so hang full (đe tinh diem sau khi check xong)
	for (i = 0; i < TETRIS_BOARD_HEIGHT; i++)
	{
		unsigned short sum = 0;
		// tinh tong cac phan tu cua hang thu i
		for (j = 0; j < TETRIS_BOARD_WIDTH; j++)
			sum += tetris.board[i][j];
		if (sum == TETRIS_BOARD_WIDTH) // neu tong bang chieu rong cua Game Board => hang full
		{
			counter++;
			// xoa hang full bang " "
			for (j = 0; j < TETRIS_BOARD_WIDTH; j++)
			{
				MoveCursorToXY(BOARD_COORD_X + 1 + 2 * j, BOARD_COORD_Y + 1 + i);
				printf(" ");
			}
			// di chuyen cac khoi phia tren hang full xuong duoi
			unsigned short k;
			for (k = i; k >= 1; k--)
				for (j = 0; j < TETRIS_BOARD_WIDTH; j++)
				{
					// xoa cac chi tiet thua sau khi di chuyen xuong
					if (!tetris.board[k - 1][j] && tetris.board[k][j])
					{
						MoveCursorToXY(BOARD_COORD_X + 1 + 2 * j, BOARD_COORD_Y + 1 + k);
						printf(" ");
					}
					// di chuyen xuong (cac phan tu cua mang)
					tetris.board[k][j] = tetris.board[k - 1][j];
				}
		}
	}
	tetris.timer -= 1000;								  // giam thoi gian game tick, lam khoi tetromino roi nhanh hon
	tetris.score += 100 * counter;						  // cong diem (voi 1 hang la 100 diem)
	MoveCursorToXY(SCORE_COORD_X + 8, SCORE_COORD_Y + 1); // update diem vao box score
	printf("%u", tetris.score);
}

// quay khoi tetromino theo chieu kim dong ho
void RotateShape(Shape *shape)
{
	Shape *temp = CopyShape(*shape); // tao 1 bien tam thoi
	unsigned short i, j;
	for (i = 0; i < shape->width; i++)
		for (j = 0; j < shape->width; j++)
			shape->shape_matrix[i][j] = temp->shape_matrix[shape->width - j - 1][i];
	DeleteShape(temp); // xoa va giai phong bien tam thoi
}

// xu ly cac thao tac nhan cua nguoi choi
void ControlCurrentShape(char key)
{
	Shape *temp = CopyShape(*current); // sao chep ra 1 bien tam thoi de kiem tra truoc vi tri
	switch (key)
	{
	case LEFT_KEY: // neu ban phim mui ten trai, khoi tetromino di chuyen sang trai
		EraseShapeFromConsole();
		temp->x--;
		if (CheckPosition(temp)) // neu khoi tetromino (tam thoi) ko bi can tro boi (cac thanh Game Board hay cac khoi khac)
			current->x--;		 // di chuyen khoi tetromino hien tai sang trai 1 don vi
		break;
	case RIGHT_KEY: // neu ban phim mui ten phai, khoi tetromino di chuyen sang phai
		EraseShapeFromConsole();
		temp->x++;
		if (CheckPosition(temp))
			current->x++; // di chuyen khoi tetromino hien tai sang phai 1 don vi
		break;
	case DOWN_KEY: // neu bam phim mui ten xuong, khoi tetromino roi xuong duoi 1 bac
		EraseShapeFromConsole();
		temp->y++;
		if (CheckPosition(temp)) // neu khoi tetromino chua cham day
			current->y++;		 // di chuyen khoi tetromino xuong duoi 1 don vi
		else					 // neu khoi tetromino da cham day
		{
			WriteShapeToBoard();	   // cap nhat toa do cua tetromino vao mang Game Board
			CheckRows();			   // kiem tra, tinh diem
			GetNewShape();			   // tao khoi tetromino moi
			PrintNextShapeToConsole(); // hien thi khoi tetromino tiep theo
		}
		break;
	case ROTATE_KEY:			  // neu bam phim mui ten len, xoay khoi tetromino
		EraseShapeFromConsole();  // xoa hinh anh cua khoi tetromino hien tai khoi man hinh Console
		RotateShape(temp);		  // xoay khoi tetromino (tam thoi)
		if (CheckPosition(temp))  // kiem tra khoi tetromino sau khi xoay
			RotateShape(current); // xoay khoi tetromino hien tai
		break;
	case PAUSE_KEY: // tam dung game
		do
		{
			key = getch();
			if (key == ESCAPE_KEY) // neu nham Esc thi thoat chuong trinh
			{
				MoveCursorToXY(0, BOARD_COORD_Y + 1 + TETRIS_BOARD_HEIGHT);
				exit(0);
			}
		} while (key != PAUSE_KEY); // neu nham 'p' mot lan nua thi quay tro lai game
		break;
	case ESCAPE_KEY: // thoat game
		MoveCursorToXY(0, BOARD_COORD_Y + 1 + TETRIS_BOARD_HEIGHT);
		exit(0);
	}
	DeleteShape(temp);		// xoa va giai phong bo nho cho bien tam thoi
	WriteShapeToBoard();	// cap nhat toa do cua khoi tetromino vao mang Game Board
	PrintShapeToConsole();	// in mang Game Board ra man hinh Console
	DeleteShapeFromBoard(); // xoa toa do cua khoi teromino ra khoi mang Game Board
}

// ghi lai diem va cac thong tin cua nguoi choi
void RecordScore()
{
	time_t mytime = time(NULL); // khai bao bien mytime de hien thi ngay gio hien tai
	char player_name[200];
	printf("Enter your name:\n");
	gets(player_name);
	// mo file voi quyen doc va ghi
	FILE *info = fopen("./tetris_record.txt", "a+");
	// ghi file
	fprintf(info, "Player Name: %s\n", player_name);
	fprintf(info, "Played Date: %s", ctime(&mytime)); // dung ctime de chuyen doi sang dang ky tu
	fprintf(info, "Score: %u\n", tetris.score);
	fprintf(info, "__________________________________________________\n");
	fclose(info); // dong file
	system("cls");
	// nhap lua chon
	printf("Press Y to see past records."
		   "\nPress R to play again."
		   "\nPress M to return to the menu."
		   "\nPress any other key to exit.");
	char key = getch();
	switch (key)
	{
	case 'Y':
	case 'y':
		ViewRecordScore();
		game_on_flag = TRUE;
		OpeningScreen();
		break;
	case 'R':
	case 'r':
		game_on_flag = TRUE;
		StartGame();
		break;
	case 'M':
	case 'm':
		game_on_flag = TRUE;
		OpeningScreen();
		break;
	}
	exit(0);
}

// doc danh sach nguoi choi da ghi truoc do
void ViewRecordScore()
{
	system("cls");
	// mo file voi quyen doc
	FILE *info = fopen("./tetris_record.txt", "r");
	if (info != NULL)
	{
		char ch;
		while ((ch = fgetc(info)) != EOF)
			printf("%c", ch);
	}
	fclose(info);
	system("pause"); // tam dung man hinh
}

// bat dau choi
void StartGame()
{
	system("cls");								// xoa man hinh
	LoadingScreen();							// man hinh load
	tetris = (Tetris){{{0}}, 0, DEFAULT_TIMER}; // khoi tao game moi
	PrintGamePlayUI();							// in man hinh game play
	GetNewShape();								// lay khoi tetromino moi
	PrintNextShapeToConsole();					// hien thi khoi tetromino tiep theo
	UpdateFrame();								// cap nhat khung hinh
	RecordScore();								// ghi lai diem cua nguoi choi
}

// Ham chinh game Tetris
void PlayTetris()
{
	OpeningScreen();
}

int main()
{
	srand((unsigned int)time(NULL)); // khoi dong bo tao random
	PlayTetris();
}
