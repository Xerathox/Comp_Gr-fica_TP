#version 330 core
out vec4 Color;

in  vec4 color_vertice;
in  vec2 textura_vertice;

uniform sampler2D texture1;

void main(){
	Color = texture(texture1, textura_vertice)
}