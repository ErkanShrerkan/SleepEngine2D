#include "pch.h"
#include <filesystem>
#include <fstream>
#include <Engine/JsonDocument.h>
#include "SceneManager.h"
#include "GameManager.h"

SceneManager::SceneManager(GameManager* aGM)
	: myGameManager(aGM)
{
	CreateDirectory(L"Assets/Scenes", NULL);
	auto it = std::filesystem::recursive_directory_iterator{ "Assets/Scenes" };
	for (const auto& dir : it)
	{
		std::string entry(dir.path().string());
		std::replace(entry.begin(), entry.end(), '\\', '/');
		std::string ext = std::string(entry.end() - 4, entry.end());
		if (ext == ".scn")
		{
			myActiveScene = std::string(entry.begin() + entry.find_last_of("/"), entry.end() - 4);
			mySceneDoc = new JsonDocument(entry);
			return;
		}
	}

	CreateScene("New Scene");
}

SceneManager::~SceneManager()
{
	delete mySceneDoc;
}

void SceneManager::SaveScene()
{
	mySceneDoc->SaveToFile(GetScenePath(myActiveScene), true);
}

void SceneManager::DeleteScene()
{
	remove(GetScenePath(myActiveScene).c_str());
}

void SceneManager::LoadScene(const std::string& aSceneName)
{
	if (!SceneExists(aSceneName))
	{
		return;
	}

	// Scene data follows this example
	//{
	//  "Entities": [
	//    {
	//      "ID": 0,
	//      "Components": 
	//      [
	//        {
	//          "ID": 0,
	//          "Exposed Variables" : 
	//          {
	//              "Speed": 500 // float
	//          }
	//        },
	//        {
	//          "ID": 1,
	//          "Exposed Variables": 
	//          {
	//              "Texture": "Texture/Texture.textute", // string
	//              "Rect": [0, 0, 1, 1] // vector
	//          }
	//        }
	//      ]
	//    }
	//  ]
	//}

	// TODO: Load Scene Data
	// populate GameManager with data from document
	for (auto& entity : mySceneDoc->GetDocument()["Entities"].GetArray())
	{
		uint entityID = entity["ID"].GetUint();
		entityID;
		for (auto& component : entity["Components"].GetArray())
		{
			uint componentID = component["ID"].GetUint();
			componentID;
			for (auto variable = component["Exposed Variables"].MemberBegin(); variable != component["Exposed Variables"].MemberEnd(); ++variable)
			{
				std::string variableName = variable->name.GetString();
				rapidjson::Type type = variable->value.GetType();

				variableName;
				switch (type)
				{
				case rapidjson::kArrayType:
					break;
				case rapidjson::kStringType:
					break;
				case rapidjson::kNumberType:
					break;
				default:
					break;
				}
			}
		}
	}

	myActiveScene = aSceneName;
}

void SceneManager::CreateScene(const std::string& aSceneName)
{
	SaveScene();

	if (SceneExists(aSceneName))
	{
		LoadScene(aSceneName);
		return;
	}

	delete mySceneDoc;
	mySceneDoc = new JsonDocument(GetScenePath(aSceneName));
	myActiveScene = aSceneName;
}

bool SceneManager::SceneExists(const std::string& aSceneName)
{
	std::ifstream t(GetScenePath(aSceneName), std::ios::in | std::ios::binary);
	std::stringstream buffer;
	buffer << t.rdbuf();
	t.close();
	return !buffer.fail();
}

std::string SceneManager::GetScenePath(const std::string& aSceneName)
{
	return "Scenes/" + aSceneName + ".scn";
}

void SceneManager::ChangeSceneName(const std::string& aNewSceneName)
{
	std::string oldName = myActiveScene;
	myActiveScene = aNewSceneName;
	SaveScene();
	myActiveScene = oldName;
	DeleteScene();
	myActiveScene = aNewSceneName;
}