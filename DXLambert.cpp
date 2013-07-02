#include "DXLambert.h"
#include <Singleton.hpp>
#include <DXManager.h>
namespace DXLib{

	bool DXLambert::setup(){
		//���[�h
		m_pEffect = DXPrimitiveHLSL::Create(
			_T("Lambert.fx"),
			GetSingleton<DXManager>()->getDevice()
		);
		m_hTechniqueMesh = (*m_pEffect)->GetTechniqueByName("LambertMesh");
		m_hWVP		 = (*m_pEffect)->GetParameterByName(NULL, "g_WVP");
		m_hLightDir  = (*m_pEffect)->GetParameterByName(NULL, "g_LightDir");
		m_hAmbient  = (*m_pEffect)->GetParameterByName(NULL, "g_Ambient");

		return true;
	}

	void DXLambert::SetMatrix(D3DXMATRIX &matWorld){
		//BeginPass����ɏ����ƃ_���B
		//�܂��ABeginPass��Draw�̒��O�ɏ������ƁB
		m_MatWorld = matWorld;

		//���[���h �~ �r���[ �~ �ˉe
		D3DXMATRIX m;
		m = matWorld * m_MatView * m_MatProj;
		(*m_pEffect)->SetMatrix( m_hWVP, &m );
	}
	
	void DXLambert::SetLightDirection(D3DXVECTOR4 &vLightDir){
		//���s�����̕����x�N�g����ݒ肷��
		D3DXVECTOR4 LightDir = vLightDir;
		D3DXMATRIX m;
		D3DXVECTOR4 v;
		D3DXMatrixInverse(&m, NULL, &m_MatWorld);
		D3DXVec4Transform(&v, &LightDir, &m);

		//���K������
		D3DXVec3Normalize( (D3DXVECTOR3*)&v, (D3DXVECTOR3*)&v );    
		(*m_pEffect)->SetVector( m_hLightDir, &v );
	}
	void DXLambert::SetViewProjection(D3DXMATRIX &view, D3DXMATRIX &proj){
		m_MatView = view;
		m_MatProj = proj;
	}
	void DXLambert::SetAmbient(D3DXVECTOR4 &vAmb){
		(*m_pEffect)->SetVector(m_hAmbient, &vAmb);
	}
};