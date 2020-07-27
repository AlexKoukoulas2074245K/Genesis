#version 330 core

in vec2 uv_frag;

uniform float opaqueness;

out vec4 frag_color;

void main()
{
	// Directly set console color
    frag_color = vec4(0.0, 0.0, 0.0, opaqueness);	
}
