#version 330 core
out vec4 FragColor;

in vec3 pass_normal;
in vec3 pass_fragPos;

struct Light {
    vec3 position;
    vec3 color;
    float strength;
};

void main() {
    //to become uniforms
    Light lights[2];
    lights[0].position = vec3(0, 3, -3);
    lights[0].color = vec3(1, .3, .1);
    lights[0].strength = 50;
    lights[1].position = vec3(5, 5, 2);
    lights[1].color = vec3(1, 1, 1);
    lights[1].strength = 20;

    vec3 sunDir = normalize(vec3(-1, -10, -1));
    vec3 sunColor = vec3(64, 156, 255) / 255.0;
    float sunStrength = 1;
    vec3 albedo = vec3(0, .5, .2);

    //ambient
    vec3 ambient = vec3(.1, .1, .1)*3;

    //diffuse
    vec3 diffuse = vec3(0);

    //sun
    vec3 sun = sunColor * max(dot(pass_normal, -sunDir), 0) * sunStrength;

    //points lights
    for (int i = 0; i < 2; i++) {
        vec3 toLight = lights[i].position - pass_fragPos;
        float distance = length(toLight);
        float att = lights[i].strength / (distance);

        float diff = max(dot(pass_normal, normalize(toLight)), 0);
        diff *= att;

        diffuse += diff * lights[i].color;
    }

    vec3 color = (sun + ambient + diffuse)*albedo;

    color = color / (color + vec3(1));
    color = pow(color, vec3(2.2));

    FragColor = vec4(color, 1);
}