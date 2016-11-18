#version 150

in vec4 color;
out vec4 outColor;
//varying vec2 gl_fragCoord;

//vec2 iResolution = vec2(400.0, 400.0);
//
void main(void)
{
////    vec2 modul = mod(gl_fragCoord,x);
////    vec2 normalizedCord= vec2(fragCoord-modul)/iResolution.xy;
//    vec4 color = texture2D(iChannel0, normalizedCord);
//    vec4 c = vec4(1, 1, 1, 1);
//    float r = mod(gl_FragCoord.x, 10);
//    c.g = (gl_FragColor.x - r) / 600.0;

    outColor = color;

/*
    float pixelation = 5.0;

    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 rg = floor(mod(4.0 * uv, 1.0) * pixelation) / pixelation;

    gl_FragColor = vec4(rg, 1.0, 1.0);
 */

}

//uniform sampler2D tex;

