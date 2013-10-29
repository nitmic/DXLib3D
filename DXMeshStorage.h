#pragma once
#include <Singleton.hpp>
#include <tString.hpp>
#include <map>

namespace DXLib{
	class DXMesh;
	class DXMeshStorage : public Singleton<DXMeshStorage>{
		friend Singleton<DXMeshStorage>;
	public:
		std::shared_ptr<DXMesh> find(TUL::tString name);
	private:
		std::map<TUL::tString, std::shared_ptr<DXMesh>> m_Meshs;
	};
};