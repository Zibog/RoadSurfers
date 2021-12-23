#define TO_STRING(x) #x

const char* VertexShaderSource = TO_STRING(
    \#version 330 core\n

    uniform struct Transform {
    mat4 viewProjection;
} transform;

uniform vec2 shift;
uniform vec3 unifBusRotate;
uniform vec3 unifBusScale;
uniform vec3 unifBusShift;
uniform vec3 lightPos;
uniform vec3 eyePos;
uniform bool unif_lightOn;

in vec3 vertCoord;
in vec3 texureCoord;
in vec3 normalCoord;

out vec2 tCoord;
out vec3 vnormal;
out vec3 lightp;
out vec3 vnew;

void main() {
    float x_angle = unifBusRotate.x;
    float y_angle = unifBusRotate.y;
    float z_angle = unifBusRotate.z;

    mat3 aff = mat3(
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

    vec3 position = vertCoord * aff;

    vec3 newNormale = mat3(transpose(inverse(aff))) * normalCoord;
    vec3 lposition = lightPos;

    vec3 temp = lposition;
    vec3 temp1 = eyePos;

    vnormal = newNormale;
    lightp = temp - position;
    vnew = temp1 - position;

    position = vertCoord * mat3(
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

    tCoord = vec2(texureCoord.x, texureCoord.y);
    vec4 position2 = vec4(position + vec3(shift, 1.0), 1.0) * mat4(
        unifBusScale.x, 0, 0, 0,
        0, unifBusScale.y, 0, 0,
        0, 0, unifBusScale.z, 0,
        0, 0, 0, 1
    ) * mat4(
        1, 0, 0, unifBusShift.x,
        0, 1, 0, unifBusShift.y,
        0, 0, 1, unifBusShift.z,
        0, 0, 0, 1
    );;
    // TODO: remove lightPos and eyePos;
    gl_Position = vec4(normalCoord, 1.0f);
    if (unif_lightOn) { gl_Position = vec4(normalCoord, 1.0f); }
    gl_Position = transform.viewProjection * position2;
}
);

const char* FragShaderSource = TO_STRING(
    \#version 330 core\n

    uniform sampler2D textureData;
uniform bool unif_lightOn;
in vec2 tCoord;
in vec3 vnormal;
in vec3 lightp;
in vec3 vnew;
out vec4 color;

const vec4 diffColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);
const vec4 specColor = vec4(0.7f, 0.7f, 0.7f, 1.0f);
//const vec4  diffColor = vec4(0.5, 0.0, 0.0, 1.0);
//const vec4  specColor = vec4(0.7, 0.7, 0.0, 1.0);
const float specPower = 30.0f;

void main() {
    vec3 n2 = normalize(vnormal);
    vec3 l2 = normalize(lightp);
    vec3 v2 = normalize(vnew);
    vec3 r = reflect(-v2, n2);
    vec4 diff = diffColor * max(dot(n2, l2), 0.0f);
    vec4 spec = specColor * pow(max(dot(l2, r), 0.0f), specPower);

    color = texture(textureData, tCoord) + (diff + spec);
    if (!unif_lightOn) { color = color * 0.4f; }
}
);
