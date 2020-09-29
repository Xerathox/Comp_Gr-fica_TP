#version 330 core
layout (location = 0) in vec3 pPosicion;
layout (location = 1) in vec3 pColor;
layout (location = 2) in vec3 pTextura;

out vec4 color_vertice;
out vec2 textura_vertice;

void main(){
	gl_Position = transformacion * vec4(pPosicion, 1.0);
	color_vertice = vec4(pColor, 1.0);
	textura_vertice = vec2(pTextura.x, pTextura.y);
}
