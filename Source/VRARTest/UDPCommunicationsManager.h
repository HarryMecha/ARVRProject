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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BroadcastSetup(int CommunicationPort, int ListenPort);
	
	void BroadcastStop();

	bool SendMessage(TArray<uint8> Packet);

	bool ReceiveMessage();

	void setHostDevice(bool isHost);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UDP")
	bool hostDevice;

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
