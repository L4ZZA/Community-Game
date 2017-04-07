#pragma once

#include <vector>

#include "../Entity.h"
#include "../../util/Timestep.h"

namespace Framework
{
	class System
	{
	public:
		virtual void update(const Timestep& ts, Entity* entity) {};
	};

	class MoveSystem : public System
	{
	public:
		void update(const Timestep& ts, Entity* entity) override;
	}; 

	class StatsSystem : public System
	{
	public:
		void update(const Timestep& ts, Entity* entity) override;
	};
}