/*String Painter*/
//#define _CRT_SECURE_NO_WARNINGS//vs专属(doge)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
/**
 * 字符串绘制器 String Painter
 * 
 * by Ltabsyy
 */
#define LimName 32
#define LimKeyBoard 64
#define LimLine 128

// 字体的存储与编辑
struct Font
{
	char character;
	int height;
	int width;
	int** bitmap;//点阵图
};
struct Fonts
{
	char* name;
	char* author;
	int numberOfCharacter;
	struct Font* font;
};
void PrintFont(struct Font font);
void PrintFonts(struct Fonts fonts);
struct Fonts ReadFonts();
void WriteFonts(struct Fonts fonts);
int IsPosInRectangle(int x, int y, int x1, int y1, int x2, int y2);
int SearchCharacter(struct Fonts fonts, char character);
struct Fonts AddFont(struct Fonts fonts);
struct Fonts DeleteFont(struct Fonts fonts, int deleteNumber);
struct Font EditFont(struct Font font);

// 字符串的绘制
struct Board
{
	char* name;
	char* author;
	int height;
	int width;
	char** s;//多行字符串
	int** map;
};
void DrawChar(struct Fonts fonts, char character, struct Board* board, int r0, int c0);
void PrintDrawing(struct Board board);
void StringPainter(struct Fonts fonts);
void SaveDrawing(struct Board board);
void SaveMap(struct Board board);

void gotoxy(int x, int y)//以覆写代替清屏，对EditFont实现抗闪烁
{
	COORD coord = {x, y};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int main()
{
	int choice, i;
	char c;
	struct Fonts fonts = ReadFonts();
	while(1)
	{
		system("cls");
		printf("[String Painter]\n");
		printf("1.绘制字符串\n");
		printf("2.编辑字体\n");
		printf("3.添加字体\n");
		printf("4.删除字体\n");
		printf("5.显示字体库\n");
		printf("6.保存字体库\n");
		printf("7.退出\n");
		printf(">");
		//scanf("%d", &choice);
		choice = 1;
		if(choice == 1)
		{
			StringPainter(fonts);
		}
		else if(choice == 2)
		{
			while(1)
			{
				getchar();
				printf("[character]>");
				scanf("%c", &c);
				i = SearchCharacter(fonts, c);
				if(i != -1) break;
			}
			fonts.font[i] = EditFont(fonts.font[i]);
		}
		else if(choice == 3)
		{
			fonts = AddFont(fonts);
		}
		else if(choice == 4)
		{
			while(1)
			{
				getchar();
				printf("[character]>");
				scanf("%c", &c);
				i = SearchCharacter(fonts, c);
				if(i != -1) break;
			}
			fonts = DeleteFont(fonts, i);
		}
		else if(choice == 5)
		{
			PrintFonts(fonts);
			system("pause");
		}
		else if(choice == 6)
		{
			WriteFonts(fonts);
			printf("已保存\n");
			system("pause");
		}
		else if(choice == 7)
		{
			break;
		}
	}
	return 0;
}

void PrintFont(struct Font font)
{
	int r, c;
	printf("character:%c\n", font.character);
	printf("size=%d*%d\n", font.height, font.width);
	printf("  ");
	for(c=0; c<font.width; c++)
	{
		printf("%d ", c);
	}
	printf("\n");
	for(r=0; r<font.height; r++)
	{
		printf("%d ", r);
		for(c=0; c<font.width; c++)
		{
			if(font.bitmap[r][c] == 1)
			{
				printf("* ");
			}
			else if(font.bitmap[r][c] == 0)
			{
				printf("  ");
			}
			else
			{
				printf("? ");
			}
		}
		printf("\n");
	}
}

void PrintFonts(struct Fonts fonts)
{
	int i;
	printf("name:%s", fonts.name);//输出空格和回车
	printf("author:%s", fonts.author);
	printf("numberOfCharacter=%d\n", fonts.numberOfCharacter);
	for(i=0; i<fonts.numberOfCharacter; i++)
	{
		PrintFont(fonts.font[i]);
	}
	/*----------------
	ASCII:
	0-8:    控制码
	9:      制表符\t
	10-13:  空白符\n \v \f \r
	14-31:  控制码
	32:     空格
	33-47:  !"#$%&'()*+,-./
	48-57:  0123456789
	58-64:  :;<=>?@
	65-70:  ABCDEF
	71-90:  GHIJKLMNOPQRSTUVWXYZ
	91-96:  [\]^_`
	97-102: abcdef
	103-122:ghijklmnopqrstuvwxyz
	123-126:{|}~
	127:    退格符
	----------------*/
}

struct Fonts ReadFonts()
{
	FILE* file;
	struct Fonts fonts;
	int** bitmap;
	int i, r, c;
	fonts.numberOfCharacter = 0;
	fonts.name =(char*) calloc(LimName, sizeof(char));
	fonts.author =(char*) calloc(LimName, sizeof(char));
	if((file = fopen("stringpainter-font.txt", "r")))
	{
		fscanf(file, "name:");
		fgets(fonts.name, LimName, file);//读取空格和回车
		fscanf(file, "author:");
		fgets(fonts.author, LimName, file);
		fscanf(file, "numberOfCharacter=%d\n", &(fonts.numberOfCharacter));
		fonts.font =(struct Font*) calloc(fonts.numberOfCharacter, sizeof(struct Font));
		for(i=0; i<fonts.numberOfCharacter; i++)
		{
			fscanf(file, "\n");
			fscanf(file, "character:%c\n", &(fonts.font[i].character));
			fscanf(file, "size=%d*%d\n", &(fonts.font[i].height), &(fonts.font[i].width));
			//分配矩阵空间
			bitmap =(int**) calloc(fonts.font[i].height, sizeof(int*));
			for(r=0; r<fonts.font[i].height; r++)
			{
				bitmap[r] =(int*) calloc(fonts.font[i].width, sizeof(int));
			}
			//读取点阵图
			for(r=0; r<fonts.font[i].height; r++)
			{
				fscanf(file, "  ");
				for(c=0; c<fonts.font[i].width; c++)
				{
					fscanf(file, "%d ", &bitmap[r][c]);
				}
				fscanf(file, "\n");
			}
			fonts.font[i].bitmap = bitmap;
		}
		fclose(file);
	}
	else
	{
		printf("[String Painter]未找到stringpainter-font.txt！正在新建字体库。\n");
		printf("[字体库名称]>");
		fgets(fonts.name, LimName, stdin);
		printf("[作者名称]>");
		fgets(fonts.author, LimName, stdin);
	}
	return fonts;
}

void WriteFonts(struct Fonts fonts)
{
	FILE* file;
	int** bitmap;
	int i, r, c;
	file = fopen("stringpainter-font.txt", "w");
	fprintf(file, "name:%s", fonts.name);//输出空格和回车
	fprintf(file, "author:%s", fonts.author);
	fprintf(file, "numberOfCharacter=%d\n", fonts.numberOfCharacter);
	for(i=0; i<fonts.numberOfCharacter; i++)
	{
		fprintf(file, "\n");
		fprintf(file, "character:%c\n", fonts.font[i].character);
		fprintf(file, "size=%d*%d\n", fonts.font[i].height, fonts.font[i].width);
		bitmap = fonts.font[i].bitmap;
		for(r=0; r<fonts.font[i].height; r++)
		{
			fprintf(file, "  ");
			for(c=0; c<fonts.font[i].width; c++)
			{
				fprintf(file, "%d ", bitmap[r][c]);
			}
			fprintf(file, "\n");
		}
	}
	fclose(file);
}

int IsPosInRectangle(int x, int y, int x1, int y1, int x2, int y2)
{
	if(y >= y1 && y <= y2 && x >= x1 && x <= x2)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int SearchCharacter(struct Fonts fonts, char character)
{
	int i;
	for(i=0; i<fonts.numberOfCharacter; i++)
	{
		if(fonts.font[i].character == character) return i;
	}
	return -1;
}

struct Fonts AddFont(struct Fonts fonts)
{
	struct Font newFont;
	struct Font* font;
	int i, r;
	getchar();
	printf("[character]>");
	newFont.character = getchar();
	printf("[height] [width]>");
	scanf("%d%d", &(newFont.height), &(newFont.width));
	//分配矩阵空间
	newFont.bitmap =(int**) calloc(newFont.height, sizeof(int*));
	for(r=0; r<newFont.height; r++)
	{
		newFont.bitmap[r] =(int*) calloc(newFont.width, sizeof(int));
	}
	//编辑点阵图
	newFont = EditFont(newFont);
	//更新字体库
	font =(struct Font*) calloc(fonts.numberOfCharacter+1, sizeof(struct Font));
	for(i=0; i<fonts.numberOfCharacter; i++)
	{
		font[i] = fonts.font[i];
	}
	font[fonts.numberOfCharacter] = newFont;
	if(fonts.numberOfCharacter != 0) free(fonts.font);
	fonts.font = font;
	fonts.numberOfCharacter++;
	return fonts;
}

struct Fonts DeleteFont(struct Fonts fonts, int deleteNumber)
{
	int i, r;
	struct Font* font =(struct Font*) calloc(fonts.numberOfCharacter-1, sizeof(struct Font));
	if(deleteNumber >= 0 && deleteNumber < fonts.numberOfCharacter)
	{
		for(i=0; i<deleteNumber; i++)
		{
			font[i] = fonts.font[i];
		}
		for(r=0; r<fonts.font[i].height; r++)//释放删除位置点阵图空间
		{
			free(fonts.font[i].bitmap[r]);
		}
		free(fonts.font[i].bitmap);
		for(i=deleteNumber; i<fonts.numberOfCharacter-1; i++)
		{
			font[i] = fonts.font[i+1];
		}
		free(fonts.font);
		fonts.font = font;
		fonts.numberOfCharacter--;
	}
	return fonts;
}

struct Font EditFont(struct Font font)
{
	int r, c;
	HANDLE hdin = GetStdHandle(STD_INPUT_HANDLE);
	COORD mousePos = {0, 0};
	COORD mouseOperatedPos = {0, 0};
	INPUT_RECORD rcd;
	DWORD rcdnum;
	int isReadyRefreshMouseOperatedPos = 0;
	//编辑点阵图
	system("cls");
	SetConsoleMode(hdin, ENABLE_MOUSE_INPUT | ENABLE_EXTENDED_FLAGS);
	while(1)
	{
		gotoxy(0, 0);
		PrintFont(font);
		printf("[鼠标左键:修改/右键:退出]\n");
		//获取操作位置
		//printf(">");
		//scanf("%d%d", &r, &c);
		while(1)
		{
			isReadyRefreshMouseOperatedPos = 1;
			ReadConsoleInput(hdin, &rcd, 1, &rcdnum);
			if(rcd.EventType == MOUSE_EVENT)
			{
				mousePos = rcd.Event.MouseEvent.dwMousePosition;
				if(IsPosInRectangle(mousePos.X/2-1, mousePos.Y-3, 0, 0, font.width-1, font.height-1))
				{
					r = mousePos.Y-3;
					c = mousePos.X/2-1;
					printf("\r>* %d %d  ", r, c);
					if(rcd.Event.MouseEvent.dwButtonState == FROM_LEFT_1ST_BUTTON_PRESSED)
					{
						if(IsPosInRectangle(mousePos.X, mousePos.Y, mouseOperatedPos.X-1, mouseOperatedPos.Y, mouseOperatedPos.X+1, mouseOperatedPos.Y))
						{
							isReadyRefreshMouseOperatedPos = 0;
						}
						else
						{
							mouseOperatedPos = mousePos;
							break;
						}
					}
				}
				if(rcd.Event.MouseEvent.dwButtonState == RIGHTMOST_BUTTON_PRESSED)
				{
					r = -1;
					c = 0;
					break;//右键退出编辑
				}
			}
			if(isReadyRefreshMouseOperatedPos == 1)
			{
				mouseOperatedPos.X = 0;
				mouseOperatedPos.Y = 0;
			}
		}
		//执行修改
		if(r<0||r>=font.height||c<0||c>=font.width) break;
		if(font.bitmap[r][c] == 0) font.bitmap[r][c] = 1;
		else font.bitmap[r][c] = 0;
	}
	SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), ENABLE_PROCESSED_INPUT | ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT
		| ENABLE_MOUSE_INPUT | ENABLE_INSERT_MODE | ENABLE_QUICK_EDIT_MODE | ENABLE_EXTENDED_FLAGS
		| ENABLE_AUTO_POSITION);
	return font;
}

void DrawChar(struct Fonts fonts, char character, struct Board* board, int r0, int c0)
{
	int r, c;
	struct Font font = fonts.font[SearchCharacter(fonts, character)];//获取字体
	for(r=r0; r<r0+font.height; r++)
	{
		for(c=c0; c<c0+font.width; c++)
		{
			if(r>=0 && r<board->height && c>=0 && c<board->width)
			{
				if(font.bitmap[r-r0][c-c0] == 1)
				{
					board->map[r][c] = 1;
				}
			}
		}
	}
}

void PrintDrawing(struct Board board)
{
	int r, c, endColumn;
	printf("name:%s", board.name);
	printf("author:%s", board.author);
	printf("Map:%d*%d\n", board.height, board.width);
	for(r=0; r<board.height; r++)
	{
		//printf("  ");
		//endColumn定位到末尾*位置
		for(endColumn = board.width-1; board.map[r][endColumn] != 1; endColumn--);
		for(c=0; c<=endColumn; c++)
		{
			if(board.map[r][c] == 1)
			{
				printf("*");
			}
			else
			{
				printf(" ");
			}
		}
		printf("\n");
	}
}

void StringPainter(struct Fonts fonts)
{
	struct Board board;
	char s[LimKeyBoard];
	int i, r, c, lineHeight;
	board.height = 0;
	board.width = 189;
	board.name =(char*) calloc(LimName, sizeof(char));
	board.author =(char*) calloc(LimName, sizeof(char));
	/*getchar();
	printf("[name:Drawing]>");
	fgets(board.name, LimName, stdin);
	printf("[author:Ltabsyy]>");
	fgets(board.author, LimName, stdin);*/
	system("cls");
	//printf("[屏幕长度:推荐88/189]>");
	//scanf("%d", &(board.width));
	board.width = 189;
	board.name = "Drawing3\n";
	board.author = "String Painter\n";
	// 最高字符高度确定为行高
	lineHeight = 0;
	for(i=0; i<fonts.numberOfCharacter; i++)
	{
		if(fonts.font[i].height > lineHeight)
		{
			lineHeight = fonts.font[i].height;
		}
	}
	board.map =(int**) calloc(LimLine, sizeof(int*));//先申请128行
	for(i=0; i<LimLine; i++)
	{
		board.map[i] =(int*) calloc(board.width, sizeof(int));
	}
	// 逐行操作
	//getchar();
	printf("输入 ` 保存文件，按回车开始");
	getchar();
	for(r=0; ; r+=lineHeight)
	{
		// 显示
		system("cls");
		PrintDrawing(board);
		printf("[String Painter]>");
		// 读取一行字符
		for(i=0; i<LimKeyBoard; i++)
		{
			//s[i] = 0;
			s[i] = getchar();
			if(s[i] == '\n')
			{
				s[i] = 0;
				break;
			}
		}
		if(s[0] == '`')
		{
			if(s[1] == 'l')
			{
				board.height++;
				r++;
				r -= lineHeight;
				continue;
			}
			else
			{
				break;
			}
		}
		if(i == LimKeyBoard)
		{
			printf("[Error]一次输入过多字符\n");
			fflush(stdin);
			system("pause");
		}
		// 更新高度
		board.height += lineHeight;
		// 绘制字符串
		c = 0;
		for(i=0; i<LimKeyBoard && s[i]!=0; i++)
		{
			if(SearchCharacter(fonts, s[i]) == -1)
			{
				printf("[Error]缺失字体:%c\n", s[i]);
				system("pause");
				continue;
			}
			// 过长时自动换行
			if(c + fonts.font[SearchCharacter(fonts, s[i])].width >= board.width)
			{
				board.height += lineHeight;
				r += lineHeight;
				c = 0;
				/*printf("[Error]屏幕长度不足\n");
				system("pause");
				break;*/
			}
			DrawChar(fonts, s[i], &board, r, c);
			c += fonts.font[SearchCharacter(fonts, s[i])].width;
		}
	}
	SaveDrawing(board);
	//SaveMap(board);
	//printf("已保存至Drawing3.txt和minesweeper-maps.txt\n");
	printf("已保存至Drawing3.txt\n");
	system("pause");
}

void SaveDrawing(struct Board board)
{
	FILE* file;
	int r, c, endColumn;
	file = fopen("Drawing3.txt", "w");
	for(r=0; r<board.height; r++)
	{
		//endColumn定位到末尾*位置
		for(endColumn = board.width-1; board.map[r][endColumn] != 1; endColumn--);
		for(c=0; c<=endColumn; c++)
		{
			if(board.map[r][c] == 1)
			{
				fprintf(file, "*");
			}
			else
			{
				fprintf(file, " ");
			}
		}
		fprintf(file, "\n");
	}
	fclose(file);
}

void SaveMap(struct Board board)
{
	FILE* file;
	int r, c;
	file = fopen("minesweeper-maps.txt", "w");
	fprintf(file, "numberOfMaps=1\n\n");
	fprintf(file, "name:%s", board.name);
	fprintf(file, "author:%s", board.author);
	fprintf(file, "Map:%d*%d\n", board.height, board.width);
	for(r=0; r<board.height; r++)
	{
		fprintf(file, "  ");
		for(c=0; c<board.width; c++)
		{
			fprintf(file, "%d ", board.map[r][c]);
		}
		fprintf(file, "\n");
	}
	fclose(file);
}
