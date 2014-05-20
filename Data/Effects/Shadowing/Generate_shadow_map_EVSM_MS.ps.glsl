void main() 
{

#ifdef ALPHA_TEST

	float alpha = texture(g_color_map, pixel_in.tex_coord).a;

	if (alpha < 0.4f)
	{
		discard;
	}

#endif // ALPHA_TEST

}