/**
*@file	DXMesh.cpp
*@brief	メッシュ定義ファイル
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
		
		Mesh		m_pMesh;		//!<	メッシュ
		unsigned long			m_NumOfMaterials;//!<	マテリアルの数

		Materials		m_pMtrls;		//!<	マテリアルリスト
		Textures		m_ppTextures;	//!<	テクスチャリスト
		D3DXVECTOR3		m_vCenter;		//!<	中心座標
		float			m_fBoundingRadius;//!<	半径
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
	*@brief	ファイルからロード
	*@return
	*@param	filename	ファイル名
	*/
	HRESULT DXMesh::load(tString meshName, std::shared_ptr<DXAbsRenderingEngine> renderingEngine){
		HRESULT hr;
		LPD3DXBUFFER lpD3DBuf;
	
		// メッシュのロード
		RawMesh * pMesh;
		if(FAILED(hr = D3DXLoadMeshFromX(
			meshName.c_str(),		//ファイル名
			D3DXMESH_SYSTEMMEM,
			DXDeviceObject::getD3DDevice(),
			NULL,
			&lpD3DBuf,
			NULL,
			&__impl__->m_NumOfMaterials,	//マテリアルの数
			&pMesh			//メッシュへのpp
		))){
			return hr;
		}
		__impl__->m_pMesh = Mesh(pMesh, SAFE_RELEASE<RawMesh>);
		auto lpD3DBuf_shared = std::shared_ptr<ID3DXBuffer>(lpD3DBuf, SAFE_RELEASE<ID3DXBuffer>);

	
		// テクスチャリストのリソース確保
		auto ppTextures = new Texture[__impl__->m_NumOfMaterials];
		if(ppTextures == nullptr){
			return E_OUTOFMEMORY;
		}
		__impl__->m_ppTextures = Textures(ppTextures, std::default_delete<Texture[]>());
	
		// マテリアルリストのリソース確保
		auto pMtrls = new Material[__impl__->m_NumOfMaterials];
		if(pMtrls == nullptr){
			return E_OUTOFMEMORY;
		}
		__impl__->m_pMtrls = Materials(pMtrls, std::default_delete<Material[]>());
	
		// テクスチャ・マテリアルのロード
		D3DXMATERIAL *d3dxMatrs = (D3DXMATERIAL *)lpD3DBuf->GetBufferPointer();
		for(unsigned long u = 0; u < __impl__->m_NumOfMaterials; ++u){
			// マテリアルのコピー
			__impl__->m_pMtrls.get()[u] = d3dxMatrs[u].MatD3D;
		
			// テクスチャのロード
			__impl__->m_ppTextures.get()[u] = DXLib::TextureLoad(d3dxMatrs[u].pTextureFilename);
		}

		if(FAILED(__impl__->ComputeBoundingShere())){
			return E_FAIL;
		}
		__impl__->m_RenderingEngine = renderingEngine;

		return S_OK;
	}

	/**
	*@brief	当たり判定用の円の半径を計算
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
	*@brief	メッシュの描画
	*@return
	*@param	matWorld	ワールド行列
	*/
	void DXMesh::draw(D3DXMATRIX & matWorld){
		auto renderingEngine = __impl__->m_RenderingEngine ? __impl__->m_RenderingEngine  : DXDeviceObject::getRenderingEngine() ;
		//DXDeviceObject::getD3DDevice()->SetTransform(D3DTS_WORLD, &matWorld);
		renderingEngine->SetMatrix(matWorld);
		draw();
	}

	/**
	*@brief	メッシュの描画.ワールド座標を指定しない
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
	*@brief	FVFの設定をする
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
	*@brief	頂点の設定
	*@return	S_OK : 成功
	*@param	pDecl 頂点宣言
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