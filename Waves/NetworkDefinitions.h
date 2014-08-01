enum NetworkMessageType { JOINREQUEST = 556656, JOINACCEPT, CLIENTSENDINPUT, SERVERSENDSTATE };

typedef struct
{
	int physicsObjectID;
	float x;
	float y;
	float z;
	float yaw;
	float pitch;
	float roll;
} MessageBlock;

typedef struct
{
	int	numMessageBlocks;
	MessageBlock* messageBlocks;
} NetworkMessageBody;

typedef struct
{
	NetworkMessageType messageType;
	NetworkMessageBody messageBody;
} NetworkMessage;