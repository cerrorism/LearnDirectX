#pragma once

#include "models.h"
#include "vertex_shader.h"
#include "pixel_shader.h"

struct Cube : public Model {
	Cube() {
		vertex = {
			{DirectX::XMFLOAT3(-0.5f,-0.5f,-0.5f), DirectX::XMFLOAT3(0, 0, 0),},
			{DirectX::XMFLOAT3(-0.5f,-0.5f, 0.5f), DirectX::XMFLOAT3(0, 0, 1),},
			{DirectX::XMFLOAT3(-0.5f, 0.5f,-0.5f), DirectX::XMFLOAT3(0, 1, 0),},
			{DirectX::XMFLOAT3(-0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(0, 1, 1),},

			{DirectX::XMFLOAT3(0.5f,-0.5f,-0.5f), DirectX::XMFLOAT3(1, 0, 0),},
			{DirectX::XMFLOAT3(0.5f,-0.5f, 0.5f), DirectX::XMFLOAT3(1, 0, 1),},
			{DirectX::XMFLOAT3(0.5f, 0.5f,-0.5f), DirectX::XMFLOAT3(1, 1, 0),},
			{DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f), DirectX::XMFLOAT3(1, 1, 1),},
		};
		index = {
			0,2,1, // -x
			1,2,3,

			4,5,6, // +x
			5,7,6,

			0,1,5, // -y
			0,5,4,

			2,6,7, // +y
			2,7,3,

			0,4,6, // -z
			0,6,2,

			1,3,7, // +z
			1,7,5,
		};
		vertexSharder = std::vector<BYTE>(vertex_shader, vertex_shader + sizeof(vertex_shader));
		pixelSharder = std::vector<BYTE>(pixel_shader, pixel_shader + sizeof(pixel_shader));
	}
};