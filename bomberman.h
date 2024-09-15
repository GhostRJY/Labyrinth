#ifndef BOMBERMAN_H
#define BOMBERMAN_H
#include <random>

// enumeration - ������������
enum KeyCodes { ENTER = 13, ESCAPE = 27, LEFT = 75, RIGHT = 77, UP = 72, DOWN = 80, SPACEBAR = 32 };
enum Colors { DARKGREEN = 2, RED = 12, YELLOW = 14, BLUE = 9 };
enum Objects { HALL, WALL, COIN, ENEMY, MAX_OBJECTS };

struct Player
{
	COORD m_position; //���������� ��� ����������� �������
	int m_health;
	int m_energy;
	int m_coins;
	char m_symbol;
	Colors m_color;

	Player(short posX = 1, short posY = 2, int health = 100, int energy = 500, int coins = 0, char symbol = 1, Colors color = BLUE) :
		m_position{ posX, posY },
		m_health{ health },
		m_energy{ energy },
		m_coins{ coins },
		m_symbol{ symbol },
		m_color{ color }
	{}

};

//��������� ��� �������� ���� ��������
struct ObjectCount
{
	int m_enemy;
	int m_stars;
	ObjectCount() :m_enemy{ 0 }, m_stars{ 0 } {}

};


// ����� ��������
auto randomNumber(int fromVal = -10, int toVal = 100) -> int
{

	std::random_device rd;
	std::mt19937 mersene(rd());
	return (fromVal + mersene() % toVal);
}

//��������� ��������������� �����
auto getRandomNumber(int min, int max) -> int
{

#ifndef RANDINIT
#define RANDINIT
	// ������ ��������� ��������� ��������� �����
	srand(time(NULL));
	rand();
#endif

	static const double fraction = 0.1 / (static_cast<double>(RAND_MAX) + 0.1);

	return static_cast<int>(min + rand() * fraction * (max - min + 1));
}

// ��������� �������
template <typename T>
void fillPlayDesk(T *playDesk, ObjectCount & objects,
				  const int &row,
				  const int &column,
				  const int &countOfObjects)
{
	for(int y{ 0 }; y < row; ++y) {
		for(int x{ 0 }; x < column; ++x) {
			// �� ������� ������� ��������� �����
			playDesk[y][x] = randomNumber(0, countOfObjects); // 0 1 2 3

			// ����� �� �����
			if(x == 0 || y == 0 || x == column - 1 || y == row - 1)
				playDesk[y][x] = WALL;

			// ���� � �����
			if(x == 0 && y == 2 || x == column - 1 && y == row - 3)
				playDesk[y][x] = HALL;

			if(playDesk[y][x] == ENEMY) {

				++objects.m_enemy;

				// ������������ ����������� ����, ��������� ���� ��� ���
				// ��������, ����������� �������� �� ������ - 10%
				int prob = randomNumber(0, 30); // 0-9
				if(prob != 0){ // 1 2 3 4 5 6 7 8 9
					--objects.m_enemy;
					playDesk[y][x] = HALL;
				}
			}

			if(playDesk[y][x] == COIN) {

				++objects.m_stars;

				// ������������ ����������� ����, ��������� ���� ��� ���
				// ��������, ����������� �������� �� ������ - 10%
				int prob = randomNumber(0, 20); // 0-9
				if(prob != 0) { // 1 2 3 4 5 6 7 8 9
					--objects.m_stars;
					playDesk[y][x] = HALL;
				}
			}
		}

	}
}

//������ ������� �� �����
//template <typename T>
void countObjects(auto playDesk, const int row, const int column, ObjectCount &objects)
{
	for(int y{ 0 }; y < row; ++y) {
		for(int x{ 0 }; x < column; ++x) {
			
			if(playDesk[row][column] == ENEMY)
				++objects.m_enemy;
			if(playDesk[row][column] == COIN)
				++objects.m_stars;
		}

	}
}

// ����� �������
void showDesk(auto playDesk, const int &row, const int &column, const HANDLE &consoleHandle)
{

	for(int y = 0; y < row; ++y) // ������� �����
	{
		for(int x = 0; x < column; ++x) {
			// cout << location[y][x];
			switch(playDesk[y][x]) {
				case HALL: // �������
					std::cout << " ";
					break;
				case WALL: // �����
					SetConsoleTextAttribute(consoleHandle, DARKGREEN); // 0-255
					std::cout << (char)177; //(char)219;
					break;
				case COIN: // �������
					SetConsoleTextAttribute(consoleHandle, YELLOW);
					std::cout << (char)15; // 249
					break;
				case ENEMY: // �����
					SetConsoleTextAttribute(consoleHandle, RED);
					std::cout << (char)1;
					break;
				default:
					std::cout << playDesk[y][x];
					break;
			}
		}
		std::cout << "\n";
	}
}

//��������� �������
void drawObject(const HANDLE &handle, const COORD &position, char symbol, Colors color)
{
	SetConsoleCursorPosition(handle, position);
	SetConsoleTextAttribute(handle, color);
	std::cout << symbol;
}

//�������� ������� � �������
void deleteObject(const HANDLE &handle, const COORD &position)
{
	SetConsoleCursorPosition(handle, position);
	//SetConsoleTextAttribute(handle, 1);
	std::cout << " ";
}

//������� �������� �������� �������� ����
void deskElementChange(int &prevElement, int &currentElement)
{
	prevElement = HALL;
	currentElement = ENEMY;
}

void moveEnemy(const HANDLE &handle, int x, int y, int moveX, int moveY)
{	
	COORD enemyPosition;
	enemyPosition.X = x;
	enemyPosition.Y = y;

	//������ ����� � ���������� �����
	deleteObject(handle, enemyPosition);

	enemyPosition.Y = y + moveY;
	enemyPosition.X = x + moveX;

	////����� ����� � ����� �����
	drawObject(handle, enemyPosition, 1, RED);
}
//��������� ����������� ������
void enemyMovement(HANDLE handle, auto location, const int row, const int column)
{
	//����� ���������� ������������� �����, ���� �������� ��� �� �����������

	int newPositionX = 0;
	int newPositionY = 0;

	for(int y{ 1 }; y < row; ++y) {
		for(int x{ 1 }; x < column; ++x) {

			if(location[y][x] == ENEMY) {

				//��� �����
				int move = 1;

				//�������� ��� �� ��� ������������ ����
				if(y != newPositionY && x != newPositionX) {
					if(location[y + move][x] != WALL) {
						deskElementChange(location[y][x], location[y + move][x]);
						moveEnemy(handle, x, y, 0, move);
						newPositionX = x;
						newPositionY = y + move;
					}

					else if(location[y - move][x] != WALL) {
						deskElementChange(location[y][x], location[y - move][x]);
						moveEnemy(handle, x, y, 0, -move);
						newPositionX = x;
						newPositionY = y - move;
					}

					else if(location[y][x + move] != WALL) {
						deskElementChange(location[y][x], location[y][x + move]);
						moveEnemy(handle, x, y, move, 0);
						newPositionX = x + move;
						newPositionY = y;
					}

					else if(location[y][x - move] != WALL) {
						deskElementChange(location[y][x], location[y][x - move]);
						moveEnemy(handle, x, y, -move, 0);
						newPositionX = x - move;
						newPositionY = y;
					}
				}
			}

		}
	}

}

//���-�� �������� ������ � �������
void deleteConsoleString(const HANDLE &handle, const COORD &positionOfString )
{	
	SetConsoleCursorPosition(handle, positionOfString);
	std::cout << "               ";
}

void showStats(const HANDLE &handle, const ObjectCount &objects, const Player &player)
{
	COORD menuPos;
	
	menuPos.X = 60;
	menuPos.Y = 7;

	//������ ������ ����� ������� ����� ����������
	deleteConsoleString(handle, menuPos);
	SetConsoleCursorPosition(handle, menuPos);
	SetConsoleTextAttribute(handle, 4);
	std::cout << "HP: " << player.m_health;
		
	menuPos.X = 60;
	menuPos.Y = 8;

	deleteConsoleString(handle, menuPos);
	SetConsoleCursorPosition(handle, menuPos);
	std::cout << "Energy: " << player.m_energy;
	
	menuPos.X = 60;
	menuPos.Y = 9;
	
	deleteConsoleString(handle, menuPos);
	SetConsoleCursorPosition(handle, menuPos);
	std::cout << "Coins collected: " << player.m_coins;
}


bool gameOver(const Player &playerStats, const ObjectCount &locationStats)
{
	if(playerStats.m_health == 0) {
		system("cls");
		std::cout << "�� �����!" << std::endl;
		return true;
	}
	
	if(playerStats.m_energy == 0) {
		system("cls");
		std::cout << "�� �������!" << std::endl;
		return true;
	}

	if(playerStats.m_coins == locationStats.m_stars) {
		system("cls");
		std::cout << "��������� ������ ��� �������!" << std::endl;
		return true;
	}

	return false;
}

#endif