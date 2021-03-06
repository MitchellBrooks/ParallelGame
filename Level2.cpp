#include <iostream>
#include <pthread.h>
#include "stdio.h"
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "Player.h"
#include "Enemy.h"
#include "Structure.h"
#include "Level2.h"

Level2::Level2()
{

}

Level2::~Level2()
{
	Mix_FreeChunk(pickup);
	Mix_FreeChunk(nextLevel);
	Mix_FreeChunk(buttonSound);
}

void Level2::activateLevel(SDL_Renderer *renderer, SDL_Rect cameraRect) {
	renderTarget = renderer;
	camera = cameraRect;
	dude1.activatePlayer(renderTarget, "player1.png", 50, 300, 3, 4, 1);
	dude2.activatePlayer(renderTarget, "player2.png", 0, 300, 3, 4, 2);
	lava.x = 0;
	lava.y = 700;
	lava.w = 5000;
	lava.h = 30;
	pin = pinStructure.activateStructure(renderTarget, "", 0, 0, 1, 5000);
	road = roadStructure.activateStructure(renderTarget, "road.png", 0, 550, 1500, 30);
	wall1 = wall1Structure.activateStructure(renderTarget, "wall.png", 200, 350, 250, 30);
	wall2 = wall2Structure.activateStructure(renderTarget, "wall.png", 550, 450, 250, 30);
	wall3 = wall3Structure.activateStructure(renderTarget, "wall.png", 1070, 430, 250, 30);
	wall4 = wall4Structure.activateStructure(renderTarget, "wallVert.png", 1320, 430, 30, 120);
	door = doorStructure.activateStructure(renderTarget, "door.png", 250, 270, 40, 80);
	key = keyStructure.activateStructure(renderTarget, "key.png", 1200, 500, 30, 30);
	KeyOrNo.activateStructure(renderTarget, "keyMini.png", 505, 0, 30, 30);
	button = buttonStructure.activateStructure(renderTarget, "button.png", 900, 505, 40, 50);
	gate = gateStructure.activateStructure(renderTarget, "gate.png", 1070, 451, 100, 100);
	gateUp = gateStructure2.activateStructure(renderTarget, "gate.png", 1070, 360, 100, 100);
	pickup = Mix_LoadWAV("pickup.wav");
	nextLevel = Mix_LoadWAV("levelUp.wav");
	buttonSound = Mix_LoadWAV("gateOpen2.wav");
}

void Level2::Draw()
{
	if (keys[SDL_SCANCODE_SPACE]) {
		cam = 2;
	}

	if (keys[SDL_SCANCODE_DELETE]) {
		cam = 1;
	}

	if (cam % 2 == 0)
		camera.x = dude1.GetOriginX() - 620;
	else {
		camera.x = dude2.GetOriginX() - 620;
	}
	if (camera.x < 0)
		camera.x = 0;

	roadStructure.Draw(renderTarget, camera);
	wall1Structure.Draw(renderTarget, camera);
	wall2Structure.Draw(renderTarget, camera);
	wall3Structure.Draw(renderTarget, camera);
	wall4Structure.Draw(renderTarget, camera);
	doorStructure.Draw(renderTarget, camera);
	buttonStructure.Draw(renderTarget, camera);
	if (!hasKey) {
		keyStructure.Draw(renderTarget, camera);
		SDL_SetTextureColorMod(doorStructure.texture, 255, 0, 0);
	}
	else {
		KeyOrNo.DrawStill(renderTarget);
	}
	if (!hasPressedButton) {
		gateStructure.Draw(renderTarget, camera);
		gate.y = 451;
		gateUp.y = 451;
		buttonKey = 0;
	}
	else {
		if (buttonKey == 0) {
			Mix_PlayChannel(1, buttonSound, 0);
			buttonKey = 1;
		}
		gateStructure2.Draw(renderTarget, camera);
		gate.y = 360;
		gateUp.y = 360;
	}
}

void Level2::DrawP1()
{
	dude1.Draw(renderTarget, camera);
}

void Level2::DrawP2()
{
	dude2.Draw(renderTarget, camera);
}


void Level2::go(int p)
{

	if (p == 1) {
		dude1.IntersectsWith(road);
		dude1.IntersectsWith(wall1);
		dude1.IntersectsWith(wall2);
		dude1.IntersectsWith(wall3);
		dude1.IntersectsWith(wall4);
		dude1.IntersectsWith(pin);
		dude1.IntersectsWith(gate);
		dude1.IntersectsWith(gateUp);
		dude1.Update(keys);
	}
	else {
		dude2.IntersectsWith(road);
		dude2.IntersectsWith(wall1);
		dude2.IntersectsWith(wall2);
		dude2.IntersectsWith(wall3);
		dude2.IntersectsWith(wall4);
		dude2.IntersectsWith(pin);
		dude2.IntersectsWith(gate);
		dude2.IntersectsWith(gateUp);
		dude2.Update(keys);

	}

	if (dude1.Passes(button) || dude2.Passes(button)) {
		hasPressedButton = true;
	}
	else {
		hasPressedButton = false;
	}

	if (dude1.Passes(key) || dude2.Passes(key)) {
		hasKey = true;
		Mix_PlayChannel(1, pickup, 0);
		pickup = nullptr;
		SDL_SetTextureColorMod(doorStructure.texture, 255, 255, 255);
		
	}


	if (hasKey) {
		if (dude1.Passes(door) && dude2.Passes(door)) {
			Mix_PlayChannel(1, nextLevel, 0);
			lvl = 3;
		}
	}

	if (dude1.Passes(lava)) {
		p1Health = 0;
	}

	if (dude2.Passes(lava)) {
		p2Health = 0;
	}

	checkDeath();
}

void Level2::checkDeath()
{
	if (p1Health == 0 || p2Health == 0)
	{
		dude1.positionRect.x = 40;
		dude1.positionRect.y = 0;
		dude2.positionRect.x = 20;
		dude2.positionRect.y = 0;
		hasKey = false;
		pickup = Mix_LoadWAV("pickup.wav");
		p1Health = 3;
		p2Health = 3;
	}
}