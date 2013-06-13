/**
*@file	DXCamera.h
*@brief	カメラヘッダ
*@date	2010/3/2s
*/

#pragma once

#include <DXDeviceObject.h>
#include <Shape.h>

class DXICamera{
public:
	D3DXVECTOR2			ComputeScreenCoodinate(D3DXVECTOR3 &vWorldPos, D3DVIEWPORT9 &viewport, D3DXMATRIX &matWorld);
protected:
	D3DXMATRIX		m_matProj;		//!<	プロジェクション行列
	D3DXMATRIX		m_matView;		//!<	ビュー行列

	D3DXVECTOR3		m_Position;		//!<	位置
	D3DXVECTOR3		m_LookAt;			//!<	視点
	D3DXVECTOR3		m_Tilt;			//!<	傾き

	float			m_NearPlane;			//!<	ニアプレーン
	float			m_FarPlane;			//!<	ファープレーン

	static const D3DXVECTOR3 m_DefaultPosition;	//!<	デフォルトの位置
	static const D3DXVECTOR3 m_DefaultLookAt;	//!<	デフォルトの視点
};

const D3DXVECTOR3 DXICamera::m_DefaultPosition = D3DXVECTOR3(0.0f, 0.0f, 5.0f);
const D3DXVECTOR3 DXICamera::m_DefaultLookAt	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);

/**
*@class	DXCamera
*@brief	カメラの基底クラス
*/
class DXCamera : public DXICamera, private DXDeviceObject{
public:
	void		transform();
	void		reset();

	DXCamera();
	virtual ~DXCamera();
protected:
protected:

	float			m_FieldOfView;		//!<	視野

};


/**
*@class	CNcDXEditCamera
*@brief	エディタ等で使えるマウスで動かせるカメラ.EditCameraProc()を呼ぶ必要がある
*/
class DXEditCamera : public DXICamera, public Singleton<DXEditCamera>{
public:
	
	BOOL CALLBACK		EditCameraProc(HWND, UINT, WPARAM, LPARAM);

	float		GetTheta()		{return m_Theta;}
	float		GetPhi()		{return m_Phi;}

	DXEditCamera();
	~DXEditCamera();
protected:
	void		MoveCursor(Point<int> &pt);
	void		MoveMouseWheel(long zDelta);
protected:

	bool		m_RButtonPressed;		//!<	右クリックされているかどうか
	bool		m_LButtonPressed;		//!<	左クリックされているかどうか
	Point<int>	m_RClickPoint;			//!<	右クリックの位置
	Point<int>	m_LClickPoint;			//!<	左クリックの位置
	Point<int>	m_CursorPositon;		//!<	カーソルの位置

	float		m_Theta;
	float		m_Phi;
};