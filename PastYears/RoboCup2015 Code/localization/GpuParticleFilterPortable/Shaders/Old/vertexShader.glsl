#version 150 //compatible with OpenGL version 3.2

in vec4 vPosition;
in vec3 vNormal;
in vec2 vUV;

out vec4 color;
out vec3 normal;
out vec3 vpos;
out vec2 uv;
out vec3 pos;

uniform mat4 mvp;
uniform mat4 world_mvp;
uniform mat4 projection;

uniform float t;

// Texture
uniform sampler2D uv_tex;
uniform sampler2D uv_n;

// Paint textuers?
uniform int btex;
uniform int btexn;

// Material
// ____________________________________
uniform vec3 material;		// (Ambient, Diffuse, Specular) in [0,1]
uniform	int	 render;		// Render type: 1 = Garoud, 2 = Phong

// Light 1
// ____________________________________
uniform int	 l1_type;		// 0 - Disable,	1 - Point,	2 - Direction
uniform vec3 l1_dir;		// If direction
uniform vec3 l1_pos;		// Position in the world
uniform float  l1_n;			// Power N of specular

// Light 2
// ____________________________________
uniform int	 l2_type;		// 0 - Disable,	1 - Point,	2 - Direction
uniform vec3 l2_dir;		// If direction
uniform vec3 l2_pos;		// Position in the world
uniform float  l2_n;			// Power N of specular

// Light ambient
// ____________________________________
uniform int	 l3_type;		// 0 - Disable

vec3 transNorm(vec3 v, vec3 vn, mat4 m) {
	vec3 s = vec3( vec4(v,1.0) * m );
	vec3 t = vec3( vec4(v + vn,1.0) * m );
	return normalize(t-s);
}


vec3 GetColorLight() {
	

	mat4 r = mvp *  world_mvp;

	vec3 white	= vec3(1.0);
	vec3 ret	= vec3(0.0);	// start from black;

	vec3 L,R;
	vec3 V = vec3(0,0,1);

	vec3 N;
	if (btexn == 0) { // No texture:
		N = normalize(normal); 
	} else { // use normal map
		N = transNorm(pos,vec3(texture2D(uv_n, uv )) * 2.0 - vec3(1.0), r );
	}

	// Ambient:
	if (l3_type != 0) ret += white * material[0];
	
	// Light 1:
	// ________________________________________________________
	if (l1_type != 0) {
		if (l1_type == 1 ) { // Point
			L = normalize( vec3( vec4(l1_pos,1.0) * world_mvp) - vpos);
		} else {			// Direction
			L = -1 * transNorm( vec3(0), l1_dir, world_mvp);
		}

		// Diffuse:
		float ndotl = dot(N,L);
		if (ndotl > 0.0)
			ret += white * material[1] * ndotl;

		// Specular:
		vec3 R = normalize(2.0 * N * ndotl - L);

		float RdotV = dot(R, V);
		if (RdotV > 0.0)
			ret += white * material[2] * pow(RdotV, l1_n);
	}	

	// Light 2:
	// ________________________________________________________
	if (l2_type != 0) {
		if (l2_type == 1 ) { // Point
			L = normalize( vec3( vec4(l2_pos,1.0) * world_mvp) - vpos);
		} else {			// Direction
			L = -1 * transNorm( vec3(0), l2_dir, world_mvp);
		}

		// Diffuse:
		float ndotl = dot(N,L);
		if (ndotl > 0.0)
			ret += white * material[1] * ndotl;

		// Specular:
		vec3 R = normalize(2.0 * N * ndotl - L);

		float RdotV = dot(R, V);
		if (RdotV > 0.0)
			ret += white * material[2] * pow(RdotV, l2_n);
	}	

	// Return color
	if (ret.x > 1.0) ret.x = 1.0;
	if (ret.y > 1.0) ret.y = 1.0;
	if (ret.z > 1.0) ret.z = 1.0;
	return ret;
}

void main()
{
	//mat4 mvp = rotation*translation*projection;
	mat4 r = mvp * world_mvp;

	normal		=  transNorm(vec3(vPosition),  vNormal , r);

	pos			= vec3(vPosition) + t*vNormal;
	vpos		= vec3(vec4(pos,1.0) * r);
	gl_Position = vec4(vpos,1.0) * projection ;
	
	uv = vUV;

	// Calculating color for vertex:
	color =vec4( GetColorLight() , 1.0 ); //vColor;
}
