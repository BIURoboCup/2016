#version 150 //compatible with OpenGL version 3.2

in vec4 color;
in vec3 normal;
in vec3 vpos;
in vec3 pos;
in vec2 uv;

out vec4 outPolFragColor;

uniform vec4 cwireframe; // if v[4] not 0.0 so constant color (no light)

uniform mat4 mvp;
uniform mat4 world_mvp;
uniform mat4 projection;

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

// N,p in world coordination
mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv )
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );

    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;

    // construct a scale-invariant frame 
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
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
		//N = transNorm(pos,vec3(texture2D(uv_n, uv )) * 2.0 - vec3(1.0), r );

		// Normal in tangent space:
		N = vec3(texture2D(uv_n, uv )) * 2.0 - vec3(1.0);
		// Convert to Model space:
		mat3 TBN = cotangent_frame( normal, vpos, uv );
		
		// N In model world
		N = (TBN)*N;

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
	// BBOx or WireFrame
	if (cwireframe.w != 0.0) {
		outPolFragColor = cwireframe;
		return;
	}

	// Lights:
	 
	if (render != 2) {
		outPolFragColor =  color; // Return the middle color for gourad.
	}else{
		if (btex == 0 ) { // No texture:
			outPolFragColor = vec4(GetColorLight(),1.0);
		} else { // use color map
			outPolFragColor = vec4(GetColorLight(),1.0)  + texture2D(uv_tex, uv );
		}
	}

}
