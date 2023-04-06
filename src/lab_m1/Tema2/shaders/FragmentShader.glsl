#version 330

in vec3 frag_color;
in vec2 texcoord;

layout(location = 0) out vec4 out_color;

uniform int has_texture;
uniform sampler2D texture_1;

void main()
{
   if (has_texture == 1)
	{
		out_color = texture(texture_1, texcoord);
	}
	else 
	{
		out_color = vec4(frag_color, 1);
	}
}