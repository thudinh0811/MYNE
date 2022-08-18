#version 120
uniform sampler2D texture;
uniform float time;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;
    uv.y += 0.025 * sin(0.2 * time);
    vec4 texColor = texture2D(texture, uv);


    // Output to screen
    gl_FragColor = texColor;
}