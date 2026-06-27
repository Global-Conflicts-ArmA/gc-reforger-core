[EntityEditorProps(category: "GameScripted/Sound")]
class GC_SoundsClass : ScriptComponentClass
{
}

class GC_Sounds : ScriptComponent
{
	[Attribute("", UIWidgets.ResourceNamePicker, "Sound project .acp file", "acp")]
	protected ResourceName m_sSoundProject;

	[Attribute("", UIWidgets.Auto, "Sound event name inside the .acp project.")]
	protected string m_sSoundEvent;
	
	[Attribute("", UIWidgets.Auto, "Volume level")]
	protected float m_fVolumeLevel;
	
	protected AudioHandle m_hSound = AudioHandle.Invalid;
	
	static const string GC_VAR_NAME   = "Volume";
	static const string GC_VAR_CONFIG = "{7832629955842580}Sounds/Ambience/GC_LocalVars.conf";
	
	//------------------------------------------------------------------------------------------------
	void PlaySoundAtOwner()
	{
		IEntity owner = GetOwner();
		if (!owner)
			return;

		PlaySoundAt(owner.GetOrigin());
	}

	void PlaySoundAt(vector position)
	{
		if (m_sSoundProject.IsEmpty())
		{
			Print("GC | Sound project is empty", LogLevel.WARNING);
			return;
		}
	
		if (m_sSoundEvent.IsEmpty())
		{
			Print("GC | Sound event is empty", LogLevel.WARNING);
			return;
		}
	
		vector mat[4];
		Math3D.MatrixIdentity4(mat);
		mat[3] = position;
	
		PrintFormat("GC | Project: %1", m_sSoundProject);
		PrintFormat("GC | Event: %1", m_sSoundEvent);
		PrintFormat("GC | Position: %1", position);
	
		m_hSound = AudioSystem.PlayEvent(m_sSoundProject, m_sSoundEvent, mat);
	
		if (m_hSound == AudioHandle.Invalid)
		{
			Print("GC | Failed to play basic sound event", LogLevel.WARNING);
			return;
		}
	
		Print("GC | Basic sound event played");
	}

	void StopSound(float fadeTime = 0.25)
	{
		if (m_hSound == AudioHandle.Invalid)
			return;

		AudioSystem.TerminateSoundFadeOut(m_hSound, true, fadeTime);
		m_hSound = AudioHandle.Invalid;
	}
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (SCR_Global.IsEditMode())
			return;
		
		GetGame().GetCallqueue().CallLater(PlaySoundAtOwner, 100, false);
	}
}