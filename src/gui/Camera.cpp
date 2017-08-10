/*
	This file is part of NSEssentials.

	Use of this source code is granted via a BSD-style license, which can be found
	in License.txt in the repository root.

	@author Wenzel Jakob
	@author Nico Schertler
*/

#ifdef HAVE_NANOGUI

#include "nsessentials/gui/Camera.h"

using namespace nse::gui;

Camera::Camera(const nanogui::Widget & parent)
	: parent(parent)
{
	params.arcball.setSize(parent.size());
}

void Camera::ComputeCameraMatrices(Eigen::Matrix4f & model, Eigen::Matrix4f & view, Eigen::Matrix4f & proj, float customAspectRatio) const
{
	view = nanogui::lookAt(params.eye, params.center, params.up);

	float fH = std::tan(params.fovy / 360.0f * (float)M_PI) * params.dnear;
	float aspectRatio = customAspectRatio == 0 ? (float)parent.width() / parent.height() : customAspectRatio;
	float fW = fH * aspectRatio;

	proj = nanogui::frustum(-fW, fW, -fH, fH, params.dnear, params.dfar);
	model = params.arcball.matrix();

	model *= nanogui::scale(Eigen::Vector3f::Constant(params._zoom * params.modelZoom));
	model *= nanogui::translate(params.modelTranslation);
}

void Camera::Zoom(float amount)
{
	params._zoom = std::max(0.1f, params._zoom * (1 + 0.1f * amount));
}

void Camera::FocusOnBBox(const nse::math::BoundingBox<float, 3>& bbox)
{
	params.modelZoom = 5.0f / bbox.diagonal().norm();
	params.modelTranslation = -bbox.center();
}

bool Camera::HandleMouseButton(const Eigen::Vector2i & p, int button, bool down, int modifiers)
{
	if (button == GLFW_MOUSE_BUTTON_1 && modifiers == 0)
	{
		if (down)
		{
			if (interactionMode == None)
			{
				interactionMode = Rotate;
				params.arcball.button(p, down);
				return true;
			}
		}
		else
		{
			if (interactionMode == Rotate)
			{
				params.arcball.button(p, false);
				interactionMode = None;
				return true;
			}
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_2 ||
		(button == GLFW_MOUSE_BUTTON_1 && modifiers == GLFW_MOD_SHIFT)) 
	{
		if (down)
		{
			if (interactionMode == None)
			{
				modelTranslation_start = params.modelTranslation;
				translation_start = p;
				interactionMode = Translate;
				return true;
			}
		}
		else
		{
			if (interactionMode == Translate)
			{
				interactionMode = None;
				return true;
			}
		}
	}

	return false;
}

bool Camera::HandleMouseMove(const Eigen::Vector2i & p, const Eigen::Vector2i & rel, int button, int modifiers)
{	
	if (interactionMode == Rotate)
	{
		params.arcball.motion(p);
		return true;
	}
	else if (interactionMode == Translate)
	{
		Eigen::Matrix4f model, view, proj;
		ComputeCameraMatrices(model, view, proj);

		Eigen::Vector2f current(2.0f * p.x() / parent.height() - 1.0f, -2.0f * p.y() / parent.height() + 1.0f);
		Eigen::Vector2f start(2.0f * translation_start.x() / parent.height() - 1.0f, -2.0f * translation_start.y() / parent.height() + 1.0f);
		auto rel = current - start;
		auto mv = view * model;
		params.modelTranslation = modelTranslation_start + 2 * (rel.x() * mv.block<1, 3>(0, 0).transpose() + rel.y() *  mv.block<1, 3>(1, 0).transpose()) / (params._zoom * params._zoom * params.modelZoom * params.modelZoom);

		return true;		
	}

	return false;
}

void Camera::resize(const Eigen::Vector2i & s)
{
	params.arcball.setSize(s);
}
#endif