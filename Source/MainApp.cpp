/*******************************************
	MainApp.cpp

	Windows functions and DirectX setup
********************************************/

#include <windows.h>
#include <d3d10.h>
#include <d3dx10.h>

#include "Defines.h"
#include "Input.h"
#include "CTimer.h"
#include "CVector2.h"
#include "PostProcessPoly.h"

namespace gen
{


//--------------------------------------------------------------------------------------
// DirectX Variables
//--------------------------------------------------------------------------------------

// The main D3D interface, this pointer is used to access most D3D functions (and is shared across all cpp files through Defines.h)
ID3D10Device* g_pd3dDevice = NULL;

// Variables used to setup D3D
IDXGISwapChain*         SwapChain = NULL;
ID3D10Texture2D*        DepthStencil = NULL;
ID3D10DepthStencilView* DepthStencilView = NULL;
ID3D10RenderTargetView* BackBufferRenderTarget = NULL;

// D3DX font for OSD
ID3DX10Font* OSDFont = NULL;


//--------------------------------------------------------------------------------------
// Windows / System Variables
//--------------------------------------------------------------------------------------

// Resource folders
extern const string MediaFolder = "Media\\";
extern const string ShaderFolder = "Source\\Render\\";


// Window rectangle (dimensions) & client window rectangle - used for toggling fullscreen
RECT ClientRect;
RECT WindowRect;
bool Fullscreen;

// Back buffer dimensions (previously called viewport dimensions - renamed for precision)
TUInt32 BackBufferWidth;
TUInt32 BackBufferHeight;

// Current mouse position
CVector2 MousePixel;

// Game timer
CTimer Timer;



//-----------------------------------------------------------------------------
// D3D management
//-----------------------------------------------------------------------------

// Initialise Direct3D
bool D3DSetup( HWND hWnd )
{
	HRESULT hr = S_OK;

	////////////////////////////////
	// Initialise Direct3D

	// Get initial window and client window dimensions
	GetWindowRect( hWnd, &WindowRect );
	GetClientRect( hWnd, &ClientRect );
	BackBufferWidth  = ClientRect.right - ClientRect.left;
	BackBufferHeight = ClientRect.bottom - ClientRect.top;


	// Create a Direct3D device (i.e. initialise D3D), and create a swap-chain (create a back buffer to render to)
	DXGI_SWAP_CHAIN_DESC sd;         // Structure to contain all the information needed
	ZeroMemory( &sd, sizeof( sd ) ); // Clear the structure to 0 - common Microsoft practice, not really good style
	sd.BufferCount = 1;
	sd.BufferDesc.Width = BackBufferWidth;               // Target window size
	sd.BufferDesc.Height = BackBufferHeight;             // --"--
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Pixel format of target window
	sd.BufferDesc.RefreshRate.Numerator = 60;          // Refresh rate of monitor
	sd.BufferDesc.RefreshRate.Denominator = 1;         // --"--
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // Discard last frame's back buffer after it is shown, alternative is DXGI_SWAP_EFFECT_SEQUENTIAL, which retains the back buffer
	sd.OutputWindow = hWnd;                   // Target window
	sd.Windowed = TRUE;                       // Whether to render in a window (TRUE) or go fullscreen (FALSE)
	if (FAILED( D3D10CreateDeviceAndSwapChain( NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, D3D10_CREATE_DEVICE_DEBUG, D3D10_SDK_VERSION, &sd, &SwapChain, &g_pd3dDevice ) )) return false;


	// Here indicate that the back-buffer can be "viewed" as a render target - rendering to the back buffer is standard behaviour, so this code is standard
	ID3D10Texture2D* pBackBuffer;
	if (FAILED( SwapChain->GetBuffer( 0, __uuidof( ID3D10Texture2D ), ( LPVOID* )&pBackBuffer ) )) return false;
	hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &BackBufferRenderTarget );
	pBackBuffer->Release();
	if( FAILED( hr ) ) return false;


	// Create a texture (bitmap) to use for a depth buffer for the main viewport
	D3D10_TEXTURE2D_DESC descDepth;
	descDepth.Width = BackBufferWidth;
	descDepth.Height = BackBufferHeight;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D10_USAGE_DEFAULT;
	descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	if( FAILED( g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &DepthStencil ) )) return false;

	// Create the depth stencil view, i.e. indicate that the texture just created is to be used as a depth buffer
	if( FAILED( g_pd3dDevice->CreateDepthStencilView( DepthStencil, NULL, &DepthStencilView ) )) return false;

	// Create a font using D3DX helper functions
    if (FAILED(D3DX10CreateFont( g_pd3dDevice, 12, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                                 DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &OSDFont ))) return false;

	return true;
}



// Reset the Direct3D device to resize window or toggle fullscreen/windowed
bool ResetDevice( HWND hWnd, bool ToggleFullscreen = false )
{
	// TODO: Should resize backbuffer, otherwise we will render to the original sized buffer that is then stretched/shrunk to fit the new window size
	return true;
}


// Uninitialise D3D
void D3DShutdown()
{
	// Release D3D interfaces
	if (g_pd3dDevice)           g_pd3dDevice->ClearState();
	if (OSDFont)                OSDFont->Release();
	if (DepthStencilView)       DepthStencilView->Release();
	if (BackBufferRenderTarget) BackBufferRenderTarget->Release();
	if (DepthStencil)           DepthStencil->Release();
	if (SwapChain)              SwapChain->Release();
	if (g_pd3dDevice)           g_pd3dDevice->Release();
}


} // namespace gen


//-----------------------------------------------------------------------------
// Windows functions - outside of namespace
//-----------------------------------------------------------------------------

// Window message handler
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
		{
            PostQuitMessage( 0 );
            return 0;
		}

        case WM_SIZE:
		{
			// Resized window - reset device to match back buffer to new window size
			if (gen::g_pd3dDevice && !gen::ResetDevice( hWnd ))
			{
				DestroyWindow( hWnd );
			}
            return 0;
		}

		case WM_KEYDOWN:
		{
			gen::EKeyCode eKeyCode = static_cast<gen::EKeyCode>(wParam);
			gen::KeyDownEvent( eKeyCode );
			break;
		}

		case WM_KEYUP:
		{
			gen::EKeyCode eKeyCode = static_cast<gen::EKeyCode>(wParam);
			gen::KeyUpEvent( eKeyCode );
			break;
		}
		case WM_MOUSEMOVE:
		{
			gen::MousePixel.x = MAKEPOINTS(lParam).x; 
			gen::MousePixel.y = MAKEPOINTS(lParam).y;
		}
    		case WM_LBUTTONDOWN:
		{
			gen::KeyDownEvent( gen::Mouse_LButton );
			break;
		}
		case WM_LBUTTONUP:
		{
			gen::KeyUpEvent( gen::Mouse_LButton );
			break;
		}
		case WM_RBUTTONDOWN:
		{
			gen::KeyDownEvent( gen::Mouse_RButton );
			break;
		}
		case WM_RBUTTONUP:
		{
			gen::KeyUpEvent( gen::Mouse_RButton );
			break;
		}
		case WM_MBUTTONDOWN:
		{
			gen::KeyDownEvent( gen::Mouse_MButton );
			break;
		}
		case WM_MBUTTONUP:
		{
			gen::KeyUpEvent( gen::Mouse_MButton );
			break;
		}
	}

    return DefWindowProc( hWnd, msg, wParam, lParam );
}

// Windows main function
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), LoadIcon( NULL, IDI_APPLICATION ),
					  LoadCursor( NULL, IDC_ARROW ), NULL, NULL,
                      "PostProcessPoly", NULL };
    RegisterClassEx( &wc );

    // Create the application's window
	HWND hWnd = CreateWindow( "PostProcessPoly", "CO3303: Polygon Post Processing",
                              WS_OVERLAPPEDWINDOW, 100, 100, 1280, 960,
                              NULL, NULL, wc.hInstance, NULL );

    // Initialize Direct3D, the scene and post-processing
	if (gen::D3DSetup( hWnd ) && gen::SceneSetup() && gen::PostProcessSetup())
    {
        // Show the window
        ShowWindow( hWnd, SW_SHOWDEFAULT );
        UpdateWindow( hWnd );

		// Reset the timer for a timed game loop
		gen::Timer.Reset();

        // Enter the message loop
        MSG msg;
        ZeroMemory( &msg, sizeof(msg) );
        while( msg.message != WM_QUIT )
        {
            if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
            {
                TranslateMessage( &msg );
                DispatchMessage( &msg );
            }
            else
			{
				// Render and update the scene - using variable timing
                gen::RenderScene();
				float updateTime = gen::Timer.GetLapTime();
				gen::UpdateScene( updateTime );

				// Toggle fullscreen / windowed
				if (gen::KeyHit( gen::Key_F1 ))
				{
					if (!gen::ResetDevice( hWnd, true ))
					{
						DestroyWindow( hWnd );
					}
				}

				// Quit on escape
				if (gen::KeyHit( gen::Key_Escape ))
				{
					DestroyWindow( hWnd );
				}
			}
        }
    }
    gen::PostProcessShutdown();
    gen::SceneShutdown();
	gen::D3DShutdown();

	UnregisterClass( "PostProcessPoly", wc.hInstance );
    return 0;
}
