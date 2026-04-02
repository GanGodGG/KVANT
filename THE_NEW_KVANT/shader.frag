#version 330 core
out vec4 FragColor;

in vec3 Color;
in vec2 TexPos;
in vec3 Normal;
in vec3 GlobalPos;
uniform sampler2D Texture;
uniform vec3 CameraPos;
uniform vec3 ambientLight;
const int MAX_LIGHT_COUNT = 16;
struct PointLight{
    vec3 position;
    vec3 color;
};
uniform PointLight pl[MAX_LIGHT_COUNT];
struct DirectionalLight{
    vec3 dir;
    vec3 color;
};
uniform DirectionalLight dl;
struct SpotLight{
    vec3 position;
    vec3 spotDir;
    float angle;
    float outerAngle;
    vec3 color;
};
uniform SpotLight sl[MAX_LIGHT_COUNT];
vec3 CalcDiffLight(vec3 color, vec3 dir, vec3 normal_){
    return max(0.0f, dot(normalize(normal_), normalize(dir))) * color;
}
vec3 CalcSpec(vec3 color, vec3 dir, vec3 normal_, vec3 view){
    vec3 _reflect = normalize(reflect(-dir, normal_));
    return pow(max(0.0f, dot(normalize(view), _reflect)), 32.0f) * color;
}
void main()
{
    vec3 acolor = texture(Texture, TexPos).xyz * Color;
    vec3 finalResult = ambientLight * acolor;
    vec3 norm = normalize(Normal);
    vec3 viewDir = CameraPos - GlobalPos;

    //dir
    vec3 Diffuse = CalcDiffLight(dl.color, dl.dir, norm);
    vec3 Specs = CalcSpec(dl.color, dl.dir, norm, viewDir);
    finalResult += (Specs+Diffuse) * Color;

    //all points:
    for(int i = 0; i < MAX_LIGHT_COUNT; ++i){
        vec3 DIR = pl[i].position - GlobalPos;
        float dis = length(pl[i].position-GlobalPos);
        float a = 1 / (1.5f + 0.5f*dis + 0.2f * dis * dis);
        vec3 _diffuse = CalcDiffLight(pl[i].color, DIR, norm);
        vec3 _specs = CalcSpec(pl[i].color, DIR, norm, viewDir);
        finalResult += a * (_diffuse+_specs) * Color;
    }

    //all spots:
    for(int i = 0; i < MAX_LIGHT_COUNT; ++i){
        vec3 DIR = sl[i].position - GlobalPos;
        float dis = length(sl[i].position-GlobalPos);
        float phi = max(0.0f, dot(normalize(sl[i].spotDir), -DIR));
        float ins = clamp((sl[i].outerAngle - phi) / (sl[i].outerAngle - sl[i].angle), 0.0f, 1.0f);
        float a = 1 / (1.5f + 0.5f*dis + 0.2f * dis * dis);

        vec3 _diffuse = CalcDiffLight(sl[i].color, DIR, norm);
        vec3 _specs = CalcSpec(sl[i].color, DIR, norm, viewDir);
        finalResult += a * ins * (_diffuse+_specs) * Color;
    }

    FragColor = vec4(finalResult, 1.0f);
}