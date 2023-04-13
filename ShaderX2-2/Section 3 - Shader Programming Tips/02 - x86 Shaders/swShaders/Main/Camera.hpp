#ifndef Camera_hpp
#define Camera_hpp

#include "Point.hpp"
#include "Vector.hpp"

namespace swShader
{
	class Camera
	{
	public:
		Camera();

		~Camera();

		Matrix getTransform() const;

		const Point &getPosition() const;
		void setPosition(const Point &P);
		void setPosition(float x, float y, float z);

		const Vector &getOrientation();
		void setOrientation(const Vector &V);
		void setOrientation(float x, float y, float z);

		void translate(const Vector &V);
		void translate(float x, float y, float z);
		void translateAbsolute(const Vector &V);
		void translateAbsolute(float x, float y, float z);
		void translateRelative(const Vector &V);
		void translateRelative(float x, float y, float z);

		void eulerRotate(const Vector &V);
		void eulerRotate(float x, float y, float z);

	private:
		Point position;
		Vector orientation;
	};
}

#endif   // Camera_hpp