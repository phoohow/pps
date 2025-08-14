/*<$static if @useBasic>*/
/*<$include util/basic.hlsl>*/
/*<$static endif>*/

void main(out float4 color)
{
    color = float4(1.0, 0.0, 0.0, pow5(0.8));
}