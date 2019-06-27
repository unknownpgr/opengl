#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec4 color2;

void main()
{
    FragColor = mix(texture(texture1, TexCoord),texture(texture2, TexCoord),0.2)*color2;
}