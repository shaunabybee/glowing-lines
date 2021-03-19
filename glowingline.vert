#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

mat4 PVM = projection * view * model;

uniform float uTime;
uniform sampler3D noise3;


void main()
{
    vec4 n1 = texture(noise3, aPos * .01f + (uTime / 2.));
    float nx = n1.r + n1.g + n1.b + n1.a - 2.;
    nx *= 5.;

    vec4 n2 = texture(noise3, aPos * .02f + (uTime / 2.));
    float ny = n2.r + n2.g + n2.b + n2.a - 2.;
    ny *= 5.;

    gl_Position = vec4(aPos.x + nx, aPos.y + ny, aPos.z, 1.0);
}