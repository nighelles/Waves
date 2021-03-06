#pragma once

#include <d3d11.h>
#include <d3dx10math.h>

#include "PhysicsEntity.h"

enum NetworkMessageType { JOINREQUEST = 0xAAAA, JOINACCEPT = 0xAAAB, CLIENTSENDINPUT, SERVERSENDSTATE };

enum { Network_W = 0, Network_A, Network_S, Network_D, Network_SHIFT, Network_CONTROL, Network_SPACE };

#define MAXNETWORKENTITIES 10
#define MAXACKDELAY 100

#define DATALENGTH 580
#define STREAMLENGTH 1015

#define ERROR_INTERPOLATION_TIME .2f

typedef struct
{
	bool keys[7];
	float mouseDX, mouseDY;
	bool mouse[2];
} NetworkedInput;

typedef struct
{
	float x, y, z;
	float vx, vy, vz;
	float rx, ry, rz;
} NetworkedEntity;

typedef struct
{
	NetworkMessageType messageType;
} NetworkMessage;

typedef struct
{
	NetworkMessageType	messageType;
	UINT32 ack;
	UINT32 clientAck;
	char data[DATALENGTH];
} ServerNetworkMessage;

typedef struct
{
	NetworkMessageType	messageType;
	UINT32 ack;
	int		playerNumber;
	NetworkedInput input;
} ClientNetworkMessage;