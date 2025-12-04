void main(out float4 color)
{
    /*<$dynamic if @hasBaseColorMap>*/
    {
        float4 value = texture(baseColorMap, uv);
        color.rgb *= value.rgb;

        /*<$static if @useBaseColorMapAlpha>*/
        color.a *= value.a;
        /*<$static else>*/
        color.a *= 0.5;
        /*<$static endif>*/
    }
    /*<$dynamic endif>*/

    /*<$dynamic if @isRaster && @useShadow>*/
    {
        float shadow = PCSS(...);
        color.rgb *= shadow;
    }
    /*<$dynamic elif @isRayTracing>*/
    {
        float shadow = RayShow(...);
        color.rgb *= shadow;
    }
    /*<$dynamic else>*/
    {
        color.rgb *= 0.9f;
    }
    /*<$dynamic endif>*/
}
