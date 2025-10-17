#pragma once

#include "yellowstone_model.hpp"

#include <memory>

namespace yellowstone {

	struct Transform2dComponent {
		glm::vec2 translation{};
		glm::vec2 scale{ 1.0f, 1.0f };
		float rotation;

		glm::mat2 mat2() {
			const float s = glm::sin(rotation);
			const float c = glm::cos(rotation);
			glm::mat2 rotMat{ {c, s}, {-s, c} };
			glm::mat2 scaleMat{ {scale.x, .0f}, {.0f, scale.y} };
			return rotMat * scaleMat;
		}
	};
	
	class YellowstoneGameObject {
	public:
		using id_t = unsigned int;
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
		Transform2dComponent transform2d{};

	private:
		YellowstoneGameObject(id_t objId) : id{ objId } {}
		id_t id;

	};
}