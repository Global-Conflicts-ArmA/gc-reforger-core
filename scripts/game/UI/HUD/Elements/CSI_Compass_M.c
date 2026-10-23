modded class CSI_Compass : SCR_ScriptedWidgetComponent
{
	override void Update()
	{		
		int yawInt;
        float yaw = m_HUDSystem.GetLocalYaw();
		yawInt = -yaw;
		
		if (m_SettingsManager.GetSettingBool(CSI_GameSettings.COMPASS_VISIBLE))
		{
			CSI_ECompassTheme compassTheme = m_SettingsManager.GetSettingInt(CSI_GameSettings.COMPASS_THEME);
			string compassImage;
			switch (compassTheme)
			{
				default : compassImage = CSI_UIHelper.CSI_STANDARD_COMPASS; break;
			};
			
			if (m_sStoredCompass != compassImage)
			{
				m_wCompass.LoadImageTexture(0, compassImage);
				m_sStoredCompass = compassImage;
			};
			
			m_wCompass.SetVisible(true);
			
			if (m_fStoredYaw != yaw)
			{
				m_wCompass.SetRotation(yaw);
				m_fStoredYaw = yaw;
			};
		} else 
			m_wCompass.SetVisible(false);

		PlayerController pc = GetGame().GetPlayerController();
		if (!pc)
			return;
	
		BaseCompartmentSlot compartment; 
		IEntity player = pc.GetControlledEntity();
		if (player)
			compartment = CSI_EntityHelper.GetCharacterVehicleCompartment(player);
		if (m_SettingsManager.GetSettingBool(CSI_GameSettings.BEARING_VISIBLE) && compartment)

		{
			string bearingAdd = "";
			
			if (yawInt < 0)
				yawInt = 360 - Math.AbsInt(yawInt);
			
			if (yawInt >= 0 & yawInt < 10)
				bearingAdd = "00";
			
			if (yawInt >= 10 & yawInt < 100)
				bearingAdd = "0";
			
			m_wBearing.SetText(bearingAdd + (yawInt.ToString()));
		} else
			m_wBearing.SetText("");
	}
}
