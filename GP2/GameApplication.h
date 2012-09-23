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
		
		ID3D10DepthStencilView*m_pDepthStencilView;
		ID3D10Texture2D * m_pDepthStencilTexture;

		ID3D10EffectShaderResourceVariable*m_pDiffuseTextureVariable;
		ID3D10ShaderResourceView*m_pTextureShaderResource;

		ID3D10EffectMatrixVariable*m_pViewMatrixVariable;
		ID3D10EffectMatrixVariable*m_pProjectionMatrixVariable;

		D3DXMATRIX m_matView;
		D3DXMATRIX m_matProjection;

		CWin32Window * m_pWindow; //variable for showing window
		
		ID3D10Buffer* m_pVertexBuffer; //variable
		ID3D10InputLayout*	m_pVertexLayout; //variable
		
		ID3D10Effect* m_pEffect; //vaiable holds the effect file, allowing us to communicate with the loaded effect
		ID3D10EffectTechnique* m_pTechnique; //this is used in the rendering process and will be retieved from the effect varaiable

		ID3D10EffectMatrixVariable* m_pWorldMatrixVariable;

		D3DXMATRIX m_matWorld;
		D3DXMATRIX m_matScale;
		D3DXMATRIX m_matRotation;
		D3DXMATRIX m_matTranslation;

		D3DXVECTOR3 m_vecPosition;
		D3DXVECTOR3 m_vecRotation;
		D3DXVECTOR3 m_vecScale;


};