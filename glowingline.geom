#version 330 core
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable

layout(lines) in;
layout(triangle_strip, max_vertices = 200) out;

out vec4 pos;
out vec4 color;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec3 uColor;

uniform float radius;
uniform float zOffset;

uniform float uTime;

mat4 PVM = projection * view * model;

const float PI = 3.14159;

void main()
{
    // This shader creates large, semi-transparent discs that end up looking like a glowing halo
    vec4 v0 = gl_in[0].gl_Position;
    vec4 v1 = gl_in[1].gl_Position;

    vec4 centerColor = vec4(uColor, 0.1f);
    vec4 edgeColor = vec4(uColor, 0.f);

    v0.z = v0.z + zOffset;
    v1.z = v1.z + zOffset;

    for (float i = 0.; i <= 2. * PI; i += PI / 8.)
    {
        gl_Position = PVM * v0;
        color = centerColor;
        EmitVertex();

        float newX = radius * cos(i) + v0.x;
        float newY = radius * sin(i) + v0.y;
        gl_Position = PVM * vec4(newX, newY, v0.z, v0.w);
        color = edgeColor;
        EmitVertex();

    }

    EndPrimitive();

} 