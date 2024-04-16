#include "EntityManager.h"

EntityManager::EntityManager() {}

void EntityManager::update()
{
	// to add entities from m_entitiesToAdd the both vector of all entities and entity Map
	// remove all dead entities from both vector of all entities and entity Map
	for (auto& e : m_entitiesToAdd)
	{
		m_entities.push_back(e);
		m_entityMap[e->tag()].push_back(e);
	}

	m_entitiesToAdd.clear();
	removeDeadEntities(m_entities);

	// to remove corresponding dead entities from entity map as well
	for (auto& kv : m_entityMap)
	{
		removeDeadEntities(kv.second);
	}
}

void EntityManager::removeDeadEntities(EntityVec& eVec)
{
	/* Since we need to remove the entity from entityvec while iterating thought, we chances of going into iterator invalidation
	 * but in below case, we use the STL function called `remove_if`, which doesn't actually physically remove elements from the container. 
	   Instead, it reorders the elements in the range so that the elements to be removed are placed at the end of the range.
	   So, form the return iterator till end, we can directl use erase, since all removeable entities are at the end.
	*/
	eVec.erase(std::remove_if(eVec.begin(), eVec.end(), [](const auto& e)
		{ return !e->isActive(); }),
		eVec.end());
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	auto e = std::shared_ptr<Entity>(new Entity(++m_totalEntities, tag));
	m_entitiesToAdd.push_back(e);
	return e;
}

const  EntityVec& EntityManager::getEntities()
{
	return m_entities; 
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}

const EntityMap& EntityManager::getEntityMap()
{
	return m_entityMap;
}