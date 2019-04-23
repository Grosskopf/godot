/*************************************************************************/
/*  opencv_texture.h                                                      */
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

#ifndef OPENCV_TEXTURE_H
#define OPENCV_TEXTURE_H

#include <iostream> // for standard I/O
#include <string>   // for strings

#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
//#include <opencv4/opencv2/videoio/videoio.hpp>
#include <iostream>
#include "core/image.h"
#include "core/reference.h"
#include "editor/editor_node.h"
#include "editor/editor_plugin.h"
#include "editor/property_editor.h"

using namespace std;
//using namespace cv;

class OpenCVTexture : public Texture {
	GDCLASS(OpenCVTexture, Texture)

private:
	//Ref<Image> data;
	PoolVector<uint8_t> frame_data;

	Thread *opencv_thread;

	bool first_time;
	bool update_queued;
	bool regen_queued;

	RID texture;
	uint32_t flags;

	Vector2i size;

	void _thread_done(/*const Ref<Image> &p_image*/);
	static void _thread_function(void *p_ud);

	void _queue_update();
	//Ref<Image> _generate_texture();
	void _update_texture();
	void _set_texture_data(/*const Ref<Image> &p_image*/);

protected:
	static void _bind_methods();
	virtual void _validate_property(PropertyInfo &property) const;

public:

	void set_width(int p_width);
	void set_height(int p_hieght);

	int get_width() const;
	int get_height() const;

	virtual void set_flags(uint32_t p_flags);
	virtual uint32_t get_flags() const;

	virtual RID get_rid() const { return texture; }
	virtual bool has_alpha() const { return false; }

	OpenCVTexture();
	virtual ~OpenCVTexture();
};

#endif // OPENCV_TEXTURE_H
