#include <iostream>
#include <windows.h> // окрашивание консоли и работа с координатами
#include <conio.h>
#include <string>
#include"BomberMan.h"



int main()
{
	setlocale(LC_ALL, "rus");
	//таблица аски для поиска символов псевдографики
	/*for (int code = 0; code < 256; code++)
	{
		cout << code << " - " << (char)code << "\n";
	}*/

	//меняем заголовок окна
	system("title Бомбер!");
	
	// дескриптор окна консоли (порядковый номер окна в системе)
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);

	// скрыть стандартный мигающий курсор
	CONSOLE_CURSOR_INFO info;
	info.bVisible = false;
	info.dwSize = 100;

	SetConsoleCursorInfo(consoleHandle, &info);

	// модель локации
	// 0 - коридоры (пустоты)
	// 1 - стена разрушаемая
	// 2 - монетки
	// 3 - враги

	const int WIDTH = 50;
	const int HEIGHT = 15;
	int location[HEIGHT][WIDTH] = {};
	ObjectCount countOfObject;


	// генерация локации
	fillPlayDesk(location, countOfObject, HEIGHT, WIDTH, MAX_OBJECTS);

	//считаю сколько на доске чего
	countObjects(location, HEIGHT, WIDTH, countOfObject);
	
	// показ локации
	showDesk(location, HEIGHT, WIDTH, consoleHandle);
	
	// размещение ГГ (стандартного главного героя, также можно передать параметры для героя)
	Player player;	
	
	// функция установки курсора в любую точку на экране консоли
	drawObject(consoleHandle, player.m_position, player.m_symbol, player.m_color);
	
	// игровой движок (интерактив с пользователем)
	while(true) {
		
		//показываю статы
		showStats(consoleHandle, countOfObject, player);

		if(location[player.m_position.Y][player.m_position.X] == ENEMY) {

			player.m_health -= 20;
			--countOfObject.m_enemy;

			location[player.m_position.Y][player.m_position.X] = HALL;
		}

		int code = _getch(); // функция приостанавливает работу программы, ждёт реакции пользователя
		
		if(code == 224)
			code = _getch(); // если первый код был 224, то повторный вызов функции вернёт только один, нормальный код
		
		// стирание ГГ в старой позиции
		deleteObject(consoleHandle, player.m_position);

		if(code == ESCAPE)
			break;

		// пользователь может нажать любую кнопку (в том числе энтер, эскейп, пробел, стрелочки), после чего вернётся код нажатой клавиши
		switch(code) {
			case ENTER:
				// cout << "ENTER\n";
				break;
			case SPACEBAR:
				// cout << "SPACEBAR\n";
				break;
			
			case RIGHT:
				if(location[player.m_position.Y][player.m_position.X + 1] != WALL)
					++player.m_position.X;
				break;
			case LEFT:
				if(player.m_position.X > 0 && location[player.m_position.Y][player.m_position.X - 1] != WALL)
					--player.m_position.X;
				break;
			case UP:
				// cout << "UP\n";
				if(location[player.m_position.Y - 1][player.m_position.X] != WALL)
					--player.m_position.Y;
				break;
			case DOWN:
				// cout << "DOWN\n";
				if(location[player.m_position.Y + 1][player.m_position.X] != WALL)
					++player.m_position.Y;
				break;
			default:
				std::cout << code << "\n";
				
		}

		// показ ГГ в новой позиции
		drawObject(consoleHandle, player.m_position, player.m_symbol, player.m_color);

		// взаимодействие ГГ с другими объектами в лабиринте
		if(location[player.m_position.Y][player.m_position.X] == COIN) {
			
			++player.m_coins;
			--countOfObject.m_stars;

			location[player.m_position.Y][player.m_position.X] = HALL;
		}
		
		if(location[player.m_position.Y][player.m_position.X] == ENEMY) {
			
			player.m_health -= 20;

			--countOfObject.m_enemy;

			location[player.m_position.Y][player.m_position.X] = HALL;
		}
		
		//уменьщаю энергию игрока каждый шаг
		player.m_energy -= 1;

		enemyMovement(consoleHandle, location, HEIGHT, WIDTH);
		
		if(gameOver(player, countOfObject)) {
			break;
		}
	}
}