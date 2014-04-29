#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <va/va.h>
#include <va/va_x11.h>

#define NUMSURFACES 3

int main(int argc, char** argv)
{
	int major, minor;
	// Multiple variables 
	int frame_width = 854;
	int frame_height = 480;
	VAStatus va_status;

	// Creating the x11 display hook and the vadisplay 
	Display* x11_display = XOpenDisplay(":0.0");
	VADisplay display = vaGetDisplay(x11_display); // Can be done without x11 by using vaGetDisplayDrm(GraphicsCardFileDescriptor)

	// Creating encoder configuration
	VAConfigAttrib attrib[2]; 
	attrib[0].type = VAConfigAttribRTFormat; 
	attrib[0].value = VA_RT_FORMAT_YUV420; 
	attrib[1].type = VAConfigAttribRateControl; 
	attrib[1].value = VA_RC_VBR; 
	VAConfigID config; 
	va_status = vaCreateConfig(display, VAProfileH264Baseline, VAEntrypointEncSlice, attrib, 2, &config); 

	VASurfaceID surfaces[NUMSURFACES]; 
	va_status = vaCreateSurfaces(display, VA_RT_FORMAT_YUV420, frame_width, frame_height,surfaces, NUMSURFACES, NULL, 0); 

	VAContextID context; 
	va_status = vaCreateContext(display, config, frame_width, frame_height, 0, surfaces, NUMSURFACES, &context); 

	// Creating the buffer that will hold the encoded data
	VABufferID coded_buf;
	unsigned int codedbuf_size = (frame_width * frame_height * 400) / (16*16); 
	va_status = vaCreateBuffer(display, context, VAEncCodedBufferType, codedbuf_size, 1, NULL, &coded_buf); 

	// Tough part, tying an image buffer to a char buffer, linking the image buffer to a surface to fill the surface when the image buffer (and so char buffer)
	// gets filled up, then filling the char buffer with the data;


	// Cleanup
	vaDestroySurfaces(display, surfaces, NUMSURFACES);
	vaDestroyConfig(display, config);
	vaDestroyContext(display, context);
	vaTerminate(display);
	XCloseDisplay(x11_display);
	return 0;
}