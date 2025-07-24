void main(out float4 color)
{
    /*<$branch if @hasBaseColorMap>*/
    {
        float4 value = texture(baseColorMap, uv);
        color.rgb *= value.rgb;

        /*<$branch if @useBaseColorMapAlpha>*/
        color.a *= value.a;
        /*<$branch else>*/
        color.a *= 0.5;
        /*<$branch endif>*/
    }
    /*<$branch endif>*/

    /*<$branch if @isRaster && @useShadow>*/
    {
        float shadow = PCSS(...);
        color.rgb *= shadow;
    }
    /*<$branch elif @isRayTracing>*/
    {
        float shadow = RayShow(...);
        color.rgb *= shadow;
    }
    /*<$branch else>*/
    {
        color.rgb *= 0.9f;
    }
    /*<$branch endif>*/
}
