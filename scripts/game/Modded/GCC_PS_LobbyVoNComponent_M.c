modded class PS_LobbyVoNComponent
{
	
	override void DisablePhysicForOwner(IEntity owner)
	{
		Physics physics = owner.GetPhysics();
		if (physics)
		{
			physics.SetVelocity("0 0 0");
			physics.SetAngularVelocity("0 0 0");
			physics.SetMass(0);
			physics.SetDamping(1, 1);
			//physics.ChangeSimulationState(SimulationState.NONE);
			physics.SetActive(ActiveState.INACTIVE);
			physics.SetInteractionLayer(EPhysicsLayerDefs.Unused);
		}
	}
}