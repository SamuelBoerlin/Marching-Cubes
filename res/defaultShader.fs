#version 330
in vec3 color_fs;
out vec4 color;

void main()
{
	color = vec4(color_fs, 1.0);
}
