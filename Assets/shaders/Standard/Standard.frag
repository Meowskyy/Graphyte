#version 330 core

const int MAX_POINT_LIGHTS = 2;                                                     
const int MAX_SPOT_LIGHTS = 2;     

out vec4 FragColor;

in VS_OUT {
    vec3 Normal;
    vec2 TexCoords;
	vec4 LightSpacePos;
	vec3 WorldPos;
} fs_in;

struct BaseLight                                                                    
{                                                                                   
    vec3 Color;                                                                     
    float AmbientIntensity;                                                         
    float DiffuseIntensity;                                                         
};                                                                                  
                                                                                    
struct DirectionalLight                                                             
{                                                                                   
    BaseLight Base;                                                          
    vec3 Direction;                                                                 
};                                                                                  
                                                                                    
struct Attenuation                                                                  
{                                                                                   
    float Constant;                                                                 
    float Linear;                                                                   
    float Exp;                                                                      
};                                                                                  
                                                                                    
struct PointLight                                                                           
{                                                                                           
    BaseLight Base;                                                                  
    vec3 Position;                                                                          
    Attenuation Atten;                                                                      
};                                                                                          
                                                                                            
struct SpotLight                                                                            
{                                                                                           
    PointLight Base;                                                                 
    vec3 Direction;                                                                         
    float Cutoff;                                                                           
};   

uniform sampler2D mainTexture;
uniform vec3 objectColor;

uniform float specularIntensity;
uniform float specularPower;

uniform vec3 cameraPosition;

uniform sampler2D shadowMap;

uniform int numPointLights;                                                                
uniform int numSpotLights;                                                                 
uniform DirectionalLight directionalLight;                                                 
uniform PointLight pointLights[MAX_POINT_LIGHTS];                                          
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];                                                                                                          

float CalcShadowFactor(vec4 LightSpacePos)
{
    vec3 ProjCoords = fs_in.LightSpacePos.xyz / fs_in.LightSpacePos.w;
    vec2 UVCoords;
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    float z = 0.5 * ProjCoords.z + 0.5;
    float Depth = texture(shadowMap, UVCoords).x;
    if (Depth < (z + 0.00001))
        return 0.5;
    else
        return 1.0;
}

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal, float ShadowFactor)                                                  
{                                                                                           
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0f);
    float DiffuseFactor = dot(Normal, -LightDirection);                                     
                                                                                            
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  
                                                                                            
    if (DiffuseFactor > 0) {                                                                
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0f);    
                                                                                            
        vec3 VertexToEye = normalize(cameraPosition - fs_in.WorldPos);                             
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     
        float SpecularFactor = dot(VertexToEye, LightReflect);                                      
        if (SpecularFactor > 0) {                                                           
            SpecularFactor = pow(SpecularFactor, specularPower);                               
            SpecularColor = vec4(Light.Color, 1.0f) * specularIntensity * SpecularFactor;                         
        }                                                                                   
    }                                                                                       
                                                                                            
    return (AmbientColor + ShadowFactor * (DiffuseColor + SpecularColor));                  
}    

vec4 CalcDirectionalLight(vec3 Normal)
{
    return CalcLightInternal(directionalLight.Base, directionalLight.Direction, Normal, 1.0);
}

vec4 CalcPointLight(PointLight l, vec3 Normal, vec4 LightSpacePos)
{
    vec3 lightDirection = fs_in.WorldPos - l.Position;
    float Distance = length(lightDirection);
    lightDirection = normalize(lightDirection);
    float ShadowFactor = CalcShadowFactor(LightSpacePos);

    vec4 Color = CalcLightInternal(l.Base, lightDirection, Normal, ShadowFactor);
    float Attenuation = l.Atten.Constant +
        l.Atten.Linear * Distance +
        l.Atten.Exp * Distance * Distance;

    return Color / Attenuation;
}

vec4 CalcSpotLight(SpotLight l, vec3 Normal, vec4 LightSpacePos)
{
    vec3 LightToPixel = normalize(fs_in.WorldPos - l.Base.Position);
    float SpotFactor = dot(LightToPixel, l.Direction);

    if (SpotFactor > l.Cutoff) {
        vec4 Color = CalcPointLight(l.Base, Normal, LightSpacePos);
        return Color * (1.0 - (1.0 - SpotFactor) * 1.0/(1.0 - l.Cutoff));
    }
    else {
        return vec4(0,0,0,0);
    }
}

void main()
{           
	vec3 Normal = normalize(fs_in.Normal);
    vec4 TotalLight = CalcDirectionalLight(Normal);

    for (int i = 0 ; i < numPointLights ; i++) {
        TotalLight += CalcPointLight(pointLights[i], Normal, fs_in.LightSpacePos);
    }

    for (int i = 0 ; i < numSpotLights ; i++) {
        TotalLight += CalcSpotLight(spotLights[i], Normal, fs_in.LightSpacePos);
    }

    vec4 SampledColor = texture2D(mainTexture, fs_in.TexCoords.xy);
    FragColor = SampledColor * TotalLight;
}