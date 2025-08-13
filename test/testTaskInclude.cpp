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
    ctx.prefixes = {
        std::string(SOURCE_DIR) + "/include/",
    };

    std::string line = R"(
/*<$include pps/pps.h>*/

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
