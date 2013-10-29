/**
*@file	DXMesh.h
*@brief	メッシュヘッダ
*/
#pragma once
#include <memory>
#include <DXPrimitive.h>
#include <tString.hpp>
#include <DXDeviceObject.h>

//class DXFilePack;
/**
*@class	DXMesh
*@brief	メッシュ
*/
namespace DXLib{
	class DXAbsRenderingEngine;

	typedef ID3DXMesh RawMesh;
	typedef std::shared_ptr<RawMesh> Mesh;
	typedef D3DMATERIAL9 Material;
	typedef std::shared_ptr<Material> Materials;

	class DXMesh{
	public:
		HRESULT		load(TUL::tString filename, std::shared_ptr<DXAbsRenderingEngine> renderingEngine=nullptr);
		DXMesh();

		void		draw(D3DXMATRIX &matWorld);
		void		draw();

		HRESULT		setFVF(DWORD dwFVF);
		HRESULT		SetVertexDecl(D3DVERTEXELEMENT9 *pDecl);
	private:
		struct Impl;
		std::shared_ptr<Impl> __impl__;
	};
	
};