/**
*@file	DXMesh.h
*@brief	メッシュヘッダ
*/
#pragma once
#include <DXDeviceObject.h>
#include <DXTexture.h>

//class DXFilePack;

/**
*@class	DXMesh
*@brief	メッシュ
*/
namespace DXLib{
	typedef ID3DXMesh RawMesh;
	typedef std::shared_ptr<RawMesh> Mesh;
	typedef D3DMATERIAL9 Material;
	typedef std::shared_ptr<Material> Materials;
	class DXMesh : public DXDeviceObject{
	public:
		HRESULT		load(tString filename);

		void		draw(D3DXMATRIX &matWorld);
		void		draw();

		HRESULT		setFVF(DWORD dwFVF);
		HRESULT		SetVertexDecl(D3DVERTEXELEMENT9 *pDecl);

		DXMesh();
		~DXMesh();
	private:
		HRESULT		ComputeBoundingShere();
	private:
		Mesh		m_pMesh;		//!<	メッシュ
		unsigned long			m_NumOfMaterials;//!<	マテリアルの数

		Materials		m_pMtrls;		//!<	マテリアルリスト
		Textures		m_ppTextures;	//!<	テクスチャリスト
		D3DXVECTOR3		m_vCenter;		//!<	中心座標
		float			m_fBoundingRadius;//!<	半径

	};

};