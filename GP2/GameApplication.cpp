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
			
		//Initialise the swap chain description by setting all its values to zero - BMD
	DXGI_SWAP_CHAIN_DESC sd;
	//http://msdn.microsoft.com/en-us/library/aa366920%28v=vs.85%29.aspx - BMD
    ZeroMemory( &sd, sizeof( sd ) );
	//What kind of surface is contained in the swap chain, in this case something we draw too
	//http://msdn.microsoft.com/en-us/library/bb173078%28v=vs.85%29.aspx - BMD
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	//Number of buffers, if we are not full screen this will be one as the desktop
	//acts as a front buffer. If we are in full screen this will be one - BMD
	if (m_pWindow->isFullScreen())
		sd.BufferCount = 2;
	else 
		sd.BufferCount=1;
	//The handle of the window which this swap chain is linked to, this must not be NULL - BMD
	sd.OutputWindow = m_pWindow->getHandleToWindow();
	//Are we in windowed mode, arggh opposite of full screen
	sd.Windowed = (BOOL)(!m_pWindow->isFullScreen());
	//Multisampling(antialsing) parameters for the swap chain - this has performance considerations - see remarks in docs
	//http://msdn.microsoft.com/en-us/library/bb173072%28v=vs.85%29.aspx - BMD
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
	//The description of the swap chain buffer
	//http://msdn.microsoft.com/en-us/library/bb173064%28v=vs.85%29.aspx - BMD
	//width & height of the buffer - this matches the size of the window - BMD
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
	//The data format of the buffer in the swap chain, 8bits used for Red, green, blue & alpha - unsigned int(UNIFORM) - BMD
	//http://msdn.microsoft.com/en-us/library/bb173059%28v=vs.85%29.aspx
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//Refresh rate of the buffer in the swap chain - BMD
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
	
	//NB. You should get use to seeing patterns like this when programming with D3D10 
	//where we use a description object which is then used in the creation of a D3D10 resource 
	//like swap chains. Also in a real application we would check to see if some of the above
	//options are support by the graphics hardware. -BMD

	//Create D3D10 Device and swap chain 
	//http://msdn.microsoft.com/en-us/library/bb205087%28v=vs.85%29.aspx - BMD
	if (FAILED(D3D10CreateDeviceAndSwapChain(NULL, //Pointer to IDXGIAdpater, this is a display adapater on the machine this can be NULL - BMD
		D3D10_DRIVER_TYPE_HARDWARE,//Type of Driver we have, it can be a hardware device, refrence(slow) or Software(not supported yet) - BMD
		NULL, //Handle to a module that implements a software rasterizer - BMD
		createDeviceFlags,//The device creation flags we used earlier on - BMD
		D3D10_SDK_VERSION,//The version of the SDK we are using this should D3D10 - BMD
		&sd,//The memory address of the swap chain description - BMD
		&m_pSwapChain, //The memory address of the swap chain pointer, if all goes well this will be intialised after this function call - BMD
		&m_pD3D10Device)))//the memory address of the D3D10 Device, if all goes well this will be initialised after this function call - BMD
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

			if(FAILED(m_pD3D10Device ->CreateBuffer( &bd, &InitData,&m_pVertexBuffer )))
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
			m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc);

			

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
			
			m_pD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			
			return true;
		}

		
	