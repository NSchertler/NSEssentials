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

void Camera::ComputeCameraMatrices(Eigen::Matrix4f & view, Eigen::Matrix4f & proj, float customAspectRatio) const
{
	auto arcBallMatrix = params.arcball.matrix();
	Eigen::Vector3f viewDirection = -arcBallMatrix.row(2).head<3>();
	Eigen::Vector3f cameraPosition = params.focusPoint - viewDirection * params.viewDistance;

	float depthOfSceneCenter = (params.sceneCenter - cameraPosition).dot(viewDirection);
	float minZNear = 0.001f * params.sceneRadius;
	float znear = std::max(minZNear, depthOfSceneCenter - params.sceneRadius);
	float zfar = std::max(znear + minZNear, depthOfSceneCenter + params.sceneRadius);

	float fH = std::tan(params.fovy / 360.0f * (float)M_PI) * znear;
	float aspectRatio = customAspectRatio == 0 ? (float)parent.width() / parent.height() : customAspectRatio;
	float fW = fH * aspectRatio;

	proj = nanogui::frustum(-fW, fW, -fH, fH, znear, zfar);

	view = nanogui::translate(Eigen::Vector3f(0, 0, -params.viewDistance)) * params.arcball.matrix() * nanogui::translate(-params.focusPoint);
}

void Camera::Zoom(float amount)
{
	params.viewDistance = std::max(0.001f * params.sceneRadius, params.viewDistance * std::pow(0.90f, amount));
}

void Camera::FocusOnBBox(const nse::math::BoundingBox<float, 3>& bbox)
{
	params.sceneCenter = 0.5f * (bbox.min + bbox.max);
	params.focusPoint = params.sceneCenter;
	params.sceneRadius = bbox.diagonal().norm() / 2.0f;

	float fov = params.fovy * std::min(1.0f, (float)parent.width() / parent.height());
	params.viewDistance = params.sceneRadius / sinf(fov / 2.0f * (float)M_PI / 180);	
}

void Camera::FocusOnPoint(const Eigen::Vector3f& point)
{
	params.focusPoint = point;
}

const Eigen::Vector3f& Camera::GetFocusPoint() const
{
	return params.focusPoint;
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
				modelTranslation_start = params.focusPoint;
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
		Eigen::Matrix4f view, proj;
		ComputeCameraMatrices(view, proj);

		Eigen::Vector2f current(2.0f * p.x() / parent.height() - 1.0f, -2.0f * p.y() / parent.height() + 1.0f);
		Eigen::Vector2f start(2.0f * translation_start.x() / parent.height() - 1.0f, -2.0f * translation_start.y() / parent.height() + 1.0f);
		auto rel = (current - start) * params.viewDistance * tanf(params.fovy / 2 * (float)M_PI / 180);
		params.focusPoint = modelTranslation_start - (rel.x() * view.block<1, 3>(0, 0).transpose() + rel.y() *  view.block<1, 3>(1, 0).transpose());

		return true;		
	}

	return false;
}

void Camera::resize(const Eigen::Vector2i & s)
{
	params.arcball.setSize(s);
}
#endif