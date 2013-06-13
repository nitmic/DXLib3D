/**
*@file	DXCamera.cpp
*@brief	カメラの定義ファイル
*@date	2010/3/2
*/
#include "DXCamera.h"


/**
*@brief	スクリーン座標の計算
*@return	スクリーン座標
*@param	vWorldPos	ワールド座標
*@param	viewport	ビューポート
*@param	matWorld	ワールド行列
*/
D3DXVECTOR2 DXICamera::ComputeScreenCoodinate(D3DXVECTOR3 &vWorldPos, D3DVIEWPORT9 &viewport, D3DXMATRIX &matWorld){
	D3DXVECTOR2 vScreenCoodinate;

	D3DXMATRIX mat;

	float x = vWorldPos.x;
	float y = vWorldPos.y;
	float z = vWorldPos.z;

	mat = m_matView * m_matProj;

	float w = 1.0f / (mat._14 * x + mat._24 * y + mat._34 * z + mat._44);
	float width = viewport.Width / 2.0f;
	float height = viewport.Height / 2.0f;

	long sx = long(width + (mat._11 * x + mat._21 * y + mat._31 * z + mat._41) * width * w);
	long sy = long(height - (mat._12 * x + mat._22 * y + mat._32 * z + mat._42) * height * w);

	vScreenCoodinate.x = sx + viewport.X;
	vScreenCoodinate.y = sy + viewport.Y;

	return vScreenCoodinate;
}




//========================================================================
//								DXCamera
//=======================================================================-

/**
*@brief	
*/
DXCamera::DXCamera(){
	m_Tilt				= D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	m_FieldOfView	= 51.92751f;
	m_NearPlane   = 0.1f;
	m_FarPlane	= 2000.0f;

	reset();
}

/**
*/
DXCamera::~DXCamera(){}

/**
*@brief	パラメータをデフォルトに戻す
*@return
*/
void DXCamera::reset(){
	m_Position	= m_DefaultPosition;
	m_LookAt	= m_DefaultLookAt;
}

/**
*@brief	カメラの設定
*@return	
*/
void DXCamera::transform(){
	D3DXMatrixLookAtLH(
		&m_matView,
		&m_Position,
		&m_LookAt,
		&m_Tilt
	);

	D3DVIEWPORT9 viewport;
	getD3DDevice()->GetViewport(&viewport);

	float aspect = (float)viewport.Width / (float)viewport.Height;

	D3DXMatrixPerspectiveFovLH(
		&m_matProj,
		D3DXToRadian(m_FieldOfView),
		aspect,
		m_NearPlane,
		m_FarPlane
	);

	getD3DDevice()->SetTransform(D3DTS_VIEW, &m_matView);
	getD3DDevice()->SetTransform(D3DTS_PROJECTION, &m_matProj);
}


//===========================================================================
//							DXEditCamera
//===========================================================================

/**
*@brief	
*/
DXEditCamera::DXEditCamera() : m_RClickPoint(0 ,0), m_LClickPoint(0 ,0), m_CursorPositon(0,0){
	m_LButtonPressed = false;
	m_RButtonPressed = false;
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
	Point<int, 2> pt;
	switch(msg)
	{
	case WM_MOUSEMOVE:
		pt['x'] = LOWORD(l);
		pt['y'] = HIWORD(l);
		MoveCursor(pt);
		m_CursorPositon = pt;
		break;
	case WM_MOUSEWHEEL:
		zDelta = GET_WHEEL_DELTA_WPARAM(w);
		MoveMouseWheel(zDelta);
		break;
	case WM_LBUTTONDOWN:
		m_RClickPoint['x'] = LOWORD(l);
		m_RClickPoint['y'] = HIWORD(l);
		m_LButtonPressed = true;
		break;
	case WM_RBUTTONDOWN:
		m_LClickPoint['x'] = LOWORD(l);
		m_LClickPoint['y'] = HIWORD(l);
		m_RButtonPressed = true;
		break;
	case WM_LBUTTONUP:
		m_LButtonPressed = false;
		break;
	case WM_RBUTTONUP:
		m_RButtonPressed = false;
		break;
	default:
		break;
	}
	return 0;
}

/**
*@brief	マウスのホイールでの移動
*@return	
*@param	zDelta
*/
void DXEditCamera::MoveMouseWheel(long zDelta){
	//方向ベクトル
	D3DXVECTOR3 vDirection = m_LookAt - m_Position;

	D3DXVec3Normalize(&vDirection, &vDirection);

	vDirection *= 0.01f;
	//新しい位置
	D3DXVECTOR3 vNewPosition = m_Position + vDirection * (float)zDelta;
	//新しい方向
	D3DXVECTOR3 vNewDirection = m_LookAt - vNewPosition;

	//角度
	float fDeg = D3DXVec3Dot(&vDirection, &vNewDirection) / 
		(D3DXVec3Length(&vDirection) * D3DXVec3Length(&vNewDirection));

	if(fDeg < 1.57f){
		m_Position = vNewPosition;
	}
}

/**
*@briefカーソルの移動
*@return	
*@param	pt	位置
*/
void DXEditCamera::MoveCursor(Point<int> &pt){
	//カーソルの位置の差
	Vector<int, 2> ptDiff;
	ptDiff.x = pt.x - m_CursorPositon.x;
	ptDiff.y = pt.y - m_CursorPositon.y;

	if(m_LButtonPressed){
		//左クリックされている.平行移動する
	
		D3DXVECTOR3 vDirection = m_LookAt - m_Position;

		D3DXVECTOR3 vCrossDirectionAndTilt;
		D3DXVec3Cross(&vCrossDirectionAndTilt, &vDirection, &m_Tilt);

		D3DXVec3Normalize(&vCrossDirectionAndTilt, &vCrossDirectionAndTilt);

		vCrossDirectionAndTilt *= 0.03f;

		//ｘ成分
		m_Position += vCrossDirectionAndTilt * (float)ptDiff.x;
		m_LookAt += vCrossDirectionAndTilt * (float)ptDiff.x;

		//y成分
		D3DXVECTOR3 vCrossDirectionAndCross;

		D3DXVec3Cross(&vCrossDirectionAndCross, &vDirection, &vCrossDirectionAndTilt);
		D3DXVec3Normalize(&vCrossDirectionAndCross, &vCrossDirectionAndCross);

		vCrossDirectionAndCross *= -0.03f;

		m_Position += vCrossDirectionAndCross * (float)ptDiff.y;
		m_LookAt += vCrossDirectionAndCross * (float)ptDiff.y;

	}else if(m_RButtonPressed){
		//右クリックされている

		//回転させる
		D3DXVECTOR3 vPolarLookAt = m_Position - m_LookAt;

		m_Theta += ptDiff.x / 100.0f;
		m_Phi += ptDiff.y / 100.0f;

		if(m_Phi > D3DXToRadian(90.0f)){
			m_Phi = D3DXToRadian(89.0f);
		}else if(m_Phi < D3DXToRadian(-90.0f)){
			m_Phi = D3DXToRadian(-89.0f);
		}
		//長さ
		float fLength = D3DXVec3Length(&vPolarLookAt);

		D3DXVECTOR3 z(0.0f, 0.0f, -1.0f);

		D3DXMATRIX mat;
		D3DXMatrixRotationYawPitchRoll(&mat, m_Theta, m_Phi, 0.0f);
		D3DXVECTOR3 vOut;
		D3DXVec3TransformCoord(&vOut, &z, &mat);
		D3DXVec3Normalize(&vOut, &vOut);

		m_Position = m_LookAt + vOut * fLength;
	}
}


