#pragma once
class JsonDocument;
class GameManager;

class SceneManager
{
public:
	SceneManager(GameManager* aGM);
	~SceneManager();

	void SaveScene();
	void DeleteScene();
	void LoadScene(const std::string& aSceneName);
	bool SceneExists(const std::string& aSceneName);
	void ChangeSceneName(const std::string& aNewSceneName);
	void CreateScene(const std::string& aSceneName);
	
	std::string GetScenePath(const std::string& aSceneName);

private:
	std::string myActiveScene = "";
	JsonDocument* mySceneDoc;
	GameManager* myGameManager;
};

