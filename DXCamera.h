/**
*@file	DXCamera.h
*@brief	�J�����w�b�_
*/

#pragma once
#include <memory>
#include <Singleton.hpp>
#include <windows.h>
#include <Windows.h>

/**
*@class	DXCamera
*@brief	�J�����̊��N���X
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
*@brief	���i�g���p
*/
namespace DXLib{
	class DXCamera : public DXICamera{
	public:
		void		transform();

		DXCamera();
		virtual ~DXCamera();
	private:
		struct Impl;
		std::shared_ptr<Impl> __impl__;
	};
};


/**
*@class	DXEditCamera
*@brief	�G�f�B�^���Ŏg����}�E�X�œ�������J����.EditCameraProc()���ĂԕK�v������
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