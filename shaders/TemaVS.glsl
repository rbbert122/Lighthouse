#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float time;
uniform bool is_water;

// Output
out vec2 texcoord;
out vec3 world_position;
out vec3 world_normal;

struct Wave {
    vec2 direction;
	float amplitude;
	float frequency;
    float phase;
};

#define MAX_WAVES 64
#define E 2.71828182845904523536

uniform struct Wave waves[MAX_WAVES];

float f(vec3 position, float t, Wave w) {
    vec2 d = w.direction;
    float xz = position.x * d.x + position.z * d.y;
    return w.amplitude * sin(xz * w.frequency + t);
}

vec3 fn(vec3 position, float t, Wave w) {
	vec2 d = w.direction;
	float xz = position.x * d.x + position.z * d.y;
	vec2 norm = w.frequency * w.amplitude * d * cos(xz * w.frequency + t);
    return vec3(norm.x, norm.y, 0.0);
}

float sf(vec3 position, float t, Wave w) {
	vec2 d = w.direction;
	float xz = position.x * d.x + position.z * d.y;
	return w.amplitude * pow(E, sin(xz * w.frequency + t) - 1.0);
}

vec3 sfn(vec3 position, float t, Wave w) {
	vec2 d = w.direction;
	float xz = position.x * d.x + position.z * d.y;
    vec2 norm = w.frequency * w.amplitude * d * cos(xz * w.frequency + t) * pow(E, sin(xz * w.frequency + t) - 1.0);

    return vec3(norm.x, norm.y, 0.0);
}

void main()
{
    // TODO(student): Pass v_texture_coord as output to fragment shader
    texcoord = v_texture_coord;
    if (is_water) {
        world_position = vec3(Model * vec4(v_position, 1.0));
        world_normal = normalize(vec3(Model * vec4(v_normal, 0.0)));

        float height = 0.0;
        vec3 normal = vec3(0.0);
        for (int i = 0; i < MAX_WAVES; i++) {
		    if (waves[i].amplitude > 0.0) {
			    height += sf(world_position, time * 0.5, waves[i]) * 0.2;
			    normal += sfn(world_position, time * 0.5, waves[i]) * 0.2;
		    }
	    }

        world_position = vec3(Model * vec4(v_position + height, 1.0));
        world_normal = normalize(vec3(Model * vec4(normalize(vec3(-normal.x, 1.0, -normal.z)), 0.0)));
    
        gl_Position = Projection * View * Model * vec4(v_position + height, 1.0);
    } else {
        world_position = vec3(Model * vec4(v_position, 1.0));
        world_normal = normalize(vec3(Model * vec4(v_normal, 0.0)));

        gl_Position = Projection * View * Model * vec4(v_position, 1.0);
    }
}
