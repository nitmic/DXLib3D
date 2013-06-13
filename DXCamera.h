/**
*@file	DXCamera.h
*@brief	�J�����w�b�_
*@date	2010/3/2s
*/

#pragma once

#include <DXDeviceObject.h>
#include <Shape.h>

class DXICamera{
public:
	D3DXVECTOR2			ComputeScreenCoodinate(D3DXVECTOR3 &vWorldPos, D3DVIEWPORT9 &viewport, D3DXMATRIX &matWorld);
protected:
	D3DXMATRIX		m_matProj;		//!<	�v���W�F�N�V�����s��
	D3DXMATRIX		m_matView;		//!<	�r���[�s��

	D3DXVECTOR3		m_Position;		//!<	�ʒu
	D3DXVECTOR3		m_LookAt;			//!<	���_
	D3DXVECTOR3		m_Tilt;			//!<	�X��

	float			m_NearPlane;			//!<	�j�A�v���[��
	float			m_FarPlane;			//!<	�t�@�[�v���[��

	static const D3DXVECTOR3 m_DefaultPosition;	//!<	�f�t�H���g�̈ʒu
	static const D3DXVECTOR3 m_DefaultLookAt;	//!<	�f�t�H���g�̎��_
};

const D3DXVECTOR3 DXICamera::m_DefaultPosition = D3DXVECTOR3(0.0f, 0.0f, 5.0f);
const D3DXVECTOR3 DXICamera::m_DefaultLookAt	= D3DXVECTOR3(0.0f, 0.0f, 0.0f);

/**
*@class	DXCamera
*@brief	�J�����̊��N���X
*/
class DXCamera : public DXICamera, private DXDeviceObject{
public:
	void		transform();
	void		reset();

	DXCamera();
	virtual ~DXCamera();
protected:
protected:

	float			m_FieldOfView;		//!<	����

};


/**
*@class	CNcDXEditCamera
*@brief	�G�f�B�^���Ŏg����}�E�X�œ�������J����.EditCameraProc()���ĂԕK�v������
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

	bool		m_RButtonPressed;		//!<	�E�N���b�N����Ă��邩�ǂ���
	bool		m_LButtonPressed;		//!<	���N���b�N����Ă��邩�ǂ���
	Point<int>	m_RClickPoint;			//!<	�E�N���b�N�̈ʒu
	Point<int>	m_LClickPoint;			//!<	���N���b�N�̈ʒu
	Point<int>	m_CursorPositon;		//!<	�J�[�\���̈ʒu

	float		m_Theta;
	float		m_Phi;
};