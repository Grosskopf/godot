/*************************************************************************/
/*  mobile_interface.cpp                                                 */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                    http://www.godotengine.org                         */
/*************************************************************************/
/* Copyright (c) 2007-2017 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2017 Godot Engine contributors (cf. AUTHORS.md)    */
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

#include "hololens_interface.h"
#include "core/os/input.h"
#include "platform/uwp/os_uwp.h"
#include "servers/visual/visual_server_global.h"
#include "drivers/gles3/shader_gles3.h"
//#include <EGL/egl.h>
//#include <EGL/eglext.h>
//#include <EGL/eglplatform.h>
//#include <GLES2/gl2.h>
//#include <GLES2/gl2ext.h>
//#include <angle_gl.h>
//#include <libANGLE/global_state.h>
//#include <GLES3/gl3platform.h>
//#include "angle_gl.h"
using namespace Windows::UI::Core;
StringName HololensARInterface::get_name() const {
	return "Native hololens";
};

int HololensARInterface::get_capabilities() const {
	return ARVRInterface::ARVR_STEREO;
};

void HololensARInterface::set_position_from_sensors() {
	_THREAD_SAFE_METHOD_

	// this is a helper function that attempts to adjust our transform using our 9dof sensors
	// 9dof is a misleading marketing term coming from 3 accelerometer axis + 3 gyro axis + 3 magnetometer axis = 9 axis
	// but in reality this only offers 3 dof (yaw, pitch, roll) orientation

	uint64_t ticks = OS::get_singleton()->get_ticks_usec();
	uint64_t ticks_elapsed = ticks - last_ticks;
	float delta_time = (double)ticks_elapsed / 1000000.0;

	// few things we need
	Input *input = Input::get_singleton();
	Vector3 down(0.0, -1.0, 0.0); // Down is Y negative
	Vector3 north(0.0, 0.0, 1.0); // North is Z positive

    //TODO
	// JIC
	orientation.orthonormalize();

	last_ticks = ticks;
};

void HololensARInterface::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_iod", "iod"), &HololensARInterface::set_iod);
	ClassDB::bind_method(D_METHOD("get_iod"), &HololensARInterface::get_iod);

	ClassDB::bind_method(D_METHOD("set_display_width", "display_width"), &HololensARInterface::set_display_width);
	ClassDB::bind_method(D_METHOD("get_display_width"), &HololensARInterface::get_display_width);

	ClassDB::bind_method(D_METHOD("set_display_to_lens", "display_to_lens"), &HololensARInterface::set_display_to_lens);
	ClassDB::bind_method(D_METHOD("get_display_to_lens"), &HololensARInterface::get_display_to_lens);

	ClassDB::bind_method(D_METHOD("set_oversample", "oversample"), &HololensARInterface::set_oversample);
	ClassDB::bind_method(D_METHOD("get_oversample"), &HololensARInterface::get_oversample);

	ClassDB::bind_method(D_METHOD("set_k1", "k"), &HololensARInterface::set_k1);
	ClassDB::bind_method(D_METHOD("get_k1"), &HololensARInterface::get_k1);

	ClassDB::bind_method(D_METHOD("set_k2", "k"), &HololensARInterface::set_k2);
	ClassDB::bind_method(D_METHOD("get_k2"), &HololensARInterface::get_k2);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "iod", PROPERTY_HINT_RANGE, "4.0,10.0,0.1"), "set_iod", "get_iod");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "display_width", PROPERTY_HINT_RANGE, "5.0,25.0,0.1"), "set_display_width", "get_display_width");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "display_to_lens", PROPERTY_HINT_RANGE, "5.0,25.0,0.1"), "set_display_to_lens", "get_display_to_lens");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "oversample", PROPERTY_HINT_RANGE, "1.0,2.0,0.1"), "set_oversample", "get_oversample");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "k1", PROPERTY_HINT_RANGE, "0.1,10.0,0.0001"), "set_k1", "get_k1");
	ADD_PROPERTY(PropertyInfo(Variant::REAL, "k2", PROPERTY_HINT_RANGE, "0.1,10.0,0.0001"), "set_k2", "get_k2");
}

void HololensARInterface::set_iod(const real_t p_iod) {
	intraocular_dist = p_iod;
};

real_t HololensARInterface::get_iod() const {
	return intraocular_dist;
};

void HololensARInterface::set_display_width(const real_t p_display_width) {
	display_width = p_display_width;
};

real_t HololensARInterface::get_display_width() const {
	return display_width;
};

void HololensARInterface::set_display_to_lens(const real_t p_display_to_lens) {
	display_to_lens = p_display_to_lens;
};

real_t HololensARInterface::get_display_to_lens() const {
	return display_to_lens;
};

void HololensARInterface::set_oversample(const real_t p_oversample) {
	oversample = p_oversample;
};

real_t HololensARInterface::get_oversample() const {
	return oversample;
};

void HololensARInterface::set_k1(const real_t p_k1) {
	k1 = p_k1;
};

real_t HololensARInterface::get_k1() const {
	return k1;
};

void HololensARInterface::set_k2(const real_t p_k2) {
	k2 = p_k2;
};

real_t HololensARInterface::get_k2() const {
	return k2;
};

bool HololensARInterface::is_stereo() {
	// needs stereo...
	return true;
};

bool HololensARInterface::is_initialized() {
	return (initialized);
};

bool HololensARInterface::initialize() {
    OutputDebugStringA("initARInterface\n");
	ARVRServer *arvr_server = ARVRServer::get_singleton();
	ERR_FAIL_NULL_V(arvr_server, false);

	if (!initialized) {
		// reset our orientation
		orientation = Basis();

		// make this our primary interface
		arvr_server->set_primary_interface(this);

		last_ticks = OS::get_singleton()->get_ticks_usec();
		;
		initialized = true;
	};

	return true;
};

void HololensARInterface::uninitialize() {
	if (initialized) {
		ARVRServer *arvr_server = ARVRServer::get_singleton();
		if (arvr_server != NULL) {
			// no longer our primary interface
			arvr_server->clear_primary_interface_if(this);
		}

		initialized = false;
	};
};

Size2 HololensARInterface::get_render_targetsize() {
	_THREAD_SAFE_METHOD_

	// we use half our window size
	Size2 target_size = OS::get_singleton()->get_window_size();
	target_size.x *= 0.5 * oversample;
	target_size.y *= oversample;

	return target_size;
};

Transform HololensARInterface::get_transform_for_eye(ARVRInterface::Eyes p_eye, const Transform &p_cam_transform) {
	_THREAD_SAFE_METHOD_
    OutputDebugStringA("get_transform_for_eye ");
    ARVRInterface::EYE_RIGHT!=p_eye ? OutputDebugStringA("Left") : OutputDebugStringA("Right");
    Transform result;
	OSUWP *os=dynamic_cast<OSUWP*>(OS::get_singleton());
    if(initialized)
    {
        ARVRInterface::EYE_RIGHT!=p_eye ? result=os->get_view_matrix_left_eye() : result=os->get_view_matrix_right_eye();
    }
    OutputDebugStringA((String(result)+"\n").utf8());
	return result;
    //OutputDebugStringA((String(result)+"\n").utf8());
	/*Transform transform_for_eye;

	ARVRServer *arvr_server = ARVRServer::get_singleton();
	ERR_FAIL_NULL_V(arvr_server, transform_for_eye);

	if (initialized) {
		float world_scale = arvr_server->get_world_scale();
        //OSUWP::get_singleton()
        // Get the draw buffer.
        // we don't need to check for the existance of our HMD, doesn't effect our values...
		// note * 0.01 to convert cm to m and * 0.5 as we're moving half in each direction...
        //EGLContext *glContext = eglGetCurrentContext();
        //Gluint *program = glContext->getState().getProgram();
        //int viewMatrixIndex = program->getUniformLocation("uHolographicViewMatrix");
        //p_eye == ARVRInterface::EYE_LEFT ? eye.matrix=views[0] : eye.matrix=views[1];

		// just scale our origin point of our transform
		Transform hmd_transform;
		hmd_transform.basis = orientation;
		hmd_transform.origin = Vector3(0.0, eye_height * world_scale, 0.0);

		transform_for_eye = p_cam_transform * (arvr_server->get_reference_frame()) * hmd_transform * transform_for_eye;
	} else {
		// huh? well just return what we got....
		transform_for_eye = p_cam_transform;
	};

	return transform_for_eye;*/
};

CameraMatrix HololensARInterface::get_projection_for_eye(ARVRInterface::Eyes p_eye, real_t p_aspect, real_t p_z_near, real_t p_z_far) {
	_THREAD_SAFE_METHOD_
	OSUWP *os=dynamic_cast<OSUWP*>(OS::get_singleton());
    CameraMatrix result;
    if(initialized)
    {
        ARVRInterface::EYE_RIGHT!=p_eye ? result=os->get_projection_matrix_left_eye() : result=os->get_projection_matrix_right_eye();
    }
    OutputDebugStringA((String(result)+"\n").utf8());
    return result;
    //OutputDebugStringA((String(eye)+"\n").utf8());
	/*if (p_eye == ARVRInterface::EYE_MONO) {
		///@TODO for now hardcode some of this, what is really needed here is that this needs to be in sync with the real cameras properties
		// which probably means implementing a specific class for iOS and Android. For now this is purely here as an example.
		// Note also that if you use a normal viewport with AR/VR turned off you can still use the tracker output of this interface
		// to position a stock standard Godot camera and have control over this.
		// This will make more sense when we implement ARkit on iOS (probably a separate interface).
		eye.set_perspective(60.0, p_aspect, p_z_near, p_z_far, false);
	} else {
        gl::Context *glContext = gl::GetValidGlobalContext();
        gl::Program *program = glContext->getState().getProgram();
        int projectionMatrixIndex = program->getUniformLocation("uHolographicProjectionMatrix");
        float[32] proj;
        program->glGetUniformfv(program,projectionMatrixIndex,&proj);
        //eye.matrix=proj;
        p_eye == ARVRInterface::EYE_LEFT ? eye.matrix=&proj[0] : eye.matrix=&proj[16];
		//TODO eye.set_for_hmd(p_eye == ARVRInterface::EYE_LEFT ? 1 : 2, p_aspect, intraocular_dist, display_width, display_to_lens, oversample, p_z_near, p_z_far);
	};*/

	//return eye;
};
void HololensARInterface::commit_for_eye(ARVRInterface::Eyes p_eye, RID p_render_target, const Rect2 &p_screen_rect) {
	_THREAD_SAFE_METHOD_

	// We must have a valid render target
	ERR_FAIL_COND(!p_render_target.is_valid());

	// Because we are rendering to our device we must use our main viewport!
	ERR_FAIL_COND(p_screen_rect == Rect2());

	float offset_x = 0.0;
	float aspect_ratio = 0.5 * p_screen_rect.size.x / p_screen_rect.size.y;
	Vector2 eye_center;

	if (p_eye != ARVRInterface::EYE_RIGHT) {
		offset_x = -1.0;
		eye_center.x = ((-intraocular_dist / 2.0) + (display_width / 4.0)) / (display_width / 2.0);
	} else if (p_eye == ARVRInterface::EYE_RIGHT) {
		eye_center.x = ((intraocular_dist / 2.0) - (display_width / 4.0)) / (display_width / 2.0);
	}

	// unset our render target so we are outputting to our main screen by making RasterizerStorageGLES3::system_fbo our current FBO
	VSG::rasterizer->set_current_render_target(RID());

	// now output to screen
	//	VSG::rasterizer->blit_render_target_to_screen(p_render_target, screen_rect, 0);

	// get our render target
	RID eye_texture = VSG::storage->render_target_get_texture(p_render_target);
	uint32_t texid = VS::get_singleton()->texture_get_texid(eye_texture);
	/*glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texid);

	lens_shader.bind();
	lens_shader.set_uniform(LensDistortedShaderGLES3::OFFSET_X, offset_x);
	lens_shader.set_uniform(LensDistortedShaderGLES3::K1, k1);
	lens_shader.set_uniform(LensDistortedShaderGLES3::K2, k2);
	lens_shader.set_uniform(LensDistortedShaderGLES3::EYE_CENTER, eye_center);
	lens_shader.set_uniform(LensDistortedShaderGLES3::UPSCALE, oversample);
	lens_shader.set_uniform(LensDistortedShaderGLES3::ASPECT_RATIO, aspect_ratio);

	glBindVertexArray(half_screen_array);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glBindVertexArray(0);*/
};
void HololensARInterface::process() {
	_THREAD_SAFE_METHOD_

	if (initialized) {
		set_position_from_sensors();
	};
};
HololensARInterface::HololensARInterface() {
	initialized = false;

	// Just set some defaults for these. At some point we need to look at adding a lookup table for common device + headset combos and/or support reading cardboard QR codes
	eye_height = 1.85;
	intraocular_dist = 6.0;
	display_width = 14.5;
	display_to_lens = 4.0;
	oversample = 1.5;
	k1 = 0.215;
	k2 = 0.215;
	last_ticks = 0;

};

HololensARInterface::~HololensARInterface() {
	// and make sure we cleanup if we haven't already
	if (is_initialized()) {
		uninitialize();
	};
};
