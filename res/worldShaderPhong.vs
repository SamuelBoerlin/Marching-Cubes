#version 330

layout(location = 0) in vec3 vertex;
layout(location=1) in vec3 vertex_color;
layout(location = 2) in vec3 vertex_normal;

uniform mat4 modelview;
uniform mat4 projection;
uniform vec3 lightPos;

out vec3 vertexNormal;
out vec3 eyeDir;
out vec3 lightDir;

out vec3 color_fs;

out vec3 vertex_fs;

void main()
{
	vertex_fs = vertex;
	color_fs = vertex_color;

	mat4 normalMatrix = transpose(inverse(modelview));
	vertexNormal = (normalMatrix * vec4(vertex_normal,0.0)).xyz;
	
	vec3 vertexInCamSpace = (modelview * vec4(vertex,1.0)).xyz;
	eyeDir = -vertexInCamSpace.xyz;
	
	vec3 lightInCamSpace = vec4(modelview * vec4(lightPos,1.0)).xyz;
	//lightDir = vec3(lightInCamSpace - vertexInCamSpace).xyz;
	lightDir = vec3(0.2, 0.5, 0.3).xyz;
	
	gl_Position = projection * vec4(vertexInCamSpace,1.0);
}
