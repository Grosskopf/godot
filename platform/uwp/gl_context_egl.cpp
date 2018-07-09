/*************************************************************************/
/*  gl_context_egl.cpp                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2018 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2018 Godot Engine contributors (cf. AUTHORS.md)    */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#include "gl_context_egl.h"

//#include <EGL/egl.h>
#include <EGL/eglext.h>
//#include <EGL/eglplatform.h>
#include <angle_windowsstore.h>

using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::UI::Input;
using namespace Windows::Devices::Input;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::Graphics::Holographic;
using namespace Windows::Perception::Spatial;
using namespace Windows::Perception;
using namespace Windows::System;
using namespace Windows::System::Threading::Core;
using namespace Microsoft::WRL;

using Platform::Exception;

void ContextEGL::release_current() {

	eglMakeCurrent(mEglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, mEglContext);
};

void ContextEGL::make_current() {

	eglMakeCurrent(mEglDisplay, mEglSurface, mEglSurface, mEglContext);
};

int ContextEGL::get_window_width() {

	return width;
};

int ContextEGL::get_window_height() {

	return height;
};

void ContextEGL::reset() {

	cleanup();
    
	window = CoreWindow::GetForCurrentThread();
#ifdef HOLOGRAPHIC
    initializeHolo();
#else
    initialize();
#endif
};

void ContextEGL::swap_buffers() {
    
#ifdef HOLOGRAPHIC
	CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);
    HolographicFrame ^ holographicFrame = mHolographicSpace->CreateNextFrame();
    holographicFrame->UpdateCurrentPrediction();
    HolographicFramePrediction ^ prediction = holographicFrame->CurrentPrediction;
    //m_deviceResources->EnsureCameraResources(holographicFrame, prediction);
    // Back buffers can change from frame to frame. Validate each buffer, and recreate
    // resource views and depth buffers as needed.
    //m_deviceResources->EnsureCameraResources(holographicFrame, prediction);
	SpatialCoordinateSystem ^ currentCoordinateSystem = mStationaryReferenceFrame->CoordinateSystem;//GetStationaryCoordinateSystemAtTimestamp(prediction->Timestamp);
    //IHolographicCameraRenderingParameters ^ spParameters;
    //UINT32 mHolographicCameraId=0;
    //HolographicCameraPose^ cameraPose;
    //holographicFrame->GetHolographicCameraPoseAt(mHolographicCameraId,cameraPose);
    //HolographicCameraPose ^ cameraPose;
    //cameraPose=prediction->CameraPoses->GetAt(mHolographicCameraId);
    //holographicFrame->GetHolographicRenderingParameters(mHolographicCameraId, &spParameters)
	IVectorView<HolographicCameraPose^>^ camPoses = prediction->CameraPoses;
	if (camPoses->Size == 0)
		return;
	//prediction->get_CameraPoses(camPoses);
	HolographicCameraPose^ cameraPose = camPoses->GetAt(camPoses->Size-1);
	//auto cameraPose = prediction->CameraPoses->First()->Current;
	//for (HolographicCameraPose^ cameraPose : camPoses)
	//for (auto cameraPose : camPoses)
	//{
    // The projection transform for each frame is provided by the HolographicCameraPose.
		HolographicStereoTransform cameraProjectionTransform = cameraPose->ProjectionTransform;
		leftProj.matrix[0][0]=cameraProjectionTransform.Left.m11;//FOR
		leftProj.matrix[0][1]=cameraProjectionTransform.Left.m12;//F*CKS
		leftProj.matrix[0][2]=cameraProjectionTransform.Left.m13;//SAKE
		leftProj.matrix[0][3]=cameraProjectionTransform.Left.m14;//Microsoft
		leftProj.matrix[1][0]=cameraProjectionTransform.Left.m21;//You
		leftProj.matrix[1][1]=cameraProjectionTransform.Left.m22;//Bloody
		leftProj.matrix[1][2]=cameraProjectionTransform.Left.m23;//*diots
		leftProj.matrix[1][3]=cameraProjectionTransform.Left.m24;//Microsoft
		leftProj.matrix[2][0]=cameraProjectionTransform.Left.m31;//What
		leftProj.matrix[2][1]=cameraProjectionTransform.Left.m32;//Where
		leftProj.matrix[2][2]=cameraProjectionTransform.Left.m33;//You
		leftProj.matrix[2][3]=cameraProjectionTransform.Left.m34;//Thinking
		leftProj.matrix[3][0]=cameraProjectionTransform.Left.m41;//when
		leftProj.matrix[3][1]=cameraProjectionTransform.Left.m42;//you
		leftProj.matrix[3][2]=cameraProjectionTransform.Left.m43;//coded
		leftProj.matrix[3][3]=cameraProjectionTransform.Left.m44;//this?
		rightProj.matrix[0][0]=cameraProjectionTransform.Right.m11;//Like
		rightProj.matrix[0][1]=cameraProjectionTransform.Right.m12;//seriously
		rightProj.matrix[0][2]=cameraProjectionTransform.Right.m13;//did
		rightProj.matrix[0][3]=cameraProjectionTransform.Right.m14;//Microsoft
		rightProj.matrix[1][0]=cameraProjectionTransform.Right.m21;//hire
		rightProj.matrix[1][1]=cameraProjectionTransform.Right.m22;//only
		rightProj.matrix[1][2]=cameraProjectionTransform.Right.m23;//chimps
		rightProj.matrix[1][3]=cameraProjectionTransform.Right.m24;//that
		rightProj.matrix[2][0]=cameraProjectionTransform.Right.m31;//get
		rightProj.matrix[2][1]=cameraProjectionTransform.Right.m32;//payed
		rightProj.matrix[2][2]=cameraProjectionTransform.Right.m33;//by
		rightProj.matrix[2][3]=cameraProjectionTransform.Right.m34;//the
		rightProj.matrix[3][0]=cameraProjectionTransform.Right.m41;//line
		rightProj.matrix[3][1]=cameraProjectionTransform.Right.m42;//of
		rightProj.matrix[3][2]=cameraProjectionTransform.Right.m43;//f*cking
		rightProj.matrix[3][3]=cameraProjectionTransform.Right.m44;//Code?
		char buff[400];
		sprintf(buff, "projtransform: %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f, %.5f \n", leftProj.matrix[0][0], leftProj.matrix[0][1], leftProj.matrix[0][2], leftProj.matrix[0][3], leftProj.matrix[1][0], leftProj.matrix[1][1], leftProj.matrix[1][2], leftProj.matrix[1][3], leftProj.matrix[2][0], leftProj.matrix[2][1], leftProj.matrix[2][2], leftProj.matrix[2][3], leftProj.matrix[3][0], leftProj.matrix[3][1], leftProj.matrix[3][2], leftProj.matrix[3][3]);
		//OutputDebugStringA(buff);
            
        //CameraResources::UpdateViewProjectionBuffer
		Platform::IBox<HolographicStereoTransform>^ viewTransformContainer=cameraPose->TryGetViewTransform(currentCoordinateSystem);
        // If TryGetViewTransform returns a null pointer, that means the pose and coordinate
        // system cannot be understood relative to one another; content cannot be rendered
        // in this coordinate system for the duration of the current frame.
        // This usually means that positional tracking is not active for the current frame, in
        // which case it is possible to use a SpatialLocatorAttachedFrameOfReference to render
        // content that is not world-locked instead.
		bool viewTransformAcquired = viewTransformContainer != nullptr;
		if (viewTransformAcquired)
		{
			HolographicStereoTransform viewTransform = viewTransformContainer->Value;
			leftView.set(viewTransform.Left.m11,
			    viewTransform.Left.m12,
			    viewTransform.Left.m13,
			    viewTransform.Left.m21,
			    viewTransform.Left.m22,
			    viewTransform.Left.m23,
			    viewTransform.Left.m31,
			    viewTransform.Left.m32,
			    viewTransform.Left.m33,
			    viewTransform.Left.m41,
			    viewTransform.Left.m42,
			    viewTransform.Left.m43+1.3f
			);
			rightView.set(viewTransform.Right.m11,
			    viewTransform.Right.m12,
			    viewTransform.Right.m13,
			    viewTransform.Right.m21,
			    viewTransform.Right.m22,
			    viewTransform.Right.m23,
			    viewTransform.Right.m31,
			    viewTransform.Right.m32,
			    viewTransform.Right.m33,
			    viewTransform.Right.m41,
			    viewTransform.Right.m42,
			    viewTransform.Right.m43 + 1.3f
			);
			char buff2[400];
			sprintf(buff2, "origin: %.5f, %.5f, %.5f, %.5f, %.5f, %.5f\n", leftView.origin.x, leftView.origin.y, leftView.origin.z, rightView.origin.x, rightView.origin.y, rightView.origin.z);
			//OutputDebugStringA(buff2);

        }
    //}
#endif
	if (eglSwapBuffers(mEglDisplay, mEglSurface) != EGL_TRUE) {
		cleanup();
        OutputDebugStringA("Hallo swap_buffers in if\n");

		window = CoreWindow::GetForCurrentThread();
        
#ifdef HOLOGRAPHIC
        initializeHolo();
        
        OutputDebugStringA("Schüss swap_buffers\n");
#else
        initialize();
#endif
		// tell rasterizer to reload textures and stuff?
    }
};

Error ContextEGL::initialize() {
    OutputDebugStringA("initializing\n");

	EGLint configAttribList[] = {
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 8,
		EGL_STENCIL_SIZE, 8,
		EGL_SAMPLE_BUFFERS, 0,
		EGL_NONE
	};

	EGLint surfaceAttribList[] = {
		EGL_NONE, EGL_NONE
	};

	EGLint numConfigs = 0;
	EGLint majorVersion = 1;
	EGLint minorVersion = 0;
	EGLDisplay display = EGL_NO_DISPLAY;
	EGLContext context = EGL_NO_CONTEXT;
	EGLSurface surface = EGL_NO_SURFACE;
	EGLConfig config = nullptr;
	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE, EGL_NONE };

	try {

		const EGLint displayAttributes[] = {
			/*EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
			EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
			EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
			EGL_NONE,*/
			// These are the default display attributes, used to request ANGLE's D3D11 renderer.
			// eglInitialize will only succeed with these attributes if the hardware supports D3D11 Feature Level 10_0+.
			EGL_PLATFORM_ANGLE_TYPE_ANGLE,
            EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,

			// EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER is an optimization that can have large performance benefits on mobile devices.
			// Its syntax is subject to change, though. Please update your Visual Studio templates if you experience compilation issues with it.
			//EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,

			// EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE is an option that enables ANGLE to automatically call
			// the IDXGIDevice3::Trim method on behalf of the application when it gets suspended.
			// Calling IDXGIDevice3::Trim when an application is suspended is a Windows Store application certification requirement.
			EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE,
            EGL_TRUE,
			EGL_NONE,
		};

		PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));

		if (!eglGetPlatformDisplayEXT) {
			throw Exception::CreateException(E_FAIL, L"Failed to get function eglGetPlatformDisplayEXT");
		}

		display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, displayAttributes);

		if (display == EGL_NO_DISPLAY) {
			throw Exception::CreateException(E_FAIL, L"Failed to get default EGL display");
		}

		if (eglInitialize(display, &majorVersion, &minorVersion) == EGL_FALSE) {
			throw Exception::CreateException(E_FAIL, L"Failed to initialize EGL");
		}

		if (eglGetConfigs(display, NULL, 0, &numConfigs) == EGL_FALSE) {
			throw Exception::CreateException(E_FAIL, L"Failed to get EGLConfig count");
		}

		if (eglChooseConfig(display, configAttribList, &config, 1, &numConfigs) == EGL_FALSE) {
			throw Exception::CreateException(E_FAIL, L"Failed to choose first EGLConfig count");
		}

		surface = eglCreateWindowSurface(display, config, reinterpret_cast<IInspectable *>(window), surfaceAttribList);
		if (surface == EGL_NO_SURFACE) {
			throw Exception::CreateException(E_FAIL, L"Failed to create EGL fullscreen surface");
		}

		context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
		if (context == EGL_NO_CONTEXT) {
			throw Exception::CreateException(E_FAIL, L"Failed to create EGL context");
		}

		if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
			throw Exception::CreateException(E_FAIL, L"Failed to make fullscreen EGLSurface current");
		}
	} catch (...) {
        OutputDebugStringA("initialize failed\n");
		return FAILED;
	};

	mEglDisplay = display;
	mEglSurface = surface;
	mEglContext = context;

	eglQuerySurface(display, surface, EGL_WIDTH, &width);
	eglQuerySurface(display, surface, EGL_HEIGHT, &height);
    
    OutputDebugStringA("initialize is ok\n");
	return OK;
};
void ContextEGL::makewindowHolo(){
    // Create a holographic space for the core window for the current view.
    
    mHolographicSpace = HolographicSpace::CreateForCoreWindow(window);
    //auto holographicNativeWindow = renderTarget->getHolographicSwapChain11()->getHolographicNativeWindow();
        
    // Get the default SpatialLocator.
    Windows::Perception::Spatial::SpatialLocator^ locator = Windows::Perception::Spatial::SpatialLocator::GetDefault();

    // Create a stationary frame of reference.
    mStationaryReferenceFrame = locator->CreateStationaryFrameOfReferenceAtCurrentLocation();
    //mReferenceFrame = locator->CreateAttachedFrameOfReferenceAtCurrentHeading();

    // The HolographicSpace has been created, so EGL can be initialized in holographic mode.
};
Error ContextEGL::initializeHolo(){
        OutputDebugStringA("initializing\n");
        if (mHolographicSpace != nullptr)
        {
            OutputDebugStringA("initializing holographic\n");
            return initializeHolo(mHolographicSpace);
        }
        else
        {
            OutputDebugStringA("initializing window\n");
            return initializeHolo(window);
        }
};

Error ContextEGL::initializeHolo(Platform::Object^ windowBasis) {


    OutputDebugStringA("initializing holo2\n");

    // The HolographicSpace has been created, so EGL can be initialized in holographic mode.
	const EGLint configAttributes[] = 
    {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
		EGL_SAMPLE_BUFFERS, 0,//test
        EGL_NONE
    };
    const EGLint contextAttributes[] = 
    { 
        //EGL_CONTEXT_CLIENT_VERSION, 2, 
        //EGL_NONE
        //test:
        EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE
    };
	EGLint surfaceAttribList[] = {
		EGL_NONE, EGL_NONE
	};
        const EGLint surfaceAttributes[] =
    {
        // EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER is part of the same optimization as EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER (see above).
        // If you have compilation issues with it then please update your Visual Studio templates.
        EGL_ANGLE_SURFACE_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_NONE
    };
    
	EGLint numConfigs = 0;
	EGLint majorVersion = 1;
	EGLint minorVersion = 0;
	EGLDisplay display = EGL_NO_DISPLAY;
	EGLContext context = EGL_NO_CONTEXT;
	EGLSurface surface = EGL_NO_SURFACE;
	EGLConfig config = nullptr;
    //const EGLint defaultDisplayAttributes[] =
    const EGLint defaultDisplayAttributes[] = 
    {
        // These are the default display attributes, used to request ANGLE's D3D11 renderer.
        // eglInitialize will only succeed with these attributes if the hardware supports D3D11 Feature Level 10_0+.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
		EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,

        // EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER is an optimization that can have large performance benefits on mobile devices.
        // Its syntax is subject to change, though. Please update your Visual Studio templates if you experience compilation issues with it.
        //EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE, test
        
        // EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE is an option that enables ANGLE to automatically call 
        // the IDXGIDevice3::Trim method on behalf of the application when it gets suspended. 
        // Calling IDXGIDevice3::Trim when an application is suspended is a Windows Store application certification requirement.
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
        EGL_NONE,
    };
    const EGLint fl9_3DisplayAttributes[] =
    {
        // These can be used to request ANGLE's D3D11 renderer, with D3D11 Feature Level 9_3.
        // These attributes are used if the call to eglInitialize fails with the default display attributes.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_PLATFORM_ANGLE_MAX_VERSION_MAJOR_ANGLE, 9,
        EGL_PLATFORM_ANGLE_MAX_VERSION_MINOR_ANGLE, 3,
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
        EGL_NONE,
    };
    const EGLint warpDisplayAttributes[] =
    {
        // These attributes can be used to request D3D11 WARP.
        // They are used if eglInitialize fails with both the default display attributes and the 9_3 display attributes.
        EGL_PLATFORM_ANGLE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_TYPE_D3D11_ANGLE,
        EGL_PLATFORM_ANGLE_DEVICE_TYPE_ANGLE, EGL_PLATFORM_ANGLE_DEVICE_TYPE_WARP_ANGLE,
        EGL_ANGLE_DISPLAY_ALLOW_RENDER_TO_BACK_BUFFER, EGL_TRUE,
        EGL_PLATFORM_ANGLE_ENABLE_AUTOMATIC_TRIM_ANGLE, EGL_TRUE,
        EGL_NONE,
    };//test
        //EGLConfig config = NULL;

    // eglGetPlatformDisplayEXT is an alternative to eglGetDisplay. It allows us to pass in display attributes, used to configure D3D11.
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = reinterpret_cast<PFNEGLGETPLATFORMDISPLAYEXTPROC>(eglGetProcAddress("eglGetPlatformDisplayEXT"));
    if (!eglGetPlatformDisplayEXT)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to get function eglGetPlatformDisplayEXT");
    }

    //
    // To initialize the display, we make three sets of calls to eglGetPlatformDisplayEXT and eglInitialize, with varying 
    // parameters passed to eglGetPlatformDisplayEXT:
    // 1) The first calls uses "defaultDisplayAttributes" as a parameter. This corresponds to D3D11 Feature Level 10_0+.
    // 2) If eglInitialize fails for step 1 (e.g. because 10_0+ isn't supported by the default GPU), then we try again 
    //    using "fl9_3DisplayAttributes". This corresponds to D3D11 Feature Level 9_3.
    // 3) If eglInitialize fails for step 2 (e.g. because 9_3+ isn't supported by the default GPU), then we try again 
    //    using "warpDisplayAttributes".  This corresponds to D3D11 Feature Level 11_0 on WARP, a D3D11 software rasterizer.
    //
    
    // This tries to initialize EGL to D3D11 Feature Level 10_0+. See above comment for details.
    display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, defaultDisplayAttributes);
    if (display == EGL_NO_DISPLAY)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to get EGL display");
    }

    //if (eglInitialize(display, &majorVersion, &minorVersion) == EGL_FALSE)
    if (eglInitialize(display, NULL, NULL) == EGL_FALSE)
    {
        //throw Exception::CreateException(E_FAIL, L"Failed to initialize EGL");
        // This tries to initialize EGL to D3D11 Feature Level 9_3, if 10_0+ is unavailable (e.g. on some mobile devices).
        //throw Exception::CreateException(E_FAIL, L"Failed to get function eglGetPlatformDisplayEXT");
        display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, fl9_3DisplayAttributes);
        if (display == EGL_NO_DISPLAY)
        {
            throw Exception::CreateException(E_FAIL, L"Failed to get EGL display");
        }

        if (eglInitialize(display, NULL, NULL) == EGL_FALSE)
        {
            // This initializes EGL to D3D11 Feature Level 11_0 on WARP, if 9_3+ is unavailable on the default GPU.
            display = eglGetPlatformDisplayEXT(EGL_PLATFORM_ANGLE_ANGLE, EGL_DEFAULT_DISPLAY, warpDisplayAttributes);
            if (display == EGL_NO_DISPLAY)
            {
                throw Exception::CreateException(E_FAIL, L"Failed to get EGL display");
            }

            if (eglInitialize(display, NULL, NULL) == EGL_FALSE)
            {
                // If all of the calls to eglInitialize returned EGL_FALSE then an error has occurred.
                throw Exception::CreateException(E_FAIL, L"Failed to initialize EGL");
            }
        }
    }
    //test
    //if (eglGetConfigs(display, NULL, 0, &numConfigs) == EGL_FALSE) {
	//		throw Exception::CreateException(E_FAIL, L"Failed to get EGLConfig count");
    //}
    //EGLint numConfigs = 0;
    //if (eglChooseConfig(display, configAttributes, &config, 1, &numConfigs) == EGL_FALSE)
    if ((eglChooseConfig(display, configAttributes, &config, 1, &numConfigs) == EGL_FALSE) || (numConfigs == 0))
    {
        throw Exception::CreateException(E_FAIL, L"Failed to choose first EGLConfig");
    }

    // Create a PropertySet and initialize with the EGLNativeWindowType.
    PropertySet^ surfaceCreationProperties = ref new PropertySet();
    surfaceCreationProperties->Insert(ref new Platform::String(EGLNativeWindowTypeProperty), windowBasis);
    if (mStationaryReferenceFrame != nullptr)
    {
        surfaceCreationProperties->Insert(ref new Platform::String(EGLBaseCoordinateSystemProperty), mStationaryReferenceFrame);
        //surfaceCreationProperties->Insert(ref new Platform::String(EGLAutomaticStereoRenderingProperty), PropertyValue::CreateBoolean(true));
        //surfaceCreationProperties->Insert(ref new Platform::String(EGLAutomaticDepthBasedImageStabilizationProperty), PropertyValue::CreateBoolean(true));
    }
	/*if (window != nullptr)
	{
		window->Insert(ref new Platform::String(EGLBaseCoordinateSystemProperty), mStationaryReferenceFrame);
		window->Insert(ref new String(EGLAutomaticStereoRenderingProperty), PropertyValue::CreateBoolean(true));
		window->Insert(ref new String(EGLAutomaticDepthBasedImageStabilizationProperty), PropertyValue::CreateBoolean(true));
	}*/

    // You can configure the surface to render at a lower resolution and be scaled up to
    // the full window size. This scaling is often free on mobile hardware.
    //
    // One way to configure the SwapChainPanel is to specify precisely which resolution it should render at.
    // Size customRenderSurfaceSize = Size(800, 600);
    // surfaceCreationProperties->Insert(ref new String(EGLRenderSurfaceSizeProperty), PropertyValue::CreateSize(customRenderSurfaceSize));
    //
    // Another way is to tell the SwapChainPanel to render at a certain scale factor compared to its size.
    // e.g. if the SwapChainPanel is 1920x1280 then setting a factor of 0.5f will make the app render at 960x640
    // float customResolutionScale = 0.5f;
    // surfaceCreationProperties->Insert(ref new String(EGLRenderResolutionScaleProperty), PropertyValue::CreateSingle(customResolutionScale));

    //surface = eglCreateWindowSurface(display, config, reinterpret_cast<IInspectable*>(surfaceCreationProperties), surfaceAttributes);test
    surface = eglCreateWindowSurface(display, config, reinterpret_cast<IInspectable*>(surfaceCreationProperties), surfaceAttributes);
    if (surface == EGL_NO_SURFACE)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to create EGL fullscreen surface");
    }

    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttributes);
    if (context == EGL_NO_CONTEXT)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to create EGL context");
    }

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
    {
        throw Exception::CreateException(E_FAIL, L"Failed to make fullscreen EGLSurface current");
    }

	mEglDisplay = display;
	mEglSurface = surface;
	mEglContext = context;
	eglQuerySurface(display, surface, EGL_WIDTH, &width);
	eglQuerySurface(display, surface, EGL_HEIGHT, &height);
    OutputDebugStringA("initializing holo_end\n");

	return OK;
};
void ContextEGL::cleanup() {

	if (mEglDisplay != EGL_NO_DISPLAY && mEglSurface != EGL_NO_SURFACE) {
		eglDestroySurface(mEglDisplay, mEglSurface);
		mEglSurface = EGL_NO_SURFACE;
	}

	if (mEglDisplay != EGL_NO_DISPLAY && mEglContext != EGL_NO_CONTEXT) {
		eglDestroyContext(mEglDisplay, mEglContext);
		mEglContext = EGL_NO_CONTEXT;
	}

	if (mEglDisplay != EGL_NO_DISPLAY) {
		eglTerminate(mEglDisplay);
		mEglDisplay = EGL_NO_DISPLAY;
	}
};
CameraMatrix ContextEGL::get_projection_matrix_left_eye(){
    return leftProj;
}
CameraMatrix ContextEGL::get_projection_matrix_right_eye(){
    return rightProj;
}
Transform ContextEGL::get_view_matrix_left_eye(){
    return leftView;
}
Transform ContextEGL::get_view_matrix_right_eye(){
    return rightView;
}
ContextEGL::ContextEGL(CoreWindow ^ p_window)
	: mEglDisplay(EGL_NO_DISPLAY),
	  mEglContext(EGL_NO_CONTEXT),
	  mEglSurface(EGL_NO_SURFACE) {

	window = p_window;
};

ContextEGL::~ContextEGL() {

	cleanup();
};
