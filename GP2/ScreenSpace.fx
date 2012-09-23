float4 VS(float4 Pos : POSITION):SV_POSITION //takes one vertex and inputs it and outputs one vertex
{
	return Pos;
}

float4 PS( float4 Pos : SV_POSITION ) : SV_Target //this specifies that the return value of the PS will be used in the Back Buffer. 
{
	return float4( 1.0f, 1.0f, 0.0f, 1.0f ); //Yellow, with Alpha
}

technique10 Render //this enables us to retrieve the technique and use it in our host application
{
	pass P0 //this is so we could retrieve th pass in the host application
	{
		SetVertexShader( CompileShader( vs_4_0, VS())); //CompileShader has 2 parameters, the shader model that it is to be comiled to, the function where the shader is implemented
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader(ps_4_0, PS())); //
	}
}