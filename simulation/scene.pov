#version 3.7;

#include "colors.inc"
#include "shapes.inc"
#include "textures.inc"
#include "glass.inc"
#include "consts.inc"


global_settings {
	assumed_gamma 1
}


camera {
	location  < 0,  0,  0 >
	look_at   < -1, -1, 1 >
	translate < 5,  5,  -5 >
}

// light_source{ < 1000, 1000, -1000 > color White}

#declare Projector = prism{
	linear_spline
	0, 0.001,  // top and bottom of the prism in y
	5,        // number of 2-D points you will use to define the prism
	< 1, 0 >, < 0, -1 >, < -1, 0 >, < 0, 1 >, < 1, 0 > // outer rim
// 	texture{ pigment{ color Red } }

	rotate< 25.25, 0, 25.25 >
	translate < -1.1,  -1.1,  1.1 >
// 	translate < 3,  3,  3 >
}


// principal light
// light_source{
// 	< 0, 0, 0 >
// 	color < 3, 3, 3 >
// 	spotlight
// 	radius 45
// 	falloff 0
// 	tightness 0
// 	point_at < -1, -1, 1 >
// 
// 	projected_through { Projector }
// 
// 	translate < 5,  6,  -5 >
// 	rotate< 0, 0, 0 >
// }


// object{
// 	Projector
// // 	translate< 2, 2, -2 >
// }



#declare FinishStd = finish{
	specular 1
	roughness 1
}


object {
	plane {
		x, 0
	}
	texture {
		pigment { White }
	}
	texture {
		pigment {
			image_map {
				png "gitter.png" 
				interpolate 2
				once
			}
			rotate < 0, 45, 0 >
			scale 3
		}
		finish{ FinishStd }
	}
}


plane { y, 0
	pigment {
		color < 0.5, 0.5, 0.5 >
	}
	finish{FinishStd}
}

object {
	plane {
		z, 0
		texture {
			pigment { White }
		}
		texture {
			pigment {
				image_map {
					png "gitter.png" 
					interpolate 2
					once
				}
				rotate < 0, 45, 0 >
				scale 3
			}
			finish{ FinishStd }
		}
	}
}




// sphere {
// 	< 0, 0, 0 >, 4
// 	pigment {
// 		color < 0.5, 0.5, 0.5 >
// 	}
// }
