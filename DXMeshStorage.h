#pragma once
#include <Singleton.hpp>
#include <tString.h>
#include <map>

namespace DXLib{
	class DXMesh;
	class DXMeshStorage : public Singleton<DXMeshStorage>{
		friend Singleton<DXMeshStorage>;
	public:
		std::shared_ptr<DXMesh> find(tString name);
	private:
		std::map<tString, std::shared_ptr<DXMesh>> m_Meshs;
	};
};