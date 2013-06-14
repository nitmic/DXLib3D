/**
*@file	DXMesh.cpp
*@brief	メッシュ定義ファイル
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
	*@brief	ファイルからロード
	*@return
	*@param	filename	ファイル名
	*/
	HRESULT DXMesh::load(tString filename){
		HRESULT hr;
		LPD3DXBUFFER lpD3DBuf;
	
		// メッシュのロード
		RawMesh * pMesh;
		if(FAILED(hr = D3DXLoadMeshFromX(
			filename.c_str(),		//ファイル名
			D3DXMESH_SYSTEMMEM,
			getD3DDevice(),
			NULL,
			&lpD3DBuf,
			NULL,
			&m_NumOfMaterials,	//マテリアルの数
			&pMesh			//メッシュへのpp
		))){
			return hr;
		}
		m_pMesh = Mesh(pMesh, SAFE_RELEASE<RawMesh>);
		auto lpD3DBuf_shared = std::shared_ptr<ID3DXBuffer>(lpD3DBuf, SAFE_RELEASE<ID3DXBuffer>);

	
		// テクスチャリストのリソース確保
		auto ppTextures = new Texture[m_NumOfMaterials];
		if(ppTextures == nullptr){
			return E_OUTOFMEMORY;
		}
		m_ppTextures = Textures(ppTextures, std::default_delete<Texture[]>());
	
		// マテリアルリストのリソース確保
		auto pMtrls = new Material[m_NumOfMaterials];
		if(pMtrls == nullptr){
			return E_OUTOFMEMORY;
		}
		m_pMtrls = Materials(pMtrls, std::default_delete<Material[]>());
	
		// テクスチャ・マテリアルのロード
		D3DXMATERIAL *d3dxMatrs = (D3DXMATERIAL *)lpD3DBuf->GetBufferPointer();
		for(unsigned long u = 0; u < m_NumOfMaterials; ++u){
			// マテリアルのコピー
			m_pMtrls.get()[u] = d3dxMatrs[u].MatD3D;
		
			// テクスチャのロード
			m_ppTextures.get()[u] = DXLib::TextureLoad(d3dxMatrs[u].pTextureFilename);
		}

		if(FAILED(ComputeBoundingShere())){
			return E_FAIL;
		}

		return S_OK;
	}

	/**
	*@brief	当たり判定用の円の半径を計算
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
	*@brief	メッシュの描画
	*@return
	*@param	matWorld	ワールド行列
	*/
	void DXMesh::draw(D3DXMATRIX & matWorld){
		getD3DDevice()->SetTransform(D3DTS_WORLD, &matWorld);
		draw();
	}

	/**
	*@brief	メッシュの描画.ワールド座標を指定しない
	*/
	void DXMesh::draw(){
		for(unsigned long u = 0; u < m_NumOfMaterials; ++u){
			getD3DDevice()->SetMaterial(&m_pMtrls.get()[u]);
			getD3DDevice()->SetTexture(0, m_ppTextures.get()[u].get());

			m_pMesh->DrawSubset(u);
		}
	}

	/**
	*@brief	FVFの設定をする
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
	*@brief	頂点の設定
	*@return	S_OK : 成功
	*@param	pDecl 頂点宣言
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