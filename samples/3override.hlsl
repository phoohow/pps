SamplerState s_LinearWrap : register(s0 /*<$overide s@sLinearWrap>*/);
SamplerState s_LinearClamp : register(s1 /*<$overide s@sLinearClamp>*/);
/*<$branch if @hasOcclusion>*/
Texture2D t_Occlusion : register(t0 /*<$overide t@tOcclusion>*/);
/*<$branch endif>*/

void main(out float4 /*<$override @type>*/ color)
{
    /*<$branch if @hasOcclusion>*/
    color.rgb *= t_Occlusion.Sample(s_LinearWrap, uv).r;
    /*<$branch endif>*/
}
