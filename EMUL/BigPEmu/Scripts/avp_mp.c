//__BIGPEMU_SCRIPT_MODULE__
//__BIGPEMU_META_DESC__		"AvP multiplayer implementation."
//__BIGPEMU_META_AUTHOR__	"Rich Whitehouse"

#include "bigpcrt.h"

#define AVP_SUPPORTED_HASH					0x1BCB3230DB41AA47ULL

#define AVP_DAMAGE_TO_PLAYER_ENERGY_SCALE	5 //effectively scales pvp damage

//tried to correlate naming to available source, where possible, but a lot of this is just reverse engineered from disassembly.
#define AVP_CUR_LEVEL						0x0002F5AA //uint16
#define AVP_NEW_LEVEL						0x0002F5AC //uint16
#define AVP_NEW_X							0x0002F5AE //int32 (16:16 with maze coordinate packed in top 16, but whatever, just assume these sizes are correct and not much else)
#define AVP_NEW_Y							0x0002F5B2 //int32

#define AVP_MAZE_DATA						0x0004FC00
#define AVP_MAZE_BLOCK_SIZE					8
#define AVP_MAZE_PAD_DIMENSION				0
#define AVP_MAZE_MAX_WIDTH					(128 + AVP_MAZE_PAD_DIMENSION)
#define AVP_MAZE_MAX_HEIGHT					(128 + AVP_MAZE_PAD_DIMENSION)
#define AVP_MAZE_MAX_BLOCK_COUNT			(AVP_MAZE_MAX_WIDTH * AVP_MAZE_MAX_HEIGHT)
#define AVP_MAZE_MAX_SIZE					(AVP_MAZE_MAX_BLOCK_COUNT * AVP_MAZE_BLOCK_SIZE)

#define AVP_AMPS_PTR						0x0002F1B4 //uint32
#define AVP_GMPS_PTR						0x0002F1B8 //uint32
#define AVP_CLIST_PTR						0x0002F1BC //uint32
#define AVP_MAZE_WIDTH						0x0002F1C0 //int32
#define AVP_MAZE_HEIGHT						0x0002F1C4 //int32
#define AVP_SPRITE_RESCALE					0x0002F1C8 //uint16
#define AVP_TRUE_WIDTH						0x0002F1CA //uint16
#define AVP_CENTER_OFFSET					0x0002F1CC //uint16
#define AVP_X_POS							0x0002F1CE //int32 (copied to $3EAD8)
#define AVP_Y_POS							0x0002F1D2 //int32 (copied to $3EADC)
#define AVP_CENTER_ANGLE					0x0002F1D6 //int32
#define AVP_PLAYER_TYPE						0x0002F1DA //uint16
#define AVP_END_COUNT						0x0002F1DE //uint16
#define AVP_AMMO_SG							0x0002F214 //uint16
#define AVP_NEW_WEAPS						0x0002F30A //uint8
#define AVP_OLD_WEAPS						0x0002F30B //uint8
#define AVP_NEW_WEAPNO						0x0002F30C //uint16
#define AVP_FIRE_DAMAGE						0x0002F316 //uint16
#define AVP_INIT_MOVE						0x00030A34 //uint32
#define AVP_RESET_MOVE						0x00030A38 //uint32
#define AVP_DO_MOVE							0x00030A3C //uint32
#define AVP_NEW_POS							0x00030A40 //uint32
#define AVP_OLD_POS							0x00030A44 //uint32
#define AVP_ANG_VEL							0x00030A48 //uint16
#define AVP_ANG_ACC							0x00030A4A //uint16
#define AVP_MAX_AVEL						0x00030A4E //uint16
#define AVP_ACC_SHIFT						0x00030A50 //uint16
#define AVP_DECC_SHIFT						0x00030A52 //uint16
#define AVP_X_VEL							0x00030A54 //uint32
#define AVP_Y_VEL							0x00030A58 //uint32
#define AVP_PLAYER_VEL						0x00030A5C //uint16
#define AVP_PLAYER_DEAD						0x00030A60 //uint16
#define AVP_GAME_OVER						0x00030A62 //uint16
#define AVP_KEY_LOCK						0x00030A64 //uint16
#define AVP_DESTRUCT_FLAG					0x00030A66 //uint16
#define AVP_LAUNCH_FLAG						0x00030A68 //uint16
#define AVP_PLAYER_ENERGY					0x00030A6A //uint16
#define AVP_MAX_ENERGY						0x00030A6C //uint16
#define AVP_MAX_SPEED						0x00030A6E //uint32
#define AVP_PAIN_COLLS						0x00030A72 //uint32
#define AVP_SCORE							0x00030A76 //uint32
#define AVP_CHEAT							0x00030A7A //uint16
#define AVP_PAIN							0x00030A7C //uint16
#define AVP_OLD_ENERGY						0x00030A7E //uint16
#define AVP_LAST_MOAN						0x00030A80 //uint16
#define AVP_DISCFLAG						0x0003EAC2 //uint16
#define AVP_NEW_MESSAGE						0x00039834 //uint32
#define AVP_CHASE_X							0x0003EAD8 //int32
#define AVP_CHASE_Y							0x0003EADC //int32
#define AVP_AMP_PROXFLAG					0x00042AE0 //uint16
#define AVP_INV_TABLE						0x00042AE8 //uint8[16]

#define AVP_AMPOFS_XPOS						0 //int32
#define AVP_AMPOFS_YPOS						4 //int32
#define AVP_AMPOFS_MODE						8 //int32 (non-0 effectively means display/in-use)
#define AVP_AMPOFS_CREATURE					12 //int16
#define AVP_AMPOFS_ANIMSEQ					14 //int16
#define AVP_AMPOFS_ANIMFRAME				16 //int16
#define AVP_AMPOFS_ANGLE					18 //int16
#define AVP_AMPOFS_TIMER					20 //int32
#define AVP_AMPOFS_XVEL						24 //int16
#define AVP_AMPOFS_LAUNCHAMP				24 //int16
#define AVP_AMPOFS_YVEL						26 //int16
#define AVP_AMPOFS_LDIR						28 //int16
#define AVP_AMPOFS_XVEC						30 //int32
#define AVP_AMPOFS_YVEC						34 //int32
#define AVP_AMPOFS_ENERGY					38 //int16
#define AVP_AMPOFS_OLDENERGY				40 //int16
#define AVP_AMPOFS_FLAGWORD					42 //int16
#define AVP_AMPOFS_YOFFSET					44 //int16
#define AVP_AMPOFS_ASTYPE					46 //int16
#define AVP_AMPSIZE							48 //always align to 4 byte boundary
#define AVP_MAX_AMP_COUNT					300
#define AVP_AMP_TOTAL_SIZE					(AVP_AMPSIZE * AVP_MAX_AMP_COUNT)
#define AMP_FLAG_KILLABLE					(1 << 0)
#define AMP_FLAG_DEADGEN					(1 << 1)
#define AMP_FLAG_PLAYER						(1 << 2)
#define AMP_FLAG_DEAD						(1 << 3)
#define AMP_FLAG_COLLECT					(1 << 4)
#define AMP_FLAG_MOTION						(1 << 5)
#define AMP_FLAG_STUN						(1 << 6)
#define AMP_FLAG_PHIT						(1 << 7)
#define AMP_FLAG_CHATFLAG					(1 << 8)
#define AMP_FLAG_EGGOPEN					(1 << 9)
#define AMP_FLAG_CLOSEHIT					(1 << 10)
#define AMP_FLAG_INVHIT						(1 << 11)
#define AMP_FLAG_SSHIELD					(1 << 12)
#define AMP_FLAG_ALTFRAME					(1 << 15)
#define AMP_FLAG_AVATAR						(1 << 13) //claim an unused a bit
#define AMP_DEFAULT_AVATAR_FLAGS			(AMP_FLAG_KILLABLE | AMP_FLAG_MOTION | AMP_FLAG_AVATAR)

#define AVP_AVATAR_AMP_MODE					-1 //"static"
#define AVP_CURLEVEL_INVALID				0xFFFFFFFF

//not all of these are currently used, but figured i might as well leave some extras documented
#define AVP_PCADDR_COLLECTIT				0x0000F940
#define AVP_PCADDR_COLLECTIT_KILLEITHER		0x0000F91C
#define AVP_PCADDR_COLLECTIT_KILLCF			0x0000F922
#define AVP_PCADDR_COLLECTIT_KILLAMP		0x0000F938
#define AVP_PCADDR_COLLECTIT_KILLGRID		0x0000F93C
#define AVP_PCADDR_PLAYER_ATTACK_CHECK		0x0000FD1A
#define AVP_PCADDR_PLAYER_SHOOTLOOP			0x0000FD24
#define AVP_PCADDR_PLAYER_DAMAGE			0x0000FD9A //a1 = amp taking damage, d4/d5 = vec to amp, a4 = player x, a5 = player y, a6 = offset of hit
#define AVP_PCADDR_PLAYER_DAMAGE_RET		0x0000FE0E
#define AVP_PCADDR_CHECK_NEWLEVEL			0x0000C2FA //.not_comp
#define AVP_PCADDR_PROCESS_ENDGAME			0x0000C5AC
#define AVP_PCADDR_ENDGAME_DEATHYELL		0x0000C66A
#define AVP_PCADDR_ENDGAME_RET				0x0000C6E4
#define AVP_PCADDR_LIFT_DOOR				0x0001156A
#define AVP_PCADDR_LIFT_DOOR_ABORT			0x00011712
#define AVP_PCADDR_NORMAL_DOOR				0x00011670
#define AVP_PCADDR_MOVEDOORS				0x00011832
#define AVP_PCADDR_CHECK_ACCESS				0x00011B7E
#define AVP_PCADDR_NO_ACCSND				0x00011C2A
#define AVP_PCADDR_SAFEPOS					0x000121A0
#define AVP_PCADDR_MAKE_AMP					0x00015078
#define AVP_PCADDR_REBUILD_LEVEL			0x000150C0
#define AVP_PCADDR_RAM_SAVE					0x00015176
#define AVP_PCADDR_AMP_REQ					0x000151E2
#define AVP_PCADDR_AMP_RELEASE				0x00015226
#define AVP_PCADDR_SAVE_LEVEL				0x00015242
#define AVP_PCADDR_RESTORE_LEVEL			0x00015272
#define AVP_PCADDR_RAND_SET					0x000153FA
#define AVP_PCADDR_UPDATE_AMPS				0x00015478
#define AVP_PCADDR_AMP_SLEEP				0x000156F4
#define AVP_PCADDR_AMP_SLEEP_TEST			0x00015700
#define AVP_PCADDR_AMP_DAMAGE_CHK			0x000157EE
#define AVP_PCADDR_AMP_CLOSE_FIGHT			0x00015836
#define AVP_PCADDR_AMP_CHKFAR				0x0001596A
#define AVP_PCADDR_AMP_HUMANMOVES			0x00015A94
#define AVP_PCADDR_AMP_OK_TO_MOVE			0x00015B8A
#define AVP_PCADDR_AMP_NOFACE				0x00015C86
#define AVP_PCADDR_AMP_RANGE_PLAYER			0x000163D0
#define AVP_PCADDR_AMP_THROWDISC			0x00016AEA
#define AVP_PCADDR_AMP_THROWDISC_RET		0x00016B7A
#define AVP_PCADDR_AMP_PRE_DISCMODE			0x00016B7C
#define AVP_PCADDR_AMP_LAUNCFIRE			0x00016C92
#define AVP_PCADDR_AMP_LAUNCHIT				0x00016CB8
#define AVP_PCADDR_AMP_LAUNCHIT_P0			0x00016D84
#define AVP_PCADDR_AMP_LAUNCHIT_P1			0x00016DE8
#define AVP_PCADDR_AMP_LAUNCHIT_P2			0x00016E4C
#define AVP_PCADDR_AMP_LAUNCHIT_P3			0x00016EB8
#define AVP_PCADDR_AMP_PRE_FLAMEON			0x00016EBA
#define AVP_PCADDR_AMP_FLAMEON				0x00016ED0
#define AVP_PCADDR_AMP_LASER				0x00017018
#define AVP_PCADDR_AMP_LASER_RET			0x00017120
#define AVP_PCADDR_AMP_WEAPON_HIT			0x0001718C
#define AVP_PCADDR_AMP_WEAPON_HIT_RET		0x0001719C
#define AVP_PCADDR_AMP_CHK_SQUARE			0x00017226
#define AVP_PCADDR_PLAYER_WEAPON			0x00017636
#define AVP_PCADDR_PLAYER_WEAPON_RET		0x000176E2
#define AVP_PCADDR_AMP_FIRE_HUG				0x00017CF6
#define AVP_PCADDR_AMP_FIRE_HUG_RET			0x00017D62
#define AVP_PCADDR_AMP_EXPLOSION			0x00018380
#define AVP_PCADDR_AMP_EXPLOSION_RET		0x00018430
#define AVP_PCADDR_OCCUPY_SQR				0x00017860
#define AVP_PCADDR_MAKE_SPARK				0x00018932
#define AVP_PCADDR_MAKE_SPARK_RET			0x00018A26
#define AVP_PCADDR_WEP_SFX					0x00009C3A
#define AVP_PCADDR_SFX						0x00009C6C

#define AVP_COLLMAP							0x0003EAE0
#define AVP_OBJMAP							0x00040AE0
#define AVP_COLLMAP_SIZE					((64 * 2) * 64)
#define AVP_OBJMAP_SIZE						((64 * 2) * 64)
#define AVP_COLLMAP_GROUP_SIZE				256
#define AVP_COLLMAP_GROUP_COUNT				(AVP_COLLMAP_SIZE / AVP_COLLMAP_GROUP_SIZE)
#define AVP_OBJMAP_GROUP_SIZE				256
#define AVP_OBJMAP_GROUP_COUNT				(AVP_OBJMAP_SIZE / AVP_COLLMAP_GROUP_SIZE)

#define AVP_PT_HUMAN						0
#define AVP_PT_ALIEN						4
#define AVP_PT_PREDATOR						8
#define AVP_AC_ALIEN						0
#define AVP_AC_PREDATOR						1
#define AVP_AC_HUMAN						2
#define AVP_AC_EGG							3
#define AVP_AC_FACEHUG						4
#define AVP_AC_ACRAWL						5
#define AVP_AC_SMART						6
#define AVP_AC_BOLT							7
#define AVP_AC_FIRE							8

#define AVP_HUWEAP_SG						(1 << 1)
#define AVP_HUWEAP_AR						(1 << 2)
#define AVP_HUWEAP_FT						(1 << 3)
#define AVP_HUWEAP_CG						(1 << 4)

#define AVP_PROTOCOL_VERSION				124
#define AVP_NET_MAZEBLOCKS_PER_MSG					1
#define AVP_NET_MAZEGROUP_SIZE						(AVP_NET_MAZEBLOCKS_PER_MSG * AVP_MAZE_BLOCK_SIZE)
//message id is an arbitrary 16-bit value used to track delta compression behind the scenes.
//this allows you to throw big piles of state data around without having to care much, as long as the bulk of the data isn't changing.
//if your'e sending multiple state blocks, you can use different id's for each.
#define AVP_NET_MSG_ID_PLAYERSTATE					100
#define AVP_NET_MSG_ID_GAMESTATE					(AVP_NET_MSG_ID_PLAYERSTATE + 1)
#define AVP_NET_MSG_ID_AMPSTATE_BEGIN				(AVP_NET_MSG_ID_GAMESTATE + 1)
#define AVP_NET_MSG_ID_AMPSTATE_END					(AVP_NET_MSG_ID_AMPSTATE_BEGIN + AVP_MAX_AMP_COUNT)
#define AVP_NET_MSG_ID_LIFT_DOOR					(AVP_NET_MSG_ID_AMPSTATE_END + 1)
#define AVP_NET_MSG_ID_CLIENT_C2SAMP				(AVP_NET_MSG_ID_LIFT_DOOR + 1)
#define AVP_NET_MSG_ID_CLIENT_DMG					(AVP_NET_MSG_ID_CLIENT_C2SAMP + 1)
#define AVP_NET_MSG_ID_CLIENT_DRIVE					(AVP_NET_MSG_ID_CLIENT_DMG + 1)
#define AVP_NET_MSG_ID_CLIENT_AMPTRANSFER			(AVP_NET_MSG_ID_CLIENT_DRIVE + 1)
#define AVP_NET_MSG_ID_HURT_CLIENT					(AVP_NET_MSG_ID_CLIENT_AMPTRANSFER + 1)
#define AVP_NET_MSG_ID_SFX							(AVP_NET_MSG_ID_HURT_CLIENT + 1)
#define AVP_NET_MSG_ID_COLLMAP_BEGIN				(AVP_NET_MSG_ID_SFX + 1)
#define AVP_NET_MSG_ID_COLLMAP_END					(AVP_NET_MSG_ID_COLLMAP_BEGIN + AVP_COLLMAP_GROUP_COUNT)
#define AVP_NET_MSG_ID_OBJMAP_BEGIN					(AVP_NET_MSG_ID_COLLMAP_END + 1)
#define AVP_NET_MSG_ID_OBJMAP_END					(AVP_NET_MSG_ID_OBJMAP_BEGIN + AVP_OBJMAP_GROUP_COUNT)
#define AVP_NET_MSG_ID_AMPLOCAL_BEGIN				(AVP_NET_MSG_ID_OBJMAP_END + 1)
#define AVP_NET_MSG_ID_AMPLOCAL_END					(AVP_NET_MSG_ID_AMPLOCAL_BEGIN + AVP_MAX_AMP_COUNT)
#define AVP_NET_MSG_ID_MAZEDATA						(AVP_NET_MSG_ID_AMPLOCAL_END + 1)
#define AVP_NET_MSG_MAX								(AVP_NET_MSG_ID_MAZEDATA + (AVP_MAZE_MAX_BLOCK_COUNT / AVP_NET_MAZEBLOCKS_PER_MSG))

#define AVP_MAX_TRANSFER_AMP_SIZE					(64 * AVP_AMPSIZE)

#define MAX_AVP_CLIENTS								32
#define AVP_MAX_PENDING_ACTIONS						32

#define AVP_CLIENTDRIVEN_DIST						((128 * 7) << 8)
#define AVP_PROJ_HIT_CLIENT_DIST					(0.3f)
#define AVP_PROJ_HIT_CLIENT_DIST_SQ					(AVP_PROJ_HIT_CLIENT_DIST * AVP_PROJ_HIT_CLIENT_DIST)
#define AVP_MAX_DISTANCE_FOR_BROADCAST_SFX			(20.0f)
#define AVP_MAX_DISTANCE_FOR_BROADCAST_SFX_SQ		(AVP_MAX_DISTANCE_FOR_BROADCAST_SFX * AVP_MAX_DISTANCE_FOR_BROADCAST_SFX)

//sorry, this one's sloppier than usual, i've got the plague at the moment and i'm just trying to hack this into being functional enough to demo.
//this is all the excuse i can offer, but in truth, it's mostly just that i don't care enough. i'm at least still trying to comment enough to make this useful as an example.
//oh yeah, and this is full of security holes, but caring about that in this context would also be silly.

typedef struct
{
	int32_t mVersion;
	int32_t mClientIndex;
	int32_t mAMPIndex;
	uint32_t mPos[2];
	uint32_t mOldPos[2];
	//we could potentially add some velocity in here to extrapolate, but the game is so sluggish and even local updates are so infrequent, it's not very noticeable
	uint32_t mAngle;
	uint32_t mType; //one of AVP_PT_* to share with native data
	uint32_t mCurLevel;
	uint32_t mWantLevel;
	uint32_t mWantPos[2];
	uint64_t mSWHash;
} TAVPPlayerState;

typedef struct
{
	int32_t mVersion;	
	uint32_t mHostPos[2];
	uint64_t mFrameCount;
	uint16_t mCurLevel;
	uint16_t mDiscFlag;
	int32_t mAMPIndices[MAX_AVP_CLIENTS];
} TAVPGameState;

typedef struct
{
	int32_t mClientIndex;
	uint32_t mCurLevel;
	uint32_t mA0;
	uint16_t mD4;
	uint16_t mD5;
	uint16_t mD1;
	uint16_t mResv;
} TAVPRemoteOpenDoor;

typedef struct
{
	uint32_t mCurLevel;
	uint32_t mPlayerType;
	uint32_t mFireDamage;
	//being totally lazy and straight up transferring context, we'll make sure it's a thing we should be damaging on the server
	uint32_t mDRegs[8];
	uint32_t mARegs[7];
} TAVPPlayerDamage;

#define SFX_R3_NO_LOCALIZE		0x12345666
typedef struct
{
	uint32_t mCurLevel;
	int32_t mClientIndex;
	int32_t mPos[2];
	uint32_t mDRegs[4];
} TAVPBroadcastSFX;

typedef struct
{
	int32_t mClientIndex;
	uint32_t mCurLevel;
	uint32_t mAMPIndex;
} TAVPLocalAMPDriveRequest;

typedef struct
{
	//don't be confused by "local" terminology - this is sync'd over the net, "local" here refers to vm-local.
	uint32_t mLocalFlags;
	int32_t mClientController;
} TLocalAMPState;

//always assumed that this will be larger than any other socket data
#define AVP_NET_BUFFER_SIZE					(sizeof(TAVPGameState) + AVP_AMP_TOTAL_SIZE + AVP_MAZE_MAX_SIZE + AVP_COLLMAP_SIZE + AVP_OBJMAP_SIZE)
//some dumbness here due to a tcc issue
static uint8_t sAVPNetBuffer[AVP_NET_BUFFER_SIZE];
static TAVPGameState *spGS = (TAVPGameState *)&sAVPNetBuffer[0];
static uint8_t *spAMP = &sAVPNetBuffer[sizeof(TAVPGameState)];
static uint8_t *spMazeData = &sAVPNetBuffer[sizeof(TAVPGameState) + AVP_AMP_TOTAL_SIZE];
static uint8_t *spCollMapData = &sAVPNetBuffer[sizeof(TAVPGameState) + AVP_AMP_TOTAL_SIZE + AVP_MAZE_MAX_SIZE];
static uint8_t *spObjMapData = &sAVPNetBuffer[sizeof(TAVPGameState) + AVP_AMP_TOTAL_SIZE + AVP_MAZE_MAX_SIZE + AVP_COLLMAP_SIZE];

//synchronizing the object map means items don't go away when picking them up on the client, so we'd need some extra handling for that.
#define AVP_SYNC_OBJMAP 0

static TAVPRemoteOpenDoor sAVPOpenDoorActions[AVP_MAX_PENDING_ACTIONS];
static uint32_t sAVPOpenDoorActionCount = 0;

static TAVPPlayerDamage sAVPPlayerDamages[AVP_MAX_PENDING_ACTIONS];
static uint32_t sAVPPlayerDamageCount = 0;

static TAVPBroadcastSFX sAVPBroadcastSFXOut[AVP_MAX_PENDING_ACTIONS];
static uint32_t sAVPBroadcastSFXOutCount = 0;

static TAVPBroadcastSFX sAVPBroadcastSFXIn[AVP_MAX_PENDING_ACTIONS];
static uint32_t sAVPBroadcastSFXInCount = 0;

static TAVPLocalAMPDriveRequest sAVPAMPDriveRequests[AVP_MAX_PENDING_ACTIONS];
static uint32_t sAVPAMPDriveRequestCount = 0;

static uint8_t sAVPTransferAMPData[AVP_MAX_TRANSFER_AMP_SIZE];
static uint32_t sAVPTransferAMPDataSize = 0;

static TAVPPlayerState sClientStates[MAX_AVP_CLIENTS];
static uint32_t sClientHurt[MAX_AVP_CLIENTS];

static TLocalAMPState sLocalAMP[AVP_MAX_AMP_COUNT];

static uint8_t sClientToServerAMPData[((AVP_AMPSIZE + sizeof(uint32_t)) * AVP_MAX_AMP_COUNT)];
static uint32_t sClientToServerAMPDataSize = 0;

static uint32_t sFreshGameStateData = 0;
static uint32_t sFreshClientStateData = 0;
static uint32_t sFreshAMPDataOnClient = 0;
static uint32_t sFreshMazeDataMaxSize = 0;
static uint32_t sFreshCollMapDataMaxSize = 0;
static uint32_t sFreshObjMapDataMaxSize = 0;
static uint32_t sSendGameStateData = 0;
static uint32_t sSendClientStateData = 0;
static uint32_t sValidLevelState = 0;
static uint64_t sLocalFrameCount = 0;
static uint32_t sCurMazeWidth = 0;
static uint32_t sCurMazeHeight = 0;
static int32_t sCurLevelLocal = -1;

static int32_t sSafeSpotLevel = -1;
static uint32_t sSafeSpotPos[2];

static int32_t sOnLoadEvent = -1;
static int32_t sOnNetUpdateEvent = -1;
static int32_t sOnNetReceiveEvent = -1;

static uint32_t sDidShowNetWarning = 0;

static int32_t sLocalClientIndex = 0;
static uint32_t sAVPLoaded = 0;

static void reset_local_amp()
{
	for (uint32_t ampIndex = 0; ampIndex < AVP_MAX_AMP_COUNT; ++ampIndex)
	{
		TLocalAMPState *pLAMP = &sLocalAMP[ampIndex];
		pLAMP->mLocalFlags = 0;
		pLAMP->mClientController = -1;
	}
}

//we're not using the correct structures to manage these, so this could be a little fucky if the game actually runs up to the top here.
//we're just being lazy, assuming that won't happen and allocating from the top. but it's not terribly improbable with enough players on a populated level.
static int32_t get_free_amp()
{
	//start from the back
	const uint32_t ampBaseAddr = bigpemu_jag_read32(AVP_AMPS_PTR);
	for (int32_t ampIndex = AVP_MAX_AMP_COUNT - 1; ampIndex >= 0; --ampIndex)
	{
		const uint32_t objAddr = ampBaseAddr + ampIndex * AVP_AMPSIZE;
		if (!bigpemu_jag_read32(objAddr + AVP_AMPOFS_MODE))
		{
			const uint32_t flags = bigpemu_jag_read16(objAddr + AVP_AMPOFS_FLAGWORD);
			if (flags == 0xFFFF || !(flags & AMP_FLAG_AVATAR))
			{
				return ampIndex;
			}
		}
	}
	return -1;
}

static void free_amp_entry(const int32_t ampIndex)
{
	if (ampIndex < 0)
	{
		return;
	}
	const uint32_t ampBaseAddr = bigpemu_jag_read32(AVP_AMPS_PTR);
	bigpemu_jag_sysmemset(ampBaseAddr + ampIndex * AVP_AMPSIZE, 0, AVP_AMPSIZE);
}

static uint32_t get_maze_group_count()
{
	const uint32_t blockCount = (sCurMazeWidth + AVP_MAZE_PAD_DIMENSION) * (sCurMazeHeight + AVP_MAZE_PAD_DIMENSION);
#if AVP_NET_MAZEBLOCKS_PER_MSG > 1
	const uint32_t groupMask = (AVP_NET_MAZEBLOCKS_PER_MSG - 1);
	const uint32_t blockCountAligned = (blockCount + groupMask) & ~groupMask;
	const uint32_t groupCount = (blockCountAligned / AVP_NET_MAZEBLOCKS_PER_MSG);
	return groupCount;
#else
	return blockCount;
#endif
}

static void avp_lift_door(const uint32_t addr)
{
	const ESharedNetConnectionType connType = bigpemu_net_connection_type();
	if (bigpemu_net_current_device() != kSharedNetDev_Script || connType != kSharedNetConn_Client)
	{
		return;
	}
	
	//don't do it locally, send an update to the server instead
	if (sAVPOpenDoorActionCount < AVP_MAX_PENDING_ACTIONS)
	{
		TAVPRemoteOpenDoor *pRemOpen = &sAVPOpenDoorActions[sAVPOpenDoorActionCount++];
		pRemOpen->mCurLevel = bigpemu_jag_read16(AVP_CUR_LEVEL);
		pRemOpen->mA0 = bigpemu_jag_m68k_get_areg(0);
		pRemOpen->mD4 = bigpemu_jag_m68k_get_dreg(4) & 0xFFFF;
		pRemOpen->mD5 = bigpemu_jag_m68k_get_dreg(5) & 0xFFFF;
		pRemOpen->mD1 = bigpemu_jag_m68k_get_dreg(1) & 0xFFFF;
		pRemOpen->mResv = 0;
	}
	//abort local code path
	bigpemu_jag_m68k_set_pc(AVP_PCADDR_LIFT_DOOR_ABORT);
}

static uint32_t sRestore68KRegs = 0;
static uint32_t sRestore68KRegData[16];
static uint32_t sRestorePlayerType = 0;
static uint32_t sRestoreFireDamage = 0;
static uint32_t sRestoreSfxAddr = 0;
static uint32_t sRestoreSfxCtrl = 0;
static void newlevel_check_state_restore()
{
	if (sRestorePlayerType)
	{
		bigpemu_jag_write16(AVP_PLAYER_TYPE, sRestorePlayerType - 1);
		sRestorePlayerType = 0;
	}
	if (sRestoreFireDamage)
	{
		bigpemu_jag_write16(AVP_FIRE_DAMAGE, sRestoreFireDamage - 1);
		sRestoreFireDamage = 0;
	}
	
	if (sRestoreSfxAddr)
	{
		bigpemu_jag_write16(sRestoreSfxAddr + 4, (uint16_t)sRestoreSfxCtrl);		
		sRestoreSfxAddr = 0;
	}
	
	if (sRestore68KRegs)
	{
		for (uint32_t regIndex = 0; regIndex < 16; ++regIndex)
		{
			if (sRestore68KRegs & (1 << regIndex))
			{
				if (regIndex >= 8)
				{
					bigpemu_jag_m68k_set_areg(regIndex - 8, sRestore68KRegData[regIndex]);
				}
				else
				{
					bigpemu_jag_m68k_set_dreg(regIndex, sRestore68KRegData[regIndex]);
				}
			}
		}
		sRestore68KRegs = 0;
	}
}

//minus stack
void full_68k_reg_backup()
{
	for (uint32_t regIndex = 0; regIndex < 7; ++regIndex)
	{
		sRestore68KRegData[regIndex] = bigpemu_jag_m68k_get_dreg(regIndex);
		sRestore68KRegData[8 + regIndex] = bigpemu_jag_m68k_get_areg(regIndex);
	}
	sRestore68KRegData[7] = bigpemu_jag_m68k_get_dreg(7);
	sRestore68KRegs = 0x7FFF;
}

static void perform_68k_call(const uint32_t retAddr, const uint32_t callAddr)
{
	const uint32_t nextSp = bigpemu_jag_m68k_get_areg(7) - 4;
	bigpemu_jag_m68k_set_areg(7, nextSp);
	bigpemu_jag_write32(nextSp, retAddr);
	bigpemu_jag_m68k_set_pc(callAddr);
}

static void relinquish_client_controller_and_redirect_player_damage(const uint32_t ampObjAddr, const uint32_t dmg)
{
	//see if the thing being damaged is client-controlled, and if so, steal control back
	const uint32_t ampBaseAddr = bigpemu_jag_read32(AVP_AMPS_PTR);
	if (ampObjAddr >= ampBaseAddr && ampObjAddr < (ampBaseAddr + AVP_AMP_TOTAL_SIZE))
	{
		const uint32_t ampIndex = (ampObjAddr - ampBaseAddr) / AVP_AMPSIZE;
		TLocalAMPState *pLAMP = &sLocalAMP[ampIndex];
		if (pLAMP->mClientController >= 0)
		{
			pLAMP->mClientController = -1;
		}
		else if (dmg)
		{
			//see if it's an avatar
			for (uint32_t clientIndex = 0; clientIndex < MAX_AVP_CLIENTS; ++clientIndex)
			{
				if (sClientStates[clientIndex].mAMPIndex == ampIndex)
				{
					//printf("set hurt: cl:%i dmg:%i\n", clientIndex, dmg);
					sClientHurt[clientIndex] += (dmg * AVP_DAMAGE_TO_PLAYER_ENERGY_SCALE);
					break;
				}
			}			
		}
	}
}

static uint32_t client_is_active(TAVPPlayerState *pCS)
{
	if (pCS->mClientIndex < 0 || pCS->mSWHash != AVP_SUPPORTED_HASH || pCS->mCurLevel != sCurLevelLocal)
	{
		return 0;
	}
	return 1;
}

static uint32_t amp_is_killable(const uint32_t ampObjAddr)
{
	uint32_t flags = bigpemu_jag_read16(ampObjAddr + AVP_AMPOFS_FLAGWORD);
	const uint32_t isKillable = ((flags & AMP_FLAG_KILLABLE) && bigpemu_jag_read16(ampObjAddr + AVP_AMPOFS_ENERGY) > 0 && bigpemu_jag_read32(ampObjAddr + AVP_AMPOFS_MODE) != 0);
	return isKillable;
}

//mimics amp_cleargrid
static void amp_remove_from_collmap(const uint32_t ampObjAddr)
{
	//intentionally reading only the first 2 bytes of each coord
	const uint32_t ux = (bigpemu_jag_read16(ampObjAddr + AVP_AMPOFS_XPOS) << 1) & 0xFFFF;
	int32_t iy = (int16_t)bigpemu_jag_read16(ampObjAddr + AVP_AMPOFS_YPOS);	
	iy <<= 7;
	const int32_t iAddr = iy + ux + AVP_COLLMAP;
	bigpemu_jag_write16(iAddr, 0);
}

static void avp_player_dmg(const uint32_t addr)
{
	const ESharedNetConnectionType connType = bigpemu_net_connection_type();
	if (bigpemu_net_current_device() != kSharedNetDev_Script || connType != kSharedNetConn_Client)
	{
		if (connType == kSharedNetConn_Host)
		{
			relinquish_client_controller_and_redirect_player_damage(bigpemu_jag_m68k_get_areg(1), bigpemu_jag_read16(AVP_FIRE_DAMAGE));
		}
		return;
	}
	
	if (sAVPPlayerDamageCount < AVP_MAX_PENDING_ACTIONS)
	{
		TAVPPlayerDamage *pPlDmg = &sAVPPlayerDamages[sAVPPlayerDamageCount++];
		pPlDmg->mCurLevel = bigpemu_jag_read16(AVP_CUR_LEVEL);
		pPlDmg->mPlayerType = bigpemu_jag_read16(AVP_PLAYER_TYPE);
		pPlDmg->mFireDamage = bigpemu_jag_read16(AVP_FIRE_DAMAGE);
		for (uint32_t regIndex = 0; regIndex < 7; ++regIndex)
		{
			pPlDmg->mDRegs[regIndex] = bigpemu_jag_m68k_get_dreg(regIndex);
			pPlDmg->mARegs[regIndex] = bigpemu_jag_m68k_get_areg(regIndex);
		}
		pPlDmg->mDRegs[7] = bigpemu_jag_m68k_get_dreg(7);
	}
	//abort local code path
	bigpemu_jag_m68k_set_pc(AVP_PCADDR_PLAYER_DAMAGE_RET);	
}

static float calc_squared_distance(const int32_t *pPos0, const int32_t *pPos1)
{
	const float fScale = 1.0f / 65536.0f;
	const float delta[2] = { (float)(pPos0[0] - pPos1[0]) * fScale, (float)(pPos0[1] - pPos1[1]) * fScale };
	return (delta[0] * delta[0] + delta[1] * delta[1]);
}

static void avp_sleep_test(const uint32_t addr)
{
	const ESharedNetConnectionType connType = bigpemu_net_connection_type();
	if (bigpemu_net_current_device() != kSharedNetDev_Script || connType == kSharedNetConn_None)
	{
		return;
	}
	
	if (!bigpemu_jag_read16(AVP_AMP_PROXFLAG))
	{
		return;
	}
	
	const uint32_t ampObjAddr = bigpemu_jag_m68k_get_areg(0);
	
	const uint32_t ampBaseAddr = bigpemu_jag_read32(AVP_AMPS_PTR);
	const uint32_t ampIndex = (ampObjAddr - ampBaseAddr) / AVP_AMPSIZE;
	
	TLocalAMPState *pLAMP = &sLocalAMP[ampIndex];	
	const int32_t clientControllerIndex = pLAMP->mClientController;
	
	const uint32_t isKillable = amp_is_killable(ampObjAddr);
	if (connType == kSharedNetConn_Client)
	{		
		//on the client, force any monsters to stay locally asleep, unless we take responsibility for them
		if (clientControllerIndex < 0 && isKillable)
		{			
			const uint32_t awakeDist = bigpemu_jag_m68k_get_dreg(1);
			if (awakeDist <= AVP_CLIENTDRIVEN_DIST)
			{
				TAVPGameState *pGS = spGS;
				TAVPPlayerState *pFirstCS = sClientStates;
				if (pGS->mVersion == AVP_PROTOCOL_VERSION && pFirstCS->mVersion == AVP_PROTOCOL_VERSION)
				{
					const int32_t xy[2] = { (int32_t)bigpemu_jag_read32(ampObjAddr + AVP_AMPOFS_XPOS), (int32_t)bigpemu_jag_read32(ampObjAddr + AVP_AMPOFS_YPOS) };
					const float lHost = calc_squared_distance((int32_t *)pGS->mHostPos, xy);
					const float lCl = calc_squared_distance((int32_t *)pFirstCS->mPos, xy);
					if (lCl < lHost)
					{
						//for now, only make one drive request per net update
						if (sAVPAMPDriveRequestCount == 0)
						{
							TAVPLocalAMPDriveRequest *pDrive = &sAVPAMPDriveRequests[sAVPAMPDriveRequestCount++];
							pDrive->mClientIndex = sLocalClientIndex;
							pDrive->mCurLevel = sCurLevelLocal;
							pDrive->mAMPIndex = ampIndex;
						}
					}
				}
			}
			
			if (clientControllerIndex != sLocalClientIndex)
			{
				bigpemu_jag_write16(AVP_AMP_PROXFLAG, 0);
			}
		}
	}
	else if (clientControllerIndex >= 0)
	{
		//otherwise, on the server, force it to sleep if it's client-driven
		if (isKillable && client_is_active(&sClientStates[clientControllerIndex]))
		{
			bigpemu_jag_write16(AVP_AMP_PROXFLAG, 0);
		}
		else
		{
			//if the client dropped off, remove the control
			pLAMP->mClientController = -1;
		}
	}
}

static uint32_t is_ct_projectile(const uint32_t ct)
{
	return (ct == AVP_AC_SMART || ct == AVP_AC_BOLT || ct == AVP_AC_FIRE);
}

static void avp_projectile_test(const uint32_t addr)
{
	if (bigpemu_net_current_device() != kSharedNetDev_Script || bigpemu_net_connection_type() != kSharedNetConn_Host)
	{
		return;
	}
	
	TAVPGameState *pGS = spGS;
	const uint32_t ampObjAddr = bigpemu_jag_m68k_get_areg(0);
	const uint32_t ct = bigpemu_jag_read16(ampObjAddr + AVP_AMPOFS_CREATURE);
	if (!is_ct_projectile(ct))
	{
		return;
	}
	
	uint32_t launchAmp = bigpemu_jag_read16(ampObjAddr + AVP_AMPOFS_LAUNCHAMP);
	if (launchAmp == 0xFFFF)
	{
		//assume it's host-owned
		const int32_t hostAmpIndex = pGS->mAMPIndices[0];
		if (hostAmpIndex < 0)
		{
			return;
		}
		launchAmp = hostAmpIndex;
	}
	
	const uint32_t ampBaseAddr = bigpemu_jag_read32(AVP_AMPS_PTR);
	const int32_t xy[2] = { (int32_t)bigpemu_jag_read32(ampObjAddr + AVP_AMPOFS_XPOS), (int32_t)bigpemu_jag_read32(ampObjAddr + AVP_AMPOFS_YPOS) };	
	for (uint32_t clientIndex = 0; clientIndex < MAX_AVP_CLIENTS; ++clientIndex)
	{
		const int32_t clAmpIndex = pGS->mAMPIndices[clientIndex];
		if (clAmpIndex < 0 || launchAmp == clAmpIndex)
		{
			continue;
		}
		
		const uint32_t clAmpObjAddr = ampBaseAddr + clAmpIndex * AVP_AMPSIZE;
		const int32_t clXy[2] = { (int32_t)bigpemu_jag_read32(clAmpObjAddr + AVP_AMPOFS_XPOS), (int32_t)bigpemu_jag_read32(clAmpObjAddr + AVP_AMPOFS_YPOS) };
		
		const float distSq = calc_squared_distance(xy, clXy);
		if (distSq <= AVP_PROJ_HIT_CLIENT_DIST_SQ)
		{
			bigpemu_jag_m68k_set_areg(1, clAmpObjAddr);
			bigpemu_jag_write16(AVP_AMP_PROXFLAG, 1);
			
			int32_t dmg = 1;
			switch (ct)
			{
			case AVP_AC_SMART:
				dmg = 50;
				break;
			case AVP_AC_BOLT:
				dmg = 100;
				break;
			case AVP_AC_FIRE:
				dmg = 25;
				break;
			default:
				break;
			}
			
			for (uint32_t otherClientIndex = 0; otherClientIndex < MAX_AVP_CLIENTS; ++otherClientIndex)
			{
				const int32_t otherClAmpIndex = pGS->mAMPIndices[otherClientIndex];
				if (otherClAmpIndex == launchAmp)
				{
					//it's from another player
					dmg *= AVP_DAMAGE_TO_PLAYER_ENERGY_SCALE;
					break;
				}
			}
			
			//printf("Projectile player damage: %i, %i / %i\n", launchAmp, clientIndex, clAmpIndex);
			
			sClientHurt[clientIndex] += dmg;
			
			bigpemu_jag_m68k_set_pc(AVP_PCADDR_AMP_WEAPON_HIT_RET);
			break;
		}
	}
}

static void avp_death(const uint32_t addr)
{
	if (bigpemu_net_current_device() != kSharedNetDev_Script || bigpemu_net_connection_type() == kSharedNetConn_None)
	{
		return;
	}

	//respawn them instead of going to game over
	//possible todo - leave a corpse amp for fun
	bigpemu_jag_write32(AVP_NEW_MESSAGE, 0);
	bigpemu_jag_write16(AVP_GAME_OVER, 0);
	bigpemu_jag_write16(AVP_KEY_LOCK, 0);
	bigpemu_jag_write16(AVP_END_COUNT, 0);
	bigpemu_jag_write16(AVP_PLAYER_ENERGY, bigpemu_jag_read16(AVP_MAX_ENERGY));
	bigpemu_jag_m68k_set_pc(AVP_PCADDR_ENDGAME_RET);
	if (sSafeSpotLevel == sCurLevelLocal && sCurLevelLocal >= 0)
	{
		bigpemu_jag_write32(AVP_X_POS, sSafeSpotPos[0]);
		bigpemu_jag_write32(AVP_Y_POS, sSafeSpotPos[1]);
	}
}

static void avp_transport_amp_from_areg(const uint32_t aReg)
{
	if (bigpemu_net_current_device() != kSharedNetDev_Script || bigpemu_net_connection_type() != kSharedNetConn_Client)
	{
		return;
	}
	
	if ((sAVPTransferAMPDataSize + AVP_AMPSIZE) <= AVP_MAX_TRANSFER_AMP_SIZE)
	{
		const uint32_t ampObjAddr = bigpemu_jag_m68k_get_areg(aReg);
		bigpemu_jag_sysmemread(&sAVPTransferAMPData[sAVPTransferAMPDataSize], ampObjAddr, AVP_AMPSIZE);
		sAVPTransferAMPDataSize += AVP_AMPSIZE;
		//zero the local copy out, it'll be propagated back from the server
		bigpemu_jag_sysmemset(ampObjAddr, 0, AVP_AMPSIZE);
	}	
}

static void avp_transport_amp_from_a0(const uint32_t addr)
{
	avp_transport_amp_from_areg(0);
}

//ahh, we do sometimes miss the niceties of c++
static void avp_transport_amp_from_a6(const uint32_t addr)
{
	avp_transport_amp_from_areg(6);
}

static void avp_playerlocal_sfx(const uint32_t addr)
{
	if (bigpemu_net_current_device() != kSharedNetDev_Script || bigpemu_net_connection_type() == kSharedNetConn_None)
	{
		return;
	}

	if (sAVPBroadcastSFXOutCount < AVP_MAX_PENDING_ACTIONS && sLocalClientIndex >= 0)
	{
		TAVPBroadcastSFX *pSfx = &sAVPBroadcastSFXOut[sAVPBroadcastSFXOutCount++];
		pSfx->mCurLevel = sCurLevelLocal;
		pSfx->mClientIndex = sLocalClientIndex;
		pSfx->mPos[0] = (int32_t)bigpemu_jag_read32(AVP_X_POS);
		pSfx->mPos[1] = (int32_t)bigpemu_jag_read32(AVP_Y_POS);
		pSfx->mDRegs[0] = bigpemu_jag_m68k_get_dreg(0);
		pSfx->mDRegs[1] = bigpemu_jag_m68k_get_dreg(1);
		pSfx->mDRegs[2] = bigpemu_jag_m68k_get_dreg(2);
		pSfx->mDRegs[3] = bigpemu_jag_m68k_get_dreg(3);
	}
}

static void avp_global_sfx(const uint32_t addr)
{
	if (bigpemu_net_current_device() != kSharedNetDev_Script || bigpemu_net_connection_type() != kSharedNetConn_Host)
	{
		return;
	}

	if (sAVPBroadcastSFXOutCount < AVP_MAX_PENDING_ACTIONS && sLocalClientIndex >= 0)
	{
		TAVPBroadcastSFX *pSfx = &sAVPBroadcastSFXOut[sAVPBroadcastSFXOutCount++];
		pSfx->mCurLevel = sCurLevelLocal;
		pSfx->mClientIndex = sLocalClientIndex;
		pSfx->mPos[0] = 0;
		pSfx->mPos[1] = 0;
		pSfx->mDRegs[0] = bigpemu_jag_m68k_get_dreg(0);
		pSfx->mDRegs[1] = bigpemu_jag_m68k_get_dreg(1);
		pSfx->mDRegs[2] = bigpemu_jag_m68k_get_dreg(2);
		pSfx->mDRegs[3] = SFX_R3_NO_LOCALIZE;
	}
}

#if !AVP_SYNC_OBJMAP
static void avp_collect_killamp(const uint32_t addr)
{
	if (bigpemu_net_current_device() != kSharedNetDev_Script || bigpemu_net_connection_type() == kSharedNetConn_None)
	{
		return;
	}
	//in multiplayer, when not attempting to sync the objmap, we always locally removing items from the map upon pickup.
	//however, we leave the amp object so that everyone can see it even after the host picks it up.
	//this is a lazy way to sidestep dealing with making them amp disappear only after a local pickup.
	bigpemu_jag_m68k_set_pc(AVP_PCADDR_COLLECTIT_KILLGRID);
}
#endif

static void fill_client_state_from_local_data(TAVPPlayerState *pCS)
{
	pCS->mVersion = AVP_PROTOCOL_VERSION;
	pCS->mClientIndex = sLocalClientIndex;
	pCS->mSWHash = (sAVPLoaded) ? AVP_SUPPORTED_HASH : 0;
	pCS->mOldPos[0] = pCS->mPos[0];
	pCS->mOldPos[1] = pCS->mPos[1];
	pCS->mPos[0] = bigpemu_jag_read32(AVP_X_POS);
	pCS->mPos[1] = bigpemu_jag_read32(AVP_Y_POS);
	pCS->mAngle = bigpemu_jag_read32(AVP_CENTER_ANGLE);
	pCS->mType = bigpemu_jag_read16(AVP_PLAYER_TYPE);
	const uint32_t curLevel = sCurLevelLocal;
	const uint32_t wantLevel = bigpemu_jag_read16(AVP_NEW_LEVEL);
	pCS->mCurLevel = curLevel;
	if (wantLevel && !pCS->mWantLevel)
	{
		pCS->mWantLevel = wantLevel;
		pCS->mWantPos[0] = bigpemu_jag_read32(AVP_NEW_X);
		pCS->mWantPos[1] = bigpemu_jag_read32(AVP_NEW_Y);
	}
}

static void toggle_client_amp(TAVPPlayerState *pCS, const int32_t mode)
{
	if (pCS->mAMPIndex >= 0)
	{
		const uint32_t ampBaseAddr = bigpemu_jag_read32(AVP_AMPS_PTR);
		const uint32_t ampOffset = ampBaseAddr + pCS->mAMPIndex * AVP_AMPSIZE;
		bigpemu_jag_write32(ampOffset + AVP_AMPOFS_MODE, mode);
	}
}

static void avp_check_newlevel(const uint32_t addr)
{
	newlevel_check_state_restore();
	
	const ESharedNetConnectionType connType = bigpemu_net_connection_type();
	if (bigpemu_net_current_device() != kSharedNetDev_Script)
	{
		//if the user has activated this script but connected in the wrong mode, warn them
		if (connType != kSharedNetConn_None && !sDidShowNetWarning)
		{
			printf_notify("Warning: AvP MP module will not be functional unless the network device type is set to 'Script'.");
			sDidShowNetWarning = 1;
		}
		return;
	}

	TAVPGameState *pGS = spGS;
	TAVPPlayerState *pFirstCS = sClientStates;
	
	sCurMazeWidth = bigpemu_jag_read32(AVP_MAZE_WIDTH);
	sCurMazeHeight = bigpemu_jag_read32(AVP_MAZE_HEIGHT);
	
	sCurLevelLocal = bigpemu_jag_read16(AVP_CUR_LEVEL);
	
	//grab a respawn-on-death spot for the current level, if necessary
	if (sSafeSpotLevel != sCurLevelLocal)
	{
		if (bigpemu_jag_read16(AVP_PLAYER_ENERGY) > 0 && !bigpemu_jag_read16(AVP_NEW_LEVEL))
		{
			sSafeSpotPos[0] = bigpemu_jag_read32(AVP_X_POS);
			sSafeSpotPos[1] = bigpemu_jag_read32(AVP_Y_POS);
			sSafeSpotLevel = sCurLevelLocal;
		}
	}
	
	sValidLevelState = 1;
	++sLocalFrameCount;
	
	if (sLocalClientIndex >= 0 && sClientHurt[sLocalClientIndex])
	{
		int32_t health = bigpemu_jag_read16(AVP_PLAYER_ENERGY);
		if (health > 0)
		{
			health = BIGPEMU_MAX(health - sClientHurt[sLocalClientIndex], 0);
			bigpemu_jag_write16(AVP_PLAYER_ENERGY, (uint16_t)health);
		}		
		sClientHurt[sLocalClientIndex] = 0;
	}	
	
	switch (connType)
	{
	case kSharedNetConn_Host:
		{
			//won't propagate over the network, just for consistency so first client data is occuped by the host
			fill_client_state_from_local_data(pFirstCS);
			
			pGS->mVersion = AVP_PROTOCOL_VERSION;
			pGS->mHostPos[0] = bigpemu_jag_read32(AVP_X_POS);
			pGS->mHostPos[1] = bigpemu_jag_read32(AVP_Y_POS);
			pGS->mFrameCount = sLocalFrameCount;
			pGS->mCurLevel = sCurLevelLocal;
			pGS->mDiscFlag = bigpemu_jag_read16(AVP_DISCFLAG);
			for (uint32_t clientIndex = 0; clientIndex < MAX_AVP_CLIENTS; ++clientIndex)
			{
				pGS->mAMPIndices[clientIndex] = sClientStates[clientIndex].mAMPIndex;
			}
			//take the whole damn chunk of amp data to propagate to clients
			const uint32_t ampBaseAddr = bigpemu_jag_read32(AVP_AMPS_PTR);
			
			const uint32_t groupCount = get_maze_group_count();
			
			bigpemu_jag_sysmemread(spMazeData, AVP_MAZE_DATA, groupCount * AVP_NET_MAZEGROUP_SIZE);

			bigpemu_jag_sysmemread(spCollMapData, AVP_COLLMAP, AVP_COLLMAP_SIZE);
#if AVP_SYNC_OBJMAP
			bigpemu_jag_sysmemread(spObjMapData, AVP_OBJMAP, AVP_OBJMAP_SIZE);
#endif
			
			const uint32_t wantNewLevel = bigpemu_jag_read16(AVP_NEW_LEVEL);
			if (wantNewLevel)
			{
				for (uint32_t clientIndex = 0; clientIndex < MAX_AVP_CLIENTS; ++clientIndex)
				{
					TAVPPlayerState *pCS = &sClientStates[clientIndex];					
					if (pCS->mAMPIndex >= 0)
					{
						free_amp_entry(pCS->mAMPIndex);
						pCS->mAMPIndex = -1;
					}
				}
				reset_local_amp();
				memset(sClientHurt, 0, sizeof(sClientHurt));
			}
			else
			{
				for (uint32_t clientIndex = 0; clientIndex < MAX_AVP_CLIENTS; ++clientIndex)
				{
					TAVPPlayerState *pCS = &sClientStates[clientIndex];

					//clean the client representation amp up if they're no longer connected or if they've got the wrong software loaded
					if (pCS->mAMPIndex >= 0 && !client_is_active(pCS))
					{
						free_amp_entry(pCS->mAMPIndex);
						pCS->mAMPIndex = -1;
						pCS->mCurLevel = AVP_CURLEVEL_INVALID;
					}
				}
				
				if (sFreshClientStateData)
				{
					//don't try to create objects if we're about to transition
					for (uint32_t clientIndex = 0; clientIndex < MAX_AVP_CLIENTS; ++clientIndex)
					{
						if (!(sFreshClientStateData & (1 << clientIndex)))
						{
							continue;
						}
						
						TAVPPlayerState *pCS = &sClientStates[clientIndex];
						if (pCS->mVersion == AVP_PROTOCOL_VERSION)
						{
							if (pCS->mWantLevel && pCS->mWantLevel != sCurLevelLocal)
							{
								const uint32_t wantLevel = bigpemu_jag_read16(AVP_NEW_LEVEL);
								if (!wantLevel)
								{
									//alright, kick it
									bigpemu_jag_write16(AVP_NEW_LEVEL, pCS->mWantLevel);
									bigpemu_jag_write32(AVP_NEW_X, pCS->mWantPos[0]);
									bigpemu_jag_write32(AVP_NEW_Y, pCS->mWantPos[1]);
									reset_local_amp();
								}
								pCS->mWantLevel = 0;
							}
						}						
					}
					sFreshClientStateData = 0;
				}
				
				for (uint32_t clientIndex = 0; clientIndex < MAX_AVP_CLIENTS; ++clientIndex)
				{
					TAVPPlayerState *pCS = &sClientStates[clientIndex];
					if (pCS->mVersion == AVP_PROTOCOL_VERSION)
					{
						if (pCS->mAMPIndex < 0 && client_is_active(pCS))
						{
							//make it!
							const int32_t objIndex = get_free_amp();
							if (objIndex >= 0)
							{
								pCS->mAMPIndex = objIndex;								
								const uint32_t ampOffset = ampBaseAddr + objIndex * AVP_AMPSIZE;
								bigpemu_jag_sysmemset(ampOffset, 0, AVP_AMPSIZE);
								bigpemu_jag_write32(ampOffset + AVP_AMPOFS_MODE, AVP_AVATAR_AMP_MODE);
								bigpemu_jag_write16(ampOffset + AVP_AMPOFS_FLAGWORD, AMP_DEFAULT_AVATAR_FLAGS);
							}
						}
						
						const int32_t objIndex = pCS->mAMPIndex;
						if (objIndex >= 0)
						{
							const uint32_t ampOffset = ampBaseAddr + objIndex * AVP_AMPSIZE;
							//printf("player actor: %i, %i, %i, %i\n", objIndex, bigpemu_jag_read32(ampOffset + AVP_AMPOFS_MODE), pCS->mPos[0], pCS->mPos[1]);
							if (bigpemu_jag_read16(ampOffset + AVP_AMPOFS_FLAGWORD) & AMP_FLAG_AVATAR)
							{
								//printf("update client object %i, %i\n", clientIndex, pCS->mType);
								uint32_t runFrames = 0;
								switch (pCS->mType)
								{
								case AVP_PT_HUMAN:
									bigpemu_jag_write16(ampOffset + AVP_AMPOFS_CREATURE, AVP_AC_HUMAN);
									runFrames = 6;
									break;
								case AVP_PT_ALIEN:
									bigpemu_jag_write16(ampOffset + AVP_AMPOFS_CREATURE, AVP_AC_ALIEN);
									runFrames = 6;
									break;
								case AVP_PT_PREDATOR:
									bigpemu_jag_write16(ampOffset + AVP_AMPOFS_CREATURE, AVP_AC_PREDATOR);
									runFrames = 8;
									break;
								default:
									runFrames = 1;
									break;
								}
								
								const uint32_t ampOffset = ampBaseAddr + objIndex * AVP_AMPSIZE;
								bigpemu_jag_write32(ampOffset + AVP_AMPOFS_MODE, AVP_AVATAR_AMP_MODE);
								bigpemu_jag_write32(ampOffset + AVP_AMPOFS_XPOS, pCS->mPos[0]);
								bigpemu_jag_write32(ampOffset + AVP_AMPOFS_YPOS, pCS->mPos[1]);
								
								bigpemu_jag_write16(ampOffset + AVP_AMPOFS_ANIMSEQ, 3); //AS_RUN (one of the only sequences that has angle frames, at least for humans)
								bigpemu_jag_write16(ampOffset + AVP_AMPOFS_ASTYPE, 0xFFFF);
																
								bigpemu_jag_write16(ampOffset + AVP_AMPOFS_ENERGY, 10000);
								bigpemu_jag_write16(ampOffset + AVP_AMPOFS_OLDENERGY, 10000);								
								
								const int32_t ang = (int32_t)pCS->mAngle >> 16;
								const int16_t angQ = (int16_t)ang;
								bigpemu_jag_write16(ampOffset + AVP_AMPOFS_ANGLE, angQ);
								
								//just do some basic bullshit, good enough
								const uint32_t isRunning = (pCS->mPos[0] != pCS->mOldPos[0] || pCS->mPos[1] != pCS->mOldPos[1]);
								bigpemu_jag_write16(ampOffset + AVP_AMPOFS_ANIMFRAME, isRunning ? (uint16_t)(sLocalFrameCount % runFrames) : 0);
							}
							else
							{
								//something in the game state murdered it, we'll have to claim a new one
								bigpemu_jag_sysmemset(ampOffset, 0, AVP_AMPSIZE);
								pCS->mAMPIndex = -1;
							}
						}
					}
				}
			}
						
			if (sAVPOpenDoorActionCount)
			{
				//client requests for doors
				//we'll handle one at a frame, jumping to the appropriate routine so we can return to this place
				TAVPRemoteOpenDoor *pOpen = &sAVPOpenDoorActions[sAVPOpenDoorActionCount - 1];

				//ignore requests from different levels
				if (pOpen->mCurLevel == sCurLevelLocal)
				{
					sRestore68KRegData[8] = bigpemu_jag_m68k_get_areg(0);
					sRestore68KRegData[8 + 4] = bigpemu_jag_m68k_get_areg(4);
					sRestore68KRegData[4] = bigpemu_jag_m68k_get_dreg(4);
					sRestore68KRegData[5] = bigpemu_jag_m68k_get_dreg(5);
					sRestore68KRegData[1] = bigpemu_jag_m68k_get_dreg(1);
					sRestore68KRegData[6] = bigpemu_jag_m68k_get_dreg(6);
					sRestore68KRegs = (1 << 8) | (1 << (8 + 4)) | (1 << 4) | (1 << 5) | (1 << 1) | (1 << 6);
					
					//conveniently, we can call right in from here, return and hit this breakpoint, then restore the registers before proceeding
					bigpemu_jag_m68k_set_areg(0, pOpen->mA0);
					bigpemu_jag_m68k_set_dreg(4, pOpen->mD4 | (bigpemu_jag_m68k_get_dreg(4) & 0xFFFF0000));
					bigpemu_jag_m68k_set_dreg(5, pOpen->mD5 | (bigpemu_jag_m68k_get_dreg(5) & 0xFFFF0000));
					bigpemu_jag_m68k_set_dreg(1, pOpen->mD1 | (bigpemu_jag_m68k_get_dreg(1) & 0xFFFF0000));
					
					perform_68k_call(addr, AVP_PCADDR_LIFT_DOOR);
				}					
				--sAVPOpenDoorActionCount;
			}
			else if (sAVPPlayerDamageCount)
			{
				TAVPPlayerDamage *pPlDmg = &sAVPPlayerDamages[sAVPPlayerDamageCount - 1];
				if (pPlDmg->mCurLevel == sCurLevelLocal)
				{
					const uint32_t ampObjAddr = pPlDmg->mARegs[1];
					relinquish_client_controller_and_redirect_player_damage(ampObjAddr, pPlDmg->mFireDamage);
					if (ampObjAddr >= ampBaseAddr && ampObjAddr < (ampBaseAddr + AVP_AMP_TOTAL_SIZE))
					{
						const uint32_t flags = bigpemu_jag_read16(ampObjAddr + AVP_AMPOFS_FLAGWORD);
						if (flags & AMP_FLAG_KILLABLE)
						{
							full_68k_reg_backup();

							for (uint32_t regIndex = 0; regIndex < 7; ++regIndex)
							{
								bigpemu_jag_m68k_set_dreg(regIndex, pPlDmg->mDRegs[regIndex]);
								bigpemu_jag_m68k_set_areg(regIndex, pPlDmg->mARegs[regIndex]);
							}
							bigpemu_jag_m68k_set_dreg(7, pPlDmg->mDRegs[7]);
							
							//make sure we do the correct effect/spark based on the inflicting player type
							sRestorePlayerType = 1 + bigpemu_jag_read16(AVP_PLAYER_TYPE);
							bigpemu_jag_write16(AVP_PLAYER_TYPE, pPlDmg->mPlayerType);

							sRestoreFireDamage = 1 + bigpemu_jag_read16(AVP_FIRE_DAMAGE);
							bigpemu_jag_write16(AVP_FIRE_DAMAGE, pPlDmg->mFireDamage);
							
							perform_68k_call(addr, AVP_PCADDR_PLAYER_DAMAGE);
							
							//printf("Shank: %08X %i %08X\n", ampObjAddr, flags, bigpemu_jag_read32(ampObjAddr + AVP_AMPOFS_MODE));
						}
					}
				}
				--sAVPPlayerDamageCount;
			}
			
			if (sAVPAMPDriveRequestCount)
			{
				for (uint32_t driveReqIndex = 0; driveReqIndex < sAVPAMPDriveRequestCount; ++driveReqIndex)
				{
					TAVPLocalAMPDriveRequest *pDrive = &sAVPAMPDriveRequests[driveReqIndex];
					if (pDrive->mCurLevel == sCurLevelLocal && pDrive->mClientIndex >= 0 && pDrive->mClientIndex < MAX_AVP_CLIENTS && client_is_active(&sClientStates[pDrive->mClientIndex]))
					{
						//don't allow the client to take control if the thing isn't killable
						const uint32_t ampObjAddr = ampBaseAddr + pDrive->mAMPIndex * AVP_AMPSIZE;
						if (amp_is_killable(ampObjAddr))
						{
							TLocalAMPState *pLAMP = &sLocalAMP[pDrive->mAMPIndex];
							if (pLAMP->mClientController < 0)
							{
								//another crappy hack, remove the thing from the collision map if we're going into client-control mode, since that map is only synchronized host->client
								amp_remove_from_collmap(ampObjAddr);
								
								pLAMP->mClientController = pDrive->mClientIndex;
								//printf("Client got control (%i): %i (%08X)\n", pLAMP->mClientController, pDrive->mAMPIndex, ampObjAddr);
							}
						}
					}
				}
				sAVPAMPDriveRequestCount = 0;
			}
			
			if (sAVPTransferAMPDataSize)
			{
				for (uint32_t offset = 0; offset < sAVPTransferAMPDataSize; offset += AVP_AMPSIZE)
				{
					const int32_t ampIndex = get_free_amp();
					if (ampIndex < 0)
					{
						break;
					}
				
					bigpemu_jag_sysmemwrite(&sAVPTransferAMPData[offset], ampBaseAddr + ampIndex * AVP_AMPSIZE, AVP_AMPSIZE);				
				}
				sAVPTransferAMPDataSize = 0;
			}

			if (sClientToServerAMPDataSize)
			{
				//stomp data on the server if it's being driven by a client
				uint32_t offset = 0;
				while (offset < sClientToServerAMPDataSize)
				{
					const uint32_t ampIndex = *(const uint32_t *)&sClientToServerAMPData[offset];
					offset += sizeof(uint32_t);
					const uint8_t *pAMPSrc = &sClientToServerAMPData[offset];
					const uint32_t ampDst = ampBaseAddr + ampIndex * AVP_AMPSIZE;
					if (sLocalAMP[ampIndex].mClientController >= 0 && amp_is_killable(ampDst))
					{
#if 0
						bigpemu_jag_sysmemwrite(pAMPSrc, ampDst, AVP_AMPSIZE);
#else //hacky, interferes less with other state and reduces the risk of screwing something up on the server instance
						bigpemu_jag_sysmemwrite(pAMPSrc + AVP_AMPOFS_XPOS, ampDst + AVP_AMPOFS_XPOS, 4);
						bigpemu_jag_sysmemwrite(pAMPSrc + AVP_AMPOFS_YPOS, ampDst + AVP_AMPOFS_YPOS, 4);
						bigpemu_jag_sysmemwrite(pAMPSrc + AVP_AMPOFS_CREATURE, ampDst + AVP_AMPOFS_CREATURE, 2);
						bigpemu_jag_sysmemwrite(pAMPSrc + AVP_AMPOFS_ANIMSEQ, ampDst + AVP_AMPOFS_ANIMSEQ, 2);
						bigpemu_jag_sysmemwrite(pAMPSrc + AVP_AMPOFS_ANIMFRAME, ampDst + AVP_AMPOFS_ANIMFRAME, 2);
						bigpemu_jag_sysmemwrite(pAMPSrc + AVP_AMPOFS_ANGLE, ampDst + AVP_AMPOFS_ANGLE, 2);
#endif
					}
					offset += AVP_AMPSIZE;
				}
			}
			
			//make sure it's toggled on for the dataset that'll go to everyone else
			if (sLocalClientIndex >= 0)
			{
				toggle_client_amp(&sClientStates[sLocalClientIndex], AVP_AVATAR_AMP_MODE);
			}
			
			bigpemu_jag_sysmemread(spAMP, ampBaseAddr, AVP_AMP_TOTAL_SIZE);

			//prevent the local amp from rendering
			if (sLocalClientIndex >= 0)
			{
				toggle_client_amp(&sClientStates[sLocalClientIndex], 0);
			}
	
			sSendGameStateData = 1;
		}
		break;
	case kSharedNetConn_Client:
		{
			fill_client_state_from_local_data(pFirstCS);
			
			if (pGS->mVersion == AVP_PROTOCOL_VERSION)
			{
				if (pFirstCS->mWantLevel && pFirstCS->mWantLevel == pGS->mCurLevel)
				{
					//zero out the client's desire once we've gotten where we wanted to go
					pFirstCS->mWantLevel = 0;
				}
				
				const uint32_t wantLevel = bigpemu_jag_read16(AVP_NEW_LEVEL);
				if (sCurLevelLocal == pGS->mCurLevel)
				{
					if (wantLevel)
					{
						//if we want to change levels, we'll send the request to the host, but never do it locally
						bigpemu_jag_write16(AVP_NEW_LEVEL, 0);
					}

					bigpemu_jag_write16(AVP_DISCFLAG, pGS->mDiscFlag);
					
					//elect to continuously stomp these (intentionally not clearing fresh flags)
					if (sFreshAMPDataOnClient)
					{
						sClientToServerAMPDataSize = 0;
						const uint32_t ampBaseAddr = bigpemu_jag_read32(AVP_AMPS_PTR);
						uint32_t ampOffset = 0;
						for (uint32_t ampIndex = 0; ampIndex < AVP_MAX_AMP_COUNT; ++ampIndex)
						{
							const TLocalAMPState *pLAMP = &sLocalAMP[ampIndex];
							if (pLAMP->mClientController != sLocalClientIndex)
							{
								bigpemu_jag_sysmemwrite(spAMP + ampOffset, ampBaseAddr + ampOffset, AVP_AMPSIZE);
							}
							else
							{
								//keep the mirror data updated when controlling it from a client
								*(uint32_t *)&sClientToServerAMPData[sClientToServerAMPDataSize] = ampIndex;
								sClientToServerAMPDataSize += sizeof(uint32_t);
								bigpemu_jag_sysmemread(&sClientToServerAMPData[sClientToServerAMPDataSize], ampBaseAddr + ampOffset, AVP_AMPSIZE);
								sClientToServerAMPDataSize += AVP_AMPSIZE;
							}
							ampOffset += AVP_AMPSIZE;
						}
						//make the local amp invisible after getting data from the server
						if (sLocalClientIndex >= 0)
						{
							toggle_client_amp(&sClientStates[sLocalClientIndex], 0);
						}						
					}
					if (sFreshMazeDataMaxSize)
					{
						bigpemu_jag_sysmemwrite(spMazeData, AVP_MAZE_DATA, sFreshMazeDataMaxSize);
					}
					if (sFreshCollMapDataMaxSize)
					{
						bigpemu_jag_sysmemwrite(spCollMapData, AVP_COLLMAP, sFreshCollMapDataMaxSize);
					}
#if AVP_SYNC_OBJMAP					
					if (sFreshObjMapDataMaxSize)
					{
						bigpemu_jag_sysmemwrite(spObjMapData, AVP_OBJMAP, sFreshObjMapDataMaxSize);
					}
#endif
				}
				else if (!wantLevel)
				{
					//try to load into the same level as the host, and pop in on top of the host
					bigpemu_jag_write16(AVP_NEW_LEVEL, pGS->mCurLevel);
					bigpemu_jag_write32(AVP_NEW_X, pGS->mHostPos[0]);
					bigpemu_jag_write32(AVP_NEW_Y, pGS->mHostPos[1]);
				}
			}
			
			sFreshGameStateData = 0;
			sSendClientStateData = 1;
		}
		break;
	default:
		break;
	}
	
	if (connType != kSharedNetConn_None)
	{
		if (pFirstCS->mType == AVP_PT_HUMAN && sCurLevelLocal != 3)
		{
			//if human and the host has left the first level, make sure they've at least got a shotgun
			const uint8_t weaps = bigpemu_jag_read8(AVP_NEW_WEAPS);
			if (!(weaps & AVP_HUWEAP_SG) && !bigpemu_jag_read16(AVP_NEW_LEVEL))
			{
				bigpemu_jag_write8(AVP_NEW_WEAPS, weaps | AVP_HUWEAP_SG);
				if (bigpemu_jag_read16(AVP_AMMO_SG) == 0)
				{
					bigpemu_jag_write16(AVP_AMMO_SG, 0x0B);
				}
			}
		}
		
		if (sAVPBroadcastSFXInCount && !sRestore68KRegs)
		{
			TAVPBroadcastSFX *pSfx = &sAVPBroadcastSFXIn[sAVPBroadcastSFXInCount - 1];
			if (pSfx->mCurLevel == sCurLevelLocal && pSfx->mClientIndex != sLocalClientIndex)
			{
				if (pSfx->mDRegs[3] == SFX_R3_NO_LOCALIZE)
				{
					full_68k_reg_backup();
					bigpemu_jag_m68k_set_dreg(0, pSfx->mDRegs[0]);
					bigpemu_jag_m68k_set_dreg(1, pSfx->mDRegs[1]);
					bigpemu_jag_m68k_set_dreg(2, pSfx->mDRegs[2]);					
					perform_68k_call(addr, AVP_PCADDR_SFX);
				}
				else
				{
					const int32_t xy[2] = { (int32_t)bigpemu_jag_read32(AVP_X_POS), (int32_t)bigpemu_jag_read32(AVP_Y_POS) };
					const float sfxDistSq = calc_squared_distance(pSfx->mPos, xy);
					if (sfxDistSq <= AVP_MAX_DISTANCE_FOR_BROADCAST_SFX_SQ)
					{
						//since the game doesn't normally attenuate, let's do a crappy form of it here.
						//not sure if we have panning control or not, need to dig deeper.
						const float sfxDist = sqrtf(sfxDistSq);
						float frac = 1.0f - (sfxDist / AVP_MAX_DISTANCE_FOR_BROADCAST_SFX);
						frac *= frac; //whatever, really, but sounds a bit better with a sharper-than-linear falloff
						const int32_t vol = (int32_t)(10000.0f * frac);
						full_68k_reg_backup();
						
						//super-cheesy hack, we aren't handling stopping sounds correctly.
						//so if it's a looping sound, temporarily kill the loop flag.
						const uint32_t sfxAddr = pSfx->mDRegs[0];
						uint32_t sfxCtrl = bigpemu_jag_read16(sfxAddr + 4);
						if (sfxCtrl & 0x8000)
						{
							sRestoreSfxAddr = sfxAddr;
							sRestoreSfxCtrl = sfxCtrl;
							sfxCtrl &= ~0x8000;
							bigpemu_jag_write16(sfxAddr + 4, (uint16_t)sfxCtrl);
						}
						
						bigpemu_jag_m68k_set_dreg(0, sfxAddr);
						bigpemu_jag_m68k_set_dreg(1, vol); //pSfx->mDRegs[1]
						bigpemu_jag_m68k_set_dreg(2, pSfx->mDRegs[2]);
						bigpemu_jag_m68k_set_dreg(3, pSfx->mDRegs[3]);
						
						perform_68k_call(addr, AVP_PCADDR_SFX);
					}
				}
			}
			--sAVPBroadcastSFXInCount;
		}
	}	
}

static void set_default_client_states()
{
	for (uint32_t clIndex = 0; clIndex < MAX_AVP_CLIENTS; ++clIndex)
	{
		TAVPPlayerState *pCS = &sClientStates[clIndex];
		pCS->mAMPIndex = -1;
		pCS->mCurLevel = AVP_CURLEVEL_INVALID;
	}	
}

static uint32_t on_sw_loaded(const int32_t eventHandle, void *pEventData)
{
	sAVPLoaded = 0;
	sDidShowNetWarning = 0;
	sValidLevelState = 0;
	sLocalFrameCount = 0;
	set_default_client_states();
	
	const uint64_t hash = bigpemu_get_loaded_fnv1a64();
	if (hash == AVP_SUPPORTED_HASH)
	{
		sAVPLoaded = 1;
		sCurMazeWidth = 0;
		sCurMazeHeight = 0;
		sCurLevelLocal = -1;
		sSafeSpotLevel = -1;
		sSendGameStateData = 0;
		sSendClientStateData = 0;
		sClientToServerAMPDataSize = 0;
		sAVPOpenDoorActionCount = 0;
		sAVPPlayerDamageCount = 0;
		sAVPBroadcastSFXOutCount = 0;
		sAVPBroadcastSFXInCount = 0;
		sAVPAMPDriveRequestCount = 0;
		sAVPTransferAMPDataSize = 0;

		bigpemu_jag_m68k_bp_add(AVP_PCADDR_CHECK_NEWLEVEL, avp_check_newlevel);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_LIFT_DOOR, avp_lift_door);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_PLAYER_DAMAGE, avp_player_dmg);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_AMP_SLEEP_TEST, avp_sleep_test);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_AMP_WEAPON_HIT, avp_projectile_test);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_ENDGAME_DEATHYELL, avp_death);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_WEP_SFX, avp_playerlocal_sfx);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_NO_ACCSND, avp_global_sfx);
		//since all door management is done on the server, we could also pepper in hooks to broadcast door sounds.
		//for now we're only bothering with making sure "access denied" is broadcast since that's a more important feedback case.
		
#if !AVP_SYNC_OBJMAP
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_COLLECTIT_KILLEITHER, avp_collect_killamp);
#endif
		
		//these are locations where we want to let the client be responsible for making a thing, then shunting it to the server.
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_MAKE_SPARK_RET, avp_transport_amp_from_a0);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_PLAYER_WEAPON_RET, avp_transport_amp_from_a6);
		
		//ai projectiles
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_AMP_THROWDISC_RET, avp_transport_amp_from_a6);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_AMP_LAUNCHIT_P0, avp_transport_amp_from_a6);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_AMP_LAUNCHIT_P1, avp_transport_amp_from_a6);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_AMP_LAUNCHIT_P2, avp_transport_amp_from_a6);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_AMP_LAUNCHIT_P3, avp_transport_amp_from_a6);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_AMP_LASER_RET, avp_transport_amp_from_a6);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_AMP_FIRE_HUG_RET, avp_transport_amp_from_a6);
		bigpemu_jag_m68k_bp_add(AVP_PCADDR_AMP_EXPLOSION_RET, avp_transport_amp_from_a6);
	}
	
	return 0;
}

static void net_update_commmon()
{
	if (sAVPBroadcastSFXOutCount > 0)
	{
		bigpemu_net_send_nodelta(AVP_NET_MSG_ID_SFX, BIGPEMU_CLIENT_DEST_ALL, sAVPBroadcastSFXOut, sizeof(TAVPBroadcastSFX) * sAVPBroadcastSFXOutCount);		
		sAVPBroadcastSFXOutCount = 0;
	}
}

static uint32_t on_net_update(const int32_t eventHandle, void *pEventData)
{
	const ESharedNetConnectionType connType = bigpemu_net_connection_type();

	sLocalClientIndex = bigpemu_net_client_index();
	
	if (!sAVPLoaded || bigpemu_net_behind(BIGPEMU_CLIENT_DEST_ALL))
	{
		//lazy/blanket approach, hold off on network delta updates if anyone is catching up to the current sequence
		return 0;
	}
	
	switch (connType)
	{
	case kSharedNetConn_Host:
		{
			if (sSendGameStateData)
			{
				bigpemu_net_send(AVP_NET_MSG_ID_GAMESTATE, BIGPEMU_CLIENT_DEST_ALL, spGS, sizeof(TAVPGameState));
				bigpemu_net_send_elems(AVP_NET_MSG_ID_AMPSTATE_BEGIN, BIGPEMU_CLIENT_DEST_ALL, spAMP, AVP_AMPSIZE, AVP_MAX_AMP_COUNT);
				
				if (sCurMazeWidth && sCurMazeHeight && (sCurMazeWidth + AVP_MAZE_PAD_DIMENSION) <= AVP_MAZE_MAX_WIDTH && (sCurMazeHeight + AVP_MAZE_PAD_DIMENSION) <= AVP_MAZE_MAX_HEIGHT)
				{
					const uint32_t groupCount = get_maze_group_count();
					bigpemu_net_send_elems(AVP_NET_MSG_ID_MAZEDATA, BIGPEMU_CLIENT_DEST_ALL, spMazeData, AVP_NET_MAZEGROUP_SIZE, groupCount);
					
					bigpemu_net_send_elems(AVP_NET_MSG_ID_COLLMAP_BEGIN, BIGPEMU_CLIENT_DEST_ALL, spCollMapData, AVP_COLLMAP_GROUP_SIZE, AVP_COLLMAP_GROUP_COUNT);
#if AVP_SYNC_OBJMAP					
					bigpemu_net_send_elems(AVP_NET_MSG_ID_OBJMAP_BEGIN, BIGPEMU_CLIENT_DEST_ALL, spObjMapData, AVP_OBJMAP_GROUP_SIZE, AVP_OBJMAP_GROUP_COUNT);
#endif
				}
				
				bigpemu_net_send_elems(AVP_NET_MSG_ID_AMPLOCAL_BEGIN, BIGPEMU_CLIENT_DEST_ALL, sLocalAMP, sizeof(TLocalAMPState), AVP_MAX_AMP_COUNT);
			
				sSendGameStateData = 0;
			}
			
			//shitty hack, overriding the hash locally for now, using data that's been propagated to the server internally.
			//this happens to handle the disconnected case a bit better. we're wasting bandwidth by syncing the hash, but usually delta compression culls it.
			for (uint32_t clientIndex = 0; clientIndex < MAX_AVP_CLIENTS; ++clientIndex)
			{
				TAVPPlayerState *pCS = &sClientStates[clientIndex];
				pCS->mSWHash = bigpemu_net_client_sw_hash(clientIndex);
				if (sClientHurt[clientIndex])
				{
					if (clientIndex != sLocalClientIndex)
					{
						bigpemu_net_send_nodelta(AVP_NET_MSG_ID_HURT_CLIENT, clientIndex, &sClientHurt[clientIndex], sizeof(uint32_t));					
						sClientHurt[clientIndex] = 0;
					}
				}
			}
			
			net_update_commmon();
		}
		break;
	case kSharedNetConn_Client:
		{
			if (sSendClientStateData)
			{
				TAVPPlayerState *pCS = sClientStates;	
				pCS->mClientIndex = sLocalClientIndex;
				pCS->mSWHash = bigpemu_net_client_sw_hash(pCS->mClientIndex);
				//another slight hack, always send the client state, but with an invalid level if we haven't entered a valid state
				if (!sValidLevelState)
				{
					pCS->mCurLevel = AVP_CURLEVEL_INVALID;
				}
				bigpemu_net_send(AVP_NET_MSG_ID_PLAYERSTATE, BIGPEMU_CLIENT_DEST_ALL, pCS, sizeof(TAVPPlayerState));
			}
			
			if (spGS->mVersion == AVP_PROTOCOL_VERSION && spGS->mCurLevel == sCurLevelLocal)
			{
				if (sAVPOpenDoorActionCount > 0)
				{
					const int32_t clientIndex = sLocalClientIndex;
					for (uint32_t actionIndex = 0; actionIndex < sAVPOpenDoorActionCount; ++actionIndex)
					{
						sAVPOpenDoorActions[actionIndex].mClientIndex = clientIndex;
					}
					
					bigpemu_net_send_nodelta(AVP_NET_MSG_ID_LIFT_DOOR, BIGPEMU_CLIENT_DEST_ALL, sAVPOpenDoorActions, sizeof(TAVPRemoteOpenDoor) * sAVPOpenDoorActionCount);
					
					sAVPOpenDoorActionCount = 0;
				}
				if (sAVPPlayerDamageCount > 0)
				{
					bigpemu_net_send_nodelta(AVP_NET_MSG_ID_CLIENT_DMG, BIGPEMU_CLIENT_DEST_ALL, sAVPPlayerDamages, sizeof(TAVPPlayerDamage) * sAVPPlayerDamageCount);
					sAVPPlayerDamageCount = 0;
				}
				if (sAVPAMPDriveRequestCount > 0)
				{
					bigpemu_net_send_nodelta(AVP_NET_MSG_ID_CLIENT_DRIVE, BIGPEMU_CLIENT_DEST_ALL, sAVPAMPDriveRequests, sizeof(TAVPLocalAMPDriveRequest) * sAVPAMPDriveRequestCount);
					sAVPAMPDriveRequestCount = 0;
				}
				if (sAVPTransferAMPDataSize > 0)
				{
					bigpemu_net_send_nodelta(AVP_NET_MSG_ID_CLIENT_AMPTRANSFER, BIGPEMU_CLIENT_DEST_ALL, sAVPTransferAMPData, sAVPTransferAMPDataSize);
					sAVPTransferAMPDataSize = 0;
				}
				
				if (sClientToServerAMPDataSize)
				{
					bigpemu_net_send(AVP_NET_MSG_ID_CLIENT_C2SAMP, BIGPEMU_CLIENT_DEST_ALL, sClientToServerAMPData, sClientToServerAMPDataSize);
				}
			}
			
			net_update_commmon();
		}
		break;
	default:
		break;
	}
	
	return 0;
}

static void net_receive_common(const uint32_t msgId, const uint32_t msgSize, const ESharedNetConnectionType connType)
{
	if (msgId == AVP_NET_MSG_ID_SFX)
	{
		const uint32_t newCount = msgSize / sizeof(TAVPBroadcastSFX);
		if ((sAVPBroadcastSFXInCount + newCount) <= AVP_MAX_PENDING_ACTIONS)
		{
			TAVPBroadcastSFX *pSfx = &sAVPBroadcastSFXIn[sAVPBroadcastSFXInCount];
			bigpemu_net_recv(NULL, pSfx, msgSize);
			sAVPBroadcastSFXInCount += newCount;
			
			if (connType == kSharedNetConn_Host)
			{
				//let the host re-broadcast sfx coming in from clients
				if ((sAVPBroadcastSFXOutCount + newCount) <= AVP_MAX_PENDING_ACTIONS)
				{
					memcpy(&sAVPBroadcastSFXOut[sAVPBroadcastSFXOutCount], pSfx, msgSize);
					sAVPBroadcastSFXOutCount += newCount;
				}
			}
		}
	}
}

static uint32_t on_net_receive(const int32_t eventHandle, void *pEventData)
{
	const ESharedNetConnectionType connType = bigpemu_net_connection_type();

	uint32_t msgId;
	const uint32_t msgSize = bigpemu_net_recv(&msgId, NULL, 0);
	if (msgSize == 0)
	{
		return 0;
	}
	
	switch (connType)
	{
	case kSharedNetConn_Host:
		{
			if (msgId == AVP_NET_MSG_ID_PLAYERSTATE)
			{
				TAVPPlayerState cs;
				bigpemu_net_recv(NULL, &cs, sizeof(cs));
				if (cs.mVersion == AVP_PROTOCOL_VERSION && cs.mClientIndex < MAX_AVP_CLIENTS)
				{
					//special case, preserve the amp index
					TAVPPlayerState *pCS = &sClientStates[cs.mClientIndex];
					const int32_t ampIndex = pCS->mAMPIndex;
					memcpy(pCS, &cs, sizeof(cs));
					pCS->mAMPIndex = ampIndex;
					sFreshClientStateData |= (1 << cs.mClientIndex);
				}
				else
				{
					const int32_t recvFromClient = bigpemu_net_lastclient();
					printf("Bad client state (%i vs %i) from client %i\n", cs.mVersion, AVP_PROTOCOL_VERSION, recvFromClient);
					bigpemu_net_disconnect(recvFromClient);
				}
			}
			else if (msgId == AVP_NET_MSG_ID_CLIENT_C2SAMP)
			{
				bigpemu_net_recv(NULL, sClientToServerAMPData, msgSize);
				sClientToServerAMPDataSize = msgSize;
			}
			else if (msgId == AVP_NET_MSG_ID_LIFT_DOOR)
			{
				const uint32_t newCount = msgSize / sizeof(TAVPRemoteOpenDoor);
				if ((sAVPOpenDoorActionCount + newCount) <= AVP_MAX_PENDING_ACTIONS)
				{
					bigpemu_net_recv(NULL, &sAVPOpenDoorActions[sAVPOpenDoorActionCount], msgSize);
					sAVPOpenDoorActionCount += newCount;
				}
			}
			else if (msgId == AVP_NET_MSG_ID_CLIENT_DMG)
			{
				const uint32_t newCount = msgSize / sizeof(TAVPPlayerDamage);
				if ((sAVPPlayerDamageCount + newCount) <= AVP_MAX_PENDING_ACTIONS)
				{
					bigpemu_net_recv(NULL, &sAVPPlayerDamages[sAVPPlayerDamageCount], msgSize);
					sAVPPlayerDamageCount += newCount;
				}
			}
			else if (msgId == AVP_NET_MSG_ID_CLIENT_DRIVE)
			{
				const uint32_t newCount = msgSize / sizeof(TAVPLocalAMPDriveRequest);
				if ((sAVPAMPDriveRequestCount + newCount) <= AVP_MAX_PENDING_ACTIONS)
				{
					bigpemu_net_recv(NULL, &sAVPAMPDriveRequests[sAVPAMPDriveRequestCount], msgSize);
					sAVPAMPDriveRequestCount += newCount;
				}
			}
			else if (msgId == AVP_NET_MSG_ID_CLIENT_AMPTRANSFER)
			{
				if ((sAVPTransferAMPDataSize + msgSize) <= AVP_MAX_TRANSFER_AMP_SIZE)
				{
					uint8_t *pAmpData = &sAVPTransferAMPData[sAVPTransferAMPDataSize];
					bigpemu_net_recv(NULL, pAmpData, msgSize);
					const int32_t recvFromClient = bigpemu_net_lastclient();
					//assign ownership on the host to the appropriate amp avatar object
					if (recvFromClient >= 0 && recvFromClient < MAX_AVP_CLIENTS)
					{
						for (uint32_t offset = 0; offset < msgSize; offset += AVP_AMPSIZE)
						{
							uint8_t *pAmpObject = pAmpData + offset;
							const uint32_t ampMode = *(uint16_t *)(pAmpObject + AVP_AMPOFS_MODE);
							if (ampMode)
							{
								const uint32_t ct = BIGPEMU_NATIVE_SWAP16(*(uint16_t *)(pAmpObject + AVP_AMPOFS_CREATURE));
								if (is_ct_projectile(ct))
								{
									const uint32_t launchAmp = *(uint16_t *)(pAmpObject + AVP_AMPOFS_LAUNCHAMP);
									if (launchAmp == 0xFFFF)
									{
										const int32_t clAmpIndex = spGS->mAMPIndices[recvFromClient];
										if (clAmpIndex >= 0)
										{
											*(uint16_t *)(pAmpObject + AVP_AMPOFS_LAUNCHAMP) = BIGPEMU_NATIVE_SWAP16((uint16_t)clAmpIndex);
										}
									}
								}
							}
						}
					}
					
					sAVPTransferAMPDataSize += msgSize;
				}
			}
			else
			{
				net_receive_common(msgId, msgSize, connType);
			}
		}
		break;
	case kSharedNetConn_Client:
		{
			if (msgId == AVP_NET_MSG_ID_GAMESTATE)
			{
				bigpemu_net_recv(NULL, spGS, sizeof(TAVPGameState));
				if (spGS->mVersion == AVP_PROTOCOL_VERSION)
				{
					if (sLocalClientIndex >= 0)
					{
						//copy the amp index over, it'll be used to prevent the local amp-atar from rendering
						sClientStates[sLocalClientIndex].mAMPIndex = spGS->mAMPIndices[sLocalClientIndex];
					}
					sFreshGameStateData = 1;
				}
				else
				{
					printf("Bad game state (%i vs %i)\n", spGS->mVersion, AVP_PROTOCOL_VERSION);
					bigpemu_net_disconnect(BIGPEMU_CLIENT_DEST_ALL);
				}				
			}
			else if (msgId >= AVP_NET_MSG_ID_AMPSTATE_BEGIN && msgId < AVP_NET_MSG_ID_AMPSTATE_END)
			{
				const uint32_t ampIndex = msgId - AVP_NET_MSG_ID_AMPSTATE_BEGIN;
				bigpemu_net_recv(NULL, spAMP + ampIndex * AVP_AMPSIZE, AVP_AMPSIZE);
				sFreshAMPDataOnClient = 1;
			}
			else if (msgId >= AVP_NET_MSG_ID_COLLMAP_BEGIN && msgId < AVP_NET_MSG_ID_COLLMAP_END)
			{
				const uint32_t groupIndex = msgId - AVP_NET_MSG_ID_COLLMAP_BEGIN;
				const uint32_t dataOffset = groupIndex * AVP_OBJMAP_GROUP_SIZE;
				bigpemu_net_recv(NULL, spCollMapData + dataOffset, AVP_OBJMAP_GROUP_SIZE);
				sFreshCollMapDataMaxSize = BIGPEMU_MAX(dataOffset + AVP_OBJMAP_GROUP_SIZE, sFreshCollMapDataMaxSize);
			}
			else if (msgId >= AVP_NET_MSG_ID_OBJMAP_BEGIN && msgId < AVP_NET_MSG_ID_OBJMAP_END)
			{
				const uint32_t groupIndex = msgId - AVP_NET_MSG_ID_OBJMAP_BEGIN;
				const uint32_t dataOffset = groupIndex * AVP_OBJMAP_GROUP_SIZE;
				bigpemu_net_recv(NULL, spObjMapData + dataOffset, AVP_OBJMAP_GROUP_SIZE);
				sFreshObjMapDataMaxSize = BIGPEMU_MAX(dataOffset + AVP_OBJMAP_GROUP_SIZE, sFreshObjMapDataMaxSize);
			}
			else if (msgId >= AVP_NET_MSG_ID_AMPLOCAL_BEGIN && msgId < AVP_NET_MSG_ID_AMPLOCAL_END)
			{
				const uint32_t ampIndex = msgId - AVP_NET_MSG_ID_AMPLOCAL_BEGIN;
				bigpemu_net_recv(NULL, &sLocalAMP[ampIndex], sizeof(TLocalAMPState));
			}
			else if (msgId >= AVP_NET_MSG_ID_MAZEDATA)
			{
				const uint32_t blockGroupIndex = msgId - AVP_NET_MSG_ID_MAZEDATA;
				const uint32_t mazeOffset = blockGroupIndex * AVP_NET_MAZEGROUP_SIZE;
				bigpemu_net_recv(NULL, spMazeData + mazeOffset, AVP_NET_MAZEGROUP_SIZE);
				sFreshMazeDataMaxSize = BIGPEMU_MAX(mazeOffset + AVP_NET_MAZEGROUP_SIZE, sFreshMazeDataMaxSize);
			}
			else if (msgId == AVP_NET_MSG_ID_HURT_CLIENT)
			{
				if (sLocalClientIndex >= 0)
				{
					bigpemu_net_recv(NULL, &sClientHurt[sLocalClientIndex], sizeof(uint32_t));
				}
			}
			else
			{
				net_receive_common(msgId, msgSize, connType);
			}
		}
		break;
	default:
		break;
	}
	
	return 0;
}

void bigp_init()
{
	void *pMod = bigpemu_get_module_handle();
	
	sOnLoadEvent = bigpemu_register_event_sw_loaded(pMod, on_sw_loaded);
	sOnNetUpdateEvent = bigpemu_register_event_net_update(pMod, on_net_update);
	sOnNetReceiveEvent = bigpemu_register_event_net_receive(pMod, on_net_receive);
	
	memset(sClientStates, 0, sizeof(sClientStates));
	set_default_client_states();
	memset(sAVPNetBuffer, 0, sizeof(sAVPNetBuffer));
	sFreshGameStateData = 0;
	sFreshClientStateData = 0;
	sFreshAMPDataOnClient = 0;
	sFreshMazeDataMaxSize = 0;
	sValidLevelState = 0;
	sLocalFrameCount = 0;
	reset_local_amp();	
}

void bigp_shutdown()
{
	void *pMod = bigpemu_get_module_handle();
	bigpemu_unregister_event(pMod, sOnLoadEvent);
	bigpemu_unregister_event(pMod, sOnNetUpdateEvent);
	bigpemu_unregister_event(pMod, sOnNetReceiveEvent);
	sOnLoadEvent = -1;
	sOnNetUpdateEvent = -1;
	sOnNetReceiveEvent = -1;
}
