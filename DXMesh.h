/**
*@file	DXMesh.h
*@brief	���b�V���w�b�_
*/
#pragma once
#include <memory>
#include <DXPrimitive.h>
#include <tString.h>

//class DXFilePack;
/**
*@class	DXMesh
*@brief	���b�V��
*/
namespace DXLib{
	typedef ID3DXMesh RawMesh;
	typedef std::shared_ptr<RawMesh> Mesh;
	typedef D3DMATERIAL9 Material;
	typedef std::shared_ptr<Material> Materials;

	class DXMesh{
	public:
		HRESULT		load(tString filename);

		void		draw(D3DXMATRIX &matWorld);
		void		draw();

		HRESULT		setFVF(DWORD dwFVF);
		HRESULT		SetVertexDecl(D3DVERTEXELEMENT9 *pDecl);

		DXMesh();
		~DXMesh();
	private:
		struct Impl;
		std::shared_ptr<Impl> __impl__;
	};
};