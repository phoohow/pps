SamplerState s_LinearWrap : register(s0 /*<$override @sLinearWrap>*/);
SamplerState s_LinearClamp : register(s1 /*<$override @sLinearClamp>*/);
/*<$static if @hasOcclusion>*/
Texture2D t_Occlusion : register(t0 /*<$override @tOcclusion>*/);
/*<$static endif>*/

void main(out float4 /*<$override @type>*/ color)
{
    /*<$static if @hasOcclusion>*/
    color.rgb *= t_Occlusion.Sample(s_LinearWrap, uv).r;
    /*<$static endif>*/
}
