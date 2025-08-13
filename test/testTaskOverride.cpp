#include <pps/pps.h>
#include <iostream>

int main()
{
    pps::Context ctx;
    ctx.isStatic = false;
    ctx.bools    = {
        {"@useBaseColorMap", true},
        {"@useBaseColorAlpha", true},
        {"@isRaster", true},
        {"@useShadow", false},
    };
    ctx.instances = {
        {"@useBaseColorMap", "mat.useBaseColorMap"},
        {"@useBaseColorAlpha", "mat.useBaseColorAlpha"},
        {"@useShadow", "scene.useShadow"},
        {"@sLinearWrap", "s10"},
        {"@sLinearClamp", "s20"},
    };

    std::string line = R"(
SamplerState s_LinearWrap : register(s0 /*<$override @sLinearWrap>*/);
SamplerState s_LinearClamp : register(s1 /*<$override @sLinearClamp>*/);

/*<$instance if @useBaseColorMap>*/
{
    float4 value = baseColorMap(...);
    color.rgb *= value.rgb;
    /*<$instance if @useBaseColorAlpha>*/
    color.a *= value.a;
    /*<$instance endif>*/
}
/*<$instance endif>*/
/*<$instance if @isRaster && @useShadow>*/
{
    color *= shadow(...);   
}
/*<$instance endif>*/
)";

    pps::PPS lang;
    auto     result = lang.process(line, &ctx);

    std::cout << "pps result:\n"
              << result << std::endl;
}
