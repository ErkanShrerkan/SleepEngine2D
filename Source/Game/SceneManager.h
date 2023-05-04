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
	void SaveSceneAs(const std::string& aSceneName);
	void LoadScene(const std::string& aSceneName);
	void LoadScene();
	void ChangeSceneName(const std::string& aNewSceneName);
	void CreateScene(const std::string& aSceneName);
	bool SceneExists(const std::string& aSceneName);
	
	std::string GetScenePath(const std::string& aSceneName);

private:
	std::string myActiveScene = "";
	uptr(JsonDocument) mySceneDoc;
	GameManager* myGameManager;
};

