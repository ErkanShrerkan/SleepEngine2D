#include "pch.h"
#include <filesystem>
#include <fstream>
#include <Engine/JsonDocument.h>
#include "SceneManager.h"
#include "GameManager.h"

#define STRING(x) #x
#define WSTRING(x) L ## #x
#define STRINGIFY(x) STRING(x)
#define WSTRINGIFY(x) WSTRING(x)
#define SCENE_PATH Assets/Scenes
#define SCENE_PATH_S STRINGIFY(SCENE_PATH)
#define SCENE_PATH_W WSTRINGIFY(SCENE_PATH)

SceneManager::SceneManager(GameManager* aGM)
	: myGameManager(aGM)
{
	mySceneDoc = std::make_unique<JsonDocument>();

	CreateDirectory(SCENE_PATH_W, NULL);
	auto it = std::filesystem::recursive_directory_iterator{ SCENE_PATH_S };
	for (const auto& dir : it)
	{
		std::string entry(dir.path().string());
		std::replace(entry.begin(), entry.end(), '\\', '/');
		std::string ext = std::string(entry.end() - 4, entry.end());
		if (ext == ".scn")
		{
			myActiveScene = std::string(entry.begin() + entry.find_last_of("/"), entry.end() - 4);

			mySceneDoc->ParseFile(GetScenePath(entry));
			LoadScene(entry);

			return;
		}
	}

	CreateScene("New Scene");
}

SceneManager::~SceneManager()
{
	SaveScene();
}

void SceneManager::SaveScene()
{
	SaveSceneAs(myActiveScene);
}

void SceneManager::DeleteScene()
{
	remove(GetScenePath(myActiveScene).c_str());
}

void SceneManager::SaveSceneAs(const std::string& aSceneName)
{
	FormatEntityComponentsForSaving();

	mySceneDoc->SaveToFile(GetScenePath(aSceneName), true);
}

void SceneManager::LoadScene(const std::string& aSceneName)
{
	if (!SceneExists(aSceneName))
		return;

	myGameManager->UnLoadAll();

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

	//for (auto& entity : mySceneDoc->GetDocument()["Entities"].GetArray())
	//{
	//	uint entityID = entity["ID"].GetUint();
	//	entityID;
	//	for (auto& component : entity["Components"].GetArray())
	//	{
	//		uint componentID = component["ID"].GetUint();
	//		componentID;
	//		for (auto variable = component["Exposed Variables"].MemberBegin(); variable != component["Exposed Variables"].MemberEnd(); ++variable)
	//		{
	//			std::string variableName = variable->name.GetString();
	//			rapidjson::Type type = variable->value.GetType();

	//			variableName;
	//			switch (type)
	//			{
	//			case rapidjson::kArrayType:
	//				break;
	//			case rapidjson::kStringType:
	//				break;
	//			case rapidjson::kNumberType:
	//				break;
	//			default:
	//				break;
	//			}
	//		}
	//	}
	//}

	myActiveScene = aSceneName;
}

void SceneManager::LoadScene()
{
	// Find main scene and load
	// if there is no main scene, create one
}

void SceneManager::CreateScene(const std::string& aSceneName)
{
	SaveScene();

	if (SceneExists(aSceneName))
	{
		LoadScene(aSceneName);
		return;
	}

	mySceneDoc->ParseFile(GetScenePath(aSceneName));
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
	std::string path(SCENE_PATH_S);
	return path + "/" + aSceneName + ".scn";
}

void SceneManager::FormatEntityComponentsForSaving()
{
	auto& entityComponents = myGameManager->GetEntityComponents();
	auto& doc = mySceneDoc->GetDocument();

	// Register which entities have which components
	rapidjson::Value entityComponentArray(rapidjson::kArrayType);
	rapidjson::Document::AllocatorType& alctr = doc.GetAllocator();
	for (auto& [entityID, components] : entityComponents)
	{
		// Entity ID
		rapidjson::Value entity(rapidjson::kObjectType);
		entity.AddMember("ID", entityID, alctr);

		// Component IDs
		rapidjson::Value componentArray(rapidjson::kArrayType);
		for (auto& [componentID, component] : components)
		{
			componentArray.PushBack(componentID, alctr);
		}
		entity.AddMember("Components", componentArray, alctr);

		entityComponentArray.PushBack(entity, alctr);
	}
	doc.AddMember("Entity Components", entityComponentArray, alctr);
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