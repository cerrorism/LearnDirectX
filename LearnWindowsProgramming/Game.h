#pragma once

#include "framework.h"
#include "DirectXResource.h"
#include "GameTimer.h"
#include "models.h"

const int maxLoadStringLength = 100;

class Game
{
public:
	Game();
	void setWindow(HWND window);
	void updateWindowSize(int width, int height);
	void tick();
	screen_size defaultScreenSize();

private:
	std::unique_ptr<DirectXResource> resource;
	std::unique_ptr<GameTimer> timer = std::make_unique<GameTimer>();
	HWND window = nullptr;
	/*
	Matrix world;
	Matrix view;
	Matrix projection;
	*/
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
	std::vector<Model> models;
	std::vector<LoadedModel> loadedModels;

	void calculateFrameStats();
	void update();
	void render();
};

