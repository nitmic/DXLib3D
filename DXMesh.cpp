/**
*@file	DXMesh.cpp
*@brief	���b�V����`�t�@�C��
*/

#include "DXMesh.h"
#include <DXTexture.h>
#include <DXDeviceObject.h>
#include <Singleton.hpp>
//#include "DXMisc.h"
//#include "DXFilePack.h"


//========================================================================
//							DXMesh
//========================================================================
namespace DXLib{
	struct DXMesh::Impl{
		HRESULT		ComputeBoundingShere();
		
		Mesh		m_pMesh;		//!<	���b�V��
		unsigned long			m_NumOfMaterials;//!<	�}�e���A���̐�

		Materials		m_pMtrls;		//!<	�}�e���A�����X�g
		Textures		m_ppTextures;	//!<	�e�N�X�`�����X�g
		D3DXVECTOR3		m_vCenter;		//!<	���S���W
		float			m_fBoundingRadius;//!<	���a
		std::shared_ptr<DXAbsRenderingEngine> m_RenderingEngine;
	};

	/**
	*@brief	
	*/
	DXMesh::DXMesh(){
		__impl__ = std::make_shared<Impl>();
		__impl__->m_NumOfMaterials = 0;
	}



	/**
	*@brief	�t�@�C�����烍�[�h
	*@return
	*@param	filename	�t�@�C����
	*/
	HRESULT DXMesh::load(tString meshName, std::shared_ptr<DXAbsRenderingEngine> renderingEngine){
		HRESULT hr;
		LPD3DXBUFFER lpD3DBuf;
	
		// ���b�V���̃��[�h
		RawMesh * pMesh;
		if(FAILED(hr = D3DXLoadMeshFromX(
			meshName.c_str(),		//�t�@�C����
			D3DXMESH_SYSTEMMEM,
			DXDeviceObject::getD3DDevice(),
			NULL,
			&lpD3DBuf,
			NULL,
			&__impl__->m_NumOfMaterials,	//�}�e���A���̐�
			&pMesh			//���b�V���ւ�pp
		))){
			return hr;
		}
		__impl__->m_pMesh = Mesh(pMesh, SAFE_RELEASE<RawMesh>);
		auto lpD3DBuf_shared = std::shared_ptr<ID3DXBuffer>(lpD3DBuf, SAFE_RELEASE<ID3DXBuffer>);

	
		// �e�N�X�`�����X�g�̃��\�[�X�m��
		auto ppTextures = new Texture[__impl__->m_NumOfMaterials];
		if(ppTextures == nullptr){
			return E_OUTOFMEMORY;
		}
		__impl__->m_ppTextures = Textures(ppTextures, std::default_delete<Texture[]>());
	
		// �}�e���A�����X�g�̃��\�[�X�m��
		auto pMtrls = new Material[__impl__->m_NumOfMaterials];
		if(pMtrls == nullptr){
			return E_OUTOFMEMORY;
		}
		__impl__->m_pMtrls = Materials(pMtrls, std::default_delete<Material[]>());
	
		// �e�N�X�`���E�}�e���A���̃��[�h
		D3DXMATERIAL *d3dxMatrs = (D3DXMATERIAL *)lpD3DBuf->GetBufferPointer();
		for(unsigned long u = 0; u < __impl__->m_NumOfMaterials; ++u){
			// �}�e���A���̃R�s�[
			__impl__->m_pMtrls.get()[u] = d3dxMatrs[u].MatD3D;
		
			// �e�N�X�`���̃��[�h
			__impl__->m_ppTextures.get()[u] = DXLib::TextureLoad(d3dxMatrs[u].pTextureFilename);
		}

		if(FAILED(__impl__->ComputeBoundingShere())){
			return E_FAIL;
		}
		__impl__->m_RenderingEngine = renderingEngine;

		return S_OK;
	}

	/**
	*@brief	�����蔻��p�̉~�̔��a���v�Z
	*@return
	*/
	HRESULT DXMesh::Impl::ComputeBoundingShere(){
		IDirect3DVertexBuffer9 * pVB;
		void * pVerteces = nullptr;

		if(FAILED(m_pMesh->GetVertexBuffer(&pVB))){
			return E_FAIL;
		}
		auto pVB_shared = std::shared_ptr<IDirect3DVertexBuffer9>(pVB, SAFE_RELEASE<IDirect3DVertexBuffer9>);

		if(FAILED(pVB->Lock(
			0,
			0,
			&pVerteces,
			0
		))){
			return E_FAIL;
		}

		D3DXComputeBoundingSphere(
			(D3DXVECTOR3 *)pVerteces,
			m_pMesh->GetNumVertices(),
			D3DXGetFVFVertexSize(m_pMesh->GetFVF()),
			&m_vCenter,
			&m_fBoundingRadius
		);

		pVB->Unlock();

		return S_OK;
	}

	/**
	*@brief	���b�V���̕`��
	*@return
	*@param	matWorld	���[���h�s��
	*/
	void DXMesh::draw(D3DXMATRIX & matWorld){
		auto renderingEngine = __impl__->m_RenderingEngine ? __impl__->m_RenderingEngine  : DXDeviceObject::getRenderingEngine() ;
		//DXDeviceObject::getD3DDevice()->SetTransform(D3DTS_WORLD, &matWorld);
		renderingEngine->SetMatrix(matWorld);
		draw();
	}

	/**
	*@brief	���b�V���̕`��.���[���h���W���w�肵�Ȃ�
	*/
	void DXMesh::draw(){
		auto renderingEngine = __impl__->m_RenderingEngine ? __impl__->m_RenderingEngine  : DXDeviceObject::getRenderingEngine() ;
		renderingEngine->BeginPass(0);
		for(unsigned long u = 0; u < __impl__->m_NumOfMaterials; ++u){
			DXDeviceObject::getD3DDevice()->SetMaterial(&__impl__->m_pMtrls.get()[u]);
			DXDeviceObject::getD3DDevice()->SetTexture(0, __impl__->m_ppTextures.get()[u].get());

			__impl__->m_pMesh->DrawSubset(u);
		}
		renderingEngine->EndPass();
	}

	/**
	*@brief	FVF�̐ݒ������
	*@return	
	*@param	dwFVF	FVF
	*/
	HRESULT DXMesh::setFVF(DWORD dwFVF){
		RawMesh * pTempMesh = nullptr;
		if(FAILED(__impl__->m_pMesh->CloneMeshFVF(
			D3DXMESH_SYSTEMMEM,
			dwFVF,
			DXDeviceObject::getD3DDevice(),
			&pTempMesh
		))){
			return E_FAIL;
		}
		__impl__->m_pMesh = Mesh(pTempMesh, SAFE_RELEASE<RawMesh>);

		return S_OK;
	}

	/**
	*@brief	���_�̐ݒ�
	*@return	S_OK : ����
	*@param	pDecl ���_�錾
	*/
	HRESULT DXMesh::SetVertexDecl(D3DVERTEXELEMENT9 *pDecl){
		RawMesh * pMesh = nullptr;
		if(FAILED(__impl__->m_pMesh->CloneMesh(
			__impl__->m_pMesh->GetOptions(), 
			pDecl, 
			DXDeviceObject::getD3DDevice(), 
			&pMesh
		))){
			return E_FAIL;
		}
		__impl__->m_pMesh = Mesh(pMesh, SAFE_RELEASE<RawMesh>);

		return S_OK;
	}
};