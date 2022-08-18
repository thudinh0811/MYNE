#version 120
uniform sampler2D texture;
uniform float time;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;
    vec4 texColor = texture2D(texture, uv);

    vec3 col = texColor.xyz * (0.5 + 0.5*cos(0.1*time+uv.xyx+vec3(0,2,4)));

    // Output to screen
    gl_FragColor = vec4(col, texColor.a);
}