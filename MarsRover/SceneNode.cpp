#include "SceneNode.h"
#include <iostream>

void SceneNode::Update() {
	if (parent) // This node has a parent ...
		worldTransform = parent->worldTransform * transform;

	else // Root node , world transform is local transform !
		worldTransform = transform;

	for (auto& i : children)
		i->Update();
}

void SceneNode::Draw(GLuint& shader_program) const
{
	for (auto& el : children)
		el->Draw(shader_program);

	if (mesh == nullptr)
		return;

	mesh->DrawObject(GetWorldTransform(),shader_program);
}

SceneNode::~SceneNode() = default;
//{
//	for (auto& i : children)
//		delete i;
//}

void SceneNode::SetTransform(const Matrix4f& matrix)
{
	transform = matrix;
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

