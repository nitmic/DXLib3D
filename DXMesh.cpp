/**
*@file	DXMesh.cpp
*@brief	���b�V����`�t�@�C��
*/

#include "DXMesh.h"
//#include "DXMisc.h"
//#include "DXFilePack.h"


//========================================================================
//							DXMesh
//========================================================================
namespace DXLib{
	/**
	*@brief	
	*/
	DXMesh::DXMesh(){
		m_NumOfMaterials = 0;
	}

	/**
	*/
	DXMesh::~DXMesh(){}


	/**
	*@brief	�t�@�C�����烍�[�h
	*@return
	*@param	filename	�t�@�C����
	*/
	HRESULT DXMesh::load(tString filename){
		HRESULT hr;
		LPD3DXBUFFER lpD3DBuf;
	
		// ���b�V���̃��[�h
		RawMesh * pMesh;
		if(FAILED(hr = D3DXLoadMeshFromX(
			filename.c_str(),		//�t�@�C����
			D3DXMESH_SYSTEMMEM,
			getD3DDevice(),
			NULL,
			&lpD3DBuf,
			NULL,
			&m_NumOfMaterials,	//�}�e���A���̐�
			&pMesh			//���b�V���ւ�pp
		))){
			return hr;
		}
		m_pMesh = Mesh(pMesh, SAFE_RELEASE<RawMesh>);
		auto lpD3DBuf_shared = std::shared_ptr<ID3DXBuffer>(lpD3DBuf, SAFE_RELEASE<ID3DXBuffer>);

	
		// �e�N�X�`�����X�g�̃��\�[�X�m��
		auto ppTextures = new Texture[m_NumOfMaterials];
		if(ppTextures == nullptr){
			return E_OUTOFMEMORY;
		}
		m_ppTextures = Textures(ppTextures, std::default_delete<Texture[]>());
	
		// �}�e���A�����X�g�̃��\�[�X�m��
		auto pMtrls = new Material[m_NumOfMaterials];
		if(pMtrls == nullptr){
			return E_OUTOFMEMORY;
		}
		m_pMtrls = Materials(pMtrls, std::default_delete<Material[]>());
	
		// �e�N�X�`���E�}�e���A���̃��[�h
		D3DXMATERIAL *d3dxMatrs = (D3DXMATERIAL *)lpD3DBuf->GetBufferPointer();
		for(unsigned long u = 0; u < m_NumOfMaterials; ++u){
			// �}�e���A���̃R�s�[
			m_pMtrls.get()[u] = d3dxMatrs[u].MatD3D;
		
			// �e�N�X�`���̃��[�h
			m_ppTextures.get()[u] = DXLib::TextureLoad(d3dxMatrs[u].pTextureFilename);
		}

		if(FAILED(ComputeBoundingShere())){
			return E_FAIL;
		}

		return S_OK;
	}

	/**
	*@brief	�����蔻��p�̉~�̔��a���v�Z
	*@return
	*/
	HRESULT DXMesh::ComputeBoundingShere(){
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
		getD3DDevice()->SetTransform(D3DTS_WORLD, &matWorld);
		draw();
	}

	/**
	*@brief	���b�V���̕`��.���[���h���W���w�肵�Ȃ�
	*/
	void DXMesh::draw(){
		for(unsigned long u = 0; u < m_NumOfMaterials; ++u){
			getD3DDevice()->SetMaterial(&m_pMtrls.get()[u]);
			getD3DDevice()->SetTexture(0, m_ppTextures.get()[u].get());

			m_pMesh->DrawSubset(u);
		}
	}

	/**
	*@brief	FVF�̐ݒ������
	*@return	
	*@param	dwFVF	FVF
	*/
	HRESULT DXMesh::setFVF(DWORD dwFVF){
		RawMesh * pTempMesh = nullptr;
		if(FAILED(m_pMesh->CloneMeshFVF(
			D3DXMESH_SYSTEMMEM,
			dwFVF,
			getD3DDevice(),
			&pTempMesh
		))){
			return E_FAIL;
		}
		m_pMesh = Mesh(pTempMesh, SAFE_RELEASE<RawMesh>);

		return S_OK;
	}

	/**
	*@brief	���_�̐ݒ�
	*@return	S_OK : ����
	*@param	pDecl ���_�錾
	*/
	HRESULT DXMesh::SetVertexDecl(D3DVERTEXELEMENT9 *pDecl){
		RawMesh * pMesh = nullptr;
		if(FAILED(m_pMesh->CloneMesh(
			m_pMesh->GetOptions(), 
			pDecl, 
			getD3DDevice(), 
			&pMesh
		))){
			return E_FAIL;
		}
		m_pMesh = Mesh(pMesh, SAFE_RELEASE<RawMesh>);

		return S_OK;
	}
};