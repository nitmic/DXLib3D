/**
*@file	DXCamera.cpp
*@brief	カメラの定義ファイル
*@date	2010/3/2
*/
#include "DXCamera.h"
#include <DXDeviceObject.h>
#include <Shape.hpp>

//========================================================================
//								file scope
//=======================================================================-

namespace DXLib{
	/*
	*@class	DXICamera::Matrix
	*@brief カメラに関する行列情報
	*/
	struct DXICamera::Matrix{
		D3DXMATRIX		matProj;		//!<	プロジェクション行列
		D3DXMATRIX		matView;		//!<	ビュー行列
	};

	/*
	*@class	DXICamera::ImplParameter
	*@brief カメラに関する情報
	*/
	struct DXICamera::Parameter{
		D3DXVECTOR3		position;		//!<	位置
		D3DXVECTOR3		lookAt;			//!<	視点
		D3DXVECTOR3		tilt;			//!<	傾き

		float			nearPlane;			//!<	ニアプレーン
		float			farPlane;			//!<	ファープレーン
		float			fieldOfView;		//!<	視野
	};
};

namespace DXLib{
namespace{
	D3DXVECTOR3 DefaultPosition(){
		return D3DXVECTOR3(0.0f, 0.0f, 5.0f);
	}
	
	D3DXVECTOR3 DefaultLookAt(){
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	
	D3DXVECTOR3 DefaultTilt(){
		return D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	}

	float DefaultFieldOfView(){
		return 51.92751f;
	}

	float DefaultNearPlane(){
		return 0.1f;
	}

	float DefaultFarPlane(){
		return 2000.0f;
	}
	
	/**
	*@brief	パラメータをデフォルトに戻す
	*@return
	*/
	void ParamReset(DXICamera::Parameter * pParam){
		pParam->position	= DefaultPosition();
		pParam->lookAt		= DefaultLookAt();
		pParam->tilt		= DefaultTilt();
		pParam->fieldOfView	= DefaultFieldOfView();
		pParam->nearPlane   = DefaultNearPlane();
		pParam->farPlane	= DefaultFarPlane();
	}

	/**
	*@brief	スクリーン座標の計算
	*@return	スクリーン座標
	*@param	vWorldPos	ワールド座標
	*@param	viewport	ビューポート
	*@param	matWorld	ワールド行列
	*/
	D3DXVECTOR2	ComputeScreenCoodinate(
		DXICamera::Matrix & cache,
		D3DXVECTOR3 &worldPos,
		D3DVIEWPORT9 &viewport,
		D3DXMATRIX &matWorld
	){
		D3DXVECTOR2 vScreenCoodinate;

		const float x = worldPos.x;
		const float y = worldPos.y;
		const float z = worldPos.z;

		const D3DXMATRIX mat = cache.matView * cache.matProj;

		const float w = 1.0f / (mat._14 * x + mat._24 * y + mat._34 * z + mat._44);
		const float width = viewport.Width / 2.0f;
		const float height = viewport.Height / 2.0f;

		const float sx = float(width + (mat._11 * x + mat._21 * y + mat._31 * z + mat._41) * width * w);
		const float sy = float(height - (mat._12 * x + mat._22 * y + mat._32 * z + mat._42) * height * w);

		vScreenCoodinate.x = sx + viewport.X;
		vScreenCoodinate.y = sy + viewport.Y;

		return vScreenCoodinate;
	}
	
	/**
	*@brief		カメラの設定
	*@param	mat		キャッシュ
	*@param	param	パラメータ
	*@param	device	DirectX 描画 Device
	*/
	void transform(
		DXICamera::Matrix * mat,
		const DXICamera::Parameter * param,
		IDirect3DDevice9 * device
	){
		D3DXMatrixLookAtLH(
			&mat->matView,
			&param->position,
			&param->lookAt,
			&param->tilt
		);

		D3DVIEWPORT9 viewport;
		device->GetViewport(&viewport);

		const float aspect = (float)viewport.Width / (float)viewport.Height;

		D3DXMatrixPerspectiveFovLH(
			&mat->matProj,
			D3DXToRadian(param->fieldOfView),
			aspect,
			param->nearPlane,
			param->farPlane
		);

		device->SetTransform(D3DTS_VIEW, &mat->matView);
		device->SetTransform(D3DTS_PROJECTION, &mat->matProj);
	}
}};

//========================================================================
//								DXCamera
//=======================================================================-

namespace DXLib{
	struct DXCamera::Impl{
		DXICamera::Matrix		m_Mat;
		DXICamera::Parameter	m_Param;
	};

	/**
	*@brief	
	*/
	DXCamera::DXCamera(){
		__impl__ = std::make_shared<Impl>();
		ParamReset(&__impl__->m_Param);
	}

	/**
	*/
	DXCamera::~DXCamera(){}


	/**
	*@brief	カメラの設定
	*@return	
	*/
	void DXCamera::transform(){
		DXLib::transform(&__impl__->m_Mat, &__impl__->m_Param, DXDeviceObject::getD3DDevice());
	}
};


//===========================================================================
//							DXEditCamera
//===========================================================================

namespace DXLib{
	struct DXEditCamera::Impl{
		void		MoveCursor(TUL::Point<float> &pt);
		void		MoveMouseWheel(long zDelta);


		bool		m_RButtonPressed;		//!<	右クリックされているかどうか
		bool		m_LButtonPressed;		//!<	左クリックされているかどうか
		TUL::Point<float,2>	m_RClickPoint;			//!<	右クリックの位置
		TUL::Point<float,2>	m_LClickPoint;			//!<	左クリックの位置
		TUL::Point<float,2>	m_CursorPositon;		//!<	カーソルの位置

		float		m_Theta;
		float		m_Phi;
		DXLib::DXICamera::Matrix		m_Mat;
		DXLib::DXICamera::Parameter		m_Param;
	};


	/**
	*@brief	
	*/
	DXEditCamera::DXEditCamera(){
		__impl__ = std::make_shared<Impl>();
		__impl__->m_LButtonPressed = false;
		__impl__->m_RButtonPressed = false;
	
		ParamReset(&__impl__->m_Param);
	}

	/**
	*/
	DXEditCamera::~DXEditCamera(){}

	/**
	*@brief	カメラのプロシージャ
	*@return	
	*/
	BOOL CALLBACK DXEditCamera::EditCameraProc(HWND hWnd, UINT msg, WPARAM w, LPARAM l){
		long zDelta;
		TUL::Point<float, 2> pt;
		switch(msg){
		case WM_MOUSEMOVE:
			pt['x'] = LOWORD(l);
			pt['y'] = HIWORD(l);
			__impl__->MoveCursor(pt);
			__impl__->m_CursorPositon['x'] = pt['x'];
			__impl__->m_CursorPositon['y'] = pt['y'];
			break;
		case WM_MOUSEWHEEL:
			zDelta = GET_WHEEL_DELTA_WPARAM(w);
			__impl__->MoveMouseWheel(zDelta);
			break;
		case WM_LBUTTONDOWN:
			__impl__->m_LButtonPressed = true;
			break;
		case WM_RBUTTONDOWN:
			__impl__->m_RButtonPressed = true;
			break;
		case WM_LBUTTONUP:
			__impl__->m_LButtonPressed = false;
			break;
		case WM_RBUTTONUP:
			__impl__->m_RButtonPressed = false;
			break;
		default:
			break;
		}
		return 0;
	}

	/**
	*@brief	カメラの設定
	*@return	
	*/
	void DXEditCamera::transform(){
		DXLib::transform(&__impl__->m_Mat, &__impl__->m_Param, DXDeviceObject::getD3DDevice());
	}

	/**
	*@brief	マウスのホイールでの移動
	*@return	
	*@param	zDelta
	*/
	void DXEditCamera::Impl::MoveMouseWheel(long zDelta){
		//方向ベクトル
		D3DXVECTOR3 vDirection = m_Param.lookAt - m_Param.position;
		D3DXVec3Normalize(&vDirection, &vDirection);
		vDirection *= 0.01f;

		//新しい位置
		const D3DXVECTOR3 newPosition = m_Param.position + vDirection * (float)zDelta;
		//新しい方向
		const D3DXVECTOR3 vNewDirection = m_Param.lookAt - newPosition;

		//角度
		const float fDeg = 
			D3DXVec3Dot(&vDirection, &vNewDirection)
			 / (D3DXVec3Length(&vDirection)
			 * D3DXVec3Length(&vNewDirection));

		if(fDeg < 1.57f){
			m_Param.position = newPosition;
		}
	}

	/**
	*@briefカーソルの移動
	*@return	
	*@param	pt	位置
	*/
	void DXEditCamera::Impl::MoveCursor(TUL::Point<float> &pt){
		//カーソルの位置の差
		TUL::Vector<float, 2> ptDiff;
		ptDiff['x'] = pt['x'] - m_CursorPositon['x'];
		ptDiff['y'] = pt['y'] - m_CursorPositon['y'];

		if(m_LButtonPressed){
			//左クリックされている.平行移動する
	
			const D3DXVECTOR3 vDirection = m_Param.lookAt - m_Param.position;

			D3DXVECTOR3 vCrossDirectionAndTilt;
			D3DXVec3Cross(&vCrossDirectionAndTilt, &vDirection, &m_Param.tilt);

			D3DXVec3Normalize(&vCrossDirectionAndTilt, &vCrossDirectionAndTilt);

			vCrossDirectionAndTilt *= 0.03f;

			//ｘ成分
			m_Param.position += vCrossDirectionAndTilt * (float)ptDiff['x'];
			m_Param.lookAt += vCrossDirectionAndTilt * (float)ptDiff['x'];

			//y成分
			D3DXVECTOR3 vCrossDirectionAndCross;

			D3DXVec3Cross(&vCrossDirectionAndCross, &vDirection, &vCrossDirectionAndTilt);
			D3DXVec3Normalize(&vCrossDirectionAndCross, &vCrossDirectionAndCross);

			vCrossDirectionAndCross *= -0.03f;

			m_Param.position += vCrossDirectionAndCross * (float)ptDiff['y'];
			m_Param.lookAt += vCrossDirectionAndCross * (float)ptDiff['y'];

		}else if(m_RButtonPressed){
			//右クリックされている

			//回転させる
			const D3DXVECTOR3 vPolarLookAt = m_Param.position - m_Param.lookAt;

			m_Theta += ptDiff['x'] / 100.0f;
			m_Phi += ptDiff['y'] / 100.0f;
			
			if(m_Phi > D3DXToRadian(90.0f)){
				m_Phi = D3DXToRadian(89.0f);
			}else if(m_Phi < D3DXToRadian(-90.0f)){
				m_Phi = D3DXToRadian(-89.0f);
			}
			//長さ
			const float fLength = D3DXVec3Length(&vPolarLookAt);

			const D3DXVECTOR3 z(0.0f, 0.0f, -1.0f);

			D3DXMATRIX mat;
			D3DXMatrixRotationYawPitchRoll(&mat, m_Theta, m_Phi, 0.0f);

			D3DXVECTOR3 vOut;
			D3DXVec3TransformCoord(&vOut, &z, &mat);
			D3DXVec3Normalize(&vOut, &vOut);

			m_Param.position = m_Param.lookAt + vOut * fLength;
		}
	}

};
