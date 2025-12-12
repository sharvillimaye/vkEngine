#pragma once

#include "yellowstone_model.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>
#include <unordered_map>

namespace yellowstone {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
	};

	struct PhysicsComponent {
		glm::vec3 velocity{ 0.0f, 0.0f, 0.0f };
		float mass = 1.0f;
		bool isStatic = false;
	};
	
	class YellowstoneGameObject {
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, YellowstoneGameObject>;

		static YellowstoneGameObject createGameObject() {
			static id_t currentId = 0;
			return YellowstoneGameObject{ currentId++ };
		}

		YellowstoneGameObject(const YellowstoneGameObject&) = delete;
		YellowstoneGameObject& operator=(const YellowstoneGameObject&) = delete;
		YellowstoneGameObject(YellowstoneGameObject&&) = default;
		YellowstoneGameObject& operator=(YellowstoneGameObject&&) = default;

		id_t getId() const { return id; }

		std::shared_ptr<YellowstoneModel> model{};
		glm::vec3 color{};
		TransformComponent transform{};
		PhysicsComponent physics{};

	private:
		YellowstoneGameObject(id_t objId) : id{ objId } {}
		id_t id;

	};
}