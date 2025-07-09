#version 410

out vec4 color;

in VSOUT {
	vec4 color;
} fsin;  

void main(void) { color = fsin.color; }