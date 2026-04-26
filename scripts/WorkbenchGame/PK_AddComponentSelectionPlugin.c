#ifdef WORKBENCH

enum PK_EComponentAction
{
	ADD = 0,
	REMOVE = 1
}

//------------------------------------------------------------------------------------------------
// Data class for the Component Modifier dialog. 
// Used by Workbench.ScriptDialog to generate a UI.
[BaseContainerProps()]
class PK_ComponentModifierDialogData : SCR_OKCancelWorkbenchDialog
{
	[Attribute("0", UIWidgets.ComboBox, "Choose whether to add or remove the component.", enums: ParamEnumArray.FromEnum(PK_EComponentAction))]
	PK_EComponentAction m_eAction;

	[Attribute("PK_EntityProtectorComponent", UIWidgets.EditBox, "Name of the component class to modify.")]
	string m_sComponentClassName;

	[Attribute("0", UIWidgets.CheckBox, "If checked, the action will be applied to the selection and all hierarchy descendants.")]
	bool m_bRecursive;
}

//------------------------------------------------------------------------------------------------
// A professional helper plugin that allows adding or removing ANY component by name.
[WorkbenchPluginAttribute(
	name: "Add/Remove Component from selection",
	description: "Adds or removes any component by class name from the selected entities.\nSupports recursive application.",
	wbModules: { "WorldEditor" },
	awesomeFontCode: 0xF0AD, // Wrench icon
	category: "PK Tools"
)]
class PK_ComponentModifierPlugin : WorldEditorPlugin
{
	//------------------------------------------------------------------------------------------------
	override void Run()
	{
		WorldEditor worldEditor = Workbench.GetModule(WorldEditor);
		if (!worldEditor) return;

		WorldEditorAPI api = worldEditor.GetApi();
		if (!api) return;

		int selCount = api.GetSelectedEntitiesCount();
		if (selCount == 0)
		{
			Print("[EntityDeleter] No entities selected.", LogLevel.WARNING);
			return;
		}

		// 1. Open the ScriptDialog with our data class
		PK_ComponentModifierDialogData data = new PK_ComponentModifierDialogData();
		if (Workbench.ScriptDialog("Component Modifier", "Specify component and action:", data) == 0)
			return; // User cancelled

		string targetComponent = data.m_sComponentClassName;
		if (targetComponent == "")
		{
			Print("[EntityDeleter] Component name cannot be empty.", LogLevel.WARNING);
			return;
		}

		// 2. Process the selection
		ref array<IEntitySource> toProcess = {};
		for (int i = 0; i < selCount; i++)
		{
			if (data.m_bRecursive)
				CollectRecursive(api.GetSelectedEntity(i), toProcess);
			else
				toProcess.Insert(api.GetSelectedEntity(i));
		}

		// 3. Begin undoable action
		string actionLabel = "Add";
		string verb = "added";
		if (data.m_eAction == PK_EComponentAction.REMOVE)
		{
			actionLabel = "Remove";
			verb = "removed";
		}
		
		api.BeginEntityAction("PK_ComponentModifierPlugin: " + actionLabel + " Component");

		int successCount = 0;
		int skippedCount = 0;
		int failedCount = 0;
		
		foreach (IEntitySource src : toProcess)
		{
			IEntityComponentSource comp = FindComponent(src, targetComponent);

			if (data.m_eAction == PK_EComponentAction.ADD)
			{
				if (comp)
				{
					skippedCount++; // Already has it
					continue;
				}

				if (api.CreateComponent(src, targetComponent))
					successCount++;
				else
					failedCount++;
			}
			else // REMOVE
			{
				if (!comp)
				{
					skippedCount++; // Doesn't have it
					continue;
				}

				if (api.DeleteComponent(src, comp))
					successCount++;
				else
					failedCount++;
			}
		}

		api.EndEntityAction();

		string resultMsg = string.Format("[EntityDeleter] Component '%1' %2 from %3 entities.", targetComponent, verb, successCount);
		Print(resultMsg, LogLevel.NORMAL);
		
		if (failedCount > 0)
		{
			string failAction = "add";
			if (data.m_eAction == PK_EComponentAction.REMOVE) failAction = "remove";
			Print(string.Format("[EntityDeleter] Failed to %1 '%2' on %3 entities. Is the class name correct?", failAction, targetComponent, failedCount), LogLevel.WARNING);
		}

		if (skippedCount > 0)
			Print(string.Format("[EntityDeleter] %1 entities were skipped.", skippedCount), LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	// Helper to find the specific component source on an entity source
	private IEntityComponentSource FindComponent(IEntitySource src, string className)
	{
		int count = src.GetComponentCount();
		for (int i = 0; i < count; i++)
		{
			IEntityComponentSource comp = src.GetComponent(i);
			if (comp.GetClassName() == className)
				return comp;
		}
		return null;
	}

	//------------------------------------------------------------------------------------------------
	private void CollectRecursive(IEntitySource src, array<IEntitySource> list)
	{
		if (!src || list.Find(src) != -1)
			return;

		list.Insert(src);

		int childCount = src.GetNumChildren();
		for (int i = 0; i < childCount; i++)
		{
			CollectRecursive(src.GetChild(i), list);
		}
	}
}

#endif
