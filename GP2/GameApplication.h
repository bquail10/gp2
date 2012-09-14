#pragma once

#include "Win32Window.h"
#include <D3D10.h>
#include <D3DX10.h>

class CGameApplication
{
public:
		CGameApplication(void);
		~CGameApplication(void);

		bool init();
		bool run();
private:
		bool initGraphics();
		bool initWindow();

		void render();
		void update();
private:
		ID3D10Device * m_pD3D10Device; // Variable used to render scene
		IDXGISwapChain * m_pSwapChain; // Variable used to render scene
		ID3D10RenderTargetView * m_pRenderTargetView; // Variable used to render scene
		CWin32Window * m_pWindow;

};