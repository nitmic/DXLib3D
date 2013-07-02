#pragma once
#include <Windows.h>
#include <memory>
#include <DXAbsRenderingPolicy.h>

namespace DXLib{
	class DX3DRendering : public DXAbsRenderingPolicy{
	public:
		bool    Setup(HWND hwnd,unsigned long width, unsigned long height, int nCmdShow);

		std::shared_ptr<DXAbsRenderingEngine> getDefaultRenderingEngine();

		bool	beginScene();
		bool	endScene();
		bool	draw();

		DX3DRendering();
	private:
		struct Impl;
		std::shared_ptr<Impl> __impl__;
	};
};