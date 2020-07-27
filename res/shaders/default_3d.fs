#version 330 core

uniform sampler2D tex;
uniform bool flip_tex_hor;
uniform bool flip_tex_ver;


vec3 light_pos = vec3(0.0f, 0.0f, 2.0f);
const vec4 material_ambient = vec4(0.2f, 0.2f, 0.2f, 1.0f);
const vec4 material_diffuse = vec4(0.3f, 0.3f, 0.3f, 1.0f);
const vec4 material_specular = vec4(0.7f, 0.7f, 0.7f, 1.0f);
const float material_shininess = 2.0f;


in vec2 uv_frag;
in vec3 normal_interp;
in vec3 vertex_pos;

out vec4 frag_color;

void main()
{
	// Calculate final uvs
    float finalUvX = uv_frag.x;
    if (flip_tex_hor) finalUvX = 1.00f - finalUvX;

    float finalUvY = 1.00f - uv_frag.y;
    if (flip_tex_ver) finalUvY = 1.00f - finalUvY;
	
	// Get texture color
    vec4 tex_color = texture(tex, vec2(finalUvX, finalUvY));

    // Normalize normal 
	vec3 normal = normalize(normal_interp);
	vec3 light_direction = normalize(light_pos - vertex_pos);

	vec3 eye_pos = normalize(-vertex_pos);
	vec3 reflected_direction = normalize(-reflect(light_direction, normal));

	vec4 diffuse_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	vec4 specular_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

	float diffuse_factor = max(dot(normal, light_direction), 0.0f);
	if (diffuse_factor > 0.0f)
	{
		diffuse_color = material_diffuse * diffuse_factor;
		diffuse_color = clamp(diffuse_color, 0.0f, 1.0f);

		specular_color = material_specular * pow(max(dot(reflected_direction, eye_pos), 0.0f), 0.3 * material_shininess);
		specular_color = clamp(specular_color, 0.0f, 1.0f);
	}

	frag_color = tex_color * material_ambient + diffuse_color + specular_color;	

}
