#include "Camera.hpp"

#include "Matrix.hpp"

namespace swShader
{
	Camera::Camera()
	{
		position = 0;
		orientation = 0;
	}

	Camera::~Camera()
	{
	}

	Matrix Camera::getTransform() const
	{
		return Matrix::eulerRotate(orientation) * Matrix::translate(-(Vector)position);
	}

	const Point &Camera::getPosition() const
	{
		return position;
	}

	void Camera::setPosition(const Point &P)
	{
		position = P;
	}

	void Camera::setPosition(float x, float y, float z)
	{
		position = Point(x, y, z);
	}

	const Vector &Camera::getOrientation()
	{
		return orientation;
	}

	void Camera::setOrientation(const Vector &V)
	{
		orientation = V;
	}

	void Camera::setOrientation(float x, float y, float z)
	{
		orientation = Vector(x, y, z);
	}

	void Camera::translate(const Vector &V)
	{
		position += V;
	}

	void Camera::translate(float x, float y, float z)
	{
		position += Vector(x, y, z);
	}

	void Camera::translateAbsolute(const Vector &V)
	{
		position += V;
	}

	void Camera::translateAbsolute(float x, float y, float z)
	{
		position += Vector(x, y, z);
	}

	void Camera::translateRelative(const Vector &V)
	{
		position += ~Matrix::eulerRotate(orientation) * V;
	}

	void Camera::translateRelative(float x, float y, float z)
	{
		position += ~Matrix::eulerRotate(orientation) * Vector(x, y, z);
	}

	void Camera::eulerRotate(const Vector &V)
	{
		orientation += V;
	}

	void Camera::eulerRotate(float x, float y, float z)
	{
		orientation += Vector(x, y, z);
	}
}