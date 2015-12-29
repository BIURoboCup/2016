#version 150 //compatible with OpenGL version 3.2

in vec4 vPosition;
in vec3 vNormal;

out vec3 normal;

void main()
{
	
	//gl_Position is a vec4 built-in GLSL output variable that holds the transformed vertex position
	gl_Position = vPosition;
	normal = vNormal;

	
}
