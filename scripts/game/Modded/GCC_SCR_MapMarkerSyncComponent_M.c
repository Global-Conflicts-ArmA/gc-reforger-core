modded class SCR_MapMarkerSyncComponent
{
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		m_bIsDeleteRestricted = false;
	}
}