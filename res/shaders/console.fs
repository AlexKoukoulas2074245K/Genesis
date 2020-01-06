#version 330 core

in vec2 uv_frag;

uniform sampler2D tex;
uniform float opaqueness;

out vec4 frag_color;

void main()
{
    float finalUvX = uv_frag.x;    
    float finalUvY = 1.00 - uv_frag.y;    

    frag_color = vec4(0.0, 0.0, 0.0, opaqueness);	
}
