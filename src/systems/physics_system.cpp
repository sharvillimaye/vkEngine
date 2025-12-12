#include "physics_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <cmath>

namespace yellowstone {

	PhysicsSystem::PhysicsSystem() {}

	void PhysicsSystem::update(FrameInfo& frameInfo) {
		float deltaTime = frameInfo.frameTime;

		// Apply physics to all non-static objects
		for (auto& kv : frameInfo.gameObjects) {
			auto& obj = kv.second;
			
			// Skip static objects
			if (obj.physics.isStatic) {
				continue;
			}

			// Apply gravity
			obj.physics.velocity.y += gravity * deltaTime;

			// Update position based on velocity
			obj.transform.translation += obj.physics.velocity * deltaTime;

			// Check ground collision (accounting for object's bottom edge)
			// In Y-down system: objects below ground (positive Y) collide
			float bottomY = obj.transform.translation.y + obj.transform.scale.y * 0.5f;
			if (bottomY >= groundY) {
				resolveGroundCollision(obj);
			}

			// Apply simple damping to prevent infinite bouncing
			obj.physics.velocity *= 0.99f;
		}

		// Check collisions between dynamic objects
		for (auto it1 = frameInfo.gameObjects.begin(); it1 != frameInfo.gameObjects.end(); ++it1) {
			auto& obj1 = it1->second;
			if (obj1.physics.isStatic) continue;

			for (auto it2 = std::next(it1); it2 != frameInfo.gameObjects.end(); ++it2) {
				auto& obj2 = it2->second;
				if (obj2.physics.isStatic) continue;

				if (checkAABBCollision(obj1, obj2)) {
					resolveCollision(obj1, obj2);
				}
			}
		}
	}

	bool PhysicsSystem::checkAABBCollision(const YellowstoneGameObject& obj1, const YellowstoneGameObject& obj2) {
		// Simple AABB collision detection
		// Using scale as half-extents (assuming objects are centered)
		glm::vec3 min1 = obj1.transform.translation - obj1.transform.scale * 0.5f;
		glm::vec3 max1 = obj1.transform.translation + obj1.transform.scale * 0.5f;
		glm::vec3 min2 = obj2.transform.translation - obj2.transform.scale * 0.5f;
		glm::vec3 max2 = obj2.transform.translation + obj2.transform.scale * 0.5f;

		return (min1.x <= max2.x && max1.x >= min2.x) &&
		       (min1.y <= max2.y && max1.y >= min2.y) &&
		       (min1.z <= max2.z && max1.z >= min2.z);
	}

	void PhysicsSystem::resolveCollision(YellowstoneGameObject& obj1, YellowstoneGameObject& obj2) {
		// Simple collision response: separate objects and reverse velocities
		glm::vec3 collisionNormal = glm::normalize(obj1.transform.translation - obj2.transform.translation);
		
		// Separate objects to prevent overlap
		float overlap = glm::length(obj1.transform.scale + obj2.transform.scale) * 0.5f - 
		                glm::length(obj1.transform.translation - obj2.transform.translation);
		if (overlap > 0.0f) {
			obj1.transform.translation += collisionNormal * overlap * 0.5f;
			obj2.transform.translation -= collisionNormal * overlap * 0.5f;
		}

		// Simple velocity exchange with damping
		glm::vec3 relativeVelocity = obj1.physics.velocity - obj2.physics.velocity;
		float velocityAlongNormal = glm::dot(relativeVelocity, collisionNormal);

		// Don't resolve if velocities are separating
		if (velocityAlongNormal > 0) return;

		// Calculate impulse
		float restitution = bounceDamping;
		float j = -(1 + restitution) * velocityAlongNormal;
		j /= (1.0f / obj1.physics.mass + 1.0f / obj2.physics.mass);

		glm::vec3 impulse = j * collisionNormal;
		obj1.physics.velocity += impulse / obj1.physics.mass;
		obj2.physics.velocity -= impulse / obj2.physics.mass;
	}

	void PhysicsSystem::resolveGroundCollision(YellowstoneGameObject& obj) {
		// Place object on ground (in Y-down system, ground is at Y=0, so object center is above)
		obj.transform.translation.y = groundY - obj.transform.scale.y * 0.5f;
		
		// Bounce if falling down (positive Y velocity in Y-down system)
		if (obj.physics.velocity.y > 0.0f) {
			obj.physics.velocity.y *= -bounceDamping;
			
			// Stop if velocity is too small
			if (std::abs(obj.physics.velocity.y) < 0.1f) {
				obj.physics.velocity.y = 0.0f;
			}
		}
	}

}

