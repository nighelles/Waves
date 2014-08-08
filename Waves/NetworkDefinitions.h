#pragma once

#include <d3d11.h>
#include <d3dx10math.h>

enum NetworkMessageType { JOINREQUEST = 0xAAAA, JOINACCEPT = 0xAAAB, CLIENTSENDINPUT, SERVERSENDSTATE };

enum { Network_W = 0, Network_A, Network_S, Network_D, Network_SHIFT, Network_CONTROL, Network_SPACE };

typedef struct
{
	bool keys[7];
	float mouseDX, mouseDY;
} NetworkedInput;

typedef struct
{
	int physicsEntityID;
	D3DXVECTOR3 position;
	D3DXVECTOR3 velocity;
	D3DXVECTOR3 rotation;
} ServerMessageBlock;

typedef struct
{
	NetworkMessageType messageType;
} NetworkMessage;

typedef struct
{
	NetworkMessageType	messageType;
	UINT32 ack;
	int					numEntityStates;
	ServerMessageBlock	entityStates[10];
} ServerNetworkMessage;

typedef struct
{
	NetworkMessageType	messageType;
	UINT32 ack;
	NetworkedInput input;
} ClientNetworkMessage;