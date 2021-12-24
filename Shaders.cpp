#define TO_STRING(x) #x

const char* VertexShaderSource = TO_STRING(
    \#version 330 core\n

    uniform struct Transform {
    mat4 viewProjection;
} transform;

uniform vec2 shift;
uniform vec3 rotation;
uniform vec3 scaling;
uniform vec3 translation;
uniform vec3 lightPos;
uniform vec3 eyePos;
uniform bool lightFlag;

in vec3 vertCoord;
in vec3 texureCoord;
in vec3 normalCoord;

out vec2 tCoord;
out vec3 vnormal;
out vec3 lightDir;
out vec3 viewDir;

void main() {
    float x_angle = rotation.x;
    float y_angle = rotation.y;
    float z_angle = rotation.z;

    mat3 rotateMatrix = mat3(
        1, 0, 0,
        0, cos(x_angle), -sin(x_angle),
        0, sin(x_angle), cos(x_angle)
    ) * mat3(
        cos(y_angle), 0, sin(y_angle),
        0, 1, 0,
        -sin(y_angle), 0, cos(y_angle)
    ) * mat3(
        cos(z_angle), -sin(z_angle), 0,
        sin(z_angle), cos(z_angle), 0,
        0, 0, 1
    );

    vec3 position = vertCoord * rotateMatrix;
    vnormal = mat3(transpose(inverse(rotateMatrix))) * normalCoord;
    lightDir = lightPos - position;
    viewDir = eyePos - position;
    tCoord = vec2(texureCoord.x, texureCoord.y);

    mat4 scaleMatr = mat4(
        scaling.x, 0, 0, 0,
        0, scaling.y, 0, 0,
        0, 0, scaling.z, 0,
        0, 0, 0, 1
    );

    mat4 translateMatr = mat4(
        1, 0, 0, translation.x,
        0, 1, 0, translation.y,
        0, 0, 1, translation.z,
        0, 0, 0, 1
    );

    mat4 shiftMatr = mat4(
        1, 0, 0, shift.x,
        0, 1, 0, shift.y,
        0, 0, 1, 1,
        0, 0, 0, 1
    );

    vec4 position2 = vec4(position, 1.0) * shiftMatr * scaleMatr * translateMatr;
    // TODO: remove lightPos and eyePos;
    gl_Position = vec4(normalCoord, 1.0f);
    if (lightFlag) { gl_Position = vec4(normalCoord, 1.0f); }
    gl_Position = transform.viewProjection * position2;
}
);

const char* FragShaderSource = TO_STRING(
    \#version 330 core\n

    uniform sampler2D textureData;
uniform bool lightFlag;
in vec2 tCoord;
in vec3 vnormal;
in vec3 lightDir;
in vec3 viewDir;
out vec4 color;

const vec4 diffColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
const vec4 specColor = vec4(0.7f, 0.7f, 0.7f, 1.0f);
//const vec4  diffColor = vec4(0.5, 0.0, 0.0, 1.0);
//const vec4  specColor = vec4(0.7, 0.7, 0.0, 1.0);
const float specPower = 30.0f;

void main() {
    vec3 n2 = normalize(vnormal);
    vec3 l2 = normalize(lightDir);
    vec3 v2 = normalize(viewDir);
    vec3 r = reflect(-v2, n2);
    vec4 diff = diffColor * max(dot(n2, l2), 0.0f);
    vec4 spec = specColor * pow(max(dot(l2, r), 0.0f), specPower);

    color = texture(textureData, tCoord) + (diff + spec);
    if (!lightFlag) { color = color * 0.4f; }
}
);
