#pragma once
#include <DXAbsRenderingEngine.h>

namespace DXLib{
	class DXLambert : public DXAbsRenderingEngine{
	public:
		bool setup();

		void SetMatrix(D3DXMATRIX &matWorld);
		void SetLightDirection(D3DXVECTOR4 &vLightDir);
		void SetViewProjection(D3DXMATRIX &view, D3DXMATRIX &proj);

		void SetAmbient(D3DXVECTOR4 &vAmb);

	private:
		std::shared_ptr<DXPrimitiveHLSL> getEffect(){return m_pEffect;} //テンプレートメソッドパターン用
		std::shared_ptr<DXPrimitiveHLSL> m_pEffect;

		D3DXHANDLE	m_hTechniqueMesh;
		D3DXHANDLE	m_hWVP;
		D3DXHANDLE	m_hAmbient;
		D3DXHANDLE	m_hLightDir;
		
		D3DXMATRIX	m_MatWorld;
		D3DXMATRIX	m_MatView;
		D3DXMATRIX	m_MatProj;
	};
};