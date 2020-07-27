#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

uniform mat4 norm;
uniform mat4 world;
uniform mat4 view;
uniform mat4 proj;

out vec2 uv_frag;
out vec3 normal_interp;
out vec3 vertex_pos;

void main()
{
    uv_frag = uv;
    normal_interp = (norm * vec4(normal, 1.0)).rgb;
    vertex_pos = (view * world * vec4(position, 1.0)).rgb;
    gl_Position = proj * view * world * vec4(position, 1.0);

}
