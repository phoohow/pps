SamplerState s_LinearWrap : register(s0 /*<$overide @sLinearWrap>*/);
SamplerState s_LinearClamp : register(s1 /*<$overide @sLinearClamp>*/);
/*<$static if @hasOcclusion>*/
Texture2D t_Occlusion : register(t0 /*<$overide @tOcclusion>*/);
/*<$static endif>*/

void main(out float4 /*<$override @type>*/ color)
{
    /*<$static if @hasOcclusion>*/
    color.rgb *= t_Occlusion.Sample(s_LinearWrap, uv).r;
    /*<$static endif>*/
}
