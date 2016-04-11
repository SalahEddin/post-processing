/*******************************************

	Camera.cpp

	Camera class implementation
********************************************/

#include <d3dx9.h>
#include "CVector4.h"
#include "MathDX.h"
#include "Camera.h"

namespace gen
{

//-----------------------------------------------------------------------------
// Constructors
//-----------------------------------------------------------------------------

// Constructor, with defaults for all parameters
CCamera::CCamera( const CVector3& position /*= CVector3::kOrigin*/, 
	              const CVector3& rotation /*= CVector3( 0.0f, 0.0f, 0.0f )*/,
                  TFloat32 nearClip /*= 1.0f*/, TFloat32 farClip /*= 100000.0f*/, 
				  TFloat32 fov /*= D3DX_PI/3.0f*/, TFloat32 aspect /*= 1.33f*/ )
{
	m_Matrix = CMatrix4x4( position, rotation );
	SetNearFarClip( nearClip, farClip );
	SetFOV( fov );
	SetAspect( aspect );
	CalculateMatrices();
}


//-----------------------------------------------------------------------------
// Camera matrix functions
//-----------------------------------------------------------------------------

// Sets up the view and projection transform matrices for the camera
void CCamera::CalculateMatrices()
{
    // Set up the view matrix
 	m_MatView = InverseAffine( m_Matrix );

	// For the projection matrix, we set up a perspective transform (which
    // transforms geometry from 3D view space to 2D viewport space, with
    // a perspective divide making objects smaller in the distance). To build
    // a perpsective transform, we need the field of view, the viewport 
	// aspect ratio, and the near and far clipping planes (which define at
    // what distances geometry should be no longer be rendered).
	TFloat32 fovY = ATan(Tan( m_FOV * 0.5f ) / m_Aspect) * 2.0f; // Need fovY, storing fovX
    D3DXMatrixPerspectiveFovLH( ToD3DXMATRIXPtr(&m_MatProj), fovY, m_Aspect,
	                            m_NearClip, m_FarClip );

	// Combine the view and projection matrix into a single matrix - this will
	// be passed to vertex shaders (more efficient this way)
	m_MatViewProj = m_MatView * m_MatProj;
}


// Controls the camera - uses the current view matrix for local movement
void CCamera::Control( EKeyCode turnUp, EKeyCode turnDown,
                       EKeyCode turnLeft, EKeyCode turnRight,  
                       EKeyCode moveForward, EKeyCode moveBackward,
                       EKeyCode moveLeft, EKeyCode moveRight,
                       TFloat32 MoveSpeed, TFloat32 RotSpeed )
{
	if (KeyHeld( turnDown ))
	{
		m_Matrix.RotateLocalX( RotSpeed );
	}
	if (KeyHeld( turnUp ))
	{
		m_Matrix.RotateLocalX( -RotSpeed );
	}
	if (KeyHeld( turnRight ))
	{
		m_Matrix.RotateY( RotSpeed );
	}
	if (KeyHeld( turnLeft ))
	{
		m_Matrix.RotateY( -RotSpeed );
	}

	// Local X movement - move in the direction of the X axis, taken from view matrix
	if (KeyHeld( moveRight )) 
	{
		m_Matrix.MoveLocalX( MoveSpeed );
	}
	if (KeyHeld( moveLeft ))
	{
		m_Matrix.MoveLocalX( -MoveSpeed );
	}

	// Local Z movement - move in the direction of the Z axis, taken from view matrix
	if (KeyHeld( moveForward ))
	{
		m_Matrix.MoveLocalZ( MoveSpeed );
	}
	if (KeyHeld( moveBackward ))
	{
		m_Matrix.MoveLocalZ( -MoveSpeed );
	}
}


//-----------------------------------------------------------------------------
// Camera picking
//-----------------------------------------------------------------------------

// Calculate the X and Y pixel coordinates for the corresponding to given world coordinate
// using this camera. Pass the viewport width and height. Return false if the world coordinate
// is behind the camera
bool CCamera::PixelFromWorldPt( CVector2* PixelPt, CVector3 worldPt, TUInt32 viewportWidth, TUInt32 viewportHeight )
{
	CVector4 viewportPt = CVector4(worldPt, 1.0f) * m_MatViewProj;
	if (viewportPt.w < 0)
	{
		return false;
	}

	viewportPt.x /= viewportPt.w;
	viewportPt.y /= viewportPt.w;
	PixelPt->x = (viewportPt.x + 1.0f) * viewportWidth * 0.5f;
	PixelPt->y = (1.0f - viewportPt.y) * viewportHeight * 0.5f;

	return true;
}

// Calculate the world coordinates of a point on the near clip plane corresponding to given 
// X and Y pixel coordinates using this camera. Pass the viewport width and height
CVector3 CCamera::WorldPtFromPixel( CVector2 pixelPt, TUInt32 viewportWidth, TUInt32 viewportHeight )
{
	CVector4 cameraPt;
	cameraPt.x = pixelPt.x / (viewportWidth * 0.5f) - 1.0f;
	cameraPt.y = 1.0f - pixelPt.y / (viewportHeight * 0.5f);
	cameraPt.z = 0.0f;
	cameraPt.w = m_NearClip;

	cameraPt.x *= cameraPt.w;
	cameraPt.y *= cameraPt.w;
	cameraPt.z *= cameraPt.w;
	
	CVector4 worldPt = cameraPt * Inverse( m_MatViewProj );

	return CVector3(worldPt);
}


//-----------------------------------------------------------------------------
// Frustrum planes
//-----------------------------------------------------------------------------

// Calculate the 6 planes of the camera's viewing frustum. Store each plane as a point (on the
// plane) and a vector (pointing away from the frustum)
//   Four frustum planes:  _________
//   Near, far, left and   \       /
//   right. Top and bottom  \     /
//   not shown               \   /
//                            \_/
//                             ^ Camera
// See http://www.lighthouse3d.com/opengl/viewfrustum/index.php for an extensive discussion of
// view frustum clipping
void CCamera::CalculateFrustrumPlanes()
{
	// Position and local direction m_FrustumVecs for camera
	CVector3 cameraRight = m_Matrix.XAxis();
	CVector3 cameraUp = m_Matrix.YAxis();
	CVector3 cameraForward = m_Matrix.ZAxis();
	CVector3 cameraPos = m_Matrix.Position();

	// Near clip plane
	m_FrustumVecs[0] = -cameraForward; // m_FrustumPts back towards camera (-ve camera local Z)
	m_FrustumVecs[0].Normalise();      // Probably don't need to normalise (scaled camera?), but safe
	m_FrustumPts[0] = cameraPos - m_NearClip * m_FrustumVecs[0];  // Point is along camera z-axis on plane

	// Far clip plane - similar process to above
	m_FrustumVecs[1] = cameraForward;
	m_FrustumVecs[1].Normalise();
	m_FrustumPts[1] = cameraPos + m_FarClip * m_FrustumVecs[1];

	// All the remaining planes have their point as the camera position. This is *behind* the
	// near clip plane, but it doesn't matter when defining the plane (which extends to infinity)
	m_FrustumPts[2] = m_FrustumPts[3] = m_FrustumPts[4] = m_FrustumPts[5] = cameraPos; 

	// Get (half) width and height of viewport in camera space (the aperture)
	TFloat32 apertureHalfHeight = Tan( m_FOV * 0.5f ) * m_NearClip;
	TFloat32 apertureHalfWidth = apertureHalfHeight * m_Aspect;
	
	// Left plane vector
	// Point on left of aperture - step left from centre of aperture calculated for near clip plane
	CVector3 leftPoint = m_FrustumPts[0] - cameraRight * apertureHalfWidth; 
	// Get vector from camera to left of aperture, cross product with camera up for vector
	m_FrustumVecs[2] = Cross( leftPoint - cameraPos, cameraUp ); // Order important
	m_FrustumVecs[2].Normalise();

	// Right plane vector - similar
	CVector3 rightPoint = m_FrustumPts[0] + cameraRight * apertureHalfWidth; 
	m_FrustumVecs[3] = Cross( cameraUp, rightPoint - cameraPos ); // Order important
	m_FrustumVecs[3].Normalise();

	// Top plane vector - similar
	CVector3 topPoint = m_FrustumPts[0] + cameraUp * apertureHalfHeight; 
	m_FrustumVecs[4] = Cross( topPoint - cameraPos, cameraRight );
	m_FrustumVecs[4].Normalise();

	// Bottom plane vector - similar
	CVector3 bottomPoint = m_FrustumPts[0] - cameraUp * apertureHalfHeight; 
	m_FrustumVecs[5] = Cross( cameraRight, bottomPoint - cameraPos );
	m_FrustumVecs[5].Normalise();
}


// Test if a sphere is visible in the viewing frustum. Tests the sphere against each plane.
// See http://www.lighthouse3d.com/opengl/viewfrustum/index.php for an extensive discussion
// of view frustum clipping including the method used here
bool CCamera::SphereInFrustum( const CVector3& Centre, TFloat32 Radius )
{
	// Check the sphere against each frustum plane
	for (int plane = 0; plane < 6; ++plane)
	{
		// Test if sphere centre is further away than its radius - outside the frustum
		if (Dot( Centre - m_FrustumPts[plane], m_FrustumVecs[plane] ) > Radius)
		{
			return false;
		}
	}
	return true;
}

// Test if a bounding box is visible in the viewing frustum. Tests one point of the bounding
// box against each plane. See http://www.lighthouse3d.com/opengl/viewfrustum/index.php for
// an extensive discussion of view frustum clipping including the method used here
bool CCamera::AABBInFrustum( const CVector3& AABBMin, const CVector3& AABBMax )
{
	// Check the bounding box against each frustum plane
	for (int plane = 0; plane < 6; ++plane)
	{
		// Get point of bounding box nearest the plane - use plane normal to find which point
		CVector3 nearPoint;
		nearPoint.x = (m_FrustumVecs[plane].x >= 0) ? AABBMin.x : AABBMax.x;
		nearPoint.y = (m_FrustumVecs[plane].y >= 0) ? AABBMin.y : AABBMax.y;
		nearPoint.z = (m_FrustumVecs[plane].z >= 0) ? AABBMin.z : AABBMax.z;

		// If point outside plane then box is outside the frustum - no need to test other planes
		if (Dot( nearPoint - m_FrustumPts[plane], m_FrustumVecs[plane] ) > 0)
		{
			return false;
		}
	}

	return true;
}


} // namespace gen