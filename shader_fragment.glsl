#version 460 core

layout(location = 0) out vec4 color;

uniform sampler2D u_Texture;

in vec2 v_TexCoord;
in float intensity;
void main(){
	vec4 texColor = texture(u_Texture, v_TexCoord);
	//color = vec4(v_TexCoord.xy, 1,0);
	color = texColor * intensity;
}