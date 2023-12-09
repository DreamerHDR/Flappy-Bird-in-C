#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <string>
#include <algorithm>
#include <sstream>

using namespace std;

const int SCREEN_WIDTH = 920;
const int SCREEN_HEIGHT = 640;


#define SIZE_HERO_W
#define SIZE_HERO_H
#define SPEED_JUMP_HERO
#define SPEED_PIPE
#define MAX_SCORES
#define DISTANCE_PIPE_W
#define DISTANCE_PIPE_H

#define SIZE_TILE_W
#define TOTAL_PIPES 
#define SIZE_TILE_H
#define MAX_RECORDS 

typedef enum GameStatus
{
	MENU = 0,
	PAUSE = 1,
	END_GAME = 2,
	SCORE_TABLE = 3,
	PLAY = 4
};

struct MainRenderTexture
{
	SDL_Rect srcRect{ NULL };
	SDL_Texture* texture = NULL;
};

struct Record
{
	string name;
	int score;
};

struct Button
{
	SDL_Rect rect{ NULL };
	MainRenderTexture ren_texture;

	int flag = 1;
};

struct Background
{
	MainRenderTexture ren_texture;
};

struct Bird
{
	SDL_Rect rect{ NULL };
	SDL_FPoint pos{ NULL };

	MainRenderTexture ren_texture;

	Record rcd[10];
	string name;
	int flag = 1;
	
	int score = 0;
	int total_scores;
	int num_records;
	
	float speed_jump = 4 /*SPEED_JUMP_HERO*/;
	float gravity = 0.16f;
};

struct Pipe
{
	SDL_Rect rect{ NULL };
	SDL_FPoint pos{ NULL };
	bool passed = false;

	MainRenderTexture ren_texture;
};

int random(int a, int b);

float randomF(float a, float b);

int max(int a, int b);

bool init();

void close();

bool ChekPointInRect(int x, int y, SDL_Rect* rect);

void InitBackground(MainRenderTexture* background);

void DrawBackground(Background* background);

void FreeTextureBackground(MainRenderTexture* background);

void InitBird(MainRenderTexture* hero);

void FreeBird(MainRenderTexture* hero);

void InitPipes(MainRenderTexture* tile);

void DrawPipes(MainRenderTexture* tile, SDL_Rect rect);

void FreePipes(MainRenderTexture* tile);

void DrawText(char* text, SDL_Rect symbol);

bool check_collision(Bird* hero, Pipe* tile, bool* quit);

void LoadImagesMainMenu(SDL_Renderer* renderer, SDL_Texture** menuTexture, SDL_Texture** playButtonTexture, SDL_Texture** leaderButtonTexture, SDL_Texture** exitButtonTexture);

void RenderMenu(SDL_Renderer* renderer, SDL_Texture* menuTexture, SDL_Texture* playButtonTexture, SDL_Texture* exitButtonTexture, SDL_Texture* leaderButtonTexture, SDL_Rect* playButtonRect, SDL_Rect* exitButtonRect, SDL_Rect* leaderButtonRect);

void LoadImagesEndGame(SDL_Renderer* renderer, SDL_Texture** menuTexture, SDL_Texture** restartButtonTexture, SDL_Texture** homeButtonTexture, SDL_Texture** exitButtonTexture);

void RenderExit(SDL_Renderer* renderer, SDL_Texture* menuTexture, SDL_Texture* restartButtonTexture, SDL_Texture* exitButtonTexture, SDL_Texture* homeButtonTexture, SDL_Rect* restartButtonRect, SDL_Rect* exitButtonRect, SDL_Rect* homeButtonRect);

bool compare_records(const Record& a, const Record& b);

void print_records(const Record records[], int num_records);

void save_records(const Record records[], int num_records, const string& filename);

int load_records(Record records[], int max_records, const string& filename);

void add_record(Record records[], int& num_records, const string& name, int score);

GameStatus leaderboard(Background* background, Record records[], int num_records);

GameStatus Game(Bird* hero, Pipe* tile, Background* background, GameStatus game_state);

GameStatus EndGame(Background* background, float* current_score, GameStatus game_state);

GameStatus MainMenu(Bird* hero, Pipe* tile, Background* background, GameStatus game_state);

void MainCycle(Bird* hero, Pipe* tile, Background* background, GameStatus game_state);

//int main(int argc, char** argv);

#endif // FUNCTIONS_H