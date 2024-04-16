#pragma once
#include <string>
#include <tuple>
#include "Components.h"

class EntityManager;

typedef std::tuple< CTransform, CLifeSpan, CInput, CBoundingBox, CAnimation, CGravity, CState > ComponentsTuple;

class Entity
{
	friend class EntityManager;

	bool					m_active	 = true;
	size_t					m_id		 = 0;
	const std::string		m_tag		 = "default";
	ComponentsTuple			m_components = std::make_tuple(CTransform(), CLifeSpan(), CInput(), CBoundingBox(), CAnimation(), CGravity(), CState());
	// constructor is private se we can never create
	// entities outside the EntityManager which had friend access
	Entity(const size_t id, const std::string& tag);

public:
	void destroy();
	bool isActive() const;
	const size_t id() const;
	const std::string& tag() const;

	// to check if component of type T exists or not
	template<typename T>
	bool hasComponent() const
	{
		return getComponent<T>().has;
	}

	// Function to add components of any type T to an entity
	// TArgs are the types of arguments passed to initialize the component
	template<typename T, typename... TArgs>
	T& addComponents(TArgs &&...mArgs)
	{
		auto& component = getComponent<T>();
		// Initialize the component with the provided arguments
		component = T(std::forward<TArgs>(mArgs)...);
		component.has = true;
		return component;
	}

	template <typename T>
	T& getComponent()
	{
		return std::get<T>(m_components);
	}

	template <typename T>
	const T& getComponent() const
	{
		return std::get<T>(m_components);
	}

	template <typename T>
	void removeComponent()
	{
		getComponent<T>() = T();
	}
};