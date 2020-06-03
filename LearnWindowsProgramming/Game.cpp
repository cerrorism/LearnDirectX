#include "Game.h"
#include "VertexShader.h"
#include "Cube.h"

namespace
{
	const DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	const DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT;

	unsigned int c_targetSampleCount = 4;
}

Game::Game()
{
	resource = std::make_unique<DirectXResource>(backBufferFormat, depthBufferFormat, 2);
	models.push_back(Cube());
}

void Game::setWindow(HWND window)
{
	resource->createDevice();
	this->window = window;
	resource->createWindowResources(window);

	world = Matrix::Identity;
	for (const Model& m : models) {
		loadedModels.push_back(resource->loadModel(m));
	}
}

void Game::updateWindowSize(int width, int height)
{
	DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.7f, 1.5f, 0.f);
	DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, -0.1f, 0.0f, 0.f);
	DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.f);

	DirectX::XMStoreFloat4x4(
		&view,
		DirectX::XMMatrixLookAtRH(
			eye,
			at,
			up
		)
	);

	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

	DirectX::XMStoreFloat4x4(
		&projection,
		DirectX::XMMatrixPerspectiveFovRH(
			DirectX::XMConvertToRadians(70),
			aspectRatio,
			0.01f,
			100.0f
		)
	);
}


void Game::calculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0;
	++frameCnt;
	if (timer->GetTotalSeconds() - timeElapsed >= 1.0) {
		auto fps = frameCnt;
		auto mspf = 1000.0 / fps;
		std::wostringstream outs;
		outs.precision(6);
		outs << "test window" << TEXT(" FPS: ") << fps << TEXT(" Frame Time: ") << mspf << TEXT(" (ms)");
		SetWindowText(window, outs.str().c_str());
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void Game::tick()
{
	timer->Tick([&]
		{
			update();
			render();
			calculateFrameStats();
		});
	
}

void Game::update()
{
	DirectX::XMStoreFloat4x4(
		&world,
		DirectX::XMMatrixRotationY(
			DirectX::XMConvertToRadians(
				timer->GetFrameCount() / 500
			)
		)
	);
}

void Game::render()
{
	// Don't try to render anything before the first Update.
	if (timer->GetFrameCount() == 0) {
		return;
	}
	resource->clear();
	for (LoadedModel m : loadedModels) {
		resource->draw(m, world, view, projection);
	}
	resource->present();

	
}

screen_size Game::defaultScreenSize()
{
	return { 800, 600 };
}
