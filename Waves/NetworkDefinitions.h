enum NetworkMessageType { JOINREQUEST = 556656, JOINACCEPT, CLIENTSENDINPUT, SERVERSENDSTATE };

typedef struct
{
	int physicsEntityID;
	float x;
	float y;
	float z;
	float yaw;
	float pitch;
	float roll;
} ServerMessageBlock;

typedef struct
{
	NetworkMessageType messageType;
} NetworkMessage;

typedef struct
{
	NetworkMessageType	messageType;
	int					numEntityStates;
	ServerMessageBlock	entityStates[10];
} ServerNetworkMessage;

typedef struct
{
	NetworkMessageType	messageType;
	bool	forward;
	bool	backward;
	bool	left;
	bool	right;
	float	mouseDX;
	float	mouseDY;
} ClientNetworkMessage;