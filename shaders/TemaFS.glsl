#version 330

// Input
in vec2 texcoord;
in vec3 world_position;
in vec3 world_normal;

// Uniform properties
uniform sampler2D texture_1;
// TODO(student): Declare various other uniforms
uniform float time;
uniform vec3 eye_position;
uniform bool is_water;
uniform bool is_lighthouse;
uniform vec3 light_house_color;

// Output
layout(location = 0) out vec4 out_color;

struct light_source
{
   int  type;
   vec3 position;
   vec3 color;
   vec3 direction;
   float cut_off_angle;
};

#define MAX_LIGHTS 32
uniform light_source lights[MAX_LIGHTS];
uniform int num_lights;

uniform float ke;
float ka = 0.01;
float kd = 0.3;
float ks = 0.2;
float material_shininess = 100.0;

vec3 point_light_contribution(vec3 light_pos, vec3 light_color)
{
	vec3 L = normalize(light_pos - world_position);
	vec3 V = normalize( eye_position - world_position );
	vec3 H = normalize( L + V );

	float ambient_light = ka;
	float diffuse_light = kd * max( dot( world_normal, L ), 0.0 );
	float specular_light = 0.0;
	if (diffuse_light > 0.0)
	{
		specular_light = ks * pow( max( dot( world_normal, H ), 0.0 ), material_shininess );
	}
	
	float light = 0;
	float d = length(light_pos - world_position);
	float constant_att = 1.0;
	float linear_att = 0.045;
	float quadratic_att = 0.0075;
	float att = 1.0 / (constant_att + linear_att * d + quadratic_att * d * d);
	light = att * (ke + ambient_light + diffuse_light + specular_light);
	return light * light_color * 1.7;
}

vec3 spot_light_contribution(vec3 light_pos, vec3 light_color, vec3 light_dir, float cut_off_angle)
{
	vec3 L = normalize(light_pos - world_position);
	vec3 V = normalize( eye_position - world_position );
	vec3 H = normalize( L + V );
	
	float ambient_light = ka;
	float diffuse_light = kd * max( dot( world_normal, L ), 0.0 );
	float specular_light = 0.0;
	if (diffuse_light > 0.0)
	{
		specular_light = ks * pow( max( dot( world_normal, H ), 0.0 ), material_shininess );
	}
	
	float light = 0.0;

	float cut_off_rad = radians(cut_off_angle);
	float spot_light = dot(-L, light_dir);
	float spot_light_limit	= cos(cut_off_rad);
	if (spot_light > spot_light_limit)
	{
		float linear_att = (spot_light - spot_light_limit) / (1.f - spot_light_limit);
		float light_att_factor = linear_att * linear_att;
		light = ke + ambient_light + light_att_factor * (diffuse_light + specular_light);
	} else {
		light = ke + ambient_light;
	}

	return light * light_color * 1.7;
}

vec3 directional_light_contribution(vec3 light_dir, vec3 light_color)
{
	vec3 direction = vec3(light_dir.x, -light_dir.y, light_dir.z);
	vec3 L = normalize(direction);
	vec3 V = normalize( eye_position - world_position );
	vec3 H = normalize( L + V );
	
	float ambient_light = ka;
	float diffuse_light = kd * max( dot( world_normal, L ), 0.0 );
	float specular_light = 0.0;
	if (diffuse_light > 0.0)
	{
		specular_light = ks * pow( max( dot( world_normal, H ), 0.0 ), material_shininess );
	}
	
	float light = ke + ambient_light + diffuse_light + specular_light;
	return light * light_color;
}

void main()
{
    if (is_water) {
		vec2 scaled_texcoord = texcoord * 10;
		vec2 modulated_coords = (vec2(sin(world_position.x * 10)) + 1) * 0.5;
		vec4 color = texture(texture_1, vec2(scaled_texcoord.x - time * 0.04 + modulated_coords.x * 0.01, scaled_texcoord.y));
		color = color * 1.4;

		vec3 light_contribution = vec3(0);
		for (int i = 0; i < num_lights; ++i)
		{
			if (lights[i].type == 0)
			{
				light_contribution += point_light_contribution(lights[i].position, lights[i].color);
			}
			if (lights[i].type == 1)
			{
				light_contribution += spot_light_contribution(lights[i].position, lights[i].color, lights[i].direction, lights[i].cut_off_angle);
			}
			if (lights[i].type == 2)
			{
				light_contribution += directional_light_contribution(lights[i].direction, lights[i].color);
			}
		}
		color = vec4(color.rgb * light_contribution, color.a);

		out_color = color;
	} else if (is_lighthouse) {
		vec4 color = vec4(light_house_color, 1.0);

		float light = 0.0;
		light = ke;

		color = vec4(color.rgb * light, color.a);
		out_color = color;
	
	} else {
		vec4 color = texture(texture_1, texcoord);

		vec3 light_contribution = vec3(0);
		for (int i = 0; i < num_lights; ++i)
		{
			if (lights[i].type == 0)
			{
				light_contribution += point_light_contribution(lights[i].position, lights[i].color);
			}
			if (lights[i].type == 1)
			{
				light_contribution += spot_light_contribution(lights[i].position, lights[i].color, lights[i].direction, lights[i].cut_off_angle);
			}
			if (lights[i].type == 2)
			{
				light_contribution += directional_light_contribution(lights[i].direction, lights[i].color);
			}
		}
		light_contribution  = clamp(light_contribution, 0.0, 1.0);
		color = vec4(color.rgb * light_contribution, color.a);

		out_color = color;
	}
}
