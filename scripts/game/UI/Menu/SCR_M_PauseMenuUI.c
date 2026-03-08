//------------------------------------------------------------------------------------------------
modded class PauseMenuUI
{	
	override void OnMenuOpen()
	{
		super.OnMenuOpen();
		SCR_ButtonTextComponent comp;
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		m_wRoot = GetRootWidget();
		comp = SCR_ButtonTextComponent.GetButtonText("Respawn", m_wRoot);
		if (comp)
		{			
			bool canRespawn;
			if (gameMode)
			{
				canRespawn = (CanRespawn());
			}

			comp.GetRootWidget().SetVisible(canRespawn);
			comp.SetVisible(canRespawn);
			comp.m_OnClicked.Clear();
			comp.m_OnClicked.Insert(OnRespawn);
		}
		
	}
}
