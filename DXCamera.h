/**
*@file	DXCamera.h
*@brief	カメラヘッダ
*/

#pragma once
#include <memory>
#include <Singleton.hpp>
#include <windows.h>
#include <Windows.h>


/**
*@class	DXCamera
*@brief	カメラの基底クラス
*/
namespace DXLib{
	class DXICamera{
	public:
		virtual void transform() = 0;
		struct Parameter;
		struct Matrix;
	};
};


/**
*@class	DXCamera
*@brief	普段使い用
*/
namespace DXLib{
	class DXCamera : public DXICamera{
	public:
		void transform();

		void setPosition(float x, float y, float z);
		void setLookAt(float x, float y, float z);
		void setTilt(float x, float y, float z);
		void setFieldOfView(float fieldOfView);

		DXCamera();
		~DXCamera();
	private:
		struct Impl;
		std::shared_ptr<Impl> __impl__;
	};
};


/**
*@class	DXEditCamera
*@brief	エディタ等で使えるマウスで動かせるカメラ.EditCameraProc()を呼ぶ必要がある
*/
namespace DXLib{
	class DXEditCamera : public DXICamera, public Singleton<DXEditCamera>{
		friend Singleton<DXEditCamera>;
	public:
		BOOL CALLBACK EditCameraProc(HWND, UINT, WPARAM, LPARAM);
		void		transform();

		DXEditCamera();
		~DXEditCamera();
	private:
		struct Impl;
		std::shared_ptr<Impl> __impl__;
	};
};
