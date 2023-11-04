#pragma once
#include "ShaderDefines.h"
#include <array>
#include <Engine\Transform.h>
#include <memory>
#include <map>
#include <CommonUtilities\Container\RefillVector.h>
// TODO: Replace vector with set
class Text;
class Sprite;
namespace SE
{
	class CSprite;
	class ISceneInstanceList
	{
	public:
		virtual void AddInstance(void* anInstance) = 0;
		virtual void AddInstance(std::vector<void*> someInstances) = 0;
	};

	template <class T>
	struct SceneInstanceListImpl : public ISceneInstanceList
	{
	public:
		CommonUtilities::RefillVector<T*> vector;

	public:
		virtual void AddInstace(void* anInstance) override
		{
			vector.push_back((T*)anInstance);
		}

		virtual void AddInstace(std::vector<void*> someInstances) override
		{
			for (auto instance : someInstances)
			{
				vector.push_back((T*)instance);
			}
		}
	};

	class CScene
	{
	public:

		void Init();
		void AddInstance(Sprite* aSprite);
		void AddInstance(std::vector<Sprite*> someSprite);
		void PrepareFrame();
		void SetLoadingFrame(bool aLoadingFrame);
		void SetPaused(bool aValue) { myIsPaused = aValue; }

		template<class T>
		CommonUtilities::RefillVector<T*> GetInstances();

		template<class T>
		void AddInstance(T* anInstance);

		template<class T>
		void AddInstance(std::vector<T*> someInstances);

		CommonUtilities::RefillVector<Sprite*>& GetSprites();
		CommonUtilities::RefillVector<Sprite*>& GetSSSprites();

	private:
		CommonUtilities::RefillVector<Sprite*> mySprites;
		CommonUtilities::RefillVector<Sprite*> mySSSprites;

		bool myLoadingFrame = false;
		bool myIsPaused = false;
	};
}
