#version 120
uniform sampler2D texture;
uniform float time;

void main()
{
    vec2 uv = gl_TexCoord[0].xy;

    vec4 col = texture2D(texture, uv);
    
    float d = pow(abs(abs(uv.y - 0.5) - 0.5), 0.3);
    col = col * d;
    col.r = col.r * 1.05;
    col.b = col.b * cos(0.06 * (time + uv.x * 15.0));
    col.g = col.g * cos(0.06 * (time + uv.x * 10.0));

    // Output to screen
    gl_FragColor = col;
}
