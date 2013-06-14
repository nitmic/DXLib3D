/**
*@file	DXMesh.h
*@brief	���b�V���w�b�_
*/
#pragma once
#include <DXDeviceObject.h>
#include <DXTexture.h>

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
		Mesh		m_pMesh;		//!<	���b�V��
		unsigned long			m_NumOfMaterials;//!<	�}�e���A���̐�

		Materials		m_pMtrls;		//!<	�}�e���A�����X�g
		Textures		m_ppTextures;	//!<	�e�N�X�`�����X�g
		D3DXVECTOR3		m_vCenter;		//!<	���S���W
		float			m_fBoundingRadius;//!<	���a

	};

};