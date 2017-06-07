#version 330
uniform sampler2D texture_unit;

in vec2 texture_coord;
in vec4 fragment_color;

out vec4 color;

void main () {
	color = texture(texture_unit, texture_coord);
}
