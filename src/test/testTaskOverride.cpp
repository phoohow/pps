#include <pps/pps.h>
#include <iostream>

int main()
{
    pps::DefineCTX conditions;
    conditions.bools = {
        {"@useBaseColorMap", true},
        {"@useBaseColorAlpha", true},
        {"@isRaster", true},
        {"@useShadow", false},
    };

    pps::ReplaceCTX replace;
    replace.texts = {
        {"@useBaseColorMap", "mat.useBaseColorMap"},
        {"@useBaseColorAlpha", "mat.useBaseColorAlpha"},
        {"@useShadow", "scene.useShadow"},
        {"@sLinearWrap", "s10"},
        {"@sLinearClamp", "s20"},
    };

    pps::PPS lang;
    std::string line = R"(
SamplerState s_LinearWrap : register(s0 /*<$override @sLinearWrap>*/);
SamplerState s_LinearClamp : register(s1 /*<$override @sLinearClamp>*/);

/*<$branch if @useBaseColorMap>*/
{
    float4 value = baseColorMap(...);
    color.rgb *= value.rgb;
    /*<$branch if @useBaseColorAlpha>*/
    color.a *= value.a;
    /*<$branch endif>*/
}
/*<$branch endif>*/
/*<$branch if @isRaster && @useShadow>*/
{
    color *= shadow(...);   
}
/*<$branch endif>*/
)";
    auto result = lang.process(line, conditions, replace, pps::IncludeCTX(), false);

    std::cout << "pps result:\n"
              << result << std::endl;
}
