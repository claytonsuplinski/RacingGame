/*
Name: Clayton Suplinski
ID: 906 580 2630
CS 559 Project 3

Texture shader - Used almost everywhere in 
the project for applying images to shapes. Also
changes to correspond with the time of day and 
environment.
*/
#version 400

in vec3 Position;
in vec3 Normal;
const vec4 LightPosition = vec4(0.0, 0.0, 0.0, 0.0);
const vec3 LightIntensity = vec3(0.85, 0.85, 0.85);
const vec3 Kd = vec3(0.6, 0.6, 0.6); // Diffuse reflectivity
const vec3 Ka = vec3(0.85, 0.85, 0.85); // Ambient reflectivity
const vec3 Ks = vec3(0.75, 0.75, 0.75); // Specular reflectivity
const float Shininess = 6.0f; // Specular shininess factor
layout (location = 0) out vec4 FragColor;

in vec2 tc;
uniform sampler2D picture;
uniform ivec2 size;
uniform float time;

vec4 color = texture(picture, tc);
vec3 setting = vec3(0.0, 0.0, 0.0);
float factor = 1.0f;

const float blurSizeH = 1.0 / 300.0;
const float blurSizeV = 1.0 / 200.0;


vec3 tempColor;














void RGBtoHSV( float r, float g, float b)
{
	float min, max, delta;
	float h, s, v; 

	min = min( r, g);
	min = min(min, b );
	max = max( r, g);
	max = max(max, b );
	v = max;				// v

	delta = max - min;

	if( max != 0 )
		s = delta / max;		// s
	else {
		// r = g = b = 0		// s = 0, v is undefined
		s = 0;
		h = -1;
		tempColor = vec3(h, s, v);
		return;
	}

	if( r == max )
		h = ( g - b ) / delta;		// between yellow & magenta
	else if( g == max )
		h = 2 + ( b - r ) / delta;	// between cyan & yellow
	else
		h = 4 + ( r - g ) / delta;	// between magenta & cyan

	h *= 60;				// degrees
	if( h < 0 )
		h += 360;

	tempColor = vec3(h, s, v);

}

void HSVtoRGB( float h, float s, float v )
{
	int i;
	float f, p, q, t;
	float r, g, b;

	if( s == 0 ) {
		// achromatic (grey)
		r = v;
		g = v;
		b = v;
		tempColor = vec3(r, g, b);
		return;
	}

	h /= 60;			// sector 0 to 5
	i = int(floor( h ));
	f = h - i;			// factorial part of h
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

	switch( i ) {
		case 0:
			r = v;
			g = t;
			b = p;
			break;
		case 1:
			r = q;
			g = v;
			b = p;
			break;
		case 2:
			r = p;
			g = v;
			b = t;
			break;
		case 3:
			r = p;
			g = q;
			b = v;
			break;
		case 4:
			r = t;
			g = p;
			b = v;
			break;
		default:		// case 5:
			r = v;
			g = p;
			b = q;
			break;
	}
	tempColor = vec3(r, g, b);
}























vec3 ads()
{
vec3 n = normalize( Normal );
vec3 s = normalize( vec3(LightPosition) - Position );
vec3 v = normalize(vec3(-Position));
vec3 r = reflect( -s, n );
//Underwater setting
if(size.y == 1){
	setting = vec3(0.0, 0.05, 0.35);
	factor = 0.45;
}
//Night
if(size.x == 1){
float dx = Position.x - 0.5;
float dy = Position.y - 0.5;
float dist = sqrt(dx * dx + dy * dy);
vec3 color2 =
mix( vec3(color.x, color.y, color.z), vec3(0.0,0.0,0.0),
smoothstep( 0.2, 4.0, dist )
);
return 
LightIntensity * factor *
( Ka * color2 +
Kd * color2 * max( dot(s, n), 0.0 ));
}
//Twilight
else if(size.x == 2){ 
	return setting + vec3(0.35, 0.15, 0.0) +
	LightIntensity * factor *
	( Ka * 0.35 * vec3(color) +
	Kd * 0.35 * vec3(color) * max( dot(s, n), 0.0 ));
}
//Normal
return setting +
LightIntensity * factor *
( Ka * vec3(color) +
Kd * vec3(color) * max( dot(s, n), 0.0 ));
}

void main()
{
//Add a little blurring (too costly)
	/*vec4 sum = vec4( 0.0 );
    for (int x = -2; x <= 2; x++)
        for (int y = -2; y <= 2; y++)
            sum += texture(
                picture,
                vec2( tc.x + x * blurSizeH, tc.y + y * blurSizeV )
            )/ 81.0;
			
			FragColor = 0.5 * sum + 0.5 * vec4(ads(), 1.0f);
			*/

	float dist1 = abs( Position.z );
	float fogFactor = (10.0f - dist1) /
	(10.0f - 0.5f);
	fogFactor = clamp( fogFactor, 0.0, 1.0 );
	//Fog
	/*if(size.y == 2){
		if(size.x == 1){FragColor = vec4(mix( vec3(0.05f, 0.05f, 0.05f), ads(), fogFactor ), 1.0);}
		else if(size.x == 2){FragColor = vec4(mix( vec3(0.25f, 0.1f, 0.05f), ads(), fogFactor ), 1.0);}
		else{FragColor = vec4(mix( vec3(0.35f, 0.35f, 0.35f), ads(), fogFactor ), 1.0);}
	}
	else{*/
		
	//}

	tempColor = ads();
	if(time != 0){
	 ///(sin(10*time) + 1.5);
	RGBtoHSV( tempColor.x, tempColor.y, tempColor.z);
	//float tempTime = sin(time) + 1 - tempColor.x/180;
	//tempTime = 180 * tempTime;
	tempColor.x += 100*time;
	tempColor.x = mod(tempColor.x, 360.0f);
	HSVtoRGB( tempColor.x, tempColor.y, tempColor.z);
	}



	FragColor = vec4(tempColor, 1.0);
}

