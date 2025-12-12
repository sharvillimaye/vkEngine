#pragma once

#include "../yellowstone_frame_info.hpp"
#include "../yellowstone_game_object.hpp"

namespace yellowstone {

	class PhysicsSystem {
	public:
		PhysicsSystem();
		~PhysicsSystem() = default;
		PhysicsSystem(const PhysicsSystem&) = delete;
		PhysicsSystem& operator=(const PhysicsSystem&) = delete;

		void update(FrameInfo& frameInfo);

	private:
		const float gravity = 9.8f; // Positive gravity pulls downward (Y-down coordinate system)
		const float bounceDamping = 0.7f; // Energy loss on bounce
		const float groundY = 0.0f; // Ground plane Y position

		bool checkAABBCollision(const YellowstoneGameObject& obj1, const YellowstoneGameObject& obj2);
		void resolveCollision(YellowstoneGameObject& obj1, YellowstoneGameObject& obj2);
		void resolveGroundCollision(YellowstoneGameObject& obj);
	};

}

