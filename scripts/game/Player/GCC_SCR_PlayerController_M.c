modded class SCR_PlayerController : PlayerController
{
	void GCC_ArtyFire(string faction, int index, int roundCount, int gunCount, int roundDispersion, vector position)
	{
		Rpc(GCC_RpcAsk_ArtyFire, faction, index, roundCount, gunCount, roundDispersion, position);
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	protected void GCC_RpcAsk_ArtyFire(string faction, int index, int roundCount, int gunCount, int roundDispersion, vector position)
	{
		GCC_Arty manager = GCC_Arty.GetInstance();
		if(!manager)
			return;
		
		manager.CallMission(faction, index, roundCount, gunCount, roundDispersion, position);
	}
}