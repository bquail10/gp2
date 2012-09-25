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
	m_pDepthStencilView=NULL;
	m_pDepthStencilTexture=NULL;
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

	if(m_pDepthStencilTexture)
		m_pDepthStencilTexture->Release();

	if(m_pDepthStencilView)
		m_pDepthStencilView->Release();
	
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
			float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; //this sets up a float array for the colour(red, green, blue, alpha), the max value can be 1.0
			m_pD3D10Device->ClearRenderTargetView( m_pRenderTargetView, ClearColor); // takes the colour from the above line and clears the render target to that colour
			m_pD3D10Device->ClearDepthStencilView(m_pDepthStencilView, D3D10_CLEAR_DEPTH,1.0f,0);

			m_pViewMatrixVariable->SetMatrix((float*)m_matView);

			m_pWorldMatrixVariable->SetMatrix((float*)m_matWorld);

			D3D10_TECHNIQUE_DESC techDesc; //this retrieves a description of the technique
			m_pTechnique->GetDesc( &techDesc); //this will allow us info about it such as number of passed contained in the technique
			for(UINT p = 0; p <techDesc.Passes; ++p) //this is a loopthat retrieves the pass we are currently on
			{
				m_pTechnique->GetPassByIndex(p)->Apply(0); //this will apply all the parameters of the pass such as render states and shader states
				m_pD3D10Device->Draw(3,0); //the draw has 2 parameter, the 1st is the number of vertice3s we want to draw, 2nd is the start location in the buffer
			}

			m_pSwapChain->Present(0, 0); //it flips the swap chain, so it goes from the back buffer to the front buffer and the rendered scene will appear
		}

		void CGameApplication::update() //function updates the game state, AI, input devices and physics
		{
			D3DXMatrixScaling(&m_matScale,m_vecScale.x,m_vecScale.y,m_vecScale.z);

			D3DXMatrixRotationYawPitchRoll(&m_matRotation,m_vecRotation.y, 
								m_vecRotation.x,m_vecRotation.z);
			
			D3DXMatrixTranslation(&m_matTranslation,m_vecPosition.x,
				m_vecPosition.y,m_vecPosition.z);

			D3DXMatrixMultiply(&m_matWorld,&m_matScale,&m_matRotation);
			D3DXMatrixMultiply(&m_matWorld,&m_matWorld,&m_matTranslation);

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
			if(FAILED(m_pSwapChain->GetBuffer(0, //this retrieves the back buffer using the function of the swap chain, 1st parameter an index of the buffer inside the swap chain, 0 retrives the buffer. 
					__uuidof(ID3D10Texture2D), //this is the id of the type of interface we are retrieving from the swap chain
					(void**)&pBackBuffer)))//this is a pointer to an address of the buffer, it is a void because the function can take any type depending on the 2nd parameter
			{ 
				return false;
			}

			if(FAILED(m_pD3D10Device->CreateRenderTargetView //this function creates the render target view
				(pBackBuffer, // this is the pointer to the resources 
				NULL, //this is a pointer to a scructure which defines options for accessing parts of the render target
				&m_pRenderTargetView))) //this is a pointer to the address of the render target view
			{
				pBackBuffer->Release(); //this then releases the back buffer if returns fails
				return false;
			}
			pBackBuffer->Release(); //this return the buffer

			D3D10_TEXTURE2D_DESC descDepth;
			descDepth.Width = width;
			descDepth.Height = height;
			descDepth.MipLevels = 1;
			descDepth.ArraySize = 1;
			descDepth.Format = DXGI_FORMAT_D32_FLOAT;
			descDepth.SampleDesc.Count = 1;
			descDepth.SampleDesc.Quality = 0;
			descDepth.Usage = D3D10_USAGE_DEFAULT;
			descDepth.BindFlags = D3D10_BIND_DEPTH_STENCIL;
			descDepth.CPUAccessFlags = 0;
			descDepth.MiscFlags = 0;

			if(FAILED(m_pD3D10Device->CreateTexture2D
				(&descDepth,
				NULL,
				&m_pDepthStencilTexture)))
			{
				return false;
			}

			D3D10_DEPTH_STENCIL_VIEW_DESC descDSV;
			descDSV.Format = descDepth.Format;
			descDSV.ViewDimension = D3D10_DSV_DIMENSION_TEXTURE2D;
			descDSV.Texture2D.MipSlice = 0;

			if(FAILED(m_pD3D10Device->CreateDepthStencilView(
				m_pDepthStencilTexture, &descDSV, &m_pDepthStencilView)))
			{
				return false;
			}

			m_pD3D10Device->OMSetRenderTargets //the function binds an array of Render Targets to the Output Merger
				(1, //the value specifies tthe amount of render targets being bound to the pipeline
				&m_pRenderTargetView, //this is a pointer to an arrat of render targets
				m_pDepthStencilView); //this is a pointer to a depth stencil, it holds depth info of the scene

			D3D10_VIEWPORT vp; //setups instance D3D10_VIEWPORT has same height and width of the window
			vp.Width = width;
			vp.Height = height;
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			m_pD3D10Device->RSSetViewports( 1, &vp); // this calls the RSSetViewports function to set the view port which is bound to the pipeline
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
			DWORD dwShaderFlags =  D3D10_SHADER_ENABLE_STRICTNESS; //this puts our effect loading in debug, will genereate more info if needing to debug shader
#if defined(DEBUG)|| defined(_DEBUG)
			dwShaderFlags |= D3D10_SHADER_DEBUG;
#endif

			if(FAILED(D3DX10CreateEffectFromFile //this function will load the effect file
				(TEXT("Transform.fx"), //this is the name of the file of the efffect 
				NULL,NULL, "fx_4_0", //"fx_4_0" is the shader profile we are using, this is equivalent of shader model 4 
				dwShaderFlags, 0, //dwShaderFlags this allows us to collect debug info about the shader
				m_pD3D10Device, //this is a pointer to a valid device which will use this effect 
				NULL, NULL, 
				&m_pEffect, //this is a pointer to the memory address of an effect object
				NULL, NULL )))
			{
				MessageBox( NULL, TEXT("The FX file cannot be located. Please run this executable from the derectory that contains the FX file."), //this displays if we don't have a valid pointer
					TEXT("Error"),
					MB_OK);
				return false;
			}

			m_pTechnique=m_pEffect->GetTechniqueByName("Render");  //this calls GetTechniqueByName and passes in a string which we are looking for in the effect

			D3D10_BUFFER_DESC bd; //this is the buffer description structure
			bd.Usage = D3D10_USAGE_DEFAULT; //this is how buffer is read/written to, DEFAULT states that the resoures read/written by the gpu
			bd.ByteWidth = sizeof( Vertex ) * 3; //this isthe size of the buffer, in this case it hold 3 vertices
			bd.BindFlags = D3D10_BIND_VERTEX_BUFFER; //this is the type of buffer we are creating
			bd.CPUAccessFlags = 0; //this is to specify that the buffer can be read/written by the CPU
			bd.MiscFlags = 0; //this is used for an additional options, 0 mean no additional options
			
			Vertex vertices[] =
			{
				D3DXVECTOR3( 0.0f, 0.5f, 0.5f), //position of the first vertice
				D3DXVECTOR3( 0.5f, -0.5f, 0.5f), //position of the second vertice
				D3DXVECTOR3( -0.5f, -0.5f, 0.5f), //position of the third vertice
			};

			D3D10_SUBRESOURCE_DATA InitData; //this initailizes D3D10_SUBRESOURCE_DATA structure
			InitData.pSysMem = vertices; //this makes pSysMem equal 

			if(FAILED(m_pD3D10Device ->CreateBuffer( //function
				&bd, //this is a pointer to a buffer description 
				&InitData, //this is a pointer to a buffer
				&m_pVertexBuffer))) //this is a memory address of a pointer to a buffer
			{
				return false;
			}

			D3D10_INPUT_ELEMENT_DESC layout[] = //this is an array
			{
				{
						"POSITION", //this is a string which specifies the smantic that this element is bound too
						0, //this is the index of the semantic, it is used to bind to a vertex
						DXGI_FORMAT_R32G32B32_FLOAT, //This is the format of the data, there are 3 components with 32 bits for each of them
						0, 
						0, //this is the starting offset of the element, it will increase for subsequent elements in the array
						D3D10_INPUT_PER_VERTEX_DATA, 0
				}
			};

			UINT numElements = sizeof( layout )/sizeof(D3D10_INPUT_ELEMENT_DESC); //this calculates and then numbers elements in the input array
			D3D10_PASS_DESC PassDesc; //this retrieves the pass description from the technique
			m_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc); //this is needed because we need to bind the layout of the vertex to a vertex contained in effect 

			if(FAILED(m_pD3D10Device->CreateInputLayout( //CreateInputLayout will create our input layout
				layout, //this is an array of input element descriptions
				numElements, //this is the number elements in the input elements array
				PassDesc.pIAInputSignature, //this is a pointer to the compiled shader code, this is retrieved from pass using input signature variable of the pass description
				PassDesc.IAInputSignatureSize, //this is the size of the shader code, this is retrieved from the pass description
				&m_pVertexLayout))) //this is a pointer to a memory address of the input layout object
			{
				return false;
			}

			m_pD3D10Device->IASetInputLayout(m_pVertexLayout); //this tells the input assembler(IA) of the input layout

			UINT stride = sizeof(Vertex); //this assigns a value to hold stride(size) of a vertex
			UINT offset = 0; //this is a vlue to hold the offset which is where vertices start in the vertex buffer
			m_pD3D10Device->IASetVertexBuffers( //this function will be used to bind one or many buffers to the IA to use
				0, //this is the input slot to bind, 0 indicates the first slot, we can as bind a buffer to other slots so we can change buffer as we render 
				1, //this is the amount of buffer we are binding
				&m_pVertexBuffer, //this a pointer to the memory address of the buffer
				&stride, //this is an array of the strides
				&offset ); //this is an array of the offsets.
			
			m_pD3D10Device->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //this tells the IA what type of primitive we are going to draw with
			
			D3DXVECTOR3 cameraPos(0.0f,0.0,-10.0f);
			D3DXVECTOR3 cameraLook(0.0f,0.0f,1.0f);
			D3DXVECTOR3 cameraUp(0.0f,1.0f,0.0f);
			D3DXMatrixLookAtLH(&m_matView,&cameraPos,
				&cameraLook,&cameraUp);

			D3D10_VIEWPORT vp;
			UINT numViewPorts=1;
			m_pD3D10Device->RSGetViewports(&numViewPorts,&vp);

			D3DXMatrixPerspectiveFovLH(&m_matProjection,(float)D3DX_PI * 0.25f,
				vp.Width / (FLOAT)vp.Height, 0.1f,100.0f);

			m_pViewMatrixVariable=
				m_pEffect->GetVariableByName("matView")->AsMatrix();
			m_pProjectionMatrixVariable=
				m_pEffect->GetVariableByName("matProjection")->AsMatrix();

			m_pProjectionMatrixVariable->SetMatrix((float*)m_matProjection);

			m_vecPosition=D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_vecScale=D3DXVECTOR3(1.0f,1.0f,1.0f);
			m_vecRotation=D3DXVECTOR3(0.0f,0.0f,0.0f);
			m_pWorldMatrixVariable=
				m_pEffect->GetVariableByName("matWorld")->AsMatrix();
				
			return true;
		}

		
	