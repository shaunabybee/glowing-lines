#version 330 core

layout(lines) in;
layout(triangle_strip, max_vertices = 64) out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float radius;

uniform float uTime;

mat4 PVM = projection * view * model;

vec4 BLUE = vec4(0.39f, 0.92f, 1.f, 1.f);

vec4 pos; 

const float PI = 3.14159;

void main()
{   
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;

    // This draws a hollow tube of triangle strips around the original points
    for (float i = 0.; i <= 2. * PI; i += PI / 8.)
    {

        float newX = radius * cos(i) + v0.x;
        float newY = radius * sin(i) + v0.y;
        pos = PVM * vec4(newX, newY, v0.z, v0.w);
        gl_Position = pos;
        EmitVertex();

        newX = radius * cos(i) + v1.x;
        newY = radius * sin(i) + v1.y;
        pos = PVM * vec4(newX, newY, v1.z, v1.w);
        gl_Position = pos;
        EmitVertex();

    }

    EndPrimitive();


} 