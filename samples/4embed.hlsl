SamplerState s_LinearWrap : register(s0);

/*<$macro if @hasEmbedAO>*/
/*<$embed @embedAO>*/
/*<$macro endif>*/

void main(out float4 color)
{
    float ao = 1.0f;
    /*<$macro if @hasEmbedAO>*/
    /*<$embed #embedAO>*/;
    /*<$macro endif>*/

    color *= ao;
}

// Note: embedAO like this
// define @embedAO
// define #embedAO
//
// @embedAO
// void evaluateAO(inout float ao)
// {
//     ao *= t_BindlessTextures[/*<@insert @aoSlot>*/].texture(s_LinearClamp, uv);
//     ao *= scene.aoScale;
//     ao = clamp(ao, 0.0, 1.0);
// }
//
// #embedAO
// evaluateAO(ao);
