#version 150

in vec4 color;
out vec4 outColor;
precision lowp float;

const float colorDepth = 31;

float dither(int x, int y, float c0)
{
    const int bayer[64] = int[64](0, 32, 8, 40, 2, 34, 10, 42,
                                  48, 16, 56, 24, 50, 18, 58, 26, /* pattern. Each input pixel */
                                  12, 44, 4, 36, 14, 46, 6, 38, /* is scaled to the 0..63 range */
                                  60, 28, 52, 20, 62, 30, 54, 22, /* before looking in this table */
                                  3, 35, 11, 43, 1, 33, 9, 41, /* to determine the action. */
                                  51, 19, 59, 27, 49, 17, 57, 25,
                                  15, 47, 7, 39, 13, 45, 5, 37,
                                  63, 31, 55, 23, 61, 29, 53, 21);

    float limit = float((bayer[x * 8 + y]+1))/64.0;

    if(c0 < limit)
        return 0.0;
    return 1.0;
}

float ditherPosterized(int x, int y, float c0) {
    float p = (c0 * colorDepth);
    float d = fract(p);
    float w = floor(p);
    float dith = dither(x, y, c0);
    float scaled = (w + dith) / colorDepth;
    return scaled;
}

void main()
{
    //    vec4 lum = vec4(0.299, 0.587, 0.114, 0);
    //    float grayscale = dot(texture(tex, texcoord), lum);
    //    vec3 rgb = texture(tex, texcoord).rgb;

    vec3 rgb = color.rgb;

    vec2 xy = gl_FragCoord.xy;
    int x = int(mod(xy.x, 8.0));
    int y = int(mod(xy.y, 8.0));

    vec3 finalRGB;
    finalRGB.r = ditherPosterized(x, y, rgb.r);
    finalRGB.g = ditherPosterized(x, y, rgb.g);
    finalRGB.b = ditherPosterized(x, y, rgb.b);
    
    //    float final = find_closest(x, y, grayscale);
    outColor = vec4(finalRGB, 1.0);
}
