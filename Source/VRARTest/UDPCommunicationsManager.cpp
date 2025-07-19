// Fill out your copyright notice in the Description page of Project Settings.

#include "UDPCommunicationsManager.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/BufferArchive.h"
#include "Command.h"

// Sets default values
AUDPCommunicationsManager::AUDPCommunicationsManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
}

// Called when the game starts or when spawned
void AUDPCommunicationsManager::BeginPlay()
{
	Super::BeginPlay();

	AARVRGameManager* Manager = Cast<AARVRGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AARVRGameManager::StaticClass()));
	if (!Manager)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Could not find Manager!"));
	}
	manager = Manager;
	connectionEstablished = false;
	deviceTypeSelected = false;
	otherUserListeningEndpoint = nullptr;

}

// Called every frame
void AUDPCommunicationsManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (deviceTypeSelected) {
		if (!connectionEstablished) {
			if (hostDevice) {
				TArray<uint8> packet;
				packet.Add(static_cast<uint8>(AARVRGameManager::EMessageType::Connection));
				packet.Add(manager->getSequenceCount());
				SendMessage(packet);
			}
		}
		ReceiveMessage();
	}
	
}

void AUDPCommunicationsManager::BroadcastSetup(int BroadcastPort, int ListenPort)
{

	ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

	BroadcastEndpoint = MakeShared<FIPv4Endpoint>(FIPv4Address(255, 255, 255, 255), BroadcastPort);

	//We need to create a seperate Endpoint, to bind the socket to, this is specific to the device, when we broadcast the message it is going from the socket to the 
	// endpoint therefore it needs to be local.
	FIPv4Endpoint  DeviceBroadcastAddress = FIPv4Endpoint(FIPv4Address::Any, BroadcastPort);


	//Gets the current platform, creates a socket of type NAME_DGram gives it a name and sets IPv6 usage to false
	BroadcastSocket = SocketSubsystem->CreateSocket(NAME_DGram, FString("BroadcastSocket"), false);

	if (!BroadcastSocket->Bind(*DeviceBroadcastAddress.ToInternetAddr()))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to bind broadcasts socket"));
		BroadcastSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(BroadcastSocket);
		BroadcastSocket = nullptr;
		return;
	}

	BroadcastSocket->SetNonBlocking(true);
	BroadcastSocket->SetReuseAddr(true);
	BroadcastSocket->SetBroadcast(true);

	UE_LOG(LogTemp, Log, TEXT("Broadcast socket bound to: %s"), *BroadcastEndpoint.Get()->ToString());


	FIPv4Endpoint ListenEndpoint(FIPv4Address::Any, ListenPort);

	//Gets the current platform, creates a socket of type NAME_DGram gives it a name and sets IPv6 usage to false
	ListenSocket = SocketSubsystem->CreateSocket(NAME_DGram, FString("ListenerSocket"), false);

	if (!ListenSocket->Bind(*ListenEndpoint.ToInternetAddr()))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to bind listener socket"));
		ListenSocket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
		ListenSocket = nullptr;
		return;
	}

	ListenSocket->SetNonBlocking(true);
	ListenSocket->SetReuseAddr(true);

	UE_LOG(LogTemp, Log, TEXT("Listener socket successfully bound to: %s"), *ListenEndpoint.ToString());


	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("UDP Manager: Setup Complete"));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("  - Broadcast Port: %d"), BroadcastPort));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("  - Listen Port: %d"), ListenPort));
}

void AUDPCommunicationsManager::BroadcastStop() 
{
	BroadcastSocket->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(BroadcastSocket);
	BroadcastSocket = nullptr;

	ListenSocket->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ListenSocket);
	ListenSocket = nullptr;
}

bool AUDPCommunicationsManager::SendMessage(TArray<uint8> Packet)
{
	TSharedPtr<FIPv4Endpoint> destinationEndpoint = nullptr;
	if (otherUserListeningEndpoint == nullptr) 
	{
		destinationEndpoint = BroadcastEndpoint;
	}
	else 
	{
		destinationEndpoint = otherUserListeningEndpoint;
	}
	int32 BytesSent = 0;
	bool broadcastSent = BroadcastSocket->SendTo(Packet.GetData(), Packet.Num(), BytesSent, *destinationEndpoint.Get()->ToInternetAddr());

	if (!broadcastSent || BytesSent == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to send broadcast message"));
		return false;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Sending packet"));

	return true;
	

}

bool AUDPCommunicationsManager::ReceiveMessage() 
{
	uint32 currentBufferSize = 0;

	while (ListenSocket->HasPendingData(currentBufferSize) && currentBufferSize > 0)
	{
		TArray<uint8> receivedPacket;
		receivedPacket.SetNumUninitialized(currentBufferSize);

		int32 BytesRead = 0;

		TSharedRef<FInternetAddr> senderAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

		if (ListenSocket->RecvFrom(receivedPacket.GetData(), receivedPacket.Num(), BytesRead, *senderAddress))
		{
			FIPv4Endpoint senderEndpoint = FIPv4Endpoint(senderAddress);
			if (!otherUserListeningEndpoint)
			{
				otherUserListeningEndpoint = MakeShared<FIPv4Endpoint>(senderEndpoint.Address, communicationPort);
			}

			manager->AddToIncomingCommandQueue(receivedPacket);

			return true;
		}
	}

	return false;

}

void AUDPCommunicationsManager::setHostDevice(bool isHost) 
{
	if (isHost) {
		communicationPort = 7001;
		listeningPort = 7002;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Host Device"));
		hostDevice = true;
	}
	else {
		communicationPort = 7002;
		listeningPort = 7001;
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Client Device"));
		hostDevice = false;
	}
	deviceTypeSelected = true;
	BroadcastSetup(communicationPort, listeningPort);

}


