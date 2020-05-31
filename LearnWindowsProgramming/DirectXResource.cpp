#include "DirectXResource.h"


using winrt::com_ptr;
using winrt::check_hresult;
using DirectX::XMStoreFloat4x4;
using DirectX::XMMatrixIdentity;
using DirectX::XMVECTORF32;
using DirectX::XMVECTOR;
using DirectX::XMFLOAT4;
using DirectX::XMFLOAT3;
using DirectX::XMVectorSet;
using DirectX::XMVectorZero;
using DirectX::XMMATRIX;
using DirectX::XMMatrixLookAtLH;
using DirectX::XMMatrixPerspectiveFovLH;
using DirectX::XMFLOAT4X4;

template<typename T>
com_ptr<ID3D11Buffer> createBuffer(const com_ptr<ID3D11Device>& device, const std::vector<T>& data, D3D11_BIND_FLAG bind) {

	CD3D11_BUFFER_DESC bufferDesc(
		sizeof(T) * data.size(),
		bind
	);
	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = &data[0];
	com_ptr<ID3D11Buffer> buffer;
	check_hresult(device->CreateBuffer(&bufferDesc, &initData, buffer.put()));
	return buffer;
}

template<typename T>
const T* single_list(const T& t) {
	return &t;
}

static const D3D_FEATURE_LEVEL featureLevels[] = {
	D3D_FEATURE_LEVEL_11_1,
	D3D_FEATURE_LEVEL_11_0,
};



DirectXResource::DirectXResource(DXGI_FORMAT backBufferFormat, DXGI_FORMAT depthBufferFormat,
	UINT backBufferCount, unsigned int flags)
	:backBufferFormat(backBufferFormat), depthBufferFormat(depthBufferFormat),
	backBufferCount(backBufferCount),
	options(flags | FlipPresentFlag)
{
}

void DirectXResource::createDevice()
{
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(_DEBUG)
	// If the project is in a debug build, enable debugging via SDK Layers with this flag.
	creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	creationFlags |= AllowTearingFlag | EnableHDRFlag | FlipPresentFlag;

	check_hresult(D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		creationFlags,
		featureLevels,
		2,
		D3D11_SDK_VERSION,
		device.put(),
		&featureLevel,
		context.put()
	));
#if defined(_DEBUG)
	{
		com_ptr<ID3D11Debug> d3dDebug = device.as<ID3D11Debug>();
		com_ptr<ID3D11InfoQueue> d3dInfoQueue = d3dDebug.as<ID3D11InfoQueue>();
		d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
		d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
		d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, true);
		d3dInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_INFO, true);
		D3D11_MESSAGE_ID hide[] =
		{
			D3D11_MESSAGE_ID_SETPRIVATEDATA_CHANGINGPARAMS,
		};
		D3D11_INFO_QUEUE_FILTER filter = {};
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		d3dInfoQueue->AddStorageFilterEntries(&filter);
	}
#endif
}

void DirectXResource::createWindowResources(HWND hwnd) {
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc.Format = backBufferFormat;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.OutputWindow = hwnd;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = backBufferCount;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

	com_ptr<IDXGIAdapter> adapter;
	device.as<IDXGIDevice>()->GetAdapter(adapter.put());
	factory.capture(adapter, &IDXGIAdapter::GetParent);
	check_hresult(factory->CreateSwapChain(
		device.get(),
		&swapChainDesc,
		swapChain.put()
	));
	configureBackBuffer();
}

void DirectXResource::configureBackBuffer()
{
	backBuffer.capture(swapChain, &IDXGISwapChain::GetBuffer, 0);
	check_hresult(device->CreateRenderTargetView(backBuffer.get(), nullptr, renderTargetView.put()));
	backBuffer->GetDesc(&backBufferDesc);
	CD3D11_TEXTURE2D_DESC depthStencilDesc(
		depthBufferFormat,
		static_cast<UINT> (backBufferDesc.Width),
		static_cast<UINT> (backBufferDesc.Height),
		1, // This depth stencil view has only one texture.
		1, // Use a single mipmap level.
		D3D11_BIND_DEPTH_STENCIL
	);
	check_hresult(device->CreateTexture2D(&depthStencilDesc, nullptr, depthStencilBuffer.put()));
	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	check_hresult(device->CreateDepthStencilView(depthStencilBuffer.get(), &depthStencilViewDesc, depthStencilView.put()));

	ZeroMemory(&viewPoint, sizeof(D3D11_VIEWPORT));
	viewPoint.Height = (float)backBufferDesc.Height;
	viewPoint.Width = (float)backBufferDesc.Width;
	viewPoint.MinDepth = 0;
	viewPoint.MaxDepth = 1;
	context->RSSetViewports(1, &viewPoint);
}

void DirectXResource::releaseBackBuffer() {
	ID3D11RenderTargetView* nullView[] = { nullptr };
	context->OMSetRenderTargets(1, nullView, nullptr);
	renderTargetView = nullptr;
	backBuffer = nullptr;
	depthStencilView = nullptr;
	depthStencilBuffer = nullptr;
	context->Flush();
}

void DirectXResource::resize(int width, int height)
{
	if (swapChain) {
		releaseBackBuffer();
		check_hresult(swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING));
		configureBackBuffer();
	}
}

void DirectXResource::clear() {
	const float teal[] = { 0.098f, 0.439f, 0.439f, 1.000f };
	context->ClearRenderTargetView(
		renderTargetView.get(),
		teal
	);
	context->ClearDepthStencilView(
		depthStencilView.get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);
	context->OMSetRenderTargets(
		1,
		single_list(renderTargetView.get()),
		depthStencilView.get()
	);
}

void DirectXResource::draw(const LoadedModel& model, const Matrix& world, const Matrix& view, const Matrix& projection)
{
	XMFLOAT4X4 preCalculatedMatrix;
	DirectX::XMStoreFloat4x4(&preCalculatedMatrix,
		DirectX::XMMatrixTranspose(
		DirectX::XMLoadFloat4x4(&world) *
			DirectX::XMLoadFloat4x4(&view) *
			DirectX::XMLoadFloat4x4(&projection)
		));

	context->UpdateSubresource(
		model.constantBuffer.get(),
		0,
		nullptr,
		&preCalculatedMatrix,
		0,
		0
	);
	

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, single_list(model.vertexBuffer.get()), &stride, &offset);
	context->IASetIndexBuffer(model.indexBuffer.get(), DXGI_FORMAT_R32_UINT, 0);

	context->IASetInputLayout(model.inputLayout.get());
	context->IASetPrimitiveTopology(model.topology);
	context->VSSetShader(model.vertexShader.get(), nullptr, 0);
	context->VSSetConstantBuffers(
		0,
		1,
		single_list(model.constantBuffer.get())
	);
	context->PSSetShader(model.pixelShader.get(), nullptr, 0);
	context->DrawIndexed(model.indexSize, 0, 0);
}

LoadedModel DirectXResource::loadModel(const Model& model)
{
	LoadedModel loadedModel = {};

	check_hresult(device->CreateVertexShader(
		&model.vertexSharder[0], model.vertexSharder.size(), nullptr, loadedModel.vertexShader.put()));

	check_hresult(device->CreatePixelShader(
		&model.pixelSharder[0], model.pixelSharder.size(), nullptr, loadedModel.pixelShader.put()));
	check_hresult(device->CreateInputLayout(
		&vertexDesc[0], vertexDesc.size(), &model.vertexSharder[0], model.vertexSharder.size(), loadedModel.inputLayout.put()));

	CD3D11_BUFFER_DESC bufferDesc(sizeof(XMFLOAT4X4), D3D11_BIND_CONSTANT_BUFFER);
	check_hresult(device->CreateBuffer(&bufferDesc, nullptr, loadedModel.constantBuffer.put()));

	loadedModel.vertexBuffer = createBuffer(device, model.vertex, D3D11_BIND_VERTEX_BUFFER);
	loadedModel.indexBuffer = createBuffer(device, model.index, D3D11_BIND_INDEX_BUFFER);
	loadedModel.indexSize = model.index.size();
	return loadedModel;
}

void DirectXResource::present() {
	check_hresult(swapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING));
}
