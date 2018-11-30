#include "SceneNode.h"
#include <iostream>
#include "RenderProperties.h"

void SceneNode::Update() {
	if (parent) // This node has a parent ...
		worldTransform = parent->worldTransform * transform;

	else // or world transform is local transform !
		worldTransform = transform;

	for (auto& i : children)
		i->Update();
}

void SceneNode::Draw(RenderProperties& rp) const
{
	for (auto& el : children)
		el->Draw(rp);

	if (mesh == nullptr)
		return;

	mesh->DrawObject(GetWorldTransform(), rp);
}

SceneNode::~SceneNode() = default;
//{
//	for (auto& i : children)
//		delete i;
//}

void SceneNode::SetOriginalPosition(const Vector3f& orig_pos)
{
	requires_recentering = true;
	move_to_orego = Matrix4f().createTranslation(-orig_pos);
	orig_translation = move_to_orego.getInverse();
}

void SceneNode::SetTransform(const Matrix4f& matrix)
{
	if (requires_recentering)
		transform = orig_translation * matrix * move_to_orego;
	else transform = matrix;

	Update();
}


void SceneNode::SetParent(const std::shared_ptr<SceneNode> &s)
{
	parent = s;
	Update();
}


void SceneNode::AddChild(const std::shared_ptr<SceneNode>& s)
{
	children.push_back(s);
	s->SetParent(GetThis());
}

bool SceneNode::SuccessfullyImported() const
{
	if (mesh == nullptr)
		return true;
	return mesh->successfullyImported;
}