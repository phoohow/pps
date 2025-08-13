void main(out float4 color)
{
    /*<$instance if @hasBaseColorMap>*/
    {
        float4 value = texture(baseColorMap, uv);
        color.rgb *= value.rgb;

        /*<$macro if @useBaseColorMapAlpha>*/
        color.a *= value.a;
        /*<$macro else>*/
        color.a *= 0.5;
        /*<$macro endif>*/
    }
    /*<$instance endif>*/

    /*<$instance if @isRaster && @useShadow>*/
    {
        float shadow = PCSS(...);
        color.rgb *= shadow;
    }
    /*<$instance elif @isRayTracing>*/
    {
        float shadow = RayShow(...);
        color.rgb *= shadow;
    }
    /*<$instance else>*/
    {
        color.rgb *= 0.9f;
    }
    /*<$instance endif>*/
}
