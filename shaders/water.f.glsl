#version 410 core

// uniform inputs

// varying inputs
in vec3 color;     // interpolated color for this fragment
in vec3 position;

uniform float time;

uniform vec3 cameraPos;                 // location of the camera
uniform vec3 lightDirection;            // the direction the incident ray of light is traveling

uniform vec3 lightColor;                // the color of the light
uniform vec3 materialColor;             // the material color for our vertex (& whole object)

uniform vec3 pointLight1Pos;
uniform vec3 pointLight1Color;
uniform vec3 pointLight2Pos;
uniform vec3 pointLight2Color;
uniform vec3 pointLight3Pos;
uniform vec3 pointLight3Color;
uniform vec3 pointLight4Pos;
uniform vec3 pointLight4Color;

uniform samplerCube skybox;

// outputs
out vec4 fragColorOut;                  // color to apply to this fragment


vec3 fade(vec3 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

vec2 fade(vec2 t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

float fade(float t) {return t*t*t*(t*(t*6.0-15.0)+10.0);}

vec2 node(vec2 coords) {
    float xc = floor(coords.x);
    float yc = floor(coords.y);
    float x = sin(yc + xc * 1.3 ) * 127382;
    float y = cos(xc - yc * 1.7 ) * 321675;

    return floor(2 * fract(vec2(7 * x + 5 * y, 3 * x + 11 * y)));
}

vec3 node(vec3 coords) {
    float xc = floor(coords.x);
    float yc = floor(coords.y);
    float zc = floor(coords.z);

    float x = sin(yc + xc * 1.3 + zc * 1.9 ) * 127382;
    float y = cos(xc - yc * 1.7 + zc * 1.4) * 321675;
    float z = cos(zc - yc * 1.54 + xc * 1.32) * 156213;
    return floor(2 * fract(vec3(7 * x + 5 * y + 13 * z, 3 * x + 11 * y + 17 * z, 5 * x + 13 * y + 7 * z)));
}

float pnoise(vec2 coords) {
    vec2 node00 = node(coords + vec2(0, 0));
    vec2 node10 = node(coords + vec2(1, 0));
    vec2 node01 = node(coords + vec2(0, 1));
    vec2 node11 = node(coords + vec2(1, 1));

    vec4 cellCoords = vec4(fract(coords), fract(coords) - 1);
    float val00 = dot(node00, cellCoords.xy);
    float val01 = dot(node01, cellCoords.xw);
    float val10 = dot(node10, cellCoords.zy);
    float val11 = dot(node11, cellCoords.zw);

    vec2 interpCoords = fade(fract(coords));

    return mix(mix(val00, val01, interpCoords.y), mix(val10, val11, interpCoords.y), interpCoords.x);
}

float pnoise(vec3 coords) {
    vec3 node000 = node(coords + vec3(0, 0, 0));
    vec3 node100 = node(coords + vec3(1, 0, 0));
    vec3 node010 = node(coords + vec3(0, 1, 0));
    vec3 node110 = node(coords + vec3(1, 1, 0));
    vec3 node001 = node(coords + vec3(0, 0, 1));
    vec3 node101 = node(coords + vec3(1, 0, 1));
    vec3 node011 = node(coords + vec3(0, 1, 1));
    vec3 node111 = node(coords + vec3(1, 1, 1));

    vec3 cellCoords = fract(coords);
    vec3 inverseCellCoords = cellCoords - 1;

    float val000 = dot(node000, cellCoords);
    float val010 = dot(node010, vec3(cellCoords.x, inverseCellCoords.y, cellCoords.z));
    float val100 = dot(node100, vec3(inverseCellCoords.x, cellCoords.yz) );
    float val110 = dot(node110, vec3(inverseCellCoords.xy, cellCoords.z) );
    float val001 = dot(node001, vec3(cellCoords.xy, inverseCellCoords.z) );
    float val011 = dot(node011, vec3(cellCoords.x, inverseCellCoords.yz) );
    float val101 = dot(node101, vec3(inverseCellCoords.x, cellCoords.y, inverseCellCoords.z) );
    float val111 = dot(node111, inverseCellCoords);

    vec3 interpCoords = fade(fract(coords));
    return mix( mix(mix(val000, val010, interpCoords.y), mix(val100, val110, interpCoords.y), interpCoords.x),
    mix(mix(val001, val011, interpCoords.y), mix(val101, val111, interpCoords.y), interpCoords.x),
    interpCoords.z);
}


void main() {
    vec3 timePos = vec3(position.xz * 0.5, pnoise(position.xz) + time);
    vec2 offset = vec2(pnoise(timePos), pnoise(timePos + 101)) * 2;

    float noise = pnoise(position.xz + offset);
    vec3 waterColor = mix(vec3(0.1, 0.1, 1), vec3(0.2, 0.45, 1), noise);


    vec3 objectNormal = vec3(0, 1, 0);

    vec3 inverseLightDirection = -normalize(lightDirection);

    // Computing Diffuse for directional light
    vec3 light = lightColor * (max(dot(objectNormal, inverseLightDirection), 0));


    vec3 vertexToPL1 = pointLight1Pos - position;
    vec3 diffuseFromPL1 = pointLight1Color * max(dot(objectNormal, normalize(vertexToPL1)), 0.125) / (1 + 0.03 * dot(vertexToPL1, vertexToPL1));

    vec3 vertexToPL2 = pointLight2Pos - position;
    vec3 diffuseFromPL2 = pointLight2Color * max(dot(objectNormal, normalize(vertexToPL2)), 0.125) / (1 + 0.03 * dot(vertexToPL2, vertexToPL2));

    vec3 vertexToPL3 = pointLight3Pos - position;
    vec3 diffuseFromPL3 = pointLight3Color * max(dot(objectNormal, normalize(vertexToPL3)), 0.125) / (1 + 0.03 * dot(vertexToPL3, vertexToPL3));


    vec3 vertexToPL4 = pointLight4Pos - position;
    vec3 diffuseFromPL4 = pointLight4Color * max(dot(objectNormal, normalize(vertexToPL4)), 0.05) / (1 + 0.03 * dot(vertexToPL4, vertexToPL4));

    light += diffuseFromPL1;
    light += diffuseFromPL2;
    light += diffuseFromPL3;
    light += diffuseFromPL4;

    light += vec3(0.10, 0.10, 0.10); //ambient light

    //reflective water
    vec3 cameraToPoint = position - cameraPos;
    float specularity = (1 - normalize(cameraToPoint).y);
    specularity *= specularity;

    cameraToPoint = reflect(cameraToPoint, normalize(vec3(offset.x, 4 / (1 -specularity), offset.y)) ); //reflection is a plane

    vec3 reflection = texture(skybox, cameraToPoint).rgb * vec3(0.1, 0.2, 0.25) * specularity;


    fragColorOut = vec4(light * waterColor + reflection, 1);
}