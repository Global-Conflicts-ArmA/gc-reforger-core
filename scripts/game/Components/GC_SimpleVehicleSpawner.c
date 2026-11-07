[EntityEditorProps(category: "GameScripted/Spawning", description: "Simple vehicle spawner with optional refill-on-move and refill-on-destroyed behavior. Server-authoritative.")]
class GC_SimpleVehicleSpawnerClass : ScriptComponentClass {}

class GC_SimpleVehicleSpawner : ScriptComponent
{
	// Wrecks within this distance (m) of the spawner are deleted and replaced. Not editable.
	protected const float WRECK_CLEANUP_DISTANCE = 5.0;

	// How often (ms) the respawn condition is checked.
	protected const int CHECK_INTERVAL_MS = 3000;

	// Prefabs to spawn
	[Attribute(params: "et", desc: "List of vehicle prefabs to spawn")]
	ref array<ResourceName> m_aPrefabList;

	[Attribute("0", desc: "If true, a random prefab from the list will be chosen. If false, first prefab is always spawned.")]
	bool m_bRandomize;

	[Attribute("1", desc: "If true, respawn behavior is enabled (vehicle moved away or destroyed at spawn). If false, acts as one-time spawner only.")]
	bool m_bEnableRespawn;

	[Attribute("5.0", uiwidget: UIWidgets.Slider, params: "1 999", desc: "Distance (m). If vehicle moves further from spawn, a new one will respawn here.")]
	float m_fRespawnDistance;

	[Attribute("2.0", uiwidget: UIWidgets.Slider, params: "0.1 999", desc: "Cooldown (s) before respawning a replacement (after vehicle moved away or wreck was removed).")]
	float m_fRespawnDelay;

	[Attribute("-1", uiwidget: UIWidgets.Slider, params: "-1 1000", desc: "Maximum number of refills (-1 = infinite). Initial spawn does not count.")]
	int m_iMaxRespawns;

	protected IEntity m_SpawnedVehicle;
	protected vector m_vSpawnOrigin;
	protected bool m_bSpawnQueued;
	protected int m_iRespawnsDone;

	//------------------------------------------------------------------------------------------------
	// Server authority guard. In singleplayer / listen-server host this is true;
	// on a pure client it is false, so clients never spawn or delete anything.
	protected bool HasAuthority()
	{
		return Replication.IsServer();
	}

	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		if (SCR_Global.IsEditMode())
			return;

		// Guard first: clients must not run any spawner logic.
		if (!HasAuthority())
			return;

		Print("[GC_SimpleVehicleSpawner] OnPostInit called for " + owner.GetName());

		if (!m_aPrefabList || m_aPrefabList.IsEmpty())
		{
			Print("[GC_SimpleVehicleSpawner] No prefabs assigned, aborting.");
			return;
		}

		// Save origin
		m_vSpawnOrigin = owner.GetOrigin();

		// Spawn first immediately (does not count as a refill)
		SpawnNewVehicle(false);

		// Schedule periodic check only if respawning is enabled
		if (m_bEnableRespawn)
			GetGame().GetCallqueue().CallLater(CheckRespawnCondition, CHECK_INTERVAL_MS, true);
	}

	//------------------------------------------------------------------------------------------------
	override void OnDelete(IEntity owner)
	{
		// Stop any pending timers so they can't fire after the spawner is gone.
		ScriptCallQueue queue = GetGame().GetCallqueue();
		if (queue)
		{
			queue.Remove(CheckRespawnCondition);
			queue.Remove(DoRespawn);
		}

		super.OnDelete(owner);
	}

	//------------------------------------------------------------------------------------------------
	protected bool CanRefill()
	{
		return m_iMaxRespawns == -1 || m_iRespawnsDone < m_iMaxRespawns;
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsVehicleDestroyed(IEntity vehicle)
	{
		if (!vehicle)
			return false;

		DamageManagerComponent damageManager = DamageManagerComponent.Cast(vehicle.FindComponent(DamageManagerComponent));
		if (!damageManager)
			return false;

		return damageManager.GetState() == EDamageState.DESTROYED;
	}

	//------------------------------------------------------------------------------------------------
	protected void DeleteVehicle(IEntity vehicle)
	{
		if (!vehicle)
			return;

		// Replicated entities should be deleted through their RplComponent on the server.
		RplComponent rpl = RplComponent.Cast(vehicle.FindComponent(RplComponent));
		if (rpl)
			rpl.DeleteRplEntity(vehicle, false);
		else
			SCR_EntityHelper.DeleteEntityAndChildren(vehicle);
	}

	//------------------------------------------------------------------------------------------------
	protected void SpawnNewVehicle(bool isRefill)
	{
		if (!HasAuthority())
			return;

		if (m_bSpawnQueued) 
			return;

		// Respect max refill limit
		if (isRefill && !CanRefill())
		{
			Print("[GC_SimpleVehicleSpawner] Max respawns reached. No further vehicles will be spawned.");
			return;
		}

		int index = 0;
		if (m_bRandomize)
			index = Math.RandomInt(0, m_aPrefabList.Count());

		ResourceName prefab = m_aPrefabList[index];
		if (prefab.IsEmpty())
		{
			Print("[GC_SimpleVehicleSpawner] Invalid prefab at index " + index);
			return;
		}

		Resource resource = Resource.Load(prefab);
		if (!resource)
		{
			Print("[GC_SimpleVehicleSpawner] Failed to load resource: " + prefab);
			return;
		}

		EntitySpawnParams params = new EntitySpawnParams();
		GetOwner().GetWorldTransform(params.Transform);
		params.TransformMode = ETransformMode.WORLD;

		IEntity vehicle = GetGame().SpawnEntityPrefab(resource, GetGame().GetWorld(), params);
		if (!vehicle)
		{
			Print("[GC_SimpleVehicleSpawner] Spawn failed for " + prefab);
			return;
		}

		m_SpawnedVehicle = vehicle;

		// Count only refills, not the first spawn
		if (isRefill)
			m_iRespawnsDone++;

		string maxText;
		if (m_iMaxRespawns == -1)
			maxText = "∞";
		else
			maxText = m_iMaxRespawns.ToString();

		Print("[GC_SimpleVehicleSpawner] Spawned vehicle: " + vehicle.GetName() + " (Respawns done: " + m_iRespawnsDone + "/" + maxText + ")");
	}

	//------------------------------------------------------------------------------------------------
	protected void CheckRespawnCondition()
	{
		if (!HasAuthority())
			return;

		if (!m_bEnableRespawn || m_bSpawnQueued)
			return;

		// Case 0: the tracked vehicle no longer exists (deleted by an admin tool, cleanup script, etc.)
		// Treat the slot as empty and refill it, so the spawner never goes dormant.
		if (!m_SpawnedVehicle)
		{
			if (!CanRefill())
				return;

			Print("[GC_SimpleVehicleSpawner] Tracked vehicle no longer exists. Scheduling respawn.");
			m_bSpawnQueued = true;
			GetGame().GetCallqueue().CallLater(DoRespawn, m_fRespawnDelay * 1000, false);
			return;
		}

		float distance = vector.DistanceXZ(m_SpawnedVehicle.GetOrigin(), m_vSpawnOrigin);

		// Case 1: vehicle was destroyed at (or near) its spawn point -> remove the wreck, then respawn.
		if (IsVehicleDestroyed(m_SpawnedVehicle) && distance <= WRECK_CLEANUP_DISTANCE)
		{
			// Don't delete the wreck if we're not allowed to replace it anyway.
			if (!CanRefill())
				return;

			Print("[GC_SimpleVehicleSpawner] Vehicle destroyed at spawn point (" + distance + "m). Deleting wreck and scheduling respawn.");

			DeleteVehicle(m_SpawnedVehicle);
			m_SpawnedVehicle = null;

			m_bSpawnQueued = true;
			GetGame().GetCallqueue().CallLater(DoRespawn, m_fRespawnDelay * 1000, false);
			return;
		}

		// Case 2: vehicle was driven/moved away -> leave it alone and refill the slot.
		if (distance > m_fRespawnDistance)
		{
			Print("[GC_SimpleVehicleSpawner] Vehicle moved " + distance + "m, scheduling respawn.");
			m_bSpawnQueued = true;
			GetGame().GetCallqueue().CallLater(DoRespawn, m_fRespawnDelay * 1000, false);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected void DoRespawn()
	{
		if (!HasAuthority())
			return;

		m_bSpawnQueued = false;

		// If an old (moved) vehicle still exists, leave it alone. We just refill the slot.
		SpawnNewVehicle(true);
	}
}
