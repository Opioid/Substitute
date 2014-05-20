void main() 
{
//	vec3 color = texture(g_color_map, pixel_in.tex_coord).rgb;




	pixel_out.color = FxaaPixelShader(pixel_in.tex_coord, g_color_map, g_inverse_source_size, 0.75f, 0.166f, 0.0833).rgb;

//	pixel_out.color = color.rrr; 
}