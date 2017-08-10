/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Nico Schertler
*/

#pragma once

#ifdef HAVE_NANOGUI
#include <nanogui/screen.h>

#include "nsessentials/gui/Camera.h"

namespace nse {
	namespace gui
	{
		//This base class provides basic functionality for 3D interaction.
		class AbstractViewer : public nanogui::Screen
		{
		public:
			AbstractViewer(const std::string& title, int width = 1280, int height = 800);

			bool scrollEvent(const Eigen::Vector2i & p, const Eigen::Vector2f & rel);
			bool mouseButtonEvent(const Eigen::Vector2i & p, int button, bool down, int modifiers);
			bool mouseMotionEvent(const Eigen::Vector2i & p, const Eigen::Vector2i & rel, int button, int modifiers);

			virtual bool keyboardEvent(int key, int scancode, int action, int mods);

			bool resizeEvent(const Eigen::Vector2i&);

			const Camera& camera() const { return _camera; }
			Camera& camera() { return _camera; }

			bool ctrlDown() const { return _ctrlDown; }
			bool shiftDown() const { return _shiftDown; }

			//returns depth buffer value
			float get3DPosition(const Eigen::Vector2i& screenPos, Eigen::Vector4f& pos);
			float get3DPosition(const Eigen::Vector2i& screenPos, Eigen::Vector3f& pos);

		protected:
			Camera _camera;

			bool _ctrlDown;
			bool _shiftDown;

			virtual bool scrollHook(const Eigen::Vector2i & p, const Eigen::Vector2f & rel) { return false; }
			virtual bool mouseButtonHook(const Eigen::Vector2i & p, int button, bool down, int modifiers) { return false; }
			virtual bool mouseMotionHook(const Eigen::Vector2i & p, const Eigen::Vector2i & rel, int button, int modifiers) { return false; }
		};

	}
}
#endif