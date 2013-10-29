#include "DXMeshStorage.h"
#include "DXMesh.h"

namespace DXLib{
	std::shared_ptr<DXMesh> DXMeshStorage::find(TUL::tString name){
		if(m_Meshs[name]==nullptr){
			auto mesh = std::make_shared<DXMesh>();
			mesh->load(name);
			m_Meshs[name] = mesh;
		}
		return m_Meshs[name];
	}
};