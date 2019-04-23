/*************************************************************************/
/*  opencv_texture.cpp                                                    */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
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

#include "opencv_texture.h"

#include "core/core_string_names.h"
#include <gd.h>
#include <gd.h>

OpenCVTexture::OpenCVTexture() {
	update_queued = false;
	opencv_thread = NULL;
	regen_queued = false;
	first_time = true;

	size = Vector2i(640, 480);
	flags = FLAGS_DEFAULT;
	texture = VS::get_singleton()->texture_create();
	_set_texture_data();
	opencv_thread = Thread::create(_thread_function, this);
	//_thread_function(this);
	_queue_update();
}

OpenCVTexture::~OpenCVTexture() {
	VS::get_singleton()->free(texture);
}

void OpenCVTexture::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_width", "width"), &OpenCVTexture::set_width);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &OpenCVTexture::set_height);

	ClassDB::bind_method(D_METHOD("_update_texture"), &OpenCVTexture::_update_texture);
	ClassDB::bind_method(D_METHOD("_thread_done"), &OpenCVTexture::_thread_done);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,2048,1,or_greater"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "height", PROPERTY_HINT_RANGE, "1,2048,1,or_greater"), "set_height", "get_height");
}

void OpenCVTexture::_validate_property(PropertyInfo &property) const {

}

void OpenCVTexture::_set_texture_data() {
	VS::get_singleton()->texture_allocate(texture, size.x, size.y, 0, Image::FORMAT_RGB8, VS::TEXTURE_TYPE_2D, flags);
}

void OpenCVTexture::_thread_done() {
	Thread::wait_to_finish(opencv_thread);
	memdelete(opencv_thread);
	opencv_thread = NULL;
	if (regen_queued) {
		opencv_thread = Thread::create(_thread_function, this);
		regen_queued = false;
	}
}

void OpenCVTexture::_thread_function(void *p_ud) {
	OpenCVTexture *st = (OpenCVTexture *)p_ud;
	Ref<OpenCVTexture> stex(st);
	//Open the default video camera
    cv::VideoCapture cap =cv::VideoCapture();
    // open the default camera, use something different from 0 otherwise;
    // Check VideoCapture documentation.
	cap.open(0);

    // if not success, exit program
    if (cap.isOpened() == false)
    {
        cout << "Cannot open the video camera" << endl;
        //cin.get(); //wait for any key press
        //return -1;
    }

    int frame_width = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH)); //get the width of frames of the video
    int frame_height = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT)); //get the height of frames of the video
    cv::Size frame_size(frame_width, frame_height);
	cv::Mat frame =cv::Mat(frame_height,frame_width,CV_8UC4);
	st->frame_data.resize(frame_width*frame_height*3);
	cap >> frame;
    do
    {
        //bool isSuccess = cap.read(frame); // read a new frame from the video camera
		cap >> frame;
		//frame.reshape(3);
		PoolVector<uint8_t>::Write w = st->frame_data.write();
		for(int i=0;i<(frame_width*frame_height);i++){
			uint8_t *pixl=frame.ptr(i/frame_width,i%frame_width);
			w[i*3]=pixl[2];
			w[i*3+1]=pixl[1];
			w[i*3+2]=pixl[0];
		}
		Ref<Image> img = memnew(Image(frame_width, frame_height, 0, Image::FORMAT_RGB8, st->frame_data));
		VS::get_singleton()->texture_set_data(st->texture,img);

    }
	while (true);
    cap.release();
}

void OpenCVTexture::_queue_update() {

	if (update_queued)
		return;

	update_queued = true;
}

void OpenCVTexture::_update_texture() {
	bool use_thread = true;
	if (first_time) {
		use_thread = false;
		first_time = false;
	}
#ifdef NO_THREADS
	use_thread = false;
#endif
	if (use_thread) {

		if (!opencv_thread) {
			opencv_thread = Thread::create(_thread_function, this);
			regen_queued = false;
		} else {
			regen_queued = true;
		}

	}
}

void OpenCVTexture::set_width(int p_width) {
	if (p_width == size.x) return;
	size.x = p_width;
	_queue_update();
}

void OpenCVTexture::set_height(int p_height) {
	if (p_height == size.y) return;
	size.y = p_height;
	_queue_update();
}

int OpenCVTexture::get_width() const {

	return size.x;
}

int OpenCVTexture::get_height() const {

	return size.y;
}

void OpenCVTexture::set_flags(uint32_t p_flags) {
	flags = p_flags;
	VS::get_singleton()->texture_set_flags(texture, flags);
}

uint32_t OpenCVTexture::get_flags() const {
	return flags;
}
