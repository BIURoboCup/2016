#version 150 //compatible with OpenGL version 3.2

layout(points) in;
layout(line_strip, max_vertices = 2) out;

in vec3 normal[];

uniform float length;
uniform mat4 mvp;
uniform mat4 world_mvp;
uniform mat4 projection;

void main()
{
	mat4 r = mvp * world_mvp * projection;

	vec3 n = normal[0];
		
	vec4 v0 = gl_in[0].gl_Position;
	vec4 v1 = v0 + vec4(n * length, 0.0);

	// Editing vertrexses:

	gl_Position = v0 * r;
	EmitVertex();
	
	gl_Position = v1 * r;
	EmitVertex();
	
	EndPrimitive();

}