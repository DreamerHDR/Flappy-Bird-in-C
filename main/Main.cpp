#include "Functions.h"

int main(int argc, char** argv)
{
	srand(time(NULL));
	setlocale(LC_ALL, "ru");
	bool quit = false;
	if (!init())
		return -1;

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		return 1;
	}

	Mix_Chunk* sound = Mix_LoadWAV_RW(SDL_RWFromFile("jump.wav", "rb"), 1);
	if (sound == nullptr) {
		return 1;
	}

	Mix_Chunk* sound1 = Mix_LoadWAV_RW(SDL_RWFromFile("point.wav", "rb"), 1);
	if (sound1 == nullptr) {
		return 1;
	}

	Mix_Chunk* sound2 = Mix_LoadWAV_RW(SDL_RWFromFile("hit.wav", "rb"), 1);
	if (sound1 == nullptr) {
		return 1;
	}


	Bird hero;

	Pipe tile[5]; //TOTAL_PIPES 

	Background background;

	InitBird(&hero.ren_texture);
	InitPipes(&tile->ren_texture);
	InitBackground(&background.ren_texture);

	printf_s("Ведите имя: ");
	getline(cin, hero.name);

	GameStatus game_state;
	game_state = MENU;
	MainCycle(&hero, tile, &background, game_state);
	Mix_PlayChannel(-1, sound, 0);
	Mix_PlayChannel(-1, sound1, 0);			
	FreeBird(&hero.ren_texture);
	FreeTextureBackground(&background.ren_texture);
	close();

	return 0;
}