#version 330 core

uniform sampler2D tex;
uniform bool flip_tex_hor;
uniform bool flip_tex_ver;
uniform vec4 material_ambient;
uniform vec4 material_diffuse;
uniform vec4 material_specular;
uniform float material_shininess;
uniform int is_affected_by_light;
uniform vec3 light_positions[32];
uniform vec3 eye_pos;

in vec2 uv_frag;
in vec3 normal_interp;
in vec3 frag_pos;

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

	// Calculate view direction
	vec3 view_direction = normalize(eye_pos - frag_pos);

	vec4 light_accumulator = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 32; ++i)
	{
		vec3 light_direction = normalize(light_positions[i]);

		vec4 diffuse_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
		vec4 specular_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);

		float diffuse_factor = max(dot(normal, light_direction), 0.0f);
		if (diffuse_factor > 0.0f)
		{
			diffuse_color = material_diffuse * diffuse_factor;
			diffuse_color = clamp(diffuse_color, 0.0f, 1.0f);	

			vec3 reflected_direction = normalize(reflect(-light_direction, normal));

			specular_color = material_specular * pow(max(dot(view_direction, reflected_direction), 0.0f), material_shininess);
			specular_color = clamp(specular_color, 0.0f, 1.0f);
		}
		

		light_accumulator.rgb += (diffuse_color + specular_color).rgb;
	}

	frag_color = tex_color;

	if (is_affected_by_light == 1)
	{
		frag_color = frag_color * material_ambient + light_accumulator;
	}	

}
