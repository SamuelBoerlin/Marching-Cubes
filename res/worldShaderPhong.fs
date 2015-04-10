#version 330

struct LightSource
{
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
	vec3 position;
};

struct Material
{
	vec3 ambient_color;
	vec3 diffuse_color;
	vec3 specular_color;
	float specular_shininess;
};

uniform LightSource light;
uniform Material material;

in vec3 vertexNormal;
in vec3 eyeDir;
in vec3 lightDir;

in vec3 color_fs;

in vec3 vertex_fs;

out vec4 color;

in vec3 spos;







uniform sampler2D inShadowMap;

float CalcShadowFactor(vec4 lightspace_Position) {

    vec3 ProjectionCoords = lightspace_Position.xyz / lightspace_Position.w;

    vec2 UVCoords;
    UVCoords.x = 0.5 * ProjectionCoords.x + 0.5;
    UVCoords.y = 0.5 * ProjectionCoords.y + 0.5;

    float Depth = texture(inShadowMap, UVCoords).x;
    if(Depth < (ProjectionCoords.z + 0.001)) return 0.5;
    else return 1.0;
}





void main()
{
	vec3 E = normalize(eyeDir);
	vec3 N = normalize(vertexNormal);
	vec3 L = normalize(lightDir);
	
	vec3 H = normalize(E + L);
	
	vec3 ambient = vec3(0);
	vec3 diffuse = vec3(0);
	vec3 specular = vec3(0);
	vec3 fogColor = vec3(0.0, 0.15, 0.3);
	
	ambient += light.ambient_color;
	diffuse += light.diffuse_color * max(dot(L,N),0) / 60 * (60 - distance(vertex_fs, light.position));
	specular += light.specular_color * pow(max(dot(H,N),0),material.specular_shininess);
	
	vec3 finalColor = color_fs * ambient + color_fs * (diffuse * 4);
	
	float fogDistance = 100;
	
	float fogAmount = clamp(pow(clamp(distance(vertex_fs, light.position) - fogDistance, 0, 50), 2) / 2500, 0, 1);
	color = vec4(mix(finalColor, fogColor, fogAmount), 1.0);
}
