void main() 
{
	pixel_out.color = texture(g_cubemap, pixel_in.view_ray_ws).rgb; 
}