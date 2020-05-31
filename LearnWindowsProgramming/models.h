#pragma once

#include "framework.h"

struct screen_size {
	unsigned int width;
	unsigned int height;

	float aspectRatio() {
		return static_cast<float>(width) / height;
	}
};

struct Vector3 : public DirectX::XMFLOAT3 {
	Vector3() noexcept : XMFLOAT3(0.f, 0.f, 0.f) {}
	constexpr Vector3(float ix, float iy, float iz) noexcept : XMFLOAT3(ix, iy, iz) {}
};

struct Matrix : public DirectX::XMFLOAT4X4 {
	Matrix() noexcept
		: XMFLOAT4X4(1.f, 0, 0, 0,
			0, 1.f, 0, 0,
			0, 0, 1.f, 0,
			0, 0, 0, 1.f) {}
	constexpr Matrix(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) noexcept
		: XMFLOAT4X4(m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33) {}
	static Matrix CreateRotationZ(float radians) noexcept;
	static Matrix CreateViewLookAt(const Vector3& eye, const Vector3& at, const Vector3& up) noexcept;
	static Matrix CreatePerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane) noexcept;
	const static Matrix Identity;
};
inline Matrix Matrix::CreateRotationZ(float radians) noexcept
{
	using namespace DirectX;
	Matrix R;
	XMStoreFloat4x4(&R, XMMatrixRotationZ(radians));
	return R;
}

inline Matrix Matrix::CreateViewLookAt(const Vector3& eye, const Vector3& at, const Vector3& up) noexcept
{
	using namespace DirectX;
	Matrix V;

	XMVECTOR eyev = XMLoadFloat3(&eye);
	XMVECTOR atv = XMLoadFloat3(&at);
	XMVECTOR upv = XMLoadFloat3(&up);
	XMStoreFloat4x4(&V, XMMatrixLookAtRH(eyev, atv, upv));
	return V;
}

inline Matrix Matrix::CreatePerspectiveFieldOfView(float fov, float aspectRatio, float nearPlane, float farPlane) noexcept
{
	using namespace DirectX;
	Matrix P;
	XMStoreFloat4x4(&P, XMMatrixPerspectiveFovRH(fov, aspectRatio, nearPlane, farPlane));
	return P;
}

struct Vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 color;
};

inline const std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDesc = {
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

struct Model {
	std::vector<Vertex> vertex;
	std::vector<UINT> index;
	std::vector<BYTE> vertexSharder;
	std::vector<BYTE> pixelSharder;
};

struct LoadedModel {
	D3D11_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	winrt::com_ptr<ID3D11Buffer> vertexBuffer = nullptr;
	winrt::com_ptr<ID3D11Buffer> indexBuffer = nullptr;
	winrt::com_ptr<ID3D11InputLayout> inputLayout = nullptr;
	winrt::com_ptr<ID3D11VertexShader> vertexShader = nullptr;
	winrt::com_ptr<ID3D11PixelShader> pixelShader = nullptr;
	winrt::com_ptr<ID3D11Buffer> constantBuffer = nullptr;
	int indexSize;
};