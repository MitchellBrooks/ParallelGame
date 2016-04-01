#include <iostream>
#include <pthread.h>
#include "stdio.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "Player.h"
#include "Enemy.h"
#include "Structure.h"
using namespace std;

#define NUM_THREADS 10


//-------------------------------------------------------------------------------------------------------------

SDL_Texture *texture = nullptr;
SDL_Window *window = nullptr;
SDL_Renderer *renderTarget = nullptr;
int frameWidth, frameHeight;
int textureWidth, textureHeight;


SDL_Event occur;

bool jump = false;
bool ground = true;
int yvel;
int xvel;

const int FPS = 60;
float frameTime = 0;
int currentTime = 0;
int prevTime = 0;
float deltaTime = 0.0f;


SDL_Rect camera;
SDL_Rect ai1;
SDL_Rect ai2;
SDL_Rect wall1;
SDL_Rect road;
SDL_Rect lives;

SDL_Rect playerRect;
SDL_Rect playerPosition;

Player player1;
Player player2;
Enemy enemy1;
Enemy enemy2;
Enemy enemy3;
Structure roadStructure;
Structure wall1Structure;
Structure p1Lives;
Structure p2Lives;

//-------------------------------------------------------------------------------------------------------------

SDL_Texture *LoadTexture(std::string filePath, SDL_Renderer *renderTarget)
{
	SDL_Texture *texture = nullptr;
	SDL_Surface *surface = IMG_Load(filePath.c_str());
	texture = SDL_CreateTextureFromSurface(renderTarget, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void setJump();

void LoadGame()
{
	SDL_Init(SDL_INIT_EVERYTHING);
	window = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1040, 480, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	renderTarget = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	player1.activatePlayer(renderTarget, "player1.png", 0, 400, 3, 4);
	player2.activatePlayer(renderTarget, "player2.png", 600, 400, 3, 4);
	enemy1.activateEnemy(renderTarget, "enemy.png", 100, 100, 1);
	enemy2.activateEnemy(renderTarget, "enemy.png", 1200, 100, 2);
	road = roadStructure.activateStructure(renderTarget, "road.png", 0, 430, 1500, 30);
	wall1 = wall1Structure.activateStructure(renderTarget, "wall.png", 400, 350, 150, 30);
	enemy3.activateEnemy(renderTarget, "enemy.png", 300, 400, 3);
	p1Lives.activateHealth(renderTarget, 0, 0, 150, 40);
	p2Lives.activateHealth(renderTarget, 885, 0, 150, 40);


	texture = LoadTexture("background.png", renderTarget);


	camera.x = 0;
	camera.y = 0;
	camera.w = 640;
	camera.h = 480;




}

void *player1Actions(void *threadid)
{
	
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	bool running = true;


	while (running) {

		SDL_PollEvent(&occur);
		if (occur.type == SDL_QUIT) {
			running = false;
		}

		prevTime = currentTime;
		currentTime = SDL_GetTicks();
		deltaTime = (currentTime - prevTime) / 1000.0f;
		player1.Update(deltaTime, keys);
		player1.IntersectsWith(road);
		player1.IntersectsWith(wall1);
		player1.IntersectsWith(enemy1);
		player1.IntersectsWith(enemy2);
		player1.IntersectsWith(enemy3);


	}
	
	pthread_exit(NULL);
	SDL_Quit();
	return 0;
}

void *player2Actions(void *threadid)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	bool running = true;

	while (running) {

		SDL_PollEvent(&occur);
		if (occur.type == SDL_QUIT) {
			running = false;
		}

		prevTime = currentTime;
		currentTime = SDL_GetTicks();
		deltaTime = (currentTime - prevTime) / 1000.0f;
		player2.Update(deltaTime, keys);
		player2.IntersectsWith(road);
		player2.IntersectsWith(wall1);
		player2.IntersectsWith(enemy1);
		player2.IntersectsWith(enemy2);
		player2.IntersectsWith(enemy3);

	}

	pthread_exit(NULL);
	SDL_Quit();
	return 0;
}

void *aiMove1(void *threadid)
{

	bool running = true;

	while (running) {

		SDL_PollEvent(&occur);
		if (occur.type == SDL_QUIT) {
			running = false;
		}
		enemy1.Update();
		
	}

	pthread_exit(NULL);
	SDL_Quit();
	return 0;
}

void *aiMove2(void *threadid)
{

	bool running = true;

	while (running) {

		SDL_PollEvent(&occur);
		if (occur.type == SDL_QUIT) {
			running = false;
		}
		enemy2.Update();

	}

	pthread_exit(NULL);
	SDL_Quit();
	return 0;
}




/*
void setJump() {
	if (ground && !jump)
	{
		jump = true;
		ground = false;
		yvel = -30;
		player1.y -= 5;

	}
}
*/

void Logic()
{
	

	pthread_t threads[NUM_THREADS];
	int rc;
	int i;

	i = 0;
	rc = pthread_create(&threads[i], NULL,
		player1Actions, (void *)i);

	if (rc) {
		exit(-1);
	}

	i = 1;
	rc = pthread_create(&threads[i], NULL,
		player2Actions, (void *)i);
	if (rc) {
		exit(-1);
	}

	i = 2;
	rc = pthread_create(&threads[i], NULL,
		aiMove1, (void *)i);
	if (rc) {
		exit(-1);
	}


	i = 3;
	rc = pthread_create(&threads[i], NULL,
		aiMove2, (void *)i);
	if (rc) {
		exit(-1);
	}
	
	
}

void DrawScreen()
{
	
	SDL_SetRenderDrawColor(renderTarget, 0, 0, 0, 0);
	SDL_RenderClear(renderTarget);
	SDL_RenderCopy(renderTarget, texture, &camera, NULL);
	player1.Draw(renderTarget, camera);
	player2.Draw(renderTarget, camera);
	enemy1.Draw(renderTarget, camera);
	enemy2.Draw(renderTarget, camera);
	roadStructure.Draw(renderTarget, camera);
	wall1Structure.Draw(renderTarget, camera);
	enemy3.Draw(renderTarget, camera);
	p1Lives.DrawStill(renderTarget);
	p2Lives.DrawStill(renderTarget);
	SDL_RenderPresent(renderTarget);


}


int main(int argc, char *argv[]) {

	int currentTime = 0;
	int prevTime = 0;
	float delta = 0.0f;
	const Uint8 *keyState;


	LoadGame();
	Logic();

	


	bool running = true;
	SDL_Event ev;

	while (running) {
		prevTime = currentTime;
		currentTime = SDL_GetTicks();
		delta= (currentTime - prevTime) / 1000.0f;
		while (SDL_PollEvent(&ev) != 0)
		{
			if (ev.type == SDL_QUIT)
				running = false;
		}
		camera.x = player1.GetOriginX() - 320;
		//camera.y = player1.GetOriginY() - 240;

		if (camera.x < 0)
			camera.x = 0;
		//if (camera.y < 0)
		//	camera.y = 0;
		DrawScreen();
	}

	SDL_DestroyWindow(window);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderTarget);

	texture = nullptr;
	window = nullptr;
	renderTarget = nullptr;
	IMG_Quit();
	SDL_Quit();
	return 0;

}

