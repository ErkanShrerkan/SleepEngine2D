#include "pch.h"
#include "AudioEngine.h"
#include <iostream>
#include <stdio.h>
#include <algorithm>
#include "CommonUtilities/Math/Matrix.hpp"
#include "Engine/CDebugDrawer.h"
#include <random>

#ifdef _DEBUG

#pragma comment(lib,"fmodL_vc.lib")
#pragma comment(lib,"fmodstudioL_vc.lib")

#else

#pragma comment(lib,"fmod_vc.lib")
#pragma comment(lib,"fmodstudio_vc.lib")

#endif

namespace SE
{
	CAudioEngine::CAudioEngine()
	{
		InitSystem();
		LoadBanks();
		LoadAllEvents();
		LoadAllBuses();
		mySystem->setNumListeners(1);
		LoadSnapshots();
		myMusic = CreateEventInstance(AudioClip::Music_Non_Diegetic_Ambience);

	}

	void CAudioEngine::LoadSnapshots()
	{
		FMOD::Studio::EventInstance* snapshot = CreateEventInstance(AudioSnapShot::BigReverb);
		myEventInstances[AudioSnapShot::BigReverb].push_back(snapshot);
		FMOD::Studio::EventInstance* snapshot2 = CreateEventInstance(AudioSnapShot::SmallReverb);
		myEventInstances[AudioSnapShot::SmallReverb].push_back(snapshot2);
		FMOD::Studio::EventInstance* snapshot3 = CreateEventInstance(AudioSnapShot::PlayerDetected);
		myEventInstances[AudioSnapShot::PlayerDetected].push_back(snapshot3);
	}

	FMOD::Studio::EventInstance* CAudioEngine::CreateEventInstance(const FMOD::Studio::ID& anID)
	{
		FMOD::Studio::EventInstance* eventInstance;
		myEventDescriptions[anID]->createInstance(&eventInstance);
		myEventInstances[anID].push_back(eventInstance);
		return eventInstance;
	}

	void CAudioEngine::StartEvent(FMOD::Studio::EventInstance* anEventInstance, const char* aEventParameterName, float aParameterValue)
	{
		anEventInstance->start();

		if (aEventParameterName != nullptr)
		{
			anEventInstance->setParameterByName(aEventParameterName, aParameterValue);
		}

		anEventInstance->release();
	}

	void CAudioEngine::StartEventOneShot(
		const FMOD_GUID& anEventID, 
		const Vector3f& anObjectPosition,
		const bool& aHasWorldPosition, 
		const char* aEventParameterName , 
		float aParameterValue)
	{
		if (myEventDescriptions[anEventID] == nullptr)
		{
			printf("FMOD error! Event could not be played. Eventdescription does not exist.\n");
		}

		FMOD::Studio::EventInstance* eventInstance;
		myEventDescriptions[anEventID]->createInstance(&eventInstance);

		if (aHasWorldPosition)
		{
			SetPosition(eventInstance, anObjectPosition);
		}
		else
		{
			SetUIPosition(eventInstance);
		}
		StartEvent(eventInstance, aEventParameterName, aParameterValue);
		
	}

	void CAudioEngine::StartEventOneShot(
		const FMOD_GUID& anEventID, 
		const Matrix4x4f& anObjectTransform, 
		const bool& aHasWorldPosition, 
		const char* aEventParameterName, 
		float aParameterValue)
	{
		if (myEventDescriptions[anEventID] == nullptr)
		{
			printf("FMOD error! Event could not be played. Eventdescription does not exist.\n");
		}

		FMOD::Studio::EventInstance* eventInstance;
		myEventDescriptions[anEventID]->createInstance(&eventInstance);

		if (aHasWorldPosition)
		{
			SetTransform(eventInstance, anObjectTransform);
		}
		else
		{
			SetUIPosition(eventInstance);
		}
		StartEvent(eventInstance, aEventParameterName, aParameterValue);
	}

	void CAudioEngine::SetGlobalParameter(const char* aEventParameterName, const float& aValue)
	{
		mySystem->setParameterByName(aEventParameterName, aValue);
	}

	void CAudioEngine::SetMuteBus(const FMOD_GUID& aBusID, const bool& aBool)
	{
		myBuses[aBusID]->setMute(aBool);
	}

	float CAudioEngine::GetBusVolume(const FMOD_GUID& aBusID)
	{
		float volume;
		myBuses[aBusID]->getVolume(&volume);
		return volume;
	}

	bool CAudioEngine::GetMuteBus(const FMOD_GUID& aBusID)
	{
		bool isMuted;
		myBuses[aBusID]->getMute(&isMuted);
		return isMuted;
	}

	void CAudioEngine::SetBusVolume(const FMOD_GUID& aBusID, const float& aVolume)
	{
		myBuses[aBusID]->setVolume(aVolume);
	}
	
	void CAudioEngine::LoadBanks()
	{
		mySystem->loadBankFile("Audio/Desktop/Master.bank", FMOD_STUDIO_LOAD_BANK_NORMAL, &myMasterBank);
		myMasterBank->loadSampleData();
	}

	void CAudioEngine::LoadSingleEvent(const FMOD_GUID& aEventID)
	{
		mySystem->getEventByID(&aEventID, &myEventDescriptions[aEventID]);
	}

	void CAudioEngine::LoadAllEvents()
	{
		int count;
		const int maxEvents = 64;
		FMOD::Studio::EventDescription* eventArray[maxEvents];
		myMasterBank->getEventList(eventArray, maxEvents, &count);

		FMOD_GUID id;
		for (std::size_t i = 0; i < count; ++i)
		{
			eventArray[i]->getID(&id);
			LoadSingleEvent(id);
		}
	}

	void CAudioEngine::LoadSingleBus(const FMOD_GUID& aBusID)
	{
		mySystem->getBusByID(&aBusID, &myBuses[aBusID]);
	}

	void CAudioEngine::LoadAllBuses()
	{
		int count;
		const int maxBuses = 64;
		FMOD::Studio::Bus* busArray[maxBuses];
		myMasterBank->getBusList(busArray, maxBuses, &count);

		FMOD_GUID id;
		for (std::size_t i = 0; i < count; ++i)
		{
			busArray[i]->getID(&id);
			LoadSingleBus(id);
		}
	}

	void CAudioEngine::Update(Matrix4x4f aListenerTransform, const bool& aShouldUpdateListener)
	{
		mySystem->update();
		if (aShouldUpdateListener)
		{
			myListenerMatrix = aListenerTransform;
			UpdateListener();
		}
		else
		{
			//UpdateListener();
		}
		

		//ShouldBeVirtual();
		SetTransform(myMusic, myListenerMatrix);
	}

	void CAudioEngine::ShouldBeVirtual()
	{
		for (auto& description : myEventDescriptions)
		{
			int instancesWithinRadius = 0;
			for (size_t instanceIndex = 0; instanceIndex < myEventInstances[description.first].size(); instanceIndex++)
			{
				FMOD::Studio::EventInstance* instance = myEventInstances[description.first][instanceIndex];
				Vector3f instancePos = GetInstancePos(instance);
				Vector3f listenerPos = GetListenerMatrix().GetPosition();
				Vector3f distanceToListener = instancePos - listenerPos;
				if (distanceToListener.Length() > 5.0f)
				{
					instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
				}
				else
				{
					FMOD_STUDIO_PLAYBACK_STATE state;
					instance->getPlaybackState(&state);
					/*if (state == FMOD_STUDIO_PLAYBACK_STOPPED || state == FMOD_STUDIO_PLAYBACK_STOPPING)
					{
						instance->start();
					}*/
					if (instanceIndex < myEventInstances[description.first].size()-1)
					{
						FMOD::Studio::EventInstance* nextInstance = myEventInstances[description.first][instanceIndex + 1];
					
						Vector3f nextInstancePos = GetInstancePos(nextInstance);
						Vector3f distance = nextInstancePos - instancePos;
						float distanceLength = distance.Length();
						if (distanceLength < 20.0f)
						{
							instancesWithinRadius++;
							if (instancesWithinRadius > 2)
							{
								nextInstance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
							}
						}
						else
						{
							instancesWithinRadius = 0;
						}
					}
				}
				
			}
		}
	}

	const Matrix4x4f& CAudioEngine::GetListenerMatrix()
	{
		return myListenerMatrix;
	}

	void CAudioEngine::SetSnapShot(const FMOD_GUID& aBusID, const bool& shouldStart)
	{
		if (shouldStart)
		{
			myEventInstances[aBusID][0]->start();
		}
		else
		{
			myEventInstances[aBusID][0]->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
		}
		
	}

	void CAudioEngine::ChangeRandomMusicIndex()
	{
		std::random_device dev;
		std::mt19937 rng(dev());
		std::uniform_int_distribution<std::mt19937::result_type> dist4(0, 3); 

		myRandomMusicIndex = dist4(rng);

	}

	Vector3f CAudioEngine::GetInstancePos(FMOD::Studio::EventInstance* instance)
	{
		FMOD_3D_ATTRIBUTES instanceVec;
		instance->get3DAttributes(&instanceVec);
		Vector3f instancePos;
		instancePos.x = instanceVec.position.x;
		instancePos.y = instanceVec.position.y;
		instancePos.z = instanceVec.position.z;
		return instancePos;
	}

	void CAudioEngine::UpdateListener()
	{
		FMOD_3D_ATTRIBUTES vec = {};

		vec.forward.x = myListenerMatrix(3, 1);
		vec.forward.y = myListenerMatrix(3, 2);
		vec.forward.z = myListenerMatrix(3, 3);

		vec.up.x = myListenerMatrix(2, 1);
		vec.up.y = myListenerMatrix(2, 2);
		vec.up.z = myListenerMatrix(2, 3);


	

		vec.position.x =  myListenerMatrix.GetPosition().x * 0.01f;
		vec.position.y =  myListenerMatrix.GetPosition().y * 0.01f;
		vec.position.z =  myListenerMatrix.GetPosition().z * 0.01f;
		mySystem->setListenerAttributes(0, &vec);
	}

	void CAudioEngine::SetTransform(FMOD::Studio::EventInstance* anInstance, const Matrix4x4f& aWorldTransform)
	{
		FMOD_3D_ATTRIBUTES vec = {};

		vec.forward.x = aWorldTransform(3, 1);
		vec.forward.y = aWorldTransform(3, 2);
		vec.forward.z = aWorldTransform(3, 3);

		vec.up.x = aWorldTransform(2, 1);
		vec.up.y = aWorldTransform(2, 2);
		vec.up.z = aWorldTransform(2, 3);

		vec.position.x = aWorldTransform.GetPosition().x * 0.01f;
		vec.position.y = aWorldTransform.GetPosition().y * 0.01f;
		vec.position.z = aWorldTransform.GetPosition().z * 0.01f;

		anInstance->set3DAttributes(&vec);
	}


	void CAudioEngine::SetPosition(FMOD::Studio::EventInstance* anInstance, const Vector3f& aWorldPosition)
	{
		FMOD_3D_ATTRIBUTES vec = {};

		vec.forward.z = 1;

		vec.up.y = 1;

		vec.position.x = aWorldPosition.x * 0.01f;
		vec.position.y = aWorldPosition.y * 0.01f;
		vec.position.z = aWorldPosition.z * 0.01f;

		anInstance->set3DAttributes(&vec);
	}
	void CAudioEngine::SetUIPosition(FMOD::Studio::EventInstance* anInstance)
	{
		Vector3f listenerPos = GetListenerMatrix().GetPosition();
		FMOD_3D_ATTRIBUTES vec = {};
		vec.forward.z = 1.0f;
		vec.up.y = 1.0f;
		vec.position.x = listenerPos.x * 0.01f;
		vec.position.y = listenerPos.y * 0.01f;
		vec.position.z = listenerPos.z * 0.01f;

		anInstance->set3DAttributes(&vec);
	}

	void CAudioEngine::InitSystem()
	{
		FMOD_RESULT result;

		result = FMOD::Studio::System::create(&mySystem); // Create the Studio System object.
		if (result != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
			exit(-1);
		}

		// Initialize FMOD Studio, which will also initialize FMOD Core
		result = mySystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);
		if (result != FMOD_OK)
		{
			printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
			exit(-1);
		}
	}


	//Might be used in future
	float CAudioEngine::dBToVolume(float adB)
	{
		return powf(10.0f, 0.05f * adB);
	}
	float CAudioEngine::VolumeTodB(float aVolume)
	{
		return 20.0f * log10f(aVolume);
	}

}