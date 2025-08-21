// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Endpoint.h"
#include "Runtime/Networking/Public/Interfaces/IPv4/IPv4Address.h"
#include "Runtime/Networking/Public/Common/UdpSocketReceiver.h"
#include "ARVRGameManager.h"
#include "UDPCommunicationsManager.generated.h"



UCLASS()
class VRARTEST_API AUDPCommunicationsManager : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	AUDPCommunicationsManager();

	// Tick will handle the sending of small Connection packets when conneciton is attempting to be stablished by the host device sending a pakcet every frame until a receipt packet is 
	// recieved and the boolean connectionEstablished is true on both devices.
	virtual void Tick(float DeltaTime) override;

	/*
		BroadcastSetup() is used to set up the endpoints and sockets needed to send and receive messages over the network, the functionality is from the Networking libraries available in Unreal,
		first the socketSubsystem is retrieved, this is device type specific and is needed to interface with the sockets of specific system. Next a endpoint is created intially set to the IPv4
		address (FIPv4Address(255, 255, 255, 255), BroadcastPort) this allows messages to be broadcast to all devices connected to the same network as the broadcaster. This casts a wide
		net in order to find the client device who will be the only device listening on the same port for this message, this port will only be used in the retrieval of the other players IPv4 address,
		therefore a general endpoint is created aswell set to IPv4Address::Any, the ip address of the device.

		The broadcast socket is then created aswell, this will be the socket messages are broadcasted from. This regular endpoint is then bound to the socket, and broadcasting on this socket is enabled. 
		This process is then repeated for the Listening socket however we will not need to create a general IPv4 address. The port number will be set to something different from the broadcasting port.
	*/
	void BroadcastSetup(int CommunicationPort, int ListenPort);
	
	// BroadcastStop() closes both of the sockets and destroys them setting the pointers to nullptr
	void BroadcastStop();

	// The SendMessage() function takes a serialised command, a packet, in it's parameters ut will then check if the other users endpoint has been found if it has it will send the packet from the
	// broadcast socket to the otherusers endpoint, if not it will broadcast to that endpoint created in Broadcast set up to send to all users on the network. Once all bytes have been sent it returns true,
	// otherwise it returns false
	bool SendMessage(TArray<uint8> Packet);

	// ReceiveMessage() is used to send any received packets to the ARVRManager, it will cehck the ListenSocket for any pending data, if some is found it will turn convert it into a uint8 array, and is sent
	// to the ARVRManager for deserialisation and execution. If the other users endpoint hasn't been found it can use the packets recieved to set this variable.
	bool ReceiveMessage();

	 // setHostDevice() just sets the specific listening and broadcasting ports depending on the boolean in the parameter.
	void setHostDevice(bool isHost);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP")
	bool hostDevice; //Determines whether this is the host device or not.

	void setDeviceTypeSelected() { deviceTypeSelected = true; };

	bool getConnectionEstablished() { return connectionEstablished; };

	void setConnectionEstablished(bool isEstablished) { connectionEstablished = isEstablished; };



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FSocket* BroadcastSocket;
	TSharedPtr<FIPv4Endpoint> BroadcastEndpoint;

	FSocket* SenderSocket;
	FSocket* ListenSocket;
	TSharedPtr<FUdpSocketReceiver> SocketReciever;

	int communicationPort;
	int listeningPort;

	TSharedPtr<FIPv4Endpoint> otherUserListeningEndpoint;

	
	bool connectionEstablished;

	bool deviceTypeSelected;

	AARVRGameManager* manager;

};
