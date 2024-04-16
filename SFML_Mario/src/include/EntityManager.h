#pragma once
#include <vector>
#include <map>
#include "Entity.h"

typedef std::vector<std::shared_ptr<Entity>> EntityVec;
typedef std::map<std::string, EntityVec> EntityMap;

class EntityManager
{
	EntityVec	 m_entities;
	EntityVec	 m_entitiesToAdd;
	EntityMap	 m_entityMap;
	size_t		 m_totalEntities = 0;

	void removeDeadEntities(EntityVec& eVec);

public:
	EntityManager();
	
	void update();

	std::shared_ptr<Entity> addEntity(const std::string& tag);
	const EntityVec& getEntities();
	const EntityVec& getEntities(const std::string& tag);
	const EntityMap& getEntityMap();
};