#version 120
uniform sampler2D texture;
uniform float time;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;
    vec4 texColor = texture2D(texture, uv);

    texColor.r = min(1.0, time);

    // Output to screen
    gl_FragColor = texColor;
}