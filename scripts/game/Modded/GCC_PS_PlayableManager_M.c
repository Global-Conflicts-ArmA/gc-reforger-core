modded class PS_PlayableManager
{
	[Attribute("1")]
	protected bool m_hidePlayersOnSpawn;
	
	override void RegisterPlayable(PS_PlayableComponent playableComponent)
	{
		super.RegisterPlayable(playableComponent);
		
		if(!m_hidePlayersOnSpawn)
			return;
		
		if (Replication.IsServer())
		{
			IEntity entity = playableComponent.GetOwner();
			Physics physics = entity.GetPhysics();
			if (physics)
			{
				physics.EnableGravity(false);
				entity.SetOrigin(entity.GetOrigin() + vector.Up * 100000);
			}
		}
	}
	
	override void ApplyPlayable(int playerId)
	{
		super.ApplyPlayable(playerId);
		
		if(!m_hidePlayersOnSpawn)
			return;
		
		PlayerManager playerManager = GetGame().GetPlayerManager();
		SCR_PlayerController playerController = SCR_PlayerController.Cast(playerManager.GetPlayerController(playerId));
		if (!playerController)
			return;
		
		RplId playableId = GetPlayableByPlayer(playerId);
		if(!playableId)
			return;
		IEntity entity = GetPlayableById(playableId).GetOwner();	
		if(!entity)
			return;
		
		PS_PlayableComponent playableComp = PS_PlayableComponent.Cast(entity.FindComponent(PS_PlayableComponent));
		Physics physics = entity.GetPhysics();
		
		vector transform[4];
		if (playableComp && physics)
		{
			playableComp.GetSpawnTransform(transform);
			entity.SetOrigin(transform[3]);
			physics.EnableGravity(true);
			entity.Update();
		}
		
		playerController.Teleport(transform[3]);
	}
}