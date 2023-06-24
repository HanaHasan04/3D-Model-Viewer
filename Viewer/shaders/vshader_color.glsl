#version 330 core
#define MAX_LIGHTS 5
#define PI 3.141592653589793

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;


// The model/view/projection matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightsTransform[MAX_LIGHTS];
uniform int vlightCount;
uniform int textureType;
uniform vec3 viewPos;

// These outputs will be available in the fragment shader as inputs
out vec3 orig_fragPos;
out vec3 fragPos;
out vec3 fragNormal;
out vec2 fragTexCoords;
out vec3 lightsPos[MAX_LIGHTS];
out vec3 tangentLightPos[MAX_LIGHTS];
out vec3 tangentViewPos;
out vec3 tangentFragPos;

void main()
{
	// Apply the model transformation to the 'position' and 'normal' properties of the vertex,
	// so the interpolated values of these properties will be available for usi n the fragment shader
	orig_fragPos = vec3(vec4(pos, 1.0f));
	fragPos = vec3(model * vec4(pos, 1.0f));
	mat3 normalMatrix = mat3(model);

	fragNormal = normalMatrix * normal;

	vec3 tangentVector = normalize(normalMatrix * tangent);
    vec3 normalVector = normalize(normalMatrix * normal);
    tangentVector = normalize(tangentVector - dot(tangentVector, normalVector) * normalVector);
    vec3 B = cross(normalVector, tangentVector);
    
    mat3 TBN = transpose(mat3(tangentVector, B, normalVector));    
    tangentViewPos = TBN * viewPos;
    tangentFragPos = TBN * fragPos;

	for (int i = 0; i < vlightCount; i++)
	{
		lightsPos[i] = vec3(lightsTransform[i] * vec4(1.2f, 1.0f, 2.0f, 1.f));
		tangentLightPos[i] = TBN * lightsPos[i];
	}

	if (textureType == 0)
		fragTexCoords = texCoords;
	else if (textureType == 1)
		fragTexCoords = vec2(pos.x, pos.z);
	else if (textureType == 2)
	{
		float theta = atan(pos.z, pos.x) + PI;	
		fragTexCoords = vec2(theta, pos.y);
	}
	else
	{
		fragTexCoords = vec2(0.f, 0.f);
		fragTexCoords.x = 0.5f + atan(normal.z, normal.x) / 2.f * PI;
		fragTexCoords.y = 0.5f - asin(normal.y) / PI;
	}


	// This is an internal OpenGL variable, we must set a value to this variable
	gl_Position = projection * view *  model * vec4(pos, 1.0f);
}