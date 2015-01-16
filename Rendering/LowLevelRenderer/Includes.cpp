/*
	Reflec-Tech
	-----------
	File		:	Includes.cpp
	Author		:	Jamie Taylor
	Last Edit	:	11/09/12
	Desc		:	Includes the low level renderer header files; the headers define pure virtual interfaces
					which will be implemented via Direct X, Open GL etc and stored in DLLs to allow for real time switching

					These "Includes.cpp" files are added in order to compile the static libraries.
*/
#include "RtVertex.h"
#include "RtGraphicsDevice.h"
#include "RtRenderingUtils.h"