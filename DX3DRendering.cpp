#include "DX3DRendering.h"
#include <DXManager.h>
#include <Singleton.hpp>
#include "DXLambert.h"
#include <DXRenderingEngineStorage.h>

namespace DXLib{
	struct DX3DRendering::Impl{
		std::shared_ptr<DXLambert> pEffect;
	};
	
	DX3DRendering::DX3DRendering(){
		__impl__ = std::make_shared<Impl>();
		__impl__->pEffect = GetSingleton<DXRenderingEngineStorage>()->getEngine<DXLambert>();
	}

	bool DX3DRendering::Setup(
		HWND hwnd,
		unsigned long width,
		unsigned long height,
		int nCmdShow
	){
		if(!GetSingleton<DXManager>()->Setup(hwnd, width, height, nCmdShow)){
			return false;
		}
		return __impl__->pEffect->setup();
	}

	bool DX3DRendering::beginScene(){
		GetSingleton<DXManager>()->beginScene();
		__impl__->pEffect->Begin();
		return true;
	}

	bool DX3DRendering::endScene(){
		__impl__->pEffect->End();
		return GetSingleton<DXManager>()->endScene();
	}
	std::shared_ptr<DXAbsRenderingEngine> DX3DRendering::getDefaultRenderingEngine(){
		return __impl__->pEffect;
	}
};
