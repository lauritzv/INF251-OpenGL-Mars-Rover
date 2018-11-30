#pragma once
#include <vector>
#include "Vector3.h"
#include "Matrix4.h"
#include "MeshObject.h"
#include "RenderProperties.h"

class SceneNode :public std::enable_shared_from_this<SceneNode> {

private:
	bool requires_recentering = false;
	Matrix4f move_to_orego;
	Matrix4f orig_translation;

protected:
	std::shared_ptr<SceneNode> parent = nullptr;
	Matrix4f worldTransform;
	Matrix4f transform;
	//Vector3f modelScale;
	std::vector <std::shared_ptr<SceneNode>> children;
	std::shared_ptr<SceneNode> GetThis() { return shared_from_this(); }

public:
	std::unique_ptr<MeshObject> mesh;
	SceneNode(std::unique_ptr<MeshObject> m) : mesh(std::move(m)) {}
	~SceneNode();

	void SetOriginalPosition(const Vector3f& orig_pos);
	void SetTransform(const Matrix4f & matrix);
	const Matrix4f & GetTransform() const { return transform; }
	Matrix4f GetWorldTransform() const { return worldTransform; }

	//Vector3f GetModelScale() const { return modelScale; }
	//void SetModelScale(const Vector3f& s) { modelScale = s; }

	void SetParent(const std::shared_ptr<SceneNode>& s);
	void SetMesh(std::unique_ptr<MeshObject> m) { mesh = std::move(m); }
	void AddChild(const std::shared_ptr<SceneNode>& s);
	bool SuccessfullyImported() const;
	void Update();
	void Draw(RenderProperties &rp) const;
};
