#include "Functions.h"

SDL_Window* window;
SDL_Surface* screenSurface;
SDL_Renderer* renderer;

#define SIZE_HERO_W 51
#define SIZE_HERO_H 36
#define SPEED_JUMP_HERO 4
#define SPEED_PIPE 2

#define DISTANCE_PIPE_W 180
#define DISTANCE_PIPE_H 200

#define SIZE_TILE_W 70
#define TOTAL_PIPES 5
#define SIZE_TILE_H 540
#define MAX_RECORDS 10


// Функция нахождения рандомного числа
// Принимает:
// a и b - 2 целых числа
// Возвращает:
// рандомное значение между промежутком [a, b]
int random(int a, int b)
{
	return rand() % (b - a + 1) + a;
}

// Функция нахождения рандомного числа
// Принимает:
// a и b - 2 вещественных числа
// Возвращает:
// рандомное значение между промежутком [a, b]
float randomF(float a, float b)
{
	return (float)rand() / RAND_MAX * (b - a) + a;
}

// Функция нахождения максимального из 2 чисел
// Принимает:
// a и b - 2 целых числа
// Возвращает:
// максимальное из 2 чисел
int max(int a, int b) {
	if (a > b)
		return a;

	return b;
}

// Функция иницилизации SDL и окна
// Возвращает:
// код ошибки
bool init()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		printf_s("SDL не смог запуститься!\n SDL_error: %s\n", SDL_GetError());
		return false;
	}

	window = SDL_CreateWindow(u8"Flappy Bird", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window)
	{
		printf_s("Окно не может быть создано! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer)
	{
		printf_s("Рендер не может быть создан! SDL_Error: %s\n", SDL_GetError());
		return false;
	}


	return true;
}

// Функция закрытия окна и SDL
// 
void close()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

bool ChekPointInRect(int x, int y, SDL_Rect* rect) {
	return (
		x > rect->x && x < rect->x + rect->w &&
		y > rect->y && y < rect->y + rect->h);
}

void InitBackground(MainRenderTexture* background)
{
	SDL_Surface* myImage_background = IMG_Load("background.png");
	if (!myImage_background)
	{
		printf_s("Ошибка загрузки изображения: %s\n", IMG_GetError());
		exit(-1);
	}
	background->texture = SDL_CreateTextureFromSurface(renderer, myImage_background);


	SDL_FreeSurface(myImage_background);
}

void DrawBackground(Background* background)
{
	SDL_RenderCopy(renderer, background->ren_texture.texture, NULL, NULL);
}

void FreeTextureBackground(MainRenderTexture* background)
{
	SDL_DestroyTexture(background->texture);
}

void InitBird(MainRenderTexture* hero)
{
	SDL_Surface* myImage_bird = IMG_Load("Bird.png");
	if (!myImage_bird)
	{
		printf_s("Ошибка загрузки изображения: %s\n", IMG_GetError());
		exit(-1);
	}
	hero->texture = SDL_CreateTextureFromSurface(renderer, myImage_bird);

	hero->srcRect = { 0, 0, myImage_bird->w, myImage_bird->h };

	SDL_FreeSurface(myImage_bird);
}

void DrawBird(Bird* hero)
{
	SDL_RenderCopy(renderer, hero->ren_texture.texture, &hero->ren_texture.srcRect, &hero->rect);
}

void FreeBird(MainRenderTexture* hero)
{
	SDL_DestroyTexture(hero->texture);
}

void InitPipes(MainRenderTexture* tile)
{
	SDL_Surface* myImage_tile = IMG_Load("pipe.png");
	if (!myImage_tile)
	{
		printf_s("Ошибка загрузки изображения: %s\n", IMG_GetError());
		exit(-1);
	}
	tile->texture = SDL_CreateTextureFromSurface(renderer, myImage_tile);

	tile->srcRect = { 0, 0, myImage_tile->w, myImage_tile->h };
	SDL_FreeSurface(myImage_tile);
}

void DrawPipes(MainRenderTexture* tile, SDL_Rect rect)
{
	SDL_RenderCopy(renderer, tile->texture, &tile->srcRect, &rect);
	int A = rect.y;
	rect.y = rect.y - SIZE_TILE_H - DISTANCE_PIPE_H;
	SDL_RenderCopyEx(renderer, tile->texture, &tile->srcRect, &rect, NULL, NULL, SDL_FLIP_VERTICAL);
	rect.y = A;
}

void FreePipes(MainRenderTexture* tile)
{
	SDL_DestroyTexture(tile->texture);
}

void DrawText(char* text, SDL_Rect symbol)
{
	TTF_Init();
	TTF_Font* my_font = TTF_OpenFont("Samson.ttf", 100);
	SDL_Color fore_color = { 255, 255, 255 };
	SDL_Surface* textSurface = NULL;
	textSurface = TTF_RenderText_Solid(my_font, text, fore_color);
	if (!textSurface)
	{
		printf_s("Ошибка отрисовки текста: %s\n", SDL_GetError());
		exit(-1);
	}

	symbol.w *= strlen(text);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_RenderCopy(renderer, texture, NULL, &symbol);

	TTF_CloseFont(my_font);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(texture);
	TTF_Quit();
}

bool check_collision(Bird* hero, Pipe* tile, bool* quit)
{
	bool collision = false;
	for (int i = 0; i < TOTAL_PIPES; i++)
	{
		if ((hero->pos.x > tile[i].pos.x - SIZE_HERO_W &&
			hero->pos.x < tile[i].pos.x + SIZE_TILE_W &&
			hero->pos.y + SIZE_HERO_H > tile[i].pos.y) ||
			(hero->pos.x > tile[i].pos.x - SIZE_HERO_W &&
				hero->pos.x < tile[i].pos.x + SIZE_TILE_W &&
				hero->pos.y < tile[i].pos.y - DISTANCE_PIPE_H))
		{
			collision = true;
			break;
		}
		else if (hero->pos.x - (SIZE_TILE_W + SIZE_HERO_W) / 2 > tile[i].pos.x - SIZE_HERO_W &&
			hero->pos.x < tile[i].pos.x + SIZE_TILE_W && !tile[i].passed)
		{
			tile[i].passed = true;
			hero->score += 1;
			break;
		}
	}
	return collision;
}

void LoadImagesMainMenu(SDL_Renderer* renderer, SDL_Texture** menuTexture, SDL_Texture** playButtonTexture, SDL_Texture** leaderButtonTexture, SDL_Texture** exitButtonTexture)
{
	// Загрузка изображений для меню и кнопок
	SDL_Surface* menuSurface = IMG_Load("menu_background.png");
	SDL_Surface* playButtonSurface = IMG_Load("play.png");
	SDL_Surface* exitButtonSurface = IMG_Load("exit.png");
	SDL_Surface* leaderButtonSurface = IMG_Load("leader.png");
	if (menuSurface == NULL || playButtonSurface == NULL || exitButtonSurface == NULL || leaderButtonSurface == NULL) {
		printf("Images could not be loaded! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	// Создание текстур для изображений
	*menuTexture = SDL_CreateTextureFromSurface(renderer, menuSurface);
	*playButtonTexture = SDL_CreateTextureFromSurface(renderer, playButtonSurface);
	*exitButtonTexture = SDL_CreateTextureFromSurface(renderer, exitButtonSurface);
	*leaderButtonTexture = SDL_CreateTextureFromSurface(renderer, leaderButtonSurface);
	if (*menuTexture == NULL || *playButtonTexture == NULL || *exitButtonTexture == NULL || *leaderButtonTexture == NULL) {
		printf("Textures could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	// Освобождение памяти, используемой для изображений
	SDL_FreeSurface(menuSurface);
	SDL_FreeSurface(playButtonSurface);
	SDL_FreeSurface(leaderButtonSurface);
	SDL_FreeSurface(exitButtonSurface);
}

void RenderMenu(SDL_Renderer* renderer, SDL_Texture* menuTexture, SDL_Texture* playButtonTexture, SDL_Texture* exitButtonTexture, SDL_Texture* leaderButtonTexture, SDL_Rect* playButtonRect, SDL_Rect* exitButtonRect, SDL_Rect* leaderButtonRect)
{
	// Отрисовка текстур на экране
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
	SDL_RenderCopy(renderer, playButtonTexture, NULL, playButtonRect);
	SDL_RenderCopy(renderer, exitButtonTexture, NULL, exitButtonRect);
	SDL_RenderCopy(renderer, leaderButtonTexture, NULL, leaderButtonRect);
	SDL_RenderPresent(renderer);
}

void LoadImagesEndGame(SDL_Renderer* renderer, SDL_Texture** menuTexture, SDL_Texture** restartButtonTexture, SDL_Texture** homeButtonTexture, SDL_Texture** exitButtonTexture)
{
	// Загрузка изображений для меню и кнопок
	SDL_Surface* menuSurface = IMG_Load("background_gameover.png");
	SDL_Surface* restartButtonSurface = IMG_Load("restart.png");
	SDL_Surface* homeButtonSurface = IMG_Load("home.png");
	SDL_Surface* exitButtonSurface = IMG_Load("exit.png");
	if (menuSurface == NULL || restartButtonSurface == NULL || homeButtonSurface == NULL || exitButtonSurface == NULL) {
		printf("Images could not be loaded! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	// Создание текстур для изображений
	*menuTexture = SDL_CreateTextureFromSurface(renderer, menuSurface);
	*restartButtonTexture = SDL_CreateTextureFromSurface(renderer, restartButtonSurface);
	*homeButtonTexture = SDL_CreateTextureFromSurface(renderer, homeButtonSurface);
	*exitButtonTexture = SDL_CreateTextureFromSurface(renderer, exitButtonSurface);
	if (*menuTexture == NULL || *restartButtonTexture == NULL || *homeButtonTexture == NULL || *exitButtonTexture == NULL) {
		printf("Textures could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	// Освобождение памяти, используемой для изображений
	SDL_FreeSurface(menuSurface);
	SDL_FreeSurface(restartButtonSurface);
	SDL_FreeSurface(homeButtonSurface);
	SDL_FreeSurface(exitButtonSurface);
}

void RenderExit(SDL_Renderer* renderer, SDL_Texture* menuTexture, SDL_Texture* restartButtonTexture, SDL_Texture* exitButtonTexture, SDL_Texture* homeButtonTexture, SDL_Rect* restartButtonRect, SDL_Rect* exitButtonRect, SDL_Rect* homeButtonRect)
{
	// Отрисовка текстур на экране
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, menuTexture, NULL, NULL);
	SDL_RenderCopy(renderer, restartButtonTexture, NULL, restartButtonRect);
	SDL_RenderCopy(renderer, homeButtonTexture, NULL, homeButtonRect);
	SDL_RenderCopy(renderer, exitButtonTexture, NULL, exitButtonRect);
	SDL_RenderPresent(renderer);
}

GameStatus Game(Bird* hero, Pipe* tile, Background* background, GameStatus game_state)
{
	SDL_Event event;
	int total_scores = 0;
	char text[12];
	SDL_Rect total_score = { 1, -5, 20, 40 };
	bool quit = false;
	int flag_hero = 1;
	const Uint8* keystate = SDL_GetKeyboardState(NULL);

	// загрузка звука
	Mix_Chunk* sound = Mix_LoadWAV("jump.wav");
	Mix_Chunk* sound1 = Mix_LoadWAV("point.wav");
	Mix_Chunk* sound2 = Mix_LoadWAV("hit.wav");

	while (!quit)
	{
#pragma region CHECK EVENTS
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
				{
					quit = true;

					break;
				}
				case SDLK_SPACE:
				{
					Mix_PlayChannelTimed(-1, sound, 0, 1000);
				}
				}

				break;
			}
			case SDL_QUIT:
			{
				// освобождение памяти звука
				Mix_FreeChunk(sound);
				sound = NULL;
				Mix_FreeChunk(sound1);
				sound1 = NULL;
				Mix_FreeChunk(sound2);
				sound2 = NULL;
				exit(0);

				break;
			}
			}
		}
		if (keystate[SDL_SCANCODE_SPACE])
		{
			hero->speed_jump = SPEED_JUMP_HERO;
		}
#pragma endregion // CHECK EVENTS

#pragma region UPDATE OBJEKT

		/*гравитация*/
		hero->speed_jump -= hero->gravity;

		/*движение камеры за персонажем, если он выше середины экрана*/
		hero->pos.y -= hero->speed_jump;

		for (int i = 0; i < TOTAL_PIPES; i++)
			tile[i].pos.x -= SPEED_PIPE;

		/*проверка выхода труб за карту и ещё её перегенерация*/
		for (int i = 0; i < TOTAL_PIPES; i++)
			if (tile[i].pos.x <= 0 - SIZE_TILE_W)
			{
				tile[i].pos.x = SCREEN_WIDTH;
				tile[i].pos.y = randomF(100 + DISTANCE_PIPE_H, SCREEN_HEIGHT - 100);
				tile[i].passed = false;
			}


		/*временная проверка на столкновение с поверхностью экрана*/
		if (hero->pos.y > SCREEN_HEIGHT - SIZE_HERO_H)
			hero->pos.y = SCREEN_HEIGHT - SIZE_HERO_H;
		else if (hero->pos.y < 0)
			hero->pos.y = 0;


		for (int i = 0; i < TOTAL_PIPES; i++)
		{
			if ((hero->pos.x > tile[i].pos.x - SIZE_HERO_W &&
				hero->pos.x < tile[i].pos.x + SIZE_TILE_W &&
				hero->pos.y + SIZE_HERO_H > tile[i].pos.y) ||
				(hero->pos.x > tile[i].pos.x - SIZE_HERO_W &&
					hero->pos.x < tile[i].pos.x + SIZE_TILE_W &&
					hero->pos.y < tile[i].pos.y - DISTANCE_PIPE_H))
			{
				break;
			}
			else if (hero->pos.x - (SIZE_TILE_W + SIZE_HERO_W) / 2 > tile[i].pos.x - SIZE_HERO_W &&
				hero->pos.x < tile[i].pos.x + SIZE_TILE_W && !tile[i].passed)
			{
				Mix_PlayChannelTimed(-1, sound1, 0, 1000);
				break;
			}
		}

		/*проверка на столкновение с каждой трубой*/
		if (check_collision(hero, tile, &quit))
		{
			// освобождение памяти звука
			Mix_FreeChunk(sound);
			sound = NULL;
			Mix_FreeChunk(sound1);
			sound1 = NULL;
			Mix_PlayChannelTimed(-1, sound2, 0, 1000);
			int loaded_records = load_records(hero->rcd, MAX_RECORDS, "records.txt");
			if (loaded_records > 0)
				hero->num_records = loaded_records;
			sort(hero->rcd, hero->rcd + hero->num_records, compare_records);

			add_record(hero->rcd, hero->num_records, hero->name, hero->score);

			sort(hero->rcd, hero->rcd + hero->num_records, compare_records);
			save_records(hero->rcd, hero->num_records, "records.txt");

			hero->score = 0;

			return END_GAME;
		}


		/*присваиваем округленные значения для отрисовки*/
		hero->rect.x = hero->pos.x;
		hero->rect.y = hero->pos.y;

		for (int i = 0; i < TOTAL_PIPES; i++)
		{
			tile[i].rect.x = tile[i].pos.x;
			tile[i].rect.y = tile[i].pos.y;
		}

#pragma endregion // UPDATE OBJEKT

#pragma region RENDER OBJEKT

		/*отрисовка заднего фона*/
		DrawBackground(background);

		/*отрисовка персонажа*/
		DrawBird(hero);
		// выгрузка аудиофайла
		for (int i = 0; i < TOTAL_PIPES; i++)
		{
			DrawPipes(&tile->ren_texture, tile[i].rect);
		}

		//SDL_itoa(score, text, 10);
		snprintf(text, 12, "Score: %d", hero->score);
		DrawText(text, total_score);
		SDL_RenderPresent(renderer);

#pragma endregion // RENDER OBJEKT

		SDL_Delay(16);
	}
	// освобождение памяти звука
	Mix_FreeChunk(sound1);
	sound1 = NULL;

	Mix_FreeChunk(sound);
	sound = NULL;

	Mix_FreeChunk(sound2);
	sound2 = NULL;
}

GameStatus EndGame(Background* background, float* current_score, GameStatus game_state)
{
	// Загрузка изображений
	SDL_Texture* menuTexture = NULL;
	SDL_Texture* restartButtonTexture = NULL;
	SDL_Texture* homeButtonTexture = NULL;
	SDL_Texture* exitButtonTexture = NULL;
	LoadImagesEndGame(renderer, &menuTexture, &restartButtonTexture, &homeButtonTexture, &exitButtonTexture);

	// Установка координат для кнопок
	SDL_Rect restartButtonRect = { SCREEN_WIDTH / 2 - 350, SCREEN_HEIGHT / 2 - 80, 300, 200 };
	SDL_Rect homeButtonRect = { SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2 - 80, 300, 200 };
	SDL_Rect exitButtonRect = { SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 100, 300, 200 };

	// Основной цикл игры
	SDL_Event event;
	bool quit = false;
	while (!quit) {
		// Обработка событий
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				exit(0);
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				int mouseX = event.button.x;
				int mouseY = event.button.y;
				if (ChekPointInRect(mouseX, mouseY, &restartButtonRect))
				{
					printf("Restart button clicked!\n");
					// Освобождение памяти, используемой для текстур
					SDL_DestroyTexture(menuTexture);
					SDL_DestroyTexture(restartButtonTexture);
					SDL_DestroyTexture(homeButtonTexture);
					SDL_DestroyTexture(exitButtonTexture);
					return PLAY;
				}
				else if (ChekPointInRect(mouseX, mouseY, &homeButtonRect))
				{
					printf("Menu button clicked!\n");
					// Освобождение памяти, используемой для текстур
					SDL_DestroyTexture(restartButtonTexture);
					SDL_DestroyTexture(homeButtonTexture);
					SDL_DestroyTexture(exitButtonTexture);
					return MENU;
				}
				else if (ChekPointInRect(mouseX, mouseY, &exitButtonRect))
				{
					printf("Exit button clicked!\n");
					// Освобождение памяти, используемой для текстур
					SDL_DestroyTexture(restartButtonTexture);
					SDL_DestroyTexture(homeButtonTexture);
					SDL_DestroyTexture(exitButtonTexture);
					exit(1);
				}
			}
		}

		// Отрисовка меню и кнопок
		RenderExit(renderer, menuTexture, restartButtonTexture, homeButtonTexture, exitButtonTexture, &restartButtonRect, &homeButtonRect, &exitButtonRect);

	}
}

GameStatus MainMenu(Bird* hero, Pipe* tile, Background* background, GameStatus game_state)
{
	// Загрузка изображений
	SDL_Texture* menuTexture = NULL;
	SDL_Texture* playButtonTexture = NULL;
	SDL_Texture* exitButtonTexture = NULL;
	SDL_Texture* leaderButtonTexture = NULL;
	LoadImagesMainMenu(renderer, &menuTexture, &playButtonTexture, &leaderButtonTexture, &exitButtonTexture);

	// Установка координат для кнопок
	SDL_Rect playButtonRect = { SCREEN_WIDTH / 2 - 350, SCREEN_HEIGHT / 2 - 80, 300, 200 };
	SDL_Rect exitButtonRect = { SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 100, 300, 200 };
	SDL_Rect leaderButtonRect = { SCREEN_WIDTH / 2 + 50, SCREEN_HEIGHT / 2 - 80, 300, 200 };

	// Основной цикл игры
	SDL_Event event;
	bool quit = false;
	while (!quit)
	{
		// Обработка событий
		while (SDL_PollEvent(&event) != 0)
		{
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN)
			{
				int mouseX = event.button.x;
				int mouseY = event.button.y;
				if (ChekPointInRect(mouseX, mouseY, &playButtonRect))
				{
					printf("Play button clicked!\n");
					// Освобождение памяти, используемой для текстур
					SDL_DestroyTexture(menuTexture);
					SDL_DestroyTexture(playButtonTexture);
					SDL_DestroyTexture(exitButtonTexture);
					SDL_DestroyTexture(leaderButtonTexture);
					return PLAY;
					
				}
				else if (ChekPointInRect(mouseX, mouseY, &exitButtonRect))
				{
					printf("Exit button clicked!\n");
					// Освобождение памяти, используемой для текстур
					SDL_DestroyTexture(menuTexture);
					SDL_DestroyTexture(playButtonTexture);
					SDL_DestroyTexture(exitButtonTexture);
					SDL_DestroyTexture(leaderButtonTexture);
					exit(1);
				}
				else if (ChekPointInRect(mouseX, mouseY, &leaderButtonRect))
				{
					printf("Leader button clicked!\n");
					// Освобождение памяти, используемой для текстур
					SDL_DestroyTexture(menuTexture);
					SDL_DestroyTexture(playButtonTexture);
					SDL_DestroyTexture(exitButtonTexture);
					SDL_DestroyTexture(leaderButtonTexture);
					return SCORE_TABLE;
				}
			}
		}

		// Отрисовка меню и кнопок
		RenderMenu(renderer, menuTexture, playButtonTexture, exitButtonTexture, leaderButtonTexture, &playButtonRect, &exitButtonRect, &leaderButtonRect);
	}
}

bool compare_records(const Record& a, const Record& b)
{
	return b.score < a.score; // сортировка по убыванию
}

void print_records(const Record records[], int num_records)
{
	printf_s("Таблица рекордов:\n");
	for (int i = 0; i < num_records; i++)
		printf_s("%s - %d\n", records[i].name.c_str(), records[i].score);

}

void save_records(const Record records[], int num_records, const string& filename)
{
	FILE* fp;
	fopen_s(&fp, filename.c_str(), "w+");
	if (fp == NULL)
	{
		printf_s("Ошибка открытия файла\n");
		return;
	}

	int num_lines = 0; // количество строк
	fprintf_s(fp, "Таблица рекордов:\n");
	for (int i = 0; i < num_records; i++)
	{
		fprintf_s(fp, "%s - %d\n", records[i].name.c_str(), records[i].score);
		num_lines++; // увеличиваем количество строк
	}

	fprintf_s(fp, "Количество строк: %d\n", num_lines); // записываем количество строк

	fclose(fp);
	printf_s("Таблица рекордов успешно сохранена в файл %s\n", filename.c_str());
}

int load_records(Record records[], int max_records, const string& filename)
{
	FILE* fp;
	fopen_s(&fp, filename.c_str(), "r+");

	if (fp == NULL)
	{
		printf_s("Ошибка открытия файла\n");
		return 0;
	}

	char line[100];
	int count = 0;
	int num_lines = 0;

	while (fgets(line, sizeof(line), fp) != NULL)
	{
		string str(line);
		size_t pos = str.find('-');

		if (pos != string::npos)
		{
			Record record;
			record.name = str.substr(0, pos - 1);
			record.score = stoi(str.substr(pos + 2));
			records[count] = record;
			count++;
		}
		else if (str.find("Количество строк: ") == 0)
			num_lines = stoi(str.substr(18));

	}

	fclose(fp);

	if (num_lines != count)
	{
		printf_s("Ошибка загрузки файла: количество записей не соответствует указанному в файле\n");
		return 0;
	}

	printf_s("Загружено %d записей из файла %s\n", count, filename.c_str());
	return count;
}

void add_record(Record records[], int& num_records, const string& name, int score) {
	if (num_records < MAX_RECORDS)
		records[num_records++] = { name, score };

	else
	{
		sort(records, records + num_records, compare_records);

		if (score > records[MAX_RECORDS - 1].score)
			records[MAX_RECORDS - 1] = { name, score };

	}
}

GameStatus leaderboard(Background* background, Record records[], int num_records)
{
	DrawBackground(background);

	int loaded_records = load_records(records, MAX_RECORDS, "records.txt");
	if (loaded_records > 0)
		num_records = loaded_records;
	sort(records, records + num_records, compare_records);

	SDL_Event event;
	bool quit = false;
	while (!quit) 
	{
		while (SDL_PollEvent(&event) != 0) 
		{
			if (event.type == SDL_QUIT)
				return MENU;
			
			else if (event.type == SDL_KEYDOWN) 
			{
				if (event.key.keysym.sym == SDLK_ESCAPE) 
					quit = true;
				
			}
		}

		// Render leaderboard text
		TTF_Init();
		TTF_Font* my_font = TTF_OpenFont("Samson.ttf", 32);
		SDL_Color color = { 255, 255, 255 };
		int y = 20;

		for (int i = 0; i < num_records; i++) 
		{
			string text = records[i].name + " - " + to_string(records[i].score);

			SDL_Surface* textSurface = TTF_RenderText_Solid(my_font, text.c_str(), color);
			SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

			int textWidth = textSurface->w;
			int textHeight = textSurface->h;

			SDL_FreeSurface(textSurface);

			int x = (SCREEN_WIDTH - textWidth) / 2;

			SDL_Rect renderQuad = { x, y, textWidth, textHeight };

			SDL_RenderCopy(renderer, textTexture, NULL, &renderQuad);
			SDL_DestroyTexture(textTexture);
			SDL_RenderPresent(renderer);
			y += 50;
		}

		TTF_CloseFont(my_font);
		TTF_Quit();
	}

	// Update screen
	SDL_RenderPresent(renderer);
}

void MainCycle(Bird* hero, Pipe* tile, Background* background, GameStatus game_state)
{
	SDL_Event event_data;

	float current_score = 0;

	bool quit = false;


	while (!quit)
	{
		while (SDL_PollEvent(&event_data))
			switch (event_data.type)
			{
			case SDL_KEYDOWN:
			{
				if (event_data.key.keysym.sym == SDLK_ESCAPE)
					quit = true;

				break;
			}
			case SDL_QUIT:
			{
				quit = true;

				break;
			}
			}

		switch (game_state)
		{
		case MENU:
		{
			game_state = MainMenu(hero, tile, background, game_state);

			break;
		}
		case PLAY:
		{
			hero->pos.x = SCREEN_WIDTH / 4 - SIZE_HERO_W / 2;
			hero->pos.y = SCREEN_HEIGHT / 2 - SIZE_HERO_H / 2;
			hero->rect.w = SIZE_HERO_W;
			hero->rect.h = SIZE_HERO_H;

			for (int i = 0; i < TOTAL_PIPES; i++)
			{
				tile[i].pos.x = SCREEN_WIDTH + (float)i * SCREEN_WIDTH / TOTAL_PIPES + (float)i * SIZE_TILE_W / TOTAL_PIPES;
				tile[i].pos.y = randomF(100 + DISTANCE_PIPE_H, SCREEN_HEIGHT - 100);
				tile[i].rect.w = SIZE_TILE_W;
				tile[i].rect.h = SIZE_TILE_H;
			}

			game_state = Game(hero, tile, background, game_state);

			break;
		}
		case END_GAME:
		{
			game_state = EndGame(background, &current_score, game_state);

			break;
		}
		case SCORE_TABLE:
		{

			game_state = leaderboard(background, hero->rcd, hero->num_records);

			break;
		}
		}

		SDL_Delay(16);
	}
}