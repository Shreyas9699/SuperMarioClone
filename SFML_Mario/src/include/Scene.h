#pragma once
#include <unordered_map>
#include <memory>
#include "EntityManager.h"
#include "Action.h"

/* Stores common Scene data: Entities, Frame Count, Actions
   Sccene-specific functionality is carried out in derived calsses, Base Scene class is abstract

   NOTE: 'virtual' allows functions in base class 'Scene' to be overridden by derived classes,
   providing polymorphic behavior. Functions declared with '= 0' must be implemented by
   derived classes, making the base class abstract and unable to be instantiated directly.
*/

class GameEngine;

typedef std::unordered_map<int, std::string> ActionMap;

class Scene
{
protected:
	GameEngine*	  m_game			= nullptr;
	bool		  m_paused			= false;
	//bool		  m_hasEnded		= false;
	size_t		  m_currentFrame	= 0;
	EntityManager m_entityManager;
	ActionMap	  m_actionMap;

	virtual void onEnd() = 0;
	void setPaused(bool paused);

public:
	Scene();
	explicit Scene(GameEngine* gameEngine);

	virtual void update() = 0;
	virtual void sDoAction(const Action& action) = 0;
	virtual void sRender() = 0;

	void simulate(const size_t frames);
	void registerAction(int inputKey, const std::string& actionName);

	size_t width() const;
	size_t height() const;
	size_t currentFrame() const;

	//bool hasEnded() const;
	const ActionMap& getActionMap() const;
	void drawLine(const Vec2& p1, const Vec2& p2);
};