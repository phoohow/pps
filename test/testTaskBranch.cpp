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
    };

    pps::PPS    lang;
    std::string line = R"(
/*<$dynamic if @useBaseColorMap>*/
{
    float4 value = baseColorMap(...);
    color.rgb *= value.rgb;
    /*<$dynamic if @useBaseColorAlpha>*/
    color.a *= value.a;
    /*<$dynamic endif>*/
}
/*<$dynamic endif>*/
/*<$dynamic if @isRaster && @useShadow>*/
{
    color *= shadow(...);   
}
/*<$dynamic endif>*/
)";

    auto result = lang.process(line, &ctx);

    std::cout << "pps result:\n"
              << result << std::endl;
}
