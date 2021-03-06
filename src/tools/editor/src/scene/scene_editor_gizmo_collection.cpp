#include "scene_editor_gizmo_collection.h"
#include "gizmos/translate_gizmo.h"
#include "gizmos/selected_bounds_gizmo.h"
#include "gizmos/polygon_gizmo.h"
#include "halley/core/graphics/camera.h"
using namespace Halley;

SceneEditorGizmoCollection::SceneEditorGizmoCollection(UIFactory& factory, Resources& resources)
	: factory(factory)
	, resources(resources)
{
	selectedBoundsGizmo = std::make_unique<SelectedBoundsGizmo>(resources);
}

void SceneEditorGizmoCollection::update(Time time, const Camera& camera, const SceneEditorInputState& inputState, SceneEditorOutputState& outputState)
{
	selectedBoundsGizmo->setCamera(camera);
	selectedBoundsGizmo->update(time, inputState);
	
	if (activeGizmo) {
		activeGizmo->setCamera(camera);
		activeGizmo->setOutputState(outputState);
		activeGizmo->update(time, inputState);
	}
}

void SceneEditorGizmoCollection::draw(Painter& painter)
{
	selectedBoundsGizmo->draw(painter);
	
	if (activeGizmo) {
		activeGizmo->draw(painter);
	}
}

void SceneEditorGizmoCollection::setSelectedEntity(const std::optional<EntityRef>& entity, ConfigNode& data)
{
	selectedEntity = entity;
	entityData = &data;
	
	selectedBoundsGizmo->setSelectedEntity(entity, *entityData);
	
	if (activeGizmo) {
		activeGizmo->setSelectedEntity(entity, *entityData);
	}
}

std::shared_ptr<UIWidget> SceneEditorGizmoCollection::setTool(SceneEditorTool tool, const String& componentName, const String& fieldName, const ConfigNode& options)
{
	if (tool != currentTool || currentTool == SceneEditorTool::Polygon) { 	// Hack
		currentTool = tool;
		activeGizmo.reset();
		
		switch (tool) {
		case SceneEditorTool::Translate:
			activeGizmo = std::make_unique<TranslateGizmo>();
			break;

		case SceneEditorTool::Polygon:
			activeGizmo = std::make_unique<PolygonGizmo>(componentName, fieldName, options, factory);
			break;
		}

		if (activeGizmo) {
			activeGizmo->setSelectedEntity(selectedEntity, *entityData);
			return activeGizmo->makeUI();
		}
	}
	return {};
}
