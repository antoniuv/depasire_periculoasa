#version 330 core

//	Variabile de intrare (dinspre Shader.vert);s
in vec2 tex_Coord;		//	Coordonata de texturare;

//	Variabile de iesire	(spre programul principal);
out vec4 out_Color;		//	Culoarea actualizata;

uniform sampler2D CarTexture;

void main(void)
  {
    out_Color = texture(CarTexture, tex_Coord);
  }