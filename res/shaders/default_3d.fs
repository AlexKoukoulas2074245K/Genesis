#version 330 core

in vec2 uv_frag;

uniform sampler2D tex;
uniform bool flip_tex_hor;
uniform bool flip_tex_ver;

out vec4 frag_color;

void main()
{
	// Calculate final uvs
    float finalUvX = uv_frag.x;
    if (flip_tex_hor) finalUvX = 1.00 - finalUvX;

    float finalUvY = 1.00 - uv_frag.y;
    if (flip_tex_ver) finalUvY = 1.00 - finalUvY;
	
	// Get texture color
    frag_color = texture(tex, vec2(finalUvX, finalUvY));
}
