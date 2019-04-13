#version 460 core
out vec4 FragColor;

in vec3 pass_normal;
in vec3 pass_fragPos;

struct Light {
	vec3 position;
	vec3 color;
};

/*struct PBRMaterial {
 sampler2D albedoMap;
 sampler2D amrMap;
 sampler2D normalMap;
};*/

//uniform Light lights[4];
//uniform PBRMaterial material;
uniform vec3 camPos;

uniform vec3 color;

//uniform samplerCube irradianceMap;
//uniform samplerCube prefilterMap;
//uniform sampler2D   brdfLUT;

const float PI = 3.14159265359;
const int NUMLIGHTS = 2;

float DistributionGGX(vec3 N, vec3 H, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness);

void main() 
{
    //to become uniforms
    Light lights[NUMLIGHTS];
    lights[0].position = vec3(0, 3, -3);
    lights[0].color = vec3(1, .3, .1) * 25;
    lights[1].position = vec3(5, 5, 2);
    lights[1].color = vec3(1, 1, 1) * 50;
    
    
	//Read all texture information
	vec3 albedo;
	/*switch(subType) {
  case 0:
   albedo = vec3(.5, .2, 0);
   break;
  case 1:
   albedo = vec3(0, .5, .2);
   break;
  case 2:
   albedo = vec3(0, .2, .5);
   break;
  default:
   albedo = vec3(1, 1, 1);
   break;
 }*/
	albedo = color;
	//vec3 albedo = vec3(0, .5, .2);
	vec3 normal = pass_normal;
    
	//albedo = vec3(1);
    
	float ao = 1;
	float metallic = 0;
	float roughness = .5;
    
	//Calcualte tangent space normal
	vec3 N = normal;
	vec3 V = normalize(camPos - pass_fragPos);
	vec3 R = reflect(-V, N);
    
	vec3 F0 = vec3(0.04f);
	F0 = mix(F0, albedo, metallic);
    
	//Direct Lighting
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < NUMLIGHTS; i++) {
		vec3 L = normalize(lights[i].position - pass_fragPos);
		vec3 H = normalize(V + L);
        
		float distance = length(lights[i].position - pass_fragPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lights[i].color * attenuation;
        
		float NDF = DistributionGGX(N, H, roughness);
		float G = GeometrySmith(N, V, L, roughness);
		vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0, roughness);
        
		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0) * max(dot(N, L), 0);
		vec3 specular = numerator / max(denominator, 0.001);
        
		vec3 kS = F;
		vec3 kD = 1.0 - kS;
		kD *= 1.0 - metallic;
        
		float NdotL = max(dot(N, L), 0);
		Lo += (kD*albedo / PI + specular) * radiance * NdotL;
	}
    
	vec3 color = vec3(.1) * albedo * ao + Lo;
    
	FragColor = vec4(color, 1.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0, float roughness) {
    return F0 + (max(vec3(1 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / max(denom, 0.001);
}
float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    
    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
