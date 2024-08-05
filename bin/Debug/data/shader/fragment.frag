#version 330 core

out vec4 FragColour;

// Uniform
uniform vec2    u_resolution;   // Canvas size (width,height)
uniform vec2    u_mouse;        // mouse position in screen pixels
uniform float   u_time;         // Time in seconds since load
uniform int     u_frame;        // current frame
// Texture
uniform sampler2D texture1;

void main()
{
    vec2 nFragCoord = gl_FragCoord.xy/u_resolution;

    FragColour = texture(texture1, nFragCoord);
}
