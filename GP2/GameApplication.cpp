#include "GameApplication.h"

struct Vertex
{
	D3DXVECTOR3 Pos;
};

CGameApplication::CGameApplication(void) //class constructor called for creating instance of a class
{
	m_pWindow=NULL;
	m_pD3D10Device=NULL;
	m_pRenderTargetView=NULL;
	m_pSwapChain=NULL;
	m_pVertexBuffer=NULL;
}

CGameApplication::~CGameApplication(void) //deconstructor deallocate all resources, D3D10 objects we call the release function
{
	if(m_pD3D10Device)
		m_pD3D10Device->ClearState(); 
	
	if(m_pVertexBuffer)
		m_pVertexBuffer->Release();

	if(m_pVertexLayout)
		m_pVertexLayout->Release();
	
	if(m_pEffect)
		m_pEffect->Release();

	if(m_pRenderTargetView)
		m_pRenderTargetView->Release();
	
	if(m_pSwapChain)
		m_pSwapChain->Release();
	
	if(m_pD3D10Device)
		m_pD3D10Device->Release();

	if(m_pWindow) //deletes the window
	{
		delete m_pWindow;
		m_pWindow=NULL;
	}
}
		bool CGameApplication::init() //function called to initialize the game, this will include sound, input and game initialization
		{
			if(!initWindow())
				return false;
			if(!initGraphics())
				return false;
			if(!initGame())
				return false;

			return true;
		}

		bool CGameApplication::run() //function 
		{
			while(m_pWindow->running()) //loop that continues til the window closes 
			{
				if(! m_pWindow->checkForWindowMessages()) //checks for window messages, update and render the scene when there are no window messages
				{
					update();
					render();
				}
			}
			return false;
		}

		void CGameApplication::render() //function will have the drawing code in it
		{
			float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f};
			m_pD3D10Device->ClearRenderTargetView( m_pRenderTargetView, ClearColor);

			D3D10_TECHNIQUE_DESC techDesc;
			m_pTechnique->GetDesc( &techDesc);
			for(UINT p = 0; p <techDesc.Passes; ++p)
			{
				m_pTechnique->GetPassByIndex(p)->Apply(0);
				m_pD3D10Device->Draw(3,0);
			}

			m_pSwapChain->Present(0, 0);
		}

		void CGameApplication::update() //function updates the game state, AI, input devices and physics
		{
		}

		bool CGameApplication::initGraphics() //function intialize Direct3D10
		{
			RECT windowRect;
			GetClientRect(m_pWindow->getHandleToWindow(),&windowRect);

			//need these values for the creation of the swap chain
			UINT width=windowRect.right-windowRect.left; //retrieves the width of the window 
			UINT height=windowRect.bottom-windowRect.top; //retrieves the height of the window
			
			UINT createDeviceFlags=0; //initialized unsigned int holds flags for device creation
#ifdef DEBUG // flags often used to put device into debug mode, makes it easier to debug for rendering issues by collecting info from driver
			createDeviceFlags|=D3D10_CREATE_DEVICE_DEBUG; //checks to see if there is a Preprocessoer flag enabled in the build
#endif													  //checks if the development enviroment is in debug
			
			DXGI_SWAP_CHAIN_DESC sd; //initialize variable of type DXGI_SWAP_CHAIN_DESC, holds all options for the craetion of the swap chain
			ZeroMemory( &sd, sizeof( sd )); //ZeroMemory is a function takes in memory address and size of a variable and initailizes all values too 0 
			
			if(m_pWindow->isFullScreen()) //checks if the window is fullscreen
				sd.BufferCount = 2; //we specify 2 buffers(front, back)
			else
				sd.BufferCount = 1; //if not full screen then just 1 buffer and the desktop used as front buffer

			sd.OutputWindow = m_pWindow->getHandleToWindow(); //associates a window handle with the swap chain desc
			sd.Windowed = (BOOL)(!m_pWindow->isFullScreen()); //this converts from boolean to a BOOL if in window mode, we use not to say if windowed or not
			sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //says the buffer used as a render target(something to draw to)

			sd.SampleDesc.Count = 1; //sets multisampling(     ) parameters for the swap chain
			sd.SampleDesc.Quality = 0; // it is turned off because it will have some preformance considerations

			//this code sets options for underlying buffer
			sd.BufferDesc.Width = width; //width of the buffer
			sd.BufferDesc.Height = height; //height of the buffer
			sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //sets format of the buffer has 8bytes for each component(red, green, blue, alpha)
			sd.BufferDesc.RefreshRate.Numerator = 60; //sets the refresh rate of 60Hz
			sd.BufferDesc.RefreshRate.Denominator = 1; //using an update on the vertical blank

			//function that creates the swap chain  and the device in one call
			//it has an if and FAILED to check if the function is failed
			if(FAILED(D3D10CreateDeviceAndSwapChain(NULL, //a pointer to IDXGIAdapter(analogous to a physical graphics card) NULL uses default adapter
				D3D10_DRIVER_TYPE_HARDWARE, //the type of driver flag, D3D10_DRIVER_TYPE_HARDWARE is a hardware device
				NULL, //a handle to a module(dynamic link library) which contains software inplementation of D3D10, will be NULL most of the time
				createDeviceFlags, //this is optional, its used to give additioanl options when we craete a device, this flag used to put into debug
				D3D10_SDK_VERSION, //this is the version of D3D10 we are working with, will always be D3D10_SDK_VERSION
				&sd, //a pointer for the swap chain desc, uses memory address operator(&) to get pointer
				&m_pSwapChain, //an address of a pointer to swap chain interface, initializes our IDXFGISwapCahin pointer
				&m_pD3D10Device))) //an address of a pointer to D3D10Device, initializes our D3D10Device pointer
				return false;
			
			ID3D10Texture2D *pBackBuffer;
			if(FAILED(m_pSwapChain->GetBuffer(0,
					__uuidof(ID3D10Texture2D),
					(void**)&pBackBuffer)));
			return false;

			if(FAILED(m_pD3D10Device->CreateRenderTargetView( pBackBuffer,
				NULL,
				&m_pRenderTargetView)))
			{
				pBackBuffer->Release();
				return false;
			}
			pBackBuffer->Release();

			m_pD3D10Device->OMSetRenderTargets(1,
				&m_pRenderTargetView,
				NULL);

			D3D10_VIEWPORT vp;
			vp.Width = width;
			vp.Height = height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			m_pD3D10Device->RSSetViewports( 1, &vp);

			return true;
		}

		bool CGameApplication::initWindow() //function will initialize Win32 window
		{
			m_pWindow=new CWin32Window(); //allocates an new instance of Win32 window
			if(!m_pWindow->init(TEXT("Lab 1 - Create Device"),800,640,false)) //init function creates the window
				return false;

			return true;
		}

		bool CGameApplication::initGame() //The function creates the objects for the game include textures, effects and 3D models
		{
			D3D10_BUFFER_DESC bd;
			bd.Usage = D3D10_USAGE_DEFAULT;
			bd.ByteWidth = sizeof( Vertex ) * 3;
			bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			bd.MiscFlags = 0;
			
			Vertex vertices[] =
			{
				D3DXVECTOR3( 0.0f, 0.5f, 0.5f), // position of the first vertice
				D3DXVECTOR3( 0.5f, -0.5f, 0.5f), // position of the second vertice
				D3DXVECTOR3( -0.5f, -0.5f, 0.5f), // position of the third vertice
			};

			D3D10_SUBRESOURCE_DATA InitData;
			InitData.pSysMem = vertices;

			if(FAILED(m_pD3D10Device ->CreateBuffer( &bd, &InitData,
				&m_pVertexBuffer )))
				return false;

			D3D10_INPUT_ELEMENT_DESC layout[] =
			{
				{
					"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
						D3D10_INPUT_PER_VERTEX_DATA, 0
				}
			};

			UINT numElements = sizeof( layout )/sizeof(D3D10_INPUT_ELEMENT_DESC);
			D3D10_PASS_DESC PassDesc;
			m_pTechnique->GetPassByIndex(0)->GetDesc( &PassDesc);

			m_pD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			if(FAILED(m_pD3D10Device->CreateInputLayout( layout,
				numElements,
				PassDesc.pIAInputSignature,
				PassDesc.IAInputSignatureSize,
				&m_pVertexLayout)))
			{
				return false;
			}

			m_pD3D10Device->IASetInputLayout(m_pVertexLayout);

			UINT stride = sizeof(Vertex);
			UINT offset = 0;
			m_pD3D10Device->IASetVertexBuffers( 0, 1,
								&m_pVertexBuffer, &stride, &offset );


			DWORD dwShaderFlags =  D3D10_SHADER_ENABLE_STRICTNESS;
#if defined(DEBUG)|| defined(_DEBUG)
			dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

			if(FAILED(D3DX10CreateEffectFromFile( TEXT("ScreenSpace.fx"),
				NULL,NULL, "fx_4_0", dwShaderFlags, 0,
				m_pD3D10Device, NULL, NULL, &m_pEffect,
				NULL, NULL )))
			{
				MessageBox( NULL, TEXT("The FX file cannot be located. Please run this executable from the derectory that contains the FX file."),
					TEXT("Error"),
					MB_OK);
				return false;
			}

			m_pTechnique=m_pEffect->GetTechniqueByName("Render");

			return true;
		}

		
	