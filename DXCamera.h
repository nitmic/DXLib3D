/**
*@file	DXCamera.h
*@brief	�J�����w�b�_
*/

#pragma once

#include <DXDeviceObject.h>
#include <Shape.h>


/**
*@class	DXCamera
*@brief	�J�����̊��N���X
*/
namespace DXLib{
	class DXICamera{
	public:
		virtual void transform() = 0;
		struct ImplParameter;
		struct ImplMatrix;
	};
};


/**
*@class	DXCamera
*@brief	���i�g���p
*/
namespace DXLib{
	class DXCamera : public DXICamera, private DXDeviceObject{
	private:
		std::shared_ptr<ImplMatrix>		m_pMat;
		std::shared_ptr<ImplParameter>	m_pParam;
	public:
		void		transform();

		DXCamera();
		virtual ~DXCamera();
	};
};


/**
*@class	DXEditCamera
*@brief	�G�f�B�^���Ŏg����}�E�X�œ�������J����.EditCameraProc()���ĂԕK�v������
*/
namespace DXLib{
	class DXEditCamera : public DXICamera, public Singleton<DXEditCamera>, private DXDeviceObject{
	public:
		BOOL CALLBACK		EditCameraProc(HWND, UINT, WPARAM, LPARAM);
		void		transform();

		DXEditCamera();
		~DXEditCamera();
	private:
		void		MoveCursor(TUL::Point<float> &pt);
		void		MoveMouseWheel(long zDelta);
	private:
		std::shared_ptr<ImplMatrix>		m_pMat;
		std::shared_ptr<ImplParameter>	m_pParam;

		bool		m_RButtonPressed;		//!<	�E�N���b�N����Ă��邩�ǂ���
		bool		m_LButtonPressed;		//!<	���N���b�N����Ă��邩�ǂ���
		TUL::Point<float,2>	m_RClickPoint;			//!<	�E�N���b�N�̈ʒu
		TUL::Point<float,2>	m_LClickPoint;			//!<	���N���b�N�̈ʒu
		TUL::Point<float,2>	m_CursorPositon;		//!<	�J�[�\���̈ʒu

		float		m_Theta;
		float		m_Phi;
	};
};