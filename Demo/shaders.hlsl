float4 vs_main(float2 inPos: POSITION) : SV_POSITION
{
    return float4(inPos, 0, 1);
}

float4 ps_main() : SV_TARGET
{
    return float4(0.2, 0.15, 0.9, 1.0);
}