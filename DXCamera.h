/**
*@file	DXCamera.h
*@brief	カメラヘッダ
*/

#pragma once

#include <DXDeviceObject.h>
#include <Shape.h>


/**
*@class	DXCamera
*@brief	カメラの基底クラス
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
*@brief	普段使い用
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
*@brief	エディタ等で使えるマウスで動かせるカメラ.EditCameraProc()を呼ぶ必要がある
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

		bool		m_RButtonPressed;		//!<	右クリックされているかどうか
		bool		m_LButtonPressed;		//!<	左クリックされているかどうか
		TUL::Point<float,2>	m_RClickPoint;			//!<	右クリックの位置
		TUL::Point<float,2>	m_LClickPoint;			//!<	左クリックの位置
		TUL::Point<float,2>	m_CursorPositon;		//!<	カーソルの位置

		float		m_Theta;
		float		m_Phi;
	};
};