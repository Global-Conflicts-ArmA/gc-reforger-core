modded class PS_GameModeCoop : SCR_BaseGameMode
{
	[Attribute("300", UIWidgets.Auto, desc: "How many seconds into the game can trenches be built more quickly", category: "Trenches", params: "0 inf")]
	protected float m_fPrepDuration;
	
	[Attribute("2", UIWidgets.Auto, desc: "Multiplier that affects trench building speed during the prep phase", category: "Trenches", params: "0.5 inf")]
	protected float m_fPrepMultiplier;
	
	[Attribute("10", UIWidgets.Auto, desc: "Multiplier that affects trench building speed after prep phase", category: "Trenches", params: "0.5 inf")]
	protected float m_fGameMultiplier;
	
	[RplProp()]
	protected bool m_bInPrepPhase = true;

	// SAFE START (moved here from the TILW mission framework)
	[Attribute("60", UIWidgets.Object, desc: "How many seconds should safe start last?", category: "Reforger Lobby", params: "0 inf 0")]
	int m_safeStartTime;
	
	override void OnGameStart()
	{
		super.OnGameStart();
		GetOnPlayerConnected().Insert(TILW_OnPlayerConnected);
	}

	protected void TILW_OnPlayerConnected(int playerId)
	{
		GetGame().GetCallqueue().CallLater(ShowJIPInfo, 7.5 * 1000, false, playerId);
	}
	
	protected void ShowJIPInfo(int playerId)
	{
		if (GetState() != SCR_EGameModeState.GAME || PS_PlayableManager.GetInstance().GetPlayableByPlayer(playerId) != RplId.Invalid())
			return;
		
		SCR_PlayerController pc = SCR_PlayerController.TILW_GetPCFromPID(playerId);
		if (!pc) return;
		
		if (m_bTeamSwitch && !m_bRemoveRedundantUnits)
			pc.TILW_ShowJIPInfo();
		else
			pc.TILW_SendHintToPlayer("JIP Not Available", "This mission does not support join-in-progress, please wait until the next mission starts.", 10);
	}
	
	override void OnGameStateChanged()
	{
		super.OnGameStateChanged();
		
		SCR_EGameModeState state = GetState();
		
		SetupTimer();
		
		if (m_bInPrepPhase && state == SCR_EGameModeState.GAME && Replication.IsServer())
			GetGame().GetCallqueue().CallLater(TogglePrepPhase, m_fPrepDuration * 1000, false, false);
	}
	
	void TogglePrepPhase(bool enabled)
	{
		if (m_bInPrepPhase == enabled)
			return;
		m_bInPrepPhase = enabled;
		Replication.BumpMe();
	}
	
	float GetActiveTrenchMultiplier()
	{
		if (m_bInPrepPhase)
			return m_fPrepMultiplier;
		return m_fGameMultiplier;
	}
	
	int m_briefingTimer = 0;
	protected void SetupTimer()
	{
		SCR_EGameModeState state = GetState();
		if(state == SCR_EGameModeState.GAME)
			return GetGame().GetCallqueue().Remove(UpdateTimer);
		
		if(state != SCR_EGameModeState.BRIEFING)
			return;

		GetGame().GetCallqueue().CallLater(UpdateTimer, 1000, true);
	}
	
	protected void UpdateTimer()
	{
		m_briefingTimer++;
	}
	
	override bool RplSave(ScriptBitWriter writer)
	{
		super.RplSave(writer);
		
		writer.WriteInt(m_briefingTimer);
		
		return true;
	}

	override bool RplLoad(ScriptBitReader reader)
	{
		super.RplLoad(reader);
		
		reader.ReadInt(m_briefingTimer);

		return true;
	}
}

//! TILW_TogglePrepPhaseInstruction turns the prep phase on or off
[BaseContainerProps(), BaseContainerCustomStringTitleField("Toggle Prep Phase")]
class TILW_TogglePrepPhaseInstruction : TILW_BaseInstruction
{
	[Attribute("1", UIWidgets.Auto, desc: "Manually turn the prep phase on or off, this affects trench building speed.")]
	protected bool m_bPrepPhaseEnabled;
	
	override void Execute()
	{
		PS_GameModeCoop gm = PS_GameModeCoop.Cast(GetGame().GetGameMode());
		if (!gm)
			return;
		gm.TogglePrepPhase(m_bPrepPhaseEnabled);
	}
}