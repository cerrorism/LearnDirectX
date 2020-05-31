cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	float4x4 worldViewProjection;
};

struct VertexIn {
	float3 pos: POSITION;
	float3 color: COLOR;
};

struct VertexOut {
	float4 pos: SV_POSITION;
	float4 color: COLOR;
};


VertexOut main(VertexIn vin) {
	VertexOut vout;
	float4 pos = float4(vin.pos, 1.0f);
	pos = mul(pos, worldViewProjection);
	vout.pos = pos;
	vout.color = float4(vin.color, 1.0f);
	return vout;
}