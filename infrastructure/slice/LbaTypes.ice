#ifndef LBANET_TYPES
#define LBANET_TYPES

[["java:package:net.lbanet.generated"]]
module LbaNet
{
	enum ModelStance { StanceNormal, StanceSporty, StanceAggresive, StanceDiscrete, StanceProtopack, StanceHorse, StanceDinofly };		
	
	
	enum ModelState { NoState, StNormal, StDying, StDrowning, StDrowningGas, StBurning, 
				StSmallHurt, StMediumHurt, StBigHurt, StHurtFall, StFinishedFall, StFalling, 
				StJumping, StMovingObject, StUseWeapon, StImmune, StHidden, StScripted,
				StProtectedHurt, StRestrictedMovingObject, StFighting, StPrepareWeapon,
				StActivateSwitch, StActivateGroundSwitch, StHappy };	
				
				
	
	enum RenderTypeEnum { RenderOsgModel, RenderSprite, RenderLba1M, RenderLba2M, RenderCross, RenderBox, RenderCapsule, RenderSphere, RenderBGImage, RenderParticle, NoRender };	
	
	enum PhysicalShapeEnum { NoShape, BoxShape, CapsuleShape, SphereShape, TriangleMeshShape };	
	
	enum PhysicalActorType { StaticAType, KynematicAType, DynamicAType, CharControlAType };	

	enum Lba1MColorPart { PolygonColor, SphereColor, LineColor };	
	
	enum ItemClientInformType { DontInform, InformChat, InformHappy };
	
	enum ParticleType { ParticleExplosion, ParticleExplosionDebris, ParticleSmoke, ParticleSmokeTrail, ParticleFire, ParticleCustom };	
	
	
	// give information about a world
	struct WorldDesc
	{
		string 			WorldName;
		string 			Description;
		string 			News;
		long			Version;
	};
	
	sequence<WorldDesc>		WorldsSeq;
	
	
	
	dictionary<long, string> 	TeleportsSeq;
	dictionary<string, string> 	FilesSeq;


	struct ItemGroupElement
	{
		long 			Id;
		int 			Min;
		int 			Max;
		float 			Probability;
		int 			Group;
	};
	sequence<ItemGroupElement>	ContainedItemList;
	
	
	// give information about an item
	struct ItemInfo
	{
		long 			Id;
		string 			Name;		
		string 			IconName;
		long 			NameTextId;		
		long 			DescriptionId;
		long 			LongDescriptionId;		
		int 			Max;
		int 			BuyPrice;
		int 			SellPrice;		
		string 			DescriptionTextExtra;
		int			Type;
		float			Effect;
		float			Effect2;		
		int			Flag;
		bool			Ephemere;
		string			StringFlag;
		int			Color1;
		int			Color2;
		ContainedItemList	List;
		long			ReplaceItem;
	};
	
	// give information about an item position in inventory
	struct ItemPosInfo
	{
		ItemInfo		Info;	
		int 			Count;
		int 			Position;
	};
		
	// map an item with a position in display grid
	dictionary<long, ItemPosInfo> 	ItemsMap;
		
	sequence<ItemInfo> 		ShortcutsSeq;

	dictionary<long, int> 		ItemList;

	
	// map information
	struct MapInfo
	{
		string 			Name;
		string 			Description;
		string			Type;
		
		// 1 - orthogonal
		// 2 - perspective
		// 3 - free 3d
		int			AutoCameraType;
		
		bool			IsInstance;
		
		float			HurtFallFactor;
		
		string			Music;
		int			Repeat;
		
		long			LinkedHoloLocation;

	};	
	dictionary<string, MapInfo> 	MapsSeq;
	
	

	struct Lba1ColorIndex
	{
		Lba1MColorPart 	ModelPart;
		int		Color;
	};
	
	dictionary<Lba1ColorIndex, int> 		Lba1ColorChangeSeq;
	
	
	
	
	// extra info not used by all models
	class ModelExtraInfoBase
	{
	};
	
		
	// extra particle info
	class ParticleExtraInfoBase
	{
	};

	class ModelExtraInfoParticle extends ModelExtraInfoBase
	{
		ParticleType		Type;
		ParticleExtraInfoBase	Info;
	};
	
		
	class PredefinedParticleInfo extends ParticleExtraInfoBase
	{
		float			WindX;
		float			WindY;		
		float			WindZ;
		float			Scale;
		float			Intensity;
		double			EmitterDuration;
		double			ParticleDuration;
		string			CustomTexture;
	};	

	
	
	
	struct ModelInfo
	{
		RenderTypeEnum		TypeRenderer;
		int			ModelId;	// id for lockup object description in file
		string			ModelName;	// depends of rendertype - if empty then use ModelId
		string			Outfit;		// only for animated models
		string			Weapon;		// only for animated models
		string			Mode;		// only for animated models
		ModelState		State;
		bool			UseLight;
		bool			CastShadow;		
		
		float			TransX;
		float			TransY;		
		float			TransZ;	
		
		float			ScaleX;
		float			ScaleY;		
		float			ScaleZ;			
		
		float			RotX;
		float			RotY;		
		float			RotZ;		
		
		
		float			ColorR;
		float			ColorG;
		float			ColorB;
		float			ColorA;			
		
		Lba1ColorChangeSeq	ColorSwaps;	// only fo lba1 models
		
		int			SkinColor;
		int			EyesColor;
		int			HairColor;
		int			OutfitColor;
		int			WeaponColor;
		
		int			MountSkinColor;
		int			MountHairColor;
		
		bool			UseBillboard;
		
		bool			UseTransparentMaterial;
		float			MatAlpha;
		
		int			ColorMaterialType;  //0 -> OFF    1 -> AMBIENT   2 -> DIFFUSE   3 -> SPECULAR   4 -> EMISSION    5 -> AMBIENT_AND_DIFFUSE
		
		float			MatAmbientColorR;
		float			MatAmbientColorG;
		float			MatAmbientColorB;
		float			MatAmbientColorA;			
		
		float			MatDiffuseColorR;
		float			MatDiffuseColorG;
		float			MatDiffuseColorB;
		float			MatDiffuseColorA;
		
		float			MatSpecularColorR;
		float			MatSpecularColorG;
		float			MatSpecularColorB;
		float			MatSpecularColorA;		
		
		float			MatEmissionColorR;
		float			MatEmissionColorG;
		float			MatEmissionColorB;
		float			MatEmissionColorA;
		
		float			MatShininess;	
		ModelExtraInfoBase      ExtraInfo;		
	};

	struct PlayerStartingInfo
	{
		int			InventorySize;
		float			StartingLife;
		float			StartingMana;
		string			StartingMap;
		long			SpawningId;
		ModelInfo		StartingModel;
		
		ItemsMap		StartingInventory;
		
		string			StartingScript;
	};


	
	
	// give full information about a world
	struct WorldInformation
	{
		WorldDesc 		Description;
		MapsSeq			Maps;
		
		PlayerStartingInfo	StartingInfo;
	};	
	
	
	
	
	
	
	// give information about a friend of a player
	struct FriendInfo
	{
		long 			Id;
		bool 			Pending;
		bool 			ToAccept;
		string 			Name;
		bool 			Removed;
	};
	
	sequence<FriendInfo> 		FriendsSeq;
	
	// give information about a private message received by player
	struct PMInfo
	{
		long 			PMId;
		long 			ReplyId;
		string 			FromName;
		string 			ToName;
		string 			Text;
		bool 			Opened;
		string 			Date;
	};
	
	sequence<PMInfo> 		PMsSeq;
	

	
	
	// give information about a player status
	struct PlayerOnline
	{
		bool 			Joined;
		string 			Nickname;
		string 			Status;
		string 			Color;
		string			Location;
	};
	
	sequence<PlayerOnline> 		PlayerOnlinesSeq;



	struct PlayerPosition
	{
		// actor name
		string			MapName;

		// actor coordinates
		float			X;
		float			Y;
		float			Z;
		float			Rotation;
	};

	

	
	struct InventoryInfo
	{
		int 			InventorySize;
		ItemsMap 		InventoryStructure;
		ShortcutsSeq 		UsedShorcuts;
	};
	
	struct LifeManaInfo
	{
		float			CurrentLife;
		float			MaxLife;
		float			CurrentMana;
		float			MaxMana;
		float			CurrentArmor;
		bool			Display;	
	};	
	
	
	struct SavedWorldInfo
	{
		PlayerPosition 		ppos;
		InventoryInfo 		inventory;
		ModelInfo		model;
		
		// life/mana info
		LifeManaInfo		lifemana;
		
		long			EquipedWeapon;
		long			EquipedOutfit;
		long			EquipedMount;		
	};	
	

	struct ObjectExtraInfo
	{
		string			Name;
		float			NameColorR;
		float			NameColorG;		
		float			NameColorB;
		bool			Display;
	};		
	
	
	
	
	// give information about a letter written by a player	
	struct LetterInfo
	{
		long 			LetterId;
		string 			Date;
		string 			From;
		string 			Subject;
		string 			Message;
	};
	
		
	// give information about a quest
	struct QuestInfo
	{
		long			Id;
		long 			ChapterTextId;		
		long 			QuestAreaTextId;	
		long	 		TittleTextId;
		long 			DescriptionTextId;
		
		bool			Visible;
	};
	
	dictionary<long, QuestInfo> 	QuestsMap;	
	
	
	// dialog part info
	sequence<long> 			PlayerChoicesSeq;	
	struct DialogPartInfo
	{
		long			NpcTextId;
		PlayerChoicesSeq 	PlayerTextsId;		
	};
	
	


	// player status information
	struct PlayerInfo
	{
		long 			Id;
		string 			Status;
		string 			NameColor;
		string			Location;
	};


	// list of connected players
	dictionary<string, PlayerInfo> ConnectedL;
	
	
	


	// physical object description
	struct ObjectPhysicDesc
	{
		PlayerPosition		Pos;
		
		// 0 = no shape
		// 1 = Box
		// 2 = Capsule
		// 3 = Sphere
		// 4 = triangle mesh
		PhysicalShapeEnum 	TypeShape;	
		
		// 1= static
		// 2= kynematic
		// 3= dynamic
		// 4= character controller
		PhysicalActorType 	TypePhysO;

		// density of the object
		float 			Density;

		// flag if object is collidable or not
		bool 			Collidable;
		
		
		// used for boxes
		float 			SizeX;	// for capsule radius = SizeX/2
		float 			SizeY;	// for sphere radius = SizeY/2
		float 			SizeZ;
		
		// used for triangle mesh
		string			Filename;
		
		
		float			Bounciness;
		float			StaticFriction; 
		float			DynamicFriction; 
		
		
		// used by movable object
		bool			AllowFreeMove;
	};



	struct PlayerMoveInfo
	{
		bool			ForcedChange;	// used when something changed (e.g. speed, animation)
		
		
		PlayerPosition		CurrentPos;
		
		float			CurrentSpeedX;
		float			CurrentSpeedY;		
		float			CurrentSpeedZ;	
		float			CurrentSpeedRotation;			
		
		string			AnimationIdx;
	};
	
	

	// playing sound information
	struct PlayingSound
	{
		int			SoundChannel;
		string 			SoundPath;
		int			NbTime;
		bool			RandomPitch;
		bool 			Paused;
	};
	
	sequence<PlayingSound> 		PlayingSoundSequence;
	




	// base class used to give client GUI parameters on how to display
	class GuiParameterBase
	{
	};
	
	sequence<GuiParameterBase> GuiParamsSeq;
	
	
	// server send inventory content to clien	
	class DisplayGameText extends GuiParameterBase
	{
		long 			Textid;	
	};
	
	// server send inventory content to clien	
	class InventoryGuiParameter extends GuiParameterBase
	{
		int			InventorySize;
		ItemsMap		InventoryContent;	
	};
	
	// server send container content to client	
	class ContainerGuiParameter extends GuiParameterBase
	{
		int			InventorySize;		
		ItemsMap		ContainerContent;
		ItemsMap		InventoryContent;	
	};
	
	// server send list of item in shop to client
	class ShopGuiParameter extends GuiParameterBase
	{
		ItemsMap		ShopContent;
		string			MoneyIconName;
	};
	
	
	// server send list of mails to display to client
	class MailboxGuiParameter extends GuiParameterBase
	{
		PMsSeq			MailboxContent;
	};	
	
	
	// server send updated friend list to client
	class FriendListGuiParameter extends GuiParameterBase
	{
		FriendsSeq		FriendList;
	};	
	
	// server send list of player online to client
	class PlayerOnlineListGuiParameter extends GuiParameterBase
	{
		PlayerOnlinesSeq	PlayerList;
	};	
	
	
	// server send list of teleport places to client
	class TeleportGuiParameter extends GuiParameterBase
	{
		TeleportsSeq		TeleportList;
	};
	
	// server send list of shortcut to client
	class ShortcutGuiParameter extends GuiParameterBase
	{
		ShortcutsSeq		ShorcutList;
	};	
	
	// server send letter to display to client	
	class LetterViewerGuiParameter extends GuiParameterBase
	{
		LetterInfo		ToDisplayInfo;
	};
	
	// server send letter to display to client	
	class QuestJournalGuiParameter extends GuiParameterBase
	{
		QuestsMap		QuestsStarted;
		QuestsMap		QuestsFinished;		
	};

	
	// server send dialog to display to client	
	class DialogGuiParameter extends GuiParameterBase
	{
		long			NpcActorId;
		long			NpcNameTextId;
		bool			SimpleDialog;
		DialogPartInfo		DialogPart;
	};










	// base class used to exchange updates between client and server concerning GUI
	class GuiUpdateBase
	{
	};
			
	sequence<GuiUpdateBase> GuiUpdatesSeq;	
	
	
	
	// client add a friend
	class AddFriend extends GuiUpdateBase
	{
		string			FriendName;
	};
	
	// client accept a friend
	class AcceptFriend extends GuiUpdateBase
	{
		long			FriendId;
	};	
	
	// client remove a friend
	class RemoveFriend extends GuiUpdateBase
	{
		long			FriendId;
	};		
	
	// client ask server to get a refreshed list of friends
	class RefreshFriends extends GuiUpdateBase
	{
	};
	
	// server send an update from a specific friend
	class FriendChangedUpdate extends GuiUpdateBase
	{
		FriendInfo		NewInfo;
	};	
	
	// server send inventory size to client
	class UpdateInventorySize extends GuiUpdateBase
	{
		int			NewSize;
	};
	
	// server send inventory update to client
	class UpdateInventoryItem extends GuiUpdateBase
	{
		ItemClientInformType	Informtype;
		ItemPosInfo		Info;
	};	
	
	// client update item position
	class SwitchItemPosition extends GuiUpdateBase
	{
		long			ItemId;
		int			NewPosition;
	};
	
	// client tell server than an object is used
	class InventoryItemUsed extends GuiUpdateBase
	{
		long			ItemId;
	};	
	
	
	// server update player online info
	class PlayerStatusChanged extends GuiUpdateBase
	{
		PlayerOnline		NewInfo;
	};
	
	
	// client tell server to update container content
	class UpdateInvContainer extends GuiUpdateBase
	{
		LbaNet::ItemList 	Taken;
		LbaNet::ItemList 	Put;
	};

	// server ask client to display a system message
	class SystemMessageUpdate extends GuiUpdateBase
	{
		string			Title;
		string			Message;		
	};
	

	// client tell server that shortcut has changed
	class ShortcutUpdate extends GuiUpdateBase
	{
		int			Position;
		long			ItemId;		
	};	

	// client tell server to destroy an item in inventory
	class DestroyItemUpdate extends GuiUpdateBase
	{
		long			ItemId;		
	};

	// client tell server to create a new written letter
	class CreateLetterUpdate extends GuiUpdateBase
	{
		string			Subject;
		string			Message;
	};
	

	// client send PM
	class SendPMUpdate extends GuiUpdateBase
	{
		PMInfo			PM;
	};	
	

	// client delete PM
	class DeletePMUpdate extends GuiUpdateBase
	{
		long			PMid;
	};
	

	// client mark read PM
	class MarkReadPMUpdate extends GuiUpdateBase
	{
		long			PMid;
	};
	

	// server send quest update to client
	class QuestUpdate extends GuiUpdateBase
	{
		QuestInfo		Quest;
		bool			Finished;
	};
	
	
	// client tell server that a dialog part was selected
	class DialogSelectedUpdate extends GuiUpdateBase
	{
		int			SelectedId;
	};
	
	// server update dialog to display to client
	class DialogUpdate extends GuiUpdateBase
	{
		DialogPartInfo		DialogPartUpdate;
	};
	
	
	// client inform server that the gui has been closed
	class GuiClosedUpdate extends GuiUpdateBase
	{
	};
	
	
	// server tell client how much money he has left
	class PlayerMoneyUpdate extends GuiUpdateBase
	{
		int			Quantity;
	};
	
	// client tell server to buy an item
	class BuyItemUpdate extends GuiUpdateBase
	{
		long			ItemId;
	};	
	
	
	
	// text received by client from server
	class ChatTextUpdate extends GuiUpdateBase 
	{
		string			Channel;
		string			Sender;
	    	string			Text;
	};
	
	//class used when a player as changed name color	
	class NameColorChangedUpdate extends GuiUpdateBase 
	{
		string 			Name;
		string 			Color;
	};
	
	
	
	// base class used to exchange updates between client and server concerning object to display on screen
	class DisplayObjectUpdateBase
	{
	};
	
	// update the model of on object
	class ModelUpdate extends DisplayObjectUpdateBase
	{
		ModelInfo		Info;
		
		// specify wether the update comes from the player or the server
		bool			UpdateFromPlayer;
	};
	
	// update the object animation using animation string
	class AnimationStringUpdate extends DisplayObjectUpdateBase
	{
		string			Animation;
	};
	
	// pause the object animation
	class PauseAnimationUpdate extends DisplayObjectUpdateBase
	{
		bool			Pause;
	};
	
	
	
	// server inform clients that extra info changed
	class ObjectExtraInfoUpdate extends DisplayObjectUpdateBase
	{
		ObjectExtraInfo			Update;
	};
	
	
	// server inform clients that life info changed
	class ObjectLifeInfoUpdate extends DisplayObjectUpdateBase
	{
		LifeManaInfo			Update;
	};
	
	
	// server inform clients that life info changed
	class Lba1ModelColorUpdate extends DisplayObjectUpdateBase
	{
		Lba1MColorPart			Colorpart;
		int				OldColor;
		int				NewColor;		
	};
	
	// server inform clients that life info changed
	class ObjectColorUpdate extends DisplayObjectUpdateBase
	{
		float				R;
		float				G;
		float				B;
		float				A;		
	};	
	
	// server inform clients that life info changed
	class ObjectAlphaColorUpdate extends DisplayObjectUpdateBase
	{
		float				A;		
	};	
	
	
	// base class for all editor update
	class EditorUpdateBase
	{
	};
	
	// base class used to exchange updates between client and server concerning physic info of an object
	class PhysicObjectUpdateBase
	{
	};
	
	// server inform clients that position changed
	class PositionUpdate extends PhysicObjectUpdateBase
	{
		PlayerPosition			Update;
	};	
		
	// server inform clients that size changed
	class SizeUpdate extends PhysicObjectUpdateBase
	{
		float				SizeX;
		float				SizeY;
		float				SizeZ;		
	};
	
	
	
		
	// base class for all editor update
	class NpcUpdateBase
	{
	};
	
	
	class StraightWalkToNpcUpd extends NpcUpdateBase
	{
		float PosX;
		float PosY;
		float PosZ;
	};
	
	class GoToNpcUpd extends NpcUpdateBase
	{
		float PosX;
		float PosY;
		float PosZ;
		float Speed;
	};	
	
	class RotateNpcUpd extends NpcUpdateBase
	{
		float Angle;
		float RotationSpeedPerSec;
		bool ManageAnimation;
	};
	
	class AnimateNpcUpd extends NpcUpdateBase
	{
		bool AnimationMove;
		int NbAnimation;
	};
	
	
	class RotateFromPointNpcUpd extends NpcUpdateBase
	{
		float Angle;
		float PosX;
		float PosY;
		float PosZ;
		float Speed;
	};
	
	
	class FollowWaypointNpcUpd extends NpcUpdateBase
	{
		float Pm1X;
		float P0X;
		float P1X;
		float P2X;
		float P3X;
		float P4X;
			
		float Pm1Y;
		float P0Y;
		float P1Y;
		float P2Y;
		float P3Y;
		float P4Y;
		
		float Pm1Z;
		float P0Z;
		float P1Z;
		float P2Z;
		float P3Z;
		float P4Z;	
	};
	
	class WalkToPointNpcUpd extends NpcUpdateBase
	{
		float PosX;
		float PosY;
		float PosZ;
		float RotationSpeedPerSec;
		bool moveForward;	
	};
	
	
		
	// base class for all sound update
	class SoundObjectUpdateBase
	{
	};

	
	
	class PlaySoundUpd extends SoundObjectUpdateBase
	{
		PlayingSound Si;
	};
	
	class PlaySoundSeqUpd extends SoundObjectUpdateBase
	{
		PlayingSoundSequence Sis;
	};		
	
	
	struct ProjectileInfo
	{	
		int			OwnerActorType;
		long			OwnerActorId;	
	
		long			Id;
		ModelInfo		DisplayDesc;
		ObjectPhysicDesc	PhysicDesc;
		long			ManagingClientId;
		
		float			Power;
		
		float			OffsetX;
		float			OffsetY;
		float			OffsetZ;

		float			ForceX;
		float			ForceY;		
		float			ForceYOnImpact;
		
		int			NbBounce;
		bool			IgnoreGravity;
		
		int			FollowTargetType; // -1 if following no target	
		long			FollowTargetId;
		
		float			LifeTime;
		bool			Comeback;
		
		int			StartAnimFrame;
		
		float			AngleOffset;
		
		string			SoundAtStart;
		string			SoundOnBounce;
		
		bool			ForceHurt;
		
		bool			MultiShoot;
		
		bool			UseTimer;
		float			Frequency;
	};
	

	struct SoundInfo
	{
		string			SoundPath;
		bool			Use3d;
		
		float			PosX;
		float			PosY;
		float			PosZ;	
		
		float			SpeedX;
		float			SpeedY;
		float			SpeedZ;			
		
	};	
	

	struct ShoutTextInfo
	{
		int			TextId;
		float			TextSize;
		
		float			TextcolorR;
		float			TextcolorG;
		float			TextcolorB;	
		
		long			ObjectType;
		long			ObjectId;	
	};	


	struct ContainerSharedInfo
	{
		long			OpeningClient;
		ItemsMap		ContainerItems;
	};	
};	

#endif


