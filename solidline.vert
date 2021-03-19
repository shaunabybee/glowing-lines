#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

mat4 PVM = projection * view * model;


uniform float uTime;
uniform sampler3D noise3;
uniform float uWaviness;
uniform bool clockwise;
uniform vec3 uOffset;
uniform float zOffset;

void main()
{
    // Get noise for the x-direction
    vec4 n1 = texture(noise3, aPos * .01f + (uTime / 1.));
    float nx = 2. * ( n1.r + n1.g + n1.b + n1.a - 2.);

    // Get noise for the y-direction
    vec4 n2 = texture(noise3, aPos * .02f + (uTime / 1.));
    float ny = 2. * ( n2.r + n2.g + n2.b + n2.a - 2.);

    vec3 pos = aPos;

    // Set the y offset
    pos.y = pos.y + uOffset.y + (0.7 * sin((pos.z) / uWaviness));
    
    // Set the x offset, also spiral either clockwise or counter-clockwise
    if (clockwise)
    {
        pos.x = pos.x + uOffset.x + (0.7 * cos((pos.z + 1.) / uWaviness));
    }
    else
    {
        pos.x = pos.x + uOffset.x + (-0.7 * cos((pos.z + 1.) / uWaviness));
    }

    // Set the z offset
    pos.z += uOffset.z;

    gl_Position = vec4(pos.x + nx, pos.y + ny, pos.z, 1.0);
}