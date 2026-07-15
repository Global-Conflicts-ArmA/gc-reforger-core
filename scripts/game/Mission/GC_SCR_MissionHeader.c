modded class SCR_MissionHeader
{
	[Attribute(desc: "Metadata for the GC Website - should be defined for all regular GC missions")]
	protected ref GC_MissionMetadata m_GC_Metadata;
}

[BaseContainerProps()]
class GC_MissionMetadata
{
	[Attribute(defvalue: "", desc: "The faction with the MOST PLAYER SLOTS. If equal, either.")]
	protected GC_ForceDefinition m_FriendlyForce;
	
	[Attribute(defvalue: "", desc: "The faction OPPOSED BY the friendly force")]
	protected GC_ForceDefinition m_OpposingForce;
	
	[Attribute(defvalue: "", desc: "Historical TIME PERIOD of the mission", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(GC_EraTag))]
	protected GC_EraTag m_eEra;
	
	[Attribute(desc: "Uncommon PLAYER-USED assets featured in the mission, like e. g. mortars or tanks", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(GC_AssetTag))]
	protected ref array<GC_AssetTag> m_eSpecialAssets;
	
	[Attribute(defvalue: "", desc: "OPTIONAL. This allows grouping of very similar missions (e. g. Roulette, an XL edition of another mission, or a version with a gameplay tweak.")]
	protected GC_MissionGroupDefinition m_MissionGroup;

}

[BaseContainerProps()]
class GC_ForceDefinition
{
	[Attribute(defvalue: "", desc: "Faction Key of the force")]
	protected string m_sFactionKey;
	
	[Attribute(defvalue: "", desc: "Overall unit type of the force", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(GC_ForceType))]
	protected GC_ForceType m_eType;
}

[BaseContainerProps(configRoot: true)]
class GC_MissionGroupDefinition
{
	[Attribute(defvalue: "", desc: "Shared identifier of the mission group (e. g. 'Roulette')")]
	protected string m_sGroupID;
	
	[Attribute(defvalue: "1", desc: "How similar this mission is to others in the group (0-1). 1 = very similar with only small differences. 0 = very different, could be played right after another mission from the group without complaints.", params: "0 1")]
	protected float m_fGroupSimilarity;
}


enum GC_EraTag
{
	ERA_OTHER,
	ERA_1970s,
	ERA_1980s,
	ERA_1990s,
	ERA_2000s,
	ERA_2010s,
	ERA_2020s,
	ERA_2030s
}

enum GC_ForceType
{
	OTHER,
	LIGHT,
	MOTORIZED,
	MECHANIZED,
	ARMORED,
	AIRMOBILE,
	AIRBORNE,
	COMBINED
}

enum GC_AssetTag
{
	UNDEFINED,
	TANKS,
	ATK_HELIS,
	MORTARS,
	ARTILLERY,
	DRONES,
	BOATS
}