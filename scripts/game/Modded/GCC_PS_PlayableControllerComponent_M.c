modded class PS_PlayableControllerComponent
{
	override void SwitchToObserver(IEntity from)
	{
		super.SwitchToObserver(from);

		GetGame().GetCallqueue().CallLater(UpdateSpectator, 100, true);
	}
	
	override void SwitchFromObserver()
	{
		super.SwitchFromObserver();

		GetGame().GetCallqueue().Remove(UpdateSpectator);
	}
	
	void UpdateSpectator()
	{
		PlayerController playerController = GetGame().GetPlayerController();
		PS_PlayableControllerComponent playableController = PS_PlayableControllerComponent.Cast(playerController.FindComponent(PS_PlayableControllerComponent));
		playableController.VoNPosition = m_eCamera.GetOrigin();
	}
}