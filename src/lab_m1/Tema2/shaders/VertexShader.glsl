#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_position;
layout(location = 3) in vec3 v_color;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 car_pos;

out vec3 frag_color;
out vec2 texcoord;

void main()
{
    frag_color = v_color;
    texcoord = v_texture_position;
    float scaleFactor = 0.015;
    vec4 position = Model * vec4(v_position, 1.0);
    vec3 dif = car_pos - vec3(position.x, position.y, position.z);
    position.y = position.y - scaleFactor * (dif.x * dif.x + dif.y * dif.y + dif.z * dif.z);
    gl_Position = Projection * View * position;
    
}
