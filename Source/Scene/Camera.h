/*******************************************
	Camera.h

	Camera class declarations
********************************************/

#pragma once

#include "Defines.h"
#include "CVector3.h"
#include "CMatrix4x4.h"
#include "Input.h"

namespace gen
{

class CCamera
{
public:

	/////////////////////////////
	// Constructors

	// Constructor, with defaults for all parameters
	CCamera( const CVector3& position = CVector3::kOrigin, 
	         const CVector3& rotation = CVector3( 0.0f, 0.0f, 0.0f ),
			 TFloat32 nearClip = 1.0f, TFloat32 farClip = 100000.0f,
			 TFloat32 fov = kfPi/3.0f, TFloat32 aspect = 1.33f );


	///////////////////////////
	// Getters / Setters

	// Direct access (reference) to position and matrix
	CVector3& Position()
	{
		return m_Matrix.Position();
	}
	CMatrix4x4& Matrix()
	{
		return m_Matrix;
	}

	// Camera internals - Getters
	TFloat32 GetNearClip()
	{
		return m_NearClip;
	}
	TFloat32 GetFarClip()
	{
		return m_FarClip;
	}
	TFloat32 GetFOV()
	{
		return m_FOV;
	}
	TFloat32 GetAspect()
	{
		return m_Aspect;
	}

	// Camera internals - Setters
	void SetNearFarClip( TFloat32 nearClip, TFloat32 farClip )
	{
		m_NearClip = nearClip;
		m_FarClip = farClip;
	}
	void SetFOV( TFloat32 fov )
	{
		m_FOV = fov;
	}
	void SetAspect( TFloat32 aspect )
	{
		m_Aspect = aspect;
	}


	// Camera matrices - Getters
	CMatrix4x4 GetViewMatrix()
	{
		return m_MatView;
	}
	CMatrix4x4 GetProjMatrix()
	{
		return m_MatProj;
	}
	CMatrix4x4 GetViewProjMatrix()
	{
		return m_MatViewProj;
	}


	/////////////////////////////
	// Camera matrix functions

	// Sets up the view and projection transform matrices for the camera
	void CalculateMatrices();

	// Controls the camera - uses the current view matrix for local movement
	void Control( EKeyCode turnUp, EKeyCode turnDown,
	              EKeyCode turnLeft, EKeyCode turnRight,  
	              EKeyCode moveForward, EKeyCode moveBackward,
	              EKeyCode moveLeft, EKeyCode moveRight,
				  TFloat32 MoveSpeed, TFloat32 RotSpeed );


	///////////////////////////
	// Camera picking

	// Calculate pixel coordinates (as a CVector2) corresponding to given world point when viewing from this
	// camera. Pass the viewport width and height. Return false if the world point is behind the camera
	bool PixelFromWorldPt( CVector2* PixelPt, CVector3 worldPt, TUInt32 ViewportWidth, TUInt32 ViewportHeight );

	// Calculate the world coordinates of a point on the near clip plane corresponding to given 
	// pixel coordinates when viewing from this camera. Pass the viewport width and height
	CVector3 WorldPtFromPixel( CVector2 pixelPt, TUInt32 ViewportWidth, TUInt32 ViewportHeight );


	///////////////////////////
	// Frustrum testing

	// Calculate the 6 planes of the camera's viewing frustum. Store each plane as a point (on the
	// plane) and a vector (pointing away from the frustum)
	// Order of planes is near, far, left, right, top, bottom
	//   Four frustum planes:  _________
	//   Near, far, left and   \       /
	//   right. Top and bottom  \     /
	//   not shown               \   /
	//                            \_/
	//                             ^ Camera
	void CalculateFrustrumPlanes();
	
	// Test if a sphere is visible in the viewing frustum. Tests the sphere against each plane.
	// See http://www.lighthouse3d.com/tutorials/view-frustum-culling/ for an extensive discussion
	// of view frustum clipping including the method used here
	bool SphereInFrustum( const CVector3& Centre, TFloat32 Radius );

	// Test if a bounding box is visible in the viewing frustum. Tests one point of the bounding
	// box against each plane. See http://www.lighthouse3d.com/tutorials/view-frustum-culling/ for
	// an extensive discussion of view frustum clipping including the method used here
	bool AABBInFrustum( const CVector3& AABBMin, const CVector3& AABBMax );


private:
	// Current positioning matrix
	CMatrix4x4 m_Matrix;

	// Near and far clip plane distances
	TFloat32 m_NearClip;
	TFloat32 m_FarClip;

	// Field of view - the angle covered from the left to the right side of the viewport
	TFloat32 m_FOV;

	// Aspect ratio of the viewport = Width / Height
	TFloat32 m_Aspect;

	// Current view and projection matrices
	CMatrix4x4 m_MatView;
	CMatrix4x4 m_MatProj;
	CMatrix4x4 m_MatViewProj; // Combined view/projection matrix

	// The six planes of the camera viewing frustum, stored as six points and vectors
	// Order of planes is near, far, left, right, top, bottom
	CVector3 m_FrustumPts[6];
	CVector3 m_FrustumVecs[6];
};


} // namespace gen