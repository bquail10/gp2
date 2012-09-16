#pragma once

#include "Win32Window.h"
#include <D3D10.h>
#include <D3DX10.h>

class CGameApplication
{
public:
		CGameApplication(void);
		~CGameApplication(void);

		bool init(); //defining the fuction
		bool run(); //defining the fuction
private:
		bool initGame(); //defining the function
		bool initGraphics(); //defining the function
		bool initWindow(); //defining the fuction

		void render(); //defining the fuction
		void update(); //defining the fuction
private:
		ID3D10Device * m_pD3D10Device; //variable used to render scene
		IDXGISwapChain * m_pSwapChain; //variable used to render scene
		ID3D10RenderTargetView * m_pRenderTargetView; //variable used to render scene
		CWin32Window * m_pWindow; //variable for showing window
		ID3D10Buffer* m_pVertexBuffer; //
		ID3D10InputLayout*	m_pVertexLayout; //
		ID3D10Effect* m_pEffect; //
		ID3D10EffectTechnique* m_pTechnique; //
};