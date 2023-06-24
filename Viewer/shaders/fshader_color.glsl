#version 330 core
#define MAX_LIGHTS 5

struct LightMaterial
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
};

struct Material
{
    sampler2D textureMap;
    sampler2D normalMap;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;


    float shininess;
};

// We set this field's properties from the C++ code
uniform LightMaterial lightsMaterial[MAX_LIGHTS];
uniform Material material;
uniform vec3 viewPos;
uniform bool textureActivated;
uniform bool toonShadingActivated;
uniform bool normalMappingActivated;

// Inputs from vertex shader (after interpolation was applied)
in vec3 fragPos;
in vec3 fragNormal;
in vec2 fragTexCoords;
in vec3 orig_fragPos;
in vec3 lightsPos[MAX_LIGHTS];
in vec3 tangentLightPos[MAX_LIGHTS];
in vec3 tangentViewPos;
in vec3 tangentFragPos;

// The final color of the fragment (pixel)
out vec4 frag_color;

void main()
{
    vec3 final_color = vec3(0.f, 0.f, 0.f);

    // for diffuse
    vec3 norm = normalize(fragNormal);

    // for specular
    vec3 viewDir = normalize(viewPos - fragPos);
    if (textureActivated && normalMappingActivated)
        viewDir = normalize(tangentViewPos - tangentFragPos);

    vec3 textureColor = vec3(texture(material.textureMap, fragTexCoords));
    vec3 normalColor = vec3(texture(material.normalMap, fragTexCoords));
    normalColor = normalize(normalColor * 2.f - 1.f);

    for (int i = 0; i < MAX_LIGHTS; i++)
	{
	    // ambient
        vec3 ambient = lightsMaterial[i].ambientStrength * lightsMaterial[i].ambient * material.ambient;

        if (textureActivated)
            ambient = lightsMaterial[i].ambientStrength * lightsMaterial[i].ambient * textureColor;
  	    
        // diffuse
        vec3 lightDir = normalize(lightsPos[i] - fragPos);

        float diff = max(dot(norm, lightDir), 0.0);

        if (textureActivated && normalMappingActivated)
        {
            lightDir = normalize(tangentLightPos[i] - tangentFragPos);
            diff = max(dot(lightDir, normalColor), 0.0);
        }

        vec3 diffuse = lightsMaterial[i].diffuseStrength * lightsMaterial[i].diffuse * (diff * material.diffuse);

        if (textureActivated)
            diffuse = lightsMaterial[i].diffuseStrength * lightsMaterial[i].diffuse * (diff * textureColor);

    
        // specular
        vec3 reflectDir = reflect(-lightDir, norm);
        if (textureActivated && normalMappingActivated)
            reflectDir = reflect(-lightDir, normalColor);

        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 specular = lightsMaterial[i].specularStrength * lightsMaterial[i].specular * spec * material.specular;

        if (textureActivated)
            specular = lightsMaterial[i].specularStrength * lightsMaterial[i].specular * spec * textureColor;

        vec3 to_light = lightsPos[i] - fragPos;
        if (textureActivated && normalMappingActivated)
            to_light = tangentLightPos[i] - tangentFragPos;

        float d = length(to_light);
        float attenuation = clamp( 10.0 / d, 0.0, 1.0);

        
        // Toon Shading
        if(toonShadingActivated)
        {
            float intensity;
	        intensity = dot(lightDir,fragNormal);

	        if (intensity > 0.95)
		    {
                ambient *= 1;
                diffuse *= 1;
                specular *= 1;
	        }
            else if (intensity > 0.5)
		    {
                ambient *= 0.8;
                diffuse *= 0.8;
                specular *= 0.8;
	        }
	        else if (intensity > 0.25)
		    {
                ambient *= 0.6;
                diffuse *= 0.6;
                specular *= 0.6;
	        }
	        else
		    {
                ambient *= 0.4;
                diffuse *= 0.4;
                specular *= 0.4;
	        }
        }

        vec3 result = attenuation * (ambient + diffuse + specular);

        final_color += result;
    }
    
    frag_color = vec4(final_color, 1.0);
}