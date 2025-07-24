int main(out float4 color)
{
    int samples = 0;

    /*<$prog start>*/
    /*<$prog define int @_samples>*/
    /*<$prog if @isNodeMode>*/
        /*<$prog @_samples = 1>*/
    /*<$prog elif @isSingleMode>*/
        /*<$prog @_samples = @samples>*/
    /*<$prog elif @isDoubleMode>*/
        /*<$prog @_samples = 2 * @samples>*/
    /*<$prog endif>*/
    /*<$prog export @_samples>*/
    /*<$prog end>*/
    for (int i = 0; i < samples /*<$override @_samples>*/; ++i)
    {
    }
}
