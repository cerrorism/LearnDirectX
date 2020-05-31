struct VertexOut {
	float4 pos: SV_POSITION;
	float4 color: COLOR;
};

float4 main(VertexOut vout) : SV_TARGET
{
	return vout.color;
}