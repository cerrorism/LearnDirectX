#pragma once

#include "framework.h"
#include "models.h"
class DirectXResource
{
public:
	static const unsigned int FlipPresentFlag = 0x1;
	static const unsigned int AllowTearingFlag = 0x2;
	static const unsigned int EnableHDRFlag = 0x4;

	DirectXResource(
		DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
		DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT,
		UINT backBufferCount = 2,
		unsigned int flags = FlipPresentFlag);
	void createDevice();
	void createWindowResources(HWND hWnd);
	void resize(int width, int height);
	void clear();
	void draw(const LoadedModel& model, const Matrix& world, const Matrix& view, const Matrix& projection);
	void draw(const LoadedModel& model, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection);
	void present();
	LoadedModel loadModel(const Model& model);
private:
	HWND window = {};
	winrt::com_ptr<IDXGIFactory> factory = nullptr;
	winrt::com_ptr<ID3D11Device> device = nullptr;
	winrt::com_ptr<ID3D11DeviceContext> context = nullptr;
	winrt::com_ptr<IDXGISwapChain> swapChain = nullptr;
	winrt::com_ptr<ID3D11Texture2D> backBuffer = nullptr;
	winrt::com_ptr<ID3D11RenderTargetView> renderTargetView = nullptr;
	winrt::com_ptr<ID3D11Texture2D> depthStencilBuffer = nullptr;
	winrt::com_ptr<ID3D11DepthStencilView> depthStencilView = nullptr;

	DXGI_FORMAT backBufferFormat;
	DXGI_FORMAT depthBufferFormat;
	UINT backBufferCount;
	D3D_FEATURE_LEVEL featureLevel;
	UINT options;

	D3D11_TEXTURE2D_DESC backBufferDesc = {};
	D3D11_VIEWPORT viewPoint = {};

	void configureBackBuffer();
	void releaseBackBuffer();
};