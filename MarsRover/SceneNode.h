#pragma once
#include <vector>
#include "Vector3.h"
#include "Matrix4.h"
#include "MeshObject.h"

class SceneNode :public std::enable_shared_from_this<SceneNode> {

protected:
	std::shared_ptr<SceneNode> parent = nullptr;
	std::unique_ptr<MeshObject> mesh;
	Matrix4f worldTransform;
	Matrix4f transform;
	Vector3f modelScale;
	std::vector <std::shared_ptr<SceneNode>> children;
	std::shared_ptr<SceneNode> GetThis() { return shared_from_this(); }

public:
	SceneNode(std::unique_ptr<MeshObject> m) : mesh(std::move(m)) {}
	~SceneNode();

	void SetTransform(const Matrix4f & matrix);
	const Matrix4f & GetTransform() const { return transform; }
	Matrix4f GetWorldTransform() const { return worldTransform; }

	//Vector3f GetModelScale() const { return modelScale; }
	//void SetModelScale(const Vector3f& s) { modelScale = s; }

	//std::unique_ptr<MeshObject> GetMesh() const { return mesh; }
	void SetParent(const std::shared_ptr<SceneNode>& s);
	void SetMesh(std::unique_ptr<MeshObject> m) { mesh = std::move(m); }
	void AddChild(const std::shared_ptr<SceneNode>& s);

	void Update();
	void Draw(GLuint &shader_program) const;
};
