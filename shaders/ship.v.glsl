#version 410 core

in vec3 vPos;
in vec3 vNormal;
in vec2 vUV;

out vec3 coords;
out vec2 uv;
out vec3 light;

uniform mat3 normalMatrix;
uniform mat4 mvpMatrix;
uniform mat4 modelMatrix;
uniform vec3 viewDir;

struct pointLight {
    vec3 position;
    vec3 color;
};
uniform pointLight pointLights[2];

struct directionalLight {
    vec3 direction;
    vec3 color;
};
uniform directionalLight directionalLights[3];

/// Calculate light component for spot lights
vec3 calculatePointLight(pointLight light){
    vec4 vPosWorldH = modelMatrix * vec4(vPos, 1.0);
    vec3 vPosWorld = vPosWorldH.xyz;

    vec3 lightVector = normalize(light.position - vPosWorld);   // l - light vector in world space
    vec3 vNormalWorld = normalMatrix * vNormal;                 // n - normal vector in world space

    /// Attenuation
    float dist = length(light.position - vPosWorld);
    vec3 lightColorA = light.color * (1.0 /(1 + dist * 0.1 + pow(dist*0.8, 2)));
    /// Diffuse component
    vec3 I_d = lightColorA * max(dot(lightVector, vNormalWorld), 0.0);
    /// Specular component
    vec3 reflectionVector = -lightVector + 2 * dot(vNormalWorld, lightVector) * vNormalWorld;    // r - reflection vector
    vec3 I_s = lightColorA * pow(dot(viewDir, reflectionVector), 0.1 * (256.0f ));

    return I_d + I_s;
}

/// Calculate light component for directional light
vec3 calculateDirectionalLight(directionalLight light){
    vec4 vPosWorldH = modelMatrix * vec4(vPos, 1.0);
    vec3 vPosWorld = vPosWorldH.xyz;

    vec3 lightVector = -light.direction;
    vec3 vNormalWorld = normalMatrix * vNormal;                 // n - normal vector in world space

    /// Diffuse component
    vec3 I_d = light.color * max(dot(lightVector, vNormalWorld), 0.0);
    /// Specular component
    vec3 reflectionVector = -lightVector + 2 * dot(vNormalWorld, lightVector) * vNormalWorld;    // r - reflection vector
    vec3 I_s = light.color * pow(dot(viewDir, reflectionVector), 0.1 * (64.0f));

    return I_d + I_s;
}

void main()
{
    uv = vUV;
    coords = vPos;

    vec3 normal = normalize(vNormal);

    light = calculatePointLight(pointLights[0]);
    light += calculatePointLight(pointLights[1]);
    light += calculateDirectionalLight(directionalLights[0]);

    gl_Position = mvpMatrix * vec4(vPos, 1.0);


}
