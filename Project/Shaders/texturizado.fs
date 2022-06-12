#version 330 core

in vec2 our_uv;
out vec4 Color;
uniform sampler2D ourTexture;

void main()
{
    Color = texture(ourTexture,our_uv);
}