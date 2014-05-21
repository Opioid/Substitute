void main()
{
	pixel_out.color = evaluate_ambient_cube(pixel_in.position, pixel_in.normal);
}