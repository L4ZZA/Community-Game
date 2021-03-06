﻿#pragma once

#include "../../level/Level.h"
#include "../../entity/EntityFactory.h"
#include "StateBase.h"
#include "../../item/ItemFactory.h"

namespace State
{
	class Playing : public Base
	{
	public:
		static Playing* instance;
		static Entity::EntityFactory* entityFactory;
		static Item::ItemFactory* itemFactory;
		
	public:
		Playing(Application* app, sf::RenderWindow* window);
		
		void event(sf::Event& event) override;
		void input() override;
		void update(const Timestep& ts) override;
		void render(sf::RenderWindow& window) override;
		void tick() override;

		bool isTilePassable(byte layer, uint x, uint y);

		Level::Level& getLevel() { return m_level; }

	private:
		Level::Level m_level;

		void singleplayerCallback() {}
		void multiplayerCallback() {}
		void creditsCallback() {}
	};
}
